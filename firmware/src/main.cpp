/**
 * @file main.cpp
 * @brief ESP32 Embedded C++ Firmware for Automated Solar Boat Bilge Drainage & Telemetry
 * @author Luca Rodrigues Gomes de Sant'Anna
 * @institution Instituto Federal Fluminense (IFF) - Campus Cabo Frio
 * @details Bachelor Thesis in Mechanical Engineering (2025)
 * 
 * Non-blocking architecture featuring:
 * 1. Hardware Watchdog Timer (esp_task_wdt) for fail-safe resilience
 * 2. In-place 5-sample median acoustic filtering for outlier rejection
 * 3. Priority emergency float switch override with 3-sample digital debounce
 * 4. Calibrated ADC battery sampling with multisampling noise suppression
 * 5. High-efficiency packed UDP broadcast telemetry (1 Hz) with CRC16-CCITT
 * 6. RTC fast memory boot tracking (RTC_DATA_ATTR) without Flash memory wear
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_task_wdt.h>
#include "config.h"

// Networking objects
WiFiUDP udp;

// RTC Fast Memory (survives watchdog resets without wearing flash storage)
RTC_DATA_ATTR static uint32_t bootCount = 0;

// Global system state
bool isPumpActive = false;
float currentWaterLevelCm = 0.0f;
bool isEmergencyActive = false;
bool isSensorFault = false;
float currentBatteryVoltage = 12.6f;
uint32_t telemetrySequenceId = 0;

// Debounce state counters
static int floatSwitchActiveCount = 0;

// Non-blocking timers
unsigned long lastSensorReadMs = 0;
unsigned long lastTelemetrySendMs = 0;

// Function prototypes
float readWaterLevelMedianCm();
bool checkEmergencyFloatSwitchDebounced();
float readCalibratedBatteryVoltage();
void sendTelemetryPacket();
uint16_t calculateCRC16(const uint8_t *data, size_t length);

void setup() {
    bootCount++;
    Serial.begin(115200);
    Serial.println(F("==============================================================="));
    Serial.println(F(" SolarBoat-Telemetry: Smart Bilge Drainage & Wireless Telemetry "));
    Serial.println(F(" Author: Luca Rodrigues Gomes de Sant'Anna                      "));
    Serial.println(F(" Institution: Instituto Federal Fluminense (IFF Cabo Frio)     "));
    Serial.printf(  " Boot Sequence: #%u (RTC Data Attr Tracker)\n", bootCount);
    Serial.println(F("==============================================================="));

    // Pin configurations
    pinMode(PIN_TRIGGER, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
    pinMode(PIN_PUMP_RELAY, OUTPUT);
    pinMode(PIN_FLOAT_SWITCH, INPUT_PULLUP);
    pinMode(PIN_BATTERY_ADC, INPUT);

    // Initial state: Relay OFF
    digitalWrite(PIN_PUMP_RELAY, RELAY_OFF);

    // 1. Initialize Wi-Fi in Access Point mode for local field telemetry
    WiFi.mode(WIFI_AP);
    WiFi.softAP("SolarBoat-Telemetry", "DSB2025Boat");
    udp.begin(TELEMETRY_UDP_PORT);
    Serial.println(F("WiFi AP 'SolarBoat-Telemetry' initialized. UDP broadcast ready."));

    // 2. Initialize Hardware Watchdog Timer (10s timeout, panic/reboot on lockup)
    esp_task_wdt_init(WATCHDOG_TIMEOUT_SECONDS, true);
    esp_task_wdt_add(NULL); // Add current loop task to watchdog monitoring
    Serial.println(F("Hardware Watchdog Timer enabled (10s timeout)."));

    Serial.println(F("System running in non-blocking event loop mode."));
}

void loop() {
    // A. Reset watchdog timer on every iteration
    esp_task_wdt_reset();

    unsigned long currentMillis = millis();

    // B. Periodic Sensor Sampling & Drainage Control (500 ms cycle)
    if (currentMillis - lastSensorReadMs >= SENSOR_INTERVAL_MS) {
        lastSensorReadMs = currentMillis;

        // 1. Read Sensors with Noise Rejection & Debounce
        isEmergencyActive = checkEmergencyFloatSwitchDebounced();
        float measuredLevel = readWaterLevelMedianCm();
        
        if (measuredLevel >= 0.0f) {
            currentWaterLevelCm = measuredLevel;
            isSensorFault = false;
        } else {
            isSensorFault = true;
        }

        currentBatteryVoltage = readCalibratedBatteryVoltage();

        // 2. Control Strategy (Hysteresis & Fail-Safe)
        if (isEmergencyActive) {
            // Priority 1: Emergency Mechanical Float Switch Override
            if (!isPumpActive) {
                Serial.println(F("[ALERT] Debounced emergency float switch triggered! Forcing pump ON..."));
                digitalWrite(PIN_PUMP_RELAY, RELAY_ON);
                isPumpActive = true;
            }
        } 
        else if (isSensorFault) {
            // Priority 2: Acoustic Sensor Timeout/Fault Fallback (Keep pump OFF unless emergency float trips)
            if (isPumpActive && !isEmergencyActive) {
                Serial.println(F("[WARN] Sensor fault detected. Turning pump OFF for dry-run protection."));
                digitalWrite(PIN_PUMP_RELAY, RELAY_OFF);
                isPumpActive = false;
            }
        } 
        else {
            // Priority 3: Ultrasonic Hysteresis Control
            if (currentWaterLevelCm >= WATER_LEVEL_PUMP_ON_CM) {
                if (!isPumpActive) {
                    Serial.println(F("[INFO] Water level >= 9.0 cm. Turning pump ON..."));
                    digitalWrite(PIN_PUMP_RELAY, RELAY_ON);
                    isPumpActive = true;
                }
            } 
            else if (currentWaterLevelCm <= WATER_LEVEL_PUMP_OFF_CM) {
                if (isPumpActive) {
                    Serial.println(F("[INFO] Water level <= 3.0 cm. Turning pump OFF (Dry-run safe)..."));
                    digitalWrite(PIN_PUMP_RELAY, RELAY_OFF);
                    isPumpActive = false;
                }
            }
        }
    }

    // C. Periodic Wireless Telemetry Broadcast (1000 ms cycle)
    if (currentMillis - lastTelemetrySendMs >= TELEMETRY_INTERVAL_MS) {
        lastTelemetrySendMs = currentMillis;
        sendTelemetryPacket();
    }
}

/**
 * @brief Reads HC-SR04 ultrasonic sensor with an in-place 5-sample median filter
 * Rejects high/low acoustic spikes and turbulence artifacts inside the stilling well.
 */
float readWaterLevelMedianCm() {
    long rawEchoSamples[FILTER_SAMPLE_COUNT];
    int validCount = 0;

    for (int i = 0; i < FILTER_SAMPLE_COUNT; i++) {
        digitalWrite(PIN_TRIGGER, LOW);
        delayMicroseconds(2);
        digitalWrite(PIN_TRIGGER, HIGH);
        delayMicroseconds(10);
        digitalWrite(PIN_TRIGGER, LOW);

        long echoDurationUs = pulseIn(PIN_ECHO, HIGH, ECHO_TIMEOUT_US);

        if (echoDurationUs > 0) {
            rawEchoSamples[validCount] = echoDurationUs;
            validCount++;
        }
        delayMicroseconds(200);
    }

    if (validCount < 3) {
        return -1.0f; // Sensor fault / timeout indicator
    }

    // In-place insertion sort to obtain the median
    for (int i = 1; i < validCount; i++) {
        long key = rawEchoSamples[i];
        int j = i - 1;
        while (j >= 0 && rawEchoSamples[j] > key) {
            rawEchoSamples[j + 1] = rawEchoSamples[j];
            j = j - 1;
        }
        rawEchoSamples[j + 1] = key;
    }

    long medianDurationUs = rawEchoSamples[validCount / 2];
    float measuredDistanceCm = (medianDurationUs * SPEED_OF_SOUND_CM_US) / 2.0f;
    float waterLevelCm = STILLING_WELL_HEIGHT_CM - measuredDistanceCm;

    return (waterLevelCm < 0.0f) ? 0.0f : waterLevelCm;
}

/**
 * @brief Reads emergency mechanical float switch with consecutive-reading debounce
 */
bool checkEmergencyFloatSwitchDebounced() {
    bool rawPinLow = (digitalRead(PIN_FLOAT_SWITCH) == LOW);
    if (rawPinLow) {
        floatSwitchActiveCount++;
        if (floatSwitchActiveCount >= FLOAT_DEBOUNCE_THRESHOLD) {
            floatSwitchActiveCount = FLOAT_DEBOUNCE_THRESHOLD; // Clamp counter
            return true;
        }
    } else {
        floatSwitchActiveCount = 0; // Reset debounce counter immediately upon normal level
    }
    return false;
}

/**
 * @brief Reads battery bank voltage via calibrated 10-sample ADC averaging
 */
float readCalibratedBatteryVoltage() {
    long adcSum = 0;
    const int ADC_SAMPLES = 10;

    for (int i = 0; i < ADC_SAMPLES; i++) {
        adcSum += analogRead(PIN_BATTERY_ADC);
        delayMicroseconds(100);
    }

    float avgAdc = (float)adcSum / (float)ADC_SAMPLES;
    // Calibrated voltage divider ratio (e.g. 100k / 22k -> 5.545x)
    float pinVoltage = (avgAdc / 4095.0f) * 3.3f;
    float batteryVoltage = pinVoltage * ((100.0f + 22.0f) / 22.0f);
    return batteryVoltage;
}

/**
 * @brief Calculates 16-bit CRC-CCITT (polynomial 0x1021) for telemetry payload verification
 */
uint16_t calculateCRC16(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= ((uint16_t)data[i] << 8);
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

/**
 * @brief Packs and broadcasts UDP telemetry frame to ground station / pilot dashboard
 */
void sendTelemetryPacket() {
    TelemetryPacket packet;
    packet.sequenceId = ++telemetrySequenceId;
    packet.uptimeMs = millis();
    packet.bootCount = bootCount;
    packet.waterLevelCm = currentWaterLevelCm;
    packet.batteryVoltage = currentBatteryVoltage;
    packet.pumpActive = isPumpActive ? 1 : 0;
    packet.emergencyOverride = isEmergencyActive ? 1 : 0;
    packet.sensorStatus = isSensorFault ? 0 : 1;
    packet.crc16 = calculateCRC16((const uint8_t *)&packet, sizeof(TelemetryPacket) - sizeof(uint16_t));

    // Broadcast over UDP
    udp.beginPacket(TELEMETRY_BROADCAST_IP, TELEMETRY_UDP_PORT);
    udp.write((const uint8_t *)&packet, sizeof(TelemetryPacket));
    udp.endPacket();

    // Serial Debug Monitor
    Serial.printf("[TELEMETRY] Boot: %u | Seq: %u | T: %lus | Level: %.1fcm | Pump: %s | Emer: %s | Bat: %.2fV\n",
                  packet.bootCount,
                  packet.sequenceId,
                  packet.uptimeMs / 1000,
                  packet.waterLevelCm,
                  packet.pumpActive ? "ON" : "OFF",
                  packet.emergencyOverride ? "ACTIVE" : "NORM",
                  packet.batteryVoltage);
}

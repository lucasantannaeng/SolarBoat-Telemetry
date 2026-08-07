/**
 * @file main.cpp
 * @brief ESP32 Embedded C++ Firmware for Automated Solar Boat Bilge Drainage & Telemetry
 * @author Luca Rodrigues Gomes de Sant'Anna
 * @institution Instituto Federal Fluminense (IFF) - Campus Cabo Frio
 * @details Bachelor Thesis in Mechanical Engineering (2025)
 */

#include <Arduino.h>
#include "config.h"

// Global tracking variable for bilge pump state
bool isPumpActive = false;

// Function prototypes
float readWaterLevelCm();
bool checkEmergencyFloatSwitch();

void setup() {
    Serial.begin(115200);
    Serial.println(F("==============================================================="));
    Serial.println(F(" SolarBoat-Telemetry: Automated Smart Bilge Drainage System "));
    Serial.println(F(" Author: Luca Rodrigues Gomes de Sant'Anna                      "));
    Serial.println(F(" Institution: Instituto Federal Fluminense (IFF Cabo Frio)     "));
    Serial.println(F("==============================================================="));

    pinMode(PIN_TRIGGER, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
    pinMode(PIN_PUMP_RELAY, OUTPUT);
    
    // Ensure relay starts in OFF state
    digitalWrite(PIN_PUMP_RELAY, RELAY_OFF);

    // Mechanical float switch with internal pull-up resistor
    pinMode(PIN_FLOAT_SWITCH, INPUT_PULLUP);

    Serial.println(F("System initialized. Monitoring bilge water levels..."));
}

void loop() {
    bool isEmergencyActive = checkEmergencyFloatSwitch();
    float currentWaterLevelCm = readWaterLevelCm();

    Serial.print(F("Water Level (cm): "));
    Serial.print(currentWaterLevelCm);
    Serial.print(F(" | Emergency Float Switch: "));
    Serial.println(isEmergencyActive ? F("ACTIVATED (FAIL-SAFE OVERRIDE)") : F("NORMAL"));

    // 1. HIGHEST PRIORITY: FAIL-SAFE OVERRIDE (MECHANICAL FLOAT SWITCH)
    if (isEmergencyActive) {
        if (!isPumpActive) {
            Serial.println(F("CRITICAL ALERT: Emergency float switch triggered! Forcing pump ON..."));
            digitalWrite(PIN_PUMP_RELAY, RELAY_ON);
            isPumpActive = true;
        }
    }
    // 2. PRIMARY CONTROL LOGIC: INTELLECTUAL ULTRASONIC HYSTERESIS
    else {
        if (currentWaterLevelCm >= WATER_LEVEL_PUMP_ON_CM) {
            if (!isPumpActive) {
                Serial.println(F("Upper threshold (9 cm) reached. Turning pump ON..."));
                digitalWrite(PIN_PUMP_RELAY, RELAY_ON);
                isPumpActive = true;
            }
        }
        else if (currentWaterLevelCm <= WATER_LEVEL_PUMP_OFF_CM) {
            if (isPumpActive) {
                Serial.println(F("Lower threshold (3 cm) reached. Turning pump OFF (Dry-run protection)..."));
                digitalWrite(PIN_PUMP_RELAY, RELAY_OFF);
                isPumpActive = false;
            }
        }
        // Note: Between 3 cm and 9 cm, previous state is maintained (6 cm Hysteresis Band)
    }

    delay(1000); // Main monitoring loop delay (1 second cycle)
}

/**
 * @brief Reads the HC-SR04 ultrasonic ToF sensor inside the Stilling-Well
 * @return Water level in cm from the bottom of the bilge. Returns -1.0f on sensor failure.
 */
float readWaterLevelCm() {
    long totalDurationUs = 0;
    int validReadingsCount = 0;

    for (int i = 0; i < FILTER_SAMPLE_COUNT; i++) {
        digitalWrite(PIN_TRIGGER, LOW);
        delayMicroseconds(2);
        digitalWrite(PIN_TRIGGER, HIGH);
        delayMicroseconds(10);
        digitalWrite(PIN_TRIGGER, LOW);

        long echoDurationUs = pulseIn(PIN_ECHO, HIGH, ECHO_TIMEOUT_US);

        if (echoDurationUs > 0) {
            totalDurationUs += echoDurationUs;
            validReadingsCount++;
        }
        delay(10);
    }

    if (validReadingsCount == 0) {
        Serial.println(F("ERROR: HC-SR04 sensor non-responsive (Timeout)."));
        return -1.0f;
    }

    long avgDurationUs = totalDurationUs / validReadingsCount;
    float measuredDistanceCm = (avgDurationUs * SPEED_OF_SOUND_CM_US) / 2.0f;
    float waterLevelCm = STILLING_WELL_HEIGHT_CM - measuredDistanceCm;

    if (waterLevelCm < 0.0f) {
        waterLevelCm = 0.0f;
    }

    return waterLevelCm;
}

/**
 * @brief Checks state of emergency mechanical float switch
 * @return true if float switch contact is closed to GND (critical water level)
 */
bool checkEmergencyFloatSwitch() {
    return (digitalRead(PIN_FLOAT_SWITCH) == LOW);
}

/*
 * =====================================================================================
 * BACHELOR THESIS - MECHANICAL ENGINEERING (IFF CABO FRIO 2025)
 * AUTHOR: Luca Rodrigues Gomes de Sant'Anna
 * ADVISOR: Prof. Fernando Antonio Trindade Campos, M.Sc.
 * 
 * PROJECT: SolarBoat-Telemetry
 * AUTOMATED BILGE DRAINAGE SYSTEM WITH SLOSHING MITIGATION
 * AND ULTRASONIC SENSING FOR SOLAR-POWERED VESSELS
 * 
 * Microcontroller: ESP32
 * Language: C/C++ (Arduino Framework)
 * =====================================================================================
 */

// --- 1. GLOBAL DEFINITIONS & CONSTANTS ---
const int PIN_TRIGGER = 5;      // GPIO5  -> HC-SR04 Trigger
const int PIN_ECHO = 18;        // GPIO18 -> HC-SR04 Echo
const int PIN_FLOAT_SWITCH = 19;// GPIO19 -> Emergency Float Switch
const int PIN_PUMP_RELAY = 4;   // GPIO4  -> Bilge Pump Relay Module

#define RELAY_ON  LOW           // Active LOW Relay ON
#define RELAY_OFF HIGH          // Active LOW Relay OFF

const float SPEED_OF_SOUND_CM_US = 0.0343;
const float STILLING_WELL_HEIGHT_CM = 30.0;

const float WATER_LEVEL_PUMP_ON_CM = 9.0;
const float WATER_LEVEL_PUMP_OFF_CM = 3.0;
const int FILTER_SAMPLE_COUNT = 5;

bool isPumpActive = false;

// Function Prototypes
float readWaterLevelCm();
bool checkEmergencyFloatSwitch();

// --- 2. SETUP FUNCTION ---
void setup() {
    Serial.begin(115200);
    Serial.println("Initializing SolarBoat-Telemetry Bilge System...");

    pinMode(PIN_TRIGGER, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
    pinMode(PIN_PUMP_RELAY, OUTPUT);
    digitalWrite(PIN_PUMP_RELAY, RELAY_OFF);

    pinMode(PIN_FLOAT_SWITCH, INPUT_PULLUP);

    Serial.println("System ready. Monitoring water level...");
}

// --- 3. MAIN LOOP FUNCTION ---
void loop() {
    bool isEmergencyActive = checkEmergencyFloatSwitch();
    float currentWaterLevelCm = readWaterLevelCm();

    Serial.print("Water Level (cm): ");
    Serial.print(currentWaterLevelCm);
    Serial.print(" | Emergency Float Switch: ");
    Serial.println(isEmergencyActive ? "ACTIVATED (OVERRIDE)" : "NORMAL");

    // FAIL-SAFE OVERRIDE LOGIC:
    if (isEmergencyActive) {
        if (!isPumpActive) {
            Serial.println("ALERT: Emergency float switch activated! Forcing pump ON...");
            digitalWrite(PIN_PUMP_RELAY, RELAY_ON);
            isPumpActive = true;
        }
    }
    // PRIMARY HYSTERESIS CONTROL LOGIC:
    else {
        if (currentWaterLevelCm >= WATER_LEVEL_PUMP_ON_CM) {
            if (!isPumpActive) {
                Serial.println("Upper threshold reached. Turning pump ON...");
                digitalWrite(PIN_PUMP_RELAY, RELAY_ON);
                isPumpActive = true;
            }
        }
        else if (currentWaterLevelCm <= WATER_LEVEL_PUMP_OFF_CM) {
            if (isPumpActive) {
                Serial.println("Lower threshold reached. Turning pump OFF...");
                digitalWrite(PIN_PUMP_RELAY, RELAY_OFF);
                isPumpActive = false;
            }
        }
    }

    delay(1000);
}

// --- 4. HELPER FUNCTIONS ---
float readWaterLevelCm() {
    long totalDurationUs = 0;
    int validReadingsCount = 0;

    for (int i = 0; i < FILTER_SAMPLE_COUNT; i++) {
        digitalWrite(PIN_TRIGGER, LOW);
        delayMicroseconds(2);
        digitalWrite(PIN_TRIGGER, HIGH);
        delayMicroseconds(10);
        digitalWrite(PIN_TRIGGER, LOW);

        long echoDurationUs = pulseIn(PIN_ECHO, HIGH, 30000);

        if (echoDurationUs > 0) {
            totalDurationUs += echoDurationUs;
            validReadingsCount++;
        }
        delay(10);
    }

    if (validReadingsCount == 0) {
        Serial.println("Error: Ultrasonic sensor non-responsive.");
        return -1.0;
    }

    long avgDurationUs = totalDurationUs / validReadingsCount;
    float measuredDistanceCm = (avgDurationUs * SPEED_OF_SOUND_CM_US) / 2.0;
    float waterLevelCm = STILLING_WELL_HEIGHT_CM - measuredDistanceCm;

    if (waterLevelCm < 0) {
        waterLevelCm = 0;
    }

    return waterLevelCm;
}

bool checkEmergencyFloatSwitch() {
    return (digitalRead(PIN_FLOAT_SWITCH) == LOW);
}

/**
 * @file config.h
 * @brief Hardware pinout definitions, constants, and hysteresis calibration thresholds
 * @author Luca Rodrigues Gomes de Sant'Anna
 * @institution Instituto Federal Fluminense (IFF) - Campus Cabo Frio
 * @project SolarBoat-Telemetry: Smart Bilge Drainage System
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- ESP32 GPIO PINOUT MAPPING ---
static const int PIN_TRIGGER     = 5;   // GPIO5  -> HC-SR04 Ultrasonic Trigger Pin
static const int PIN_ECHO        = 18;  // GPIO18 -> HC-SR04 Ultrasonic Echo Pin
static const int PIN_FLOAT_SWITCH= 19;  // GPIO19 -> Emergency Mechanical Float Switch (INPUT_PULLUP)
static const int PIN_PUMP_RELAY  = 4;   // GPIO4  -> Bilge Pump Relay Module (Active LOW)

// --- RELAY MODULE LOGIC LEVELS (ACTIVE LOW) ---
#define RELAY_ON  LOW   // LOW signal activates the relay (turns pump ON)
#define RELAY_OFF HIGH  // HIGH signal deactivates the relay (turns pump OFF)

// --- PHYSICAL AND CALIBRATION CONSTANTS ---
static const float SPEED_OF_SOUND_CM_US     = 0.0343f; // Speed of sound in air at 20°C (cm/µs)
static const float STILLING_WELL_HEIGHT_CM  = 30.0f;   // Total height of Stilling-Well tube (cm)

// --- HYSTERESIS CONTROL THRESHOLDS (ULTRASONIC SENSOR) ---
static const float WATER_LEVEL_PUMP_ON_CM   = 9.0f;   // Turn pump ON when water level >= 9.0 cm
static const float WATER_LEVEL_PUMP_OFF_CM  = 3.0f;   // Turn pump OFF when water level <= 3.0 cm

// --- DIGITAL FILTERING & SAMPLING PARAMETERS ---
static const int FILTER_SAMPLE_COUNT        = 5;      // Number of samples for simple moving average
static const unsigned long ECHO_TIMEOUT_US  = 30000;  // pulseIn timeout (30 ms)

#endif // CONFIG_H

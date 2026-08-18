# 🚤 SolarBoat-Telemetry: Smart Bilge Drainage & Wireless Telemetry System

[![PlatformIO Build](https://img.shields.io/badge/PlatformIO-ESP32%20Build-orange?logo=platformio&logoColor=white)](https://platformio.org/)
[![C++17](https://img.shields.io/badge/Language-C%2B%2B17-00599C?logo=c%2B%2B&logoColor=white)](https://isocpp.org/)
[![Microcontroller](https://img.shields.io/badge/Hardware-ESP32--WROOM--32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Institution](https://img.shields.io/badge/IFF-Campus%20Cabo%20Frio-green.svg)](https://www.iff.edu.br)

> **Bachelor Thesis in Mechanical Engineering** presented to the **Instituto Federal de Educação, Ciência e Tecnologia Fluminense (IFF) - Campus Cabo Frio** (2025).

---

## 📌 Author & Academic Information

* **Author:** Luca Rodrigues Gomes de Sant'Anna ([@lucasantannaeng](https://github.com/lucasantannaeng))
* **Advisor:** Prof. Fernando Antonio Trindade Campos, M.Sc.
* **Evaluation Board:** Prof. Flávio Félix Feliciano, D.Sc. & Prof. Josinira Antunes de Amorim, D.Sc.
* **Institution:** Instituto Federal Fluminense (IFF) - Campus Cabo Frio
* **Field:** Naval Architecture, IoT Telemetry, Fluid Mechanics & Embedded Systems

---

## 🌊 Overview & Engineering Challenge

In lightweight solar-powered competition vessels (e.g., *Desafio Solar Brasil - DSB*), accidental bilge water accumulation introduces severe parasitic drag, lowers metacenter height ($\overline{GM}$), and risks short-circuiting low-voltage drive inverters.

Conventional bilge systems relying solely on mechanical float switches are notoriously prone to false triggering or jamming caused by hull slamming and wave-induced liquid motion (*sloshing*).

**SolarBoat-Telemetry** implements an integrated mechanical-electronic solution:
1. **Deflector Baffles:** Dissipate bulk liquid kinetic energy and suppress longitudinal sloshing inside the hull.
2. **Calming Well (Stilling-Well):** A $\varnothing 100\text{ mm}$ tube with $\varnothing 6\text{--}8\text{ mm}$ equalization ports acting as a low-pass hydraulic filter.
3. **Non-Blocking Firmware with Hysteresis:** Ultrasonic Time-of-Flight (HC-SR04) with moving average filtering and an operational hysteresis loop ($9\text{ cm}$ ON / $3\text{ cm}$ OFF).
4. **Wireless UDP Telemetry (1 Hz):** High-speed binary broadcast (`TelemetryPacket`) over local Wi-Fi Access Point to ground station telemetry dashboards.
5. **Hardware Watchdog Timer (`esp_task_wdt`):** Autonomous 10-second fail-safe reboot to eliminate system lockups in harsh marine environments.
6. **Hardware Fail-Safe:** Mechanical float switch priority interrupt for dry-run and emergency flood protection.

---

## 🏗️ Repository Architecture

```
SolarBoat-Telemetry/
├── firmware/
│   ├── platformio.ini              # PlatformIO environment & toolchain config
│   ├── include/
│   │   └── config.h                # Hardware pinouts, TelemetryPacket & hysteresis thresholds
│   ├── src/
│   │   └── main.cpp                # Non-blocking C++ event loop with watchdog & UDP telemetry
│   └── src_arduino/
│       └── firmware.ino            # Standalone Arduino IDE sketch
├── docs/
│   ├── TCC_DOCUMENTO_FINAL.pdf     # Full 63-page Bachelor Thesis manuscript
│   ├── ABSTRACT.md                 # English Abstract and Portuguese Resumo
│   ├── THESIS_CHAPTERS_SUMMARY.md  # Synthesis of Chapters 1 through 6
│   ├── CALCULATIONS_AND_DIMENSIONING.md # Fluid dynamics math, mass tables & hysteresis
│   └── ELECTRICAL_SCHEMATIC.md     # Circuit diagram & ESP32 pinout mapping
├── hardware/                       # Mechanical specifications & Bill of Materials
├── LICENSE
└── README.md
```

---

## 🔌 ESP32 Pinout & Telemetry Payload

### GPIO Mapping

| Pin | Peripheral | Purpose |
| :--- | :--- | :--- |
| **GPIO 5** | HC-SR04 Trigger | Ultrasonic Pulse Emitter (10 µs trigger) |
| **GPIO 18** | HC-SR04 Echo | Pulse-width Time-of-Flight Receiver |
| **GPIO 19** | Float Switch | Emergency Mechanical Override (`INPUT_PULLUP`) |
| **GPIO 4** | Relay Module | Bilge Pump Power Actuation (Active LOW) |
| **GPIO 34** | ADC Divider | Battery Bank Voltage Monitoring (0-16V scale) |

### Telemetry Packet Structure (Binary UDP)

```cpp
struct __attribute__((packed)) TelemetryPacket {
    uint32_t uptimeMs;          // System uptime in milliseconds
    float waterLevelCm;         // Filtered water level inside calming well (cm)
    float batteryVoltage;       // Battery bank voltage (V)
    uint8_t pumpActive;         // 1 = Pump ON, 0 = Pump OFF
    uint8_t emergencyOverride;  // 1 = Emergency float switch active
    uint8_t sensorStatus;       // 1 = Acoustic sensor healthy, 0 = Fault/Timeout
    uint16_t checksum;          // Frame integrity checksum
};
```

---

## ⚡ Firmware Compilation & Flashing

### Using PlatformIO (Recommended)

```bash
# Clone the repository
git clone https://github.com/lucasantannaeng/SolarBoat-Telemetry.git
cd SolarBoat-Telemetry/firmware

# Compile the firmware
pio run

# Flash to connected ESP32
pio run --target upload

# Open Serial Telemetry Monitor (115200 baud)
pio device monitor
```

---

## 📄 Academic Citation

If you utilize this research or code in your academic or technical work, please cite:

```bibtex
@mastersthesis{santanna2025solarboat,
  author    = {Luca Rodrigues Gomes de Sant'Anna},
  title     = {Sistema Inteligente de Drenagem de Por{\~a}o e Telemetria para Embarca{\c{c}}{\~o}es Solares},
  school    = {Instituto Federal Fluminense (IFF) - Campus Cabo Frio},
  year      = {2025},
  type      = {Trabalho de Conclus{\~a}o de Curso (Gradua{\c{c}}{\~a}o em Engenharia Mec{\^a}nica)}
}
```

---

## 📄 License

Licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

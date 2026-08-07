# 🚤 SolarBoat-Telemetry: Smart Bilge Drainage System

[![PlatformIO Build](https://github.com/lucasantannaeng/SolarBoat-Telemetry/actions/workflows/platformio-build.yml/badge.svg)](https://github.com/lucasantannaeng/SolarBoat-Telemetry/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Microcontroller](https://img.shields.io/badge/Microcontroller-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![Institution](https://img.shields.io/badge/IFF-Campus%20Cabo%20Frio-green.svg)](https://www.iff.edu.br)

> **Bachelor Thesis in Mechanical Engineering** presented to the **Instituto Federal de Educação, Ciência e Tecnologia Fluminense (IFF) - Campus Cabo Frio** (2025).

---

## 📌 Author & Academic Supervision

- **Author:** Luca Rodrigues Gomes de Sant'Anna ([@lucasantannaeng](https://github.com/lucasantannaeng))
- **Advisor:** Prof. Fernando Antonio Trindade Campos, M.Sc.
- **Evaluation Board:** Prof. Flávio Félix Feliciano, D.Sc. & Prof. Josinira Antunes de Amorim, D.Sc.
- **Institution:** Instituto Federal Fluminense (IFF) - Campus Cabo Frio
- **Domain:** Naval Architecture / Mechanical Engineering / Embedded Systems & IoT

---

## 🌊 Project Overview

In light solar-powered vessels (such as those competing in the **Desafio Solar Brasil - DSB**), accidental water accumulation in the bilge increases hydrodynamic drag, compromises metacenter height ($\overline{GM}$), and risks damaging sensitive motor drive electronics.

Conventional bilge systems relying solely on mechanical float switches operate reactively and are highly prone to false triggering or jamming caused by wave-induced liquid motion (*sloshing*).

**SolarBoat-Telemetry** implements a **hybrid engineering solution (mechanical + electronic)**:
1. **Global Hydrodynamic Damping (Baffles):** PVC deflector plates ($1000\times 200\times 4\text{ mm}$) that dissipate fluid kinetic energy and suppress wave resonance in the bilge.
2. **Local Mechanical Filtering (Stilling-Well):** A Ø100 mm PVC calm well with Ø6--8 mm equalization holes that dampens surface turbulence, providing a flat water level for acoustic sensing.
3. **Intelligent Ultrasonic Sensing (HC-SR04 + ESP32):** Non-intrusive Time-of-Flight (ToF) measurement featuring a 5-sample simple moving average filter and an operational **hysteresis window** ($9\text{ cm}$ turn ON / $3\text{ cm}$ turn OFF).
4. **Hardware Fail-Safe (Emergency Float Switch Override):** Direct priority override via GPIO pull-up to prevent vessel flooding in case of sensor or firmware faults.

---

## 🏗️ Repository Architecture

```text
SolarBoat-Telemetry/
├── .github/
│   └── workflows/
│       └── platformio-build.yml        # CI/CD automated firmware compilation workflow
├── .gitignore                         # Strict DevSecOps rules preventing secret leaks
├── LICENSE                            # MIT Open-Source License
├── README.md                          # Master English documentation
├── docs/                              # Detailed academic & engineering documentation
│   ├── TCC_DOCUMENTO_FINAL.pdf        # Complete thesis manuscript in PDF (63 pages)
│   ├── ABSTRACT.md                    # English Abstract & Portuguese Resumo
│   ├── THESIS_CHAPTERS_SUMMARY.md     # Chapter-by-chapter synthesis (Chapters 1 to 6)
│   ├── CALCULATIONS_AND_DIMENSIONING.md# Fluid dynamics math, mass tables & hysteresis
│   └── ELECTRICAL_SCHEMATIC.md        # Circuit diagram & ESP32 pinout mapping
├── firmware/                          # ESP32 Embedded C++ Firmware
│   ├── platformio.ini                 # PlatformIO build configuration
│   ├── include/
│   │   └── config.h                   # Pinout mapping, parameters & threshold constants
│   ├── src/
│   │   └── main.cpp                   # Main C++ modular firmware
│   └── src_arduino/
│       └── firmware.ino               # Standalone Arduino IDE sketch (Appendix A)
└── hardware/                          # Mechanical Specs & Bill of Materials
    ├── components_bom.csv             # Bill of Materials (BOM) with component masses
    ├── stilling_well_spec.md          # Technical specifications for Ø100mm PVC Stilling-Well
    └── baffles_spec.md                # Technical specifications for Anti-Sloshing Baffles
```

---

## 🔌 Circuit Pinout & Wiring (ESP32)

| Peripheral Device | Pin Name | ESP32 GPIO Pin | Description |
| :--- | :--- | :--- | :--- |
| **HC-SR04 Ultrasonic**| Trigger | **GPIO 5** | $10\,\mu\text{s}$ sampling pulse |
| **HC-SR04 Ultrasonic**| Echo | **GPIO 18** | Time-of-flight echo signal ($3.3\text{V}$) |
| **Float Switch** | N/O Terminal | **GPIO 19** | `INPUT_PULLUP` (Emergency level = LOW) |
| **Relay Module** | Input (IN) | **GPIO 4** | Active LOW relay trigger |

```text
                       +-----------------------------+
                       |   ESP32 Microcontroller     |
                       |                             |
  [HC-SR04] Trigger <--- GPIO 5                      |
  [HC-SR04] Echo    ---> GPIO 18                     |
  [Float Switch]    ---> GPIO 19 (Internal Pull-Up)  |
  [Relay Module]    <--- GPIO 4  (Active LOW)        |
                       +-----------------------------+
```

---

## 📐 Summary of Calculations & System Impact

- **Vessel Nominal Displacement:** $360\text{ kg}$
- **Total System Embarked Mass:** $2.88\text{ kg}$ ($0.79\%$ of displacement)
- **Stilling-Well Chamber:** Ø100 mm PVC pipe $\times$ 30 cm height (Mass: $0.122\text{ kg}$)
- **Anti-Sloshing Baffles:** 2 PVC plates $1000\times 200\times 4\text{ mm}$ (Mass: $2.224\text{ kg}$)
- **Seaflo 500 GPH Pump + Sensors + Cables:** $0.533\text{ kg}$
- **Hysteresis Operational Thresholds:** 
  - **Turn ON:** $\ge 9.0\text{ cm}$
  - **Turn OFF:** $\le 3.0\text{ cm}$
  - **Hysteresis Band:** $6.0\text{ cm}$ (protects relay & pump from chatter)

---

## 🚀 How to Build and Flash Firmware

### Option A: Using PlatformIO (Recommended)
```bash
cd firmware
pio run --target upload
```

### Option B: Using Arduino IDE
1. Navigate to `firmware/src_arduino/` and open `firmware.ino`.
2. Select **ESP32 Dev Module** under Tools -> Board.
3. Set Baud Rate to `115200` baud in Serial Monitor.
4. Compile and upload to ESP32 board.

---

## 📜 License

This project is open-source software licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

---

## 🎓 Academic Citation

If you use this project or reference this work in your research, please cite:

```bibtex
@thesis{santanna2025drenagem,
  author       = {Luca Rodrigues Gomes de Sant'Anna},
  title        = {Estudo e Desenvolvimento de um Sistema Automatizado de Drenagem de Porão com Mitigação de Sloshing e Acionamento Inteligente Baseado em Sensores Ultrassônicos},
  school       = {Instituto Federal de Educação, Ciência e Tecnologia Fluminense (IFF) - Campus Cabo Frio},
  year         = {2025},
  type         = {Trabalho de Conclusão de Curso (Bacharelado em Engenharia Mecânica)},
  address      = {Cabo Frio, RJ, Brazil},
  pages        = {63}
}
```

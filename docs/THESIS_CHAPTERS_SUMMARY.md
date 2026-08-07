# 📖 Detailed Chapter Summary of Thesis

## 📌 Chapter 1: Introduction
- **Context:** Challenges of solar boat racing (Desafio Solar Brasil - DSB).
- **Problem Statement:** High sensitivity of light hulls to internal water mass accumulation, causing hydrodynamic drag increases, loss of metacenter height ($\overline{GM}$), and potential water damage to solar motor electronics.
- **Limitations of Traditional Solutions:** Conventional mechanical float switches operate reactively only at critical levels and suffer from mechanical jams or false triggering due to wave sloshing.
- **Proposed Solution:** Hybrid non-intrusive automated system combining ultrasonic ToF distance sensing, Stilling-Well hydraulic dampening, Anti-Sloshing Baffles, and Fail-Safe float switch override.

---

## 📌 Chapter 2: Objectives

### 2.1 General Objective
Study, design, and develop an automated bilge drainage system controlled by an ESP32 microcontroller, integrating fluid mechanical sloshing mitigation and intelligent sensor control algorithms.

### 2.2 Specific Objectives
1. Perform hydrodynamic dimensioning of the Stilling-Well and anti-sloshing Baffles.
2. Specify hardware components (ESP32, HC-SR04 ultrasonic ToF sensor, relay module, Seaflo 500 GPH pump, mechanical float switch).
3. Develop embedded C++ firmware featuring simple moving average noise filtering and hysteresis control logic.
4. Design a hardware-based fail-safe emergency override system.
5. Quantify total embarked system mass and assess percentage impact on vessel displacement ($360\text{ kg}$).

---

## 📌 Chapter 3: Theoretical Foundation
- **Naval Architecture & Hydrostatic Safety:** Metacentric height ($\overline{GM}$), free surface effect, and vessel stability limits.
- **Sloshing Dynamics:** Fluid oscillation behavior in partially filled compartments and kinetic energy dissipation mechanisms.
- **Time-of-Flight (ToF) Ultrasonic Sensing:** HC-SR04 operating principles, acoustic speed equations, and measurement range limits.
- **Embedded Systems & Microcontrollers:** ESP32 architecture, GPIO configuration, internal pull-up resistors, and relay switching logic.

---

## 📌 Chapter 4: Materials and Methods
- **Baffle Geometry:** PVC plates ($1000\times 200\times 4\text{ mm}$) featuring a $10\text{ mm}$ perforation grid.
- **Stilling-Well Specification:** PVC pipe Ø100 mm $\times$ 30 cm height with 4 radial bottom holes of Ø6--8 mm.
- **Embarked Mass Calculation:** Itemized breakdown of all hardware components.
- **Firmware Logic:** Iterative C++ algorithm with 5-sample filter, timeout handling, and hysteresis thresholds ($9\text{ cm}$ ON / $3\text{ cm}$ OFF).

---

## 📌 Chapter 5: Results and Discussion
- **Sloshing Mitigation:** Significant reduction of surface wave amplitude inside the Stilling-Well chamber.
- **Sensor Measurement Stability:** High reading precision with residual noise reduced to under $\pm 5\text{ mm}$.
- **Hysteresis Performance:** Complete elimination of rapid relay toggling (*chattering*), protecting pump electrical contacts.
- **Mass Impact Evaluation:** System adds only $2.88\text{ kg}$, representing just $0.79\%$ of the $360\text{ kg}$ total vessel displacement.

---

## 📌 Chapter 6: Conclusion and Future Work
- **Conclusion:** The hybrid mechanical/electronic system demonstrates high reliability, low power consumption, and minimal weight impact for solar racing boats.
- **Future Work Recommendations:**
  1. Add IoT telemetry (Wi-Fi/Bluetooth/LoRa) using ESP32 wireless capabilities.
  2. Upgrade to IP67 waterproof ultrasonic sensor (e.g., JSN-SR04T / A02YYUW).
  3. Integrate with cloud monitoring platforms (ThingsBoard / Grafana dashboard).

# 📐 Technical Calculations and Mathematical Modeling

This document presents the complete mathematical equations, hydrodynamic dimensioning, embarked mass calculations, and control parameters from the thesis.

---

## 1. Time-of-Flight (ToF) Distance & Water Level Conversion

The HC-SR04 ultrasonic sensor emits a $40\text{ kHz}$ acoustic burst and measures the round-trip echo time $\Delta t$ in microseconds ($\mu\text{s}$).

$$\text{Distance (cm)} = \frac{\Delta t \times v_{\text{sound}}}{2}$$

Where:
- $v_{\text{sound}} = 0.0343\text{ cm/\mu s}$ (Speed of sound in dry air at $20^\circ\text{C}$).
- The division by 2 accounts for the out-and-back wave path.

The water level $h_{\text{water}}$ measured from the bottom of the Stilling-Well of height $H_{\text{well}} = 30.0\text{ cm}$ is:

$$h_{\text{water}} = H_{\text{well}} - \text{Distance (cm)}$$

---

## 2. Hysteresis Control Band

To prevent relay contact wear and high-frequency pump toggling (*chattering*), a dual-threshold hysteresis band $\Delta H$ is established:

- **Pump Turn-ON Threshold ($h_{\text{on}}$):** $\ge 9.0\text{ cm}$
- **Pump Turn-OFF Threshold ($h_{\text{off}}$):** $\le 3.0\text{ cm}$
- **Hysteresis Band Width ($\Delta H$):**

$$\Delta H = h_{\text{on}} - h_{\text{off}} = 9.0\text{ cm} - 3.0\text{ cm} = 6.0\text{ cm}$$

---

## 3. Stilling-Well Hydrodynamic Dimensioning

- **Outer Diameter ($D$):** $100\text{ mm}$ ($0.10\text{ m}$)
- **Height ($H$):** $300\text{ mm}$ ($0.30\text{ m}$)
- **PVC Material Volume ($V$):** $0.0000873\text{ m}^3$
- **Stilling-Well Mass ($m_{\text{sw}}$):**

$$m_{\text{sw}} = V \times \rho_{\text{pvc}} = 0.0000873\text{ m}^3 \times 1400\text{ kg/m}^3 = 0.12222\text{ kg}$$

---

## 4. Anti-Sloshing Baffles Dimensioning

- **Plate Dimensions:** $1000\text{ mm} \times 200\text{ mm} \times 4\text{ mm}$ (2 units)
- **Total PVC Volume ($V_{\text{baffles}}$):** $0.00158865\text{ m}^3$
- **Baffles Mass ($m_{\text{baffles}}$):**

$$m_{\text{baffles}} = V_{\text{baffles}} \times \rho_{\text{pvc}} = 0.00158865\text{ m}^3 \times 1400\text{ kg/m}^3 = 2.22411\text{ kg}$$

---

## 5. Embarked Mass Itemization & Displacement Impact

| Component | Mass / Apparent Weight (kg / kgf) |
| :--- | :---: |
| Stilling-Well Pipe | $0.12222$ |
| Baffles (2 plates) | $2.22411$ |
| Seaflo 500 GPH Bilge Pump | $0.37000$ |
| HC-SR04 Sensor | $0.00300$ |
| ESP32 Microcontroller | $0.01000$ |
| Wiring & Connectors | $0.05000$ |
| Strainer Base | $0.05000$ |
| Relay Module | $0.01500$ |
| Mechanical Float Switch | $0.02500$ |
| **Total Embarked Mass ($m_{\text{total}}$)** | **$2.87933\text{ kg}$ ($\approx 2.88\text{ kg}$)** |

### Vessel Displacement Impact Percentage
- **Solar Vessel Nominal Displacement ($D_{\text{boat}}$):** $360\text{ kg}$
- **Mass Increase Percentage ($\Delta m\%$):**

$$\Delta m\% = \left( \frac{m_{\text{total}}}{D_{\text{boat}}} \right) \times 100 = \left( \frac{2.87933}{360} \right) \times 100 = 0.7998\% \approx 0.80\%$$

**Conclusion:** The complete system adds less than **$0.80\%$** to the vessel total weight, maintaining full compliance with solar boat racing performance constraints.

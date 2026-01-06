# 🛡️ Guardian Tracker – BLE 5.0 Child Safety System

A **Bluetooth Low Energy (BLE 5.0)** based wearable system designed to help parents
**locate and guide their children in crowded environments** such as festivals,
markets, and public gatherings — where GPS fails.

## 📌 Problem Statement
Child separation and abduction risks increase significantly in crowded public spaces.
Existing GPS-based trackers suffer from **poor indoor accuracy, high power
consumption, and network dependency**.

## 💡 Solution Overview
Guardian Tracker uses **BLE 5.0 proximity and directional tracking** with intuitive
visual and haptic feedback to enable **fast parent–child reunification** without GPS.

The system consists of:
- 👶 **Child Device**
- 👨‍👩‍👧 **Parent Device**

Both communicate using BLE 5.0 and provide real-time distance and direction cues.

## 🧩 Key Features

### Child Device
- BLE 5.0 communication (ESP32)
- 🔊 Buzzer for public alert when out of range
- 🔘 Distress button
- 💡 8 directional LEDs to guide toward parent
- 🌈 RGB LED for proximity indication

### Parent Device
- BLE 5.0 scanner
- 💡 8 directional LEDs to locate child
- 🌈 RGB LED for distance trend
- 📳 Vibration alerts for multiple distance thresholds
- 📱 Mobile app (prototype)

## 🧭 Direction Identification Method
- Direction is inferred using **RSSI-based comparison**
- Multiple RSSI samples are taken and filtered
- Strongest RSSI direction lights the corresponding LED
- Calibrated over time to reduce noise in crowded environments

> This approach works indoors and outdoors without GPS.

## ⚙️ Technology Stack
- **ESP32 (BLE 5.0)**
- RSSI-based proximity & direction estimation
- Embedded C firmware
- Mobile app (prototype)
- LED + buzzer + vibration feedback

## 🧪 Testing & Validation
- Real-world crowded environment testing
- RSSI calibration over multiple days
- Emergency scenario simulation
- Signal interference handling

## 🚀 Advantages Over GPS Trackers
- Works **indoors & crowded spaces**
- Very **low power**
- No mobile network dependency
- Instant visual & haptic guidance
- Cost-effective and scalable

## 📄 Documentation
- Full project report available in [`docs/`](docs/Guardian_Tracker_Report.pdf)

## 🎓 Academic Context
Developed as part of **22ES390 – Design Thinking**  
Department of Electronics and Communication Engineering  
Thiagarajar College of Engineering, Madurai  
(Dec 2024)

## 🔮 Future Improvements
- BLE 5.1 Angle of Arrival (AoA)
- Smaller wearable PCB
- Battery optimization
- Law enforcement alert integration

---

⭐ If you find this project useful, consider starring the repo!

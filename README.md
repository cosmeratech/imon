# IMON - interrupting overheating before it fries your circuits

# IMON – Industrial Monitor

**IMON** is an **interrupt-driven, real-time industrial safety monitoring system**.  
It keeps your machines and operators safe by detecting **overheating, unsafe proximity, and vibrations**, reacting instantly without blocking delays.

---

## Features

- **Real-time monitoring** using interrupts and hardware timers  
- Detects **machine overheating**  
- Detects **vibration / instability** in equipment  
- Detects **unsafe human proximity**  
- **Operator acknowledgment** via button press  
- **Warning LED and status LED** indicators  
- **Non-blocking architecture** — no delay() used  
- Logs sensor data to **Serial Monitor** for debugging  

---

## 🛠 Hardware Setup

| Component               | Pin        | Notes                               |
|-------------------------|------------|-------------------------------------|
| Operator Button         | D8         | Input to acknowledge warnings       |
| Warning LED             | D13        | Lights up on critical or warning    |
| Status LED              | D12        | Lights up when system is OK         |
| Ultrasonic Sensor TRIG  | D6         | Measures proximity                  |
| Ultrasonic Sensor ECHO  | D9         | Captures echo                        |
| Temperature Sensor (LM35) | A0       | Measures machine temperature        |
| Vibration Sensor        | A1         | Detects mechanical vibrations      |

---

## Software Architecture

- **Timer1 ISR**: triggers every 500ms for periodic checks  
- **Pin Change ISR**: handles button press and ultrasonic echo measurement instantly  
- **Main Loop**: evaluates sensor data and updates LEDs/logs  

### Safety Logic

1. **Critical Fault:** Temperature > TEMP_LIMIT or Vibration > VIBRATION_LIMIT → Warning LED ON  
2. **Warning State:** Proximity < PROXIMITY_LIMIT and button not pressed → Warning LED ON  
3. **Safe:** All conditions normal → Status LED ON  

---

## Thresholds

```cpp
const int TEMP_LIMIT = 65;          // °C
const int PROXIMITY_LIMIT = 50;     // cm
const int VIBRATION_LIMIT = 600;    // analog units

# IoT-Farming-System

A real-time smart agriculture monitoring system that tracks soil moisture, temperature, and humidity with an ESP32 and syncs the readings to a Firebase Realtime Database, which is visualized on a modern, responsive web dashboard. An optional 5V relay module automates a water pump based on soil moisture levels.

## Features

- **Real-Time Monitoring:** Live updates of soil and environmental conditions without refreshing the page.
- **Glassmorphism Dashboard:** A sleek, futuristic UI with progress bars and live sensor values.
- **Cloud Integration:** Uses Firebase Realtime Database for seamless data synchronization between hardware and the web.
- **Automated Irrigation:** The ESP32 turns the water pump ON when soil moisture drops to 30% and OFF when it reaches 45% (hysteresis to avoid rapid switching).
- **Enhanced Firmware (enhanced by students):**
  - Automatic WiFi reconnection with retry intervals.
  - Averaged soil readings for stable percentages.
  - NTP timestamp in every payload (enables historical analysis).
  - Raw ADC reading + device id + pump state in the payload.
  - DHT11 read retries and non-blocking reporting.

## Hardware Components

- **Microcontroller:** ESP32 (WROOM-32)
- **Soil Moisture Sensor:** Analog capacitive soil probe
- **DHT11 Sensor:** For temperature and humidity tracking
- **Relay Module:** 5V relay (drives the 12V DC water pump) — optional but recommended for automation
- **Water Pump:** 12V DC pump (when the relay is enabled)
- **Connecting Wires & Power Supply**

## Circuit Connection

- **Soil Moisture Sensor:** Connected to **Pin 32** (Analog)
- **DHT11 Sensor:** Connected to **Pin 4** (Digital)
- **Pump Relay Module:** Connected to **Pin 25** (Digital output)
- **VCC/GND:** Connected to the respective 3.3V and Ground pins on the ESP32

## Software Stack

- **Firmware:** C++ (Arduino IDE) — `firmware/IoT_Farming_System.ino`
- **Frontend:** HTML5, CSS3 (glassmorphism), JavaScript (ES6+) — `index.html`
- **Database:** Firebase Realtime Database
- **Libraries:**
  - `WiFi.h` & `HTTPClient.h` for ESP32 connectivity
  - `SimpleDHT` for DHT11 data processing (by pointedbits)
  - `Firebase JS SDK` (via REST calls in the dashboard)

## Setup Instructions

### 1. Firebase Configuration

1. Create a project in the [Firebase Console](https://console.firebase.google.com/).
2. Create a **Realtime Database** (in the region closest to you, or update the host in the code to `asia-southeast1`).
3. Set the database rules to allow reading/writing while testing:

```json
{
  "rules": {
    ".read": true,
    ".write": true
  }
}
```

4. Copy your `FIREBASE_HOST` (database URL). For the dashboard, the URL is the same plus `/sensor.json`.

### 2. ESP32 Firmware

1. Install the ESP32 boards in the Arduino IDE (` Boards Manager: esp32`).
2. Open `firmware/IoT_Farming_System.ino` in the Arduino IDE.
3. Update the following variables at the top of the file:
   - `WIFI_SSID`: Your WiFi name
   - `WIFI_PASSWORD`: Your WiFi password
   - `FIREBASE_HOST`: Your database URL
4. Upload the code to your ESP32 (press the EN button to reset after upload).

Notes:
- The firmware pushes to the `sensor.json` node — the dashboard reads the same node.
- Automated pump logic is enabled by default (`AUTO_PUMP = true`); set it to `false` to run without the relay.
- Tune `MOISTURE_DRY_THRESHOLD` (30) and `MOISTURE_WET_THRESHOLD` (45) in `firmware/IoT_Farming_System.ino` to match your soil.

### 3. Web Dashboard

1. Open `index.html` in any modern browser.
2. If you use a different Firebase host, update the `DB_URL` variable in the `<script>` section of `index.html`.
3. The dashboard polls the database every 2 seconds and shows moisture, temperature, humidity, pump status and a moisture bar with alerts.

## Calibration

The firmware is pre-configured with the following calibration values for the capacitive soil sensor:

- **Wet Value:** 800 — raw reading at 100% moisture
- **Dry Value:** 3500 — raw reading at 0% moisture

*Note: Calibrate these in the code after testing in your soil — they vary with sensor and soil type.*

## Project Structure

```
IoT-Farming-System-main/
├── firmware/
│   └── IoT_Farming_System.ino   # Enhanced ESP32 firmware
├── index.html                   # Live dashboard (glassmorphism)
├── README.md
└── info.txt
```

## Development Workflow (GitHub)

This repository is deployed on GitHub:

```
git clone https://github.com/<username>/IoT-Farming-System.git
```

## Team / Curriculum

**Developed as part of the B.Tech Computer Science Engineering curriculum.**

- Krishnav Barman (202400252) — Project Lead
- Samarjeet (202400343) — Firmware / Automation
- Aayush Bora (202400101)
- Shourya Sharma (202400705)

Under the guidance of Dr. Samarendra Nath Sur, Associate Professor, Department of Computer Science & Engineering, Sikkim Manipal Institute of Technology.

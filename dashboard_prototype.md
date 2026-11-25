# Dashboard Prototype — IoT Smart Irrigation Dashboard
## Layout

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ 🌱 IoT Smart Irrigation Dashboard                [ 🟢 System Live (pulsing) ]│
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌───────────────────────────────┐  ┌──────────────────────────┐          │
│   │        Moisture Gauge         │  │  Temperature             │          │
│   │   (ring + progress bar)       │  │  progress bar 0–40 °C   │          │
│   │   47% — "moisture"            │  │  24 °C                  │          │
│   │   badge: OK — in range        │  └──────────────────────────┘          │
│   └───────────────────────────────┘  ┌──────────────────────────┐          │
│                                      │  Humidity                │          │
│   ┌───────────────────────────────┐  │  progress bar 0–100 %   │          │
│   │  Pump Status                  │  │  61 %                   │          │
│   │  ● ON (pulsing dot)           │  └──────────────────────────┘          │
│   │                               │  ┌──────────────────────────┐          │
│   │  Last Update                  │  │  2026-11-26 12:34:10     │          │
│   └───────────────────────────────┘  └──────────────────────────┘          │
│                                                                             │
├─────────────────────────────────────────────────────────────────────────────┤
│ Recent readings                                                             │
│ ┌────────────┬────────┬───────────────────────┬───────────────┐             │
│ │ Raw ADC    │ Device │ Timestamp             │ Status alert  │             │
│ └────────────┴────────┴───────────────────────┴───────────────┘             │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Details
- **Moisture gauge:** animated SVG ring (donut) with gradient stroke around the current value (e.g. 47%); center shows the percentage; badge below says "DRY — pump ON" / "OK — in range" / "WET — optimal".
- **Thresholds** (kept in sync with `firmware/IoT_Farming_System.ino`): DRY ≤ 30, WET ≥ 45.
- **Bars:** temperature scaled 0–40 °C, humidity 0–100%.
- **Pump status:** live ON/OFF chip with a pulsing dot.
- **Stats table:** raw ADC value, device id, timestamp, status alert text.


# Automatic Watering System for My Plants (Arduino Uno)

Final Arduino Uno project for automatic plant watering with soil sensing, relay-controlled pump, and LED status.

## Folder Structure

- `src/automatic_watering_system.ino` - Arduino sketch
- `docs/circuit.md` - wiring reference
- `images/` - image placeholders for build proof/screenshots
- `README.md` - setup, run, calibration, troubleshooting

## Hardware Mapping

- Soil Moisture Sensor: `AO -> A0`, `VCC -> 5V`, `GND -> GND`
- Relay Module: `IN -> D7`, `VCC -> 5V`, `GND -> GND`
- LED: `D8 -> 220Ω -> LED -> GND`
- Pump: through relay `COM/NO`, powered by external supply

## How to Run

1. Open Arduino IDE.
2. Open `src/automatic_watering_system.ino`.
3. Select **Board: Arduino Uno**.
4. Select the correct COM port.
5. Upload sketch.
6. Open Serial Monitor at **9600 baud**.

## Calibration (dryValue / wetValue)

1. Keep sensor in dry soil/air and note Serial raw value.
2. Put sensor in wet soil/water and note Serial raw value.
3. Update in code:
   - `const int dryValue = ...;`
   - `const int wetValue = ...;`
4. Re-upload and tune thresholds:
   - `DRY_THRESHOLD_PERCENT`
   - `WET_THRESHOLD_PERCENT`

## Safety Logic Implemented

- Moving-average filtering to reduce noisy readings.
- Minimum pump ON runtime: **5 seconds**.
- Pump restart cooldown after OFF: **30 seconds**.
- Hysteresis via dry/wet thresholds to prevent relay chatter.

## Serial Output

Serial prints each cycle:
- raw/averaged moisture reading
- moisture percentage
- relay status
- pump status

Action logs are also printed when pump state changes.

## Troubleshooting

- **Pump never turns ON:**
  - Lower `DRY_THRESHOLD_PERCENT` or calibrate `dryValue/wetValue`.
  - Verify relay input on D7 and relay active-low/high setting.
- **Pump always ON:**
  - Increase dry threshold, verify wet calibration, and check sensor insertion depth.
- **Random toggling:**
  - Confirm common ground and stable power supply.
  - Keep sensor wires short and secure.
- **Relay LED changes but pump doesn't run:**
  - Recheck COM/NO wiring and external pump power polarity.

## Images

Place photos/screenshots in `images/` using placeholder names from `images/README.md`.

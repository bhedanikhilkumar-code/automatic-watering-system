# Automatic Watering System for My Plants (Arduino Uno)

This project automatically waters plants using a soil moisture sensor, relay-controlled pump, and LED indicator.

## Project Structure

- `automatic_watering_system.ino` → main Arduino sketch
- `README.md` → setup, wiring, calibration, and run guide
- `images/` → placeholder folder for build/wiring photos

## Hardware Required

- Arduino Uno
- Soil Moisture Sensor module (analog output)
- 1-channel relay module (5V)
- LED
- 220Ω resistor
- DC water pump
- External power supply for pump
- Jumper wires + breadboard

## Wiring (Requested Mapping)

### Soil Moisture Sensor
- `AO` → `A0`
- `VCC` → `5V`
- `GND` → `GND`

### Relay Module
- `IN` → `D7`
- `VCC` → `5V`
- `GND` → `GND`

### LED
- `D8` → `220Ω resistor` → LED anode (+)
- LED cathode (-) → `GND`

### Pump via Relay Contacts
- External supply `+` → Relay `COM`
- Relay `NO` → Pump `+`
- Pump `-` → External supply `-`
- Keep a common ground reference with Arduino GND when required by your relay/supply setup.

> Important: Do **not** power the pump directly from Arduino 5V.

## Control Logic

- Sensor readings are smoothed using a moving average.
- Moisture percentage is calculated from calibrated `dryValue` and `wetValue`.
- If moisture is **DRY** (below dry threshold): relay ON, pump ON, LED ON.
- If moisture is **WET** (above wet threshold): relay OFF, pump OFF, LED OFF.

### Protection Timing

- **Minimum pump runtime:** 5 seconds
- **Cooldown after pump stops:** 30 seconds

This prevents rapid ON/OFF switching and extends pump/relay life.

## Calibration Steps (Important)

1. Upload sketch and open Serial Monitor at **9600 baud**.
2. Note raw value when probe is in **dry soil/air** → set this as `dryValue`.
3. Note raw value when probe is in **wet soil/water** → set this as `wetValue`.
4. Update thresholds as needed:
   - `DRY_THRESHOLD_PERCENT`
   - `WET_THRESHOLD_PERCENT`
5. Re-upload sketch and verify pump behavior.

## How to Run

1. Open Arduino IDE.
2. Load `automatic_watering_system.ino`.
3. Select board: **Arduino Uno**.
4. Select the correct COM/serial port.
5. Upload sketch.
6. Open Serial Monitor at **9600 baud**.
7. Confirm messages show raw value, moisture percentage, and pump status.

## Images

Add photos to the `images/` folder (for example: wiring photo, relay+pump photo, serial monitor screenshot).

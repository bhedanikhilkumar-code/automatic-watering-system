# Automatic Watering System for My Plants (Arduino Uno)

A complete Arduino Uno project that waters plants automatically when soil is dry.

## Project Structure

- `automatic_watering_system.ino` → main Arduino sketch (ready to upload)
- `README.md` → setup, wiring, and run instructions
- `images/` → placeholder folder for your wiring/build photos
  - `images/README.md` explains what photos to add

## Hardware Required

- Arduino Uno
- Soil Moisture Sensor module (analog output)
- 1-channel Relay Module (5V)
- LED
- 220Ω resistor (for LED)
- Mini DC water pump
- External power supply for pump (voltage/current as per pump)
- Jumper wires + breadboard

## Wiring (Requested Mapping)

### 1) Soil Moisture Sensor
- `AO` → `A0`
- `VCC` → `5V`
- `GND` → `GND`

### 2) Relay Module
- `IN` → `D7`
- `VCC` → `5V`
- `GND` → `GND`

### 3) LED
- `D8` → `220Ω resistor` → `LED anode (+)`
- `LED cathode (-)` → `GND`

### 4) Pump through Relay
- External supply `+` → Relay `COM`
- Relay `NO` → Pump `+`
- Pump `-` → External supply `-`
- Share ground reference when needed: external supply `-`/GND with Arduino GND

> Note: Do not power the pump directly from Arduino 5V pin.

## How the Code Works

- Reads moisture sensor from `A0`.
- Applies a moving average (noise/debounce effect).
- Converts averaged raw value to moisture percentage (0–100%).
- Uses dry/wet thresholds:
  - **DRY**: relay ON, pump ON, LED ON
  - **WET**: relay OFF, pump OFF, LED OFF
- Adds safety timing:
  - minimum pump run time (default 4 seconds)
  - cooldown time before restart (default 4 seconds)
- Prints raw value, averaged value, moisture %, and status to Serial Monitor.

## Calibration Notes

Open `automatic_watering_system.ino` and tune:

- `SENSOR_DRY_VALUE` (typical higher analog value)
- `SENSOR_WET_VALUE` (typical lower analog value)
- `DRY_THRESHOLD_PERCENT`
- `WET_THRESHOLD_PERCENT`

Simple calibration:
1. Read sensor in dry soil/air and note value.
2. Read sensor in wet soil/water and note value.
3. Update constants and upload again.

## How to Run

1. Open Arduino IDE.
2. Open `automatic_watering_system.ino`.
3. Select **Tools → Board → Arduino Uno**.
4. Select correct serial/COM port.
5. Upload sketch.
6. Open **Serial Monitor** at **9600 baud**.
7. Test by changing soil moisture around the probe and observe relay/LED/pump behavior.

## Add Photos

Put your build photos in `images/` (see `images/README.md`), then reference them in this README, for example:

```md
![Wiring](images/wiring_top_view.jpg)
```
# Automatic Watering System (Arduino Uno)

This project automatically waters a plant when the soil becomes dry.
It uses an **Arduino Uno**, a **soil moisture sensor (A0)**, a **relay module (D7)**, an **LED indicator (D8)**, and a **DC water pump controlled through the relay**.

## Project Files

- `automatic_watering_system.ino` — complete Arduino sketch (ready to upload)
- `README.md` — wiring, working principle, setup, and calibration notes

## Components Required

- Arduino Uno
- Soil moisture sensor module (analog output)
- 1-channel relay module (5V)
- LED + 220Ω resistor
- Mini DC water pump
- External power supply for pump (recommended)
- Jumper wires and breadboard

## Pin Mapping (as requested)

- Soil Moisture Sensor analog output → `A0`
- Relay control input (`IN`) → `D7`
- LED anode (through 220Ω resistor) → `D8`
- LED cathode → `GND`

## Simple Circuit Explanation

1. **Soil Moisture Sensor**
   - `VCC` → Arduino `5V`
   - `GND` → Arduino `GND`
   - `AO` → Arduino `A0`

2. **Relay Module (control side)**
   - `VCC` → Arduino `5V`
   - `GND` → Arduino `GND`
   - `IN` → Arduino `D7`

3. **LED Indicator**
   - Arduino `D8` → 220Ω resistor → LED anode (+)
   - LED cathode (−) → Arduino `GND`

4. **Pump wiring through relay contacts**
   - External supply `+` → relay `COM`
   - relay `NO` → pump `+`
   - pump `−` → external supply `−`
   - **Important:** connect external supply `GND` to Arduino `GND` for a common reference.

> ⚠️ Do **not** power most pumps directly from Arduino 5V pin. Use a suitable external supply.

## How It Works

- The Arduino reads soil moisture from `A0`.
- The code converts raw analog reading to a moisture percentage.
- If moisture is below the dry threshold, relay turns ON and pump starts.
- If moisture rises above the wet threshold, relay turns OFF and pump stops.
- LED on `D8` mirrors pump status:
  - **ON** = pump ON
  - **OFF** = pump OFF

The sketch uses:
- **Hysteresis** (separate dry/wet thresholds) to prevent relay chatter.
- **Minimum ON/OFF times** to avoid fast switching and protect relay/pump.

## Upload Steps

1. Open Arduino IDE.
2. Open `automatic_watering_system.ino`.
3. Select **Board: Arduino Uno**.
4. Select the correct COM port.
5. Upload the sketch.
6. Open Serial Monitor at `9600` baud to observe sensor and pump state.

## Calibration Tips

In `automatic_watering_system.ino`, tune these constants for your sensor:

- `SENSOR_DRY_VALUE` (reading in dry condition)
- `SENSOR_WET_VALUE` (reading in wet condition)
- `DRY_THRESHOLD_PERCENT`
- `WET_THRESHOLD_PERCENT`

Typical method:
1. Note analog value in dry air/soil.
2. Note analog value in water/saturated soil.
3. Update constants and re-upload.

## Safety Notes

- Keep water away from Arduino/USB connections.
- Use an external power source sized for the pump current.
- Ensure relay contacts and wiring are secure.
- If your relay module is active-high instead of active-low, set `RELAY_ACTIVE_LOW` to `false` in code.

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

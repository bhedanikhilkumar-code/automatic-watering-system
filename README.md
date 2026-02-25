# Automatic Watering System for My Plants (Arduino Uno)

Improved Arduino Uno project for automatic plant watering with soil sensing, relay-controlled pump, and LED status.

## Folder Structure

- `src/automatic_watering_system.ino` - Arduino sketch (main logic)
- `docs/circuit.md` - wiring reference
- `images/` - image placeholders for build proof/screenshots
- `README.md` - setup, calibration, run, troubleshooting

## Hardware Mapping

- Soil Moisture Sensor: `AO -> A0`, `VCC -> 5V`, `GND -> GND`
- Relay Module: `IN -> D7`, `VCC -> 5V`, `GND -> GND`
- LED: `D8 -> 220Ω -> LED -> GND`
- Pump: through relay `COM/NO`, powered by external supply

## Control Logic (What improved)

- Moving-average filtering (`AVERAGE_SAMPLES`) for stable readings.
- Moisture percentage mapping from calibrated `dryValue` / `wetValue`.
- Hysteresis thresholds:
  - `DRY_THRESHOLD_PERCENT` -> start pump
  - `WET_THRESHOLD_PERCENT` -> stop pump
- Safety timers:
  - minimum pump ON time = **5 seconds**
  - cooldown after OFF = **30 seconds**
- Boot behavior improved: cooldown is pre-cleared once so watering can start immediately if soil is already dry.
- Calibration validation at startup (warns if `dryValue <= wetValue`).

## How to Run

1. Open Arduino IDE.
2. Open `src/automatic_watering_system.ino`.
3. Select **Board: Arduino Uno**.
4. Select the correct COM port.
5. Upload sketch.
6. Open Serial Monitor at **9600 baud**.

## Calibration (dryValue / wetValue)

1. Keep probe in dry soil/air and note displayed raw value.
2. Put probe in wet soil/water and note displayed raw value.
3. Update these in code:
   - `int dryValue = ...;`
   - `int wetValue = ...;`
4. Ensure `dryValue` is **greater than** `wetValue`.
5. Re-upload and tune:
   - `DRY_THRESHOLD_PERCENT`
   - `WET_THRESHOLD_PERCENT`

## Serial Monitor Output

Each cycle prints:
- `Raw=<value>` (smoothed average raw reading)
- `Moisture=<percent>%`
- `Relay=ON/OFF`
- `Pump=ON/OFF`

State transitions also print action lines, e.g.:
- `ACTION: Soil DRY -> Relay ON | Pump ON | LED ON`
- `ACTION: Soil WET enough -> Relay OFF | Pump OFF | LED OFF`

## Troubleshooting

- **No watering even when dry**
  - Recalibrate `dryValue/wetValue` and verify `dryValue > wetValue`.
  - Increase `DRY_THRESHOLD_PERCENT`.
  - Confirm relay signal wiring on D7.
- **Pump turns on/off too often**
  - Check stable sensor wiring and common ground.
  - Increase sample count (`AVERAGE_SAMPLES`) or widen threshold gap.
- **Relay clicks but pump not running**
  - Verify COM/NO wiring and external pump supply.
  - Check pump polarity and supply current capacity.
- **Pump always ON**
  - Lower `DRY_THRESHOLD_PERCENT` or increase `WET_THRESHOLD_PERCENT` after calibration.

## Images

Add photos/screenshots in `images/` using placeholder names from `images/README.md`.

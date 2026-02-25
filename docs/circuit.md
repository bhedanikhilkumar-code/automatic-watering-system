# Circuit Wiring (Arduino Uno)

## 1) Soil Moisture Sensor
- AO -> A0
- VCC -> 5V
- GND -> GND

## 2) Relay Module
- IN -> D7
- VCC -> 5V
- GND -> GND

## 3) LED Indicator
- D8 -> 220Ω resistor -> LED anode (+)
- LED cathode (-) -> GND

## 4) Pump via Relay Contacts
- External power + -> Relay COM
- Relay NO -> Pump +
- Pump - -> External power -

Use an external power source sized for the pump. Do not drive pump current from Arduino 5V.

## Notes
- If your relay is active-HIGH, set `RELAY_ACTIVE_LOW` to `false` in code.
- Keep wiring dry and secure.

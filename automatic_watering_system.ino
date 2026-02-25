/*
  Automatic Watering System
  Board: Arduino Uno

  Hardware:
  - Soil moisture sensor analog output -> A0
  - Relay module IN pin -> D7
  - LED (+ resistor) -> D8
  - Water pump powered through relay COM/NO contacts

  Behavior:
  - Reads soil moisture and maps it to a 0-100% moisture value.
  - Turns pump ON when moisture drops below DRY_THRESHOLD_PERCENT.
  - Turns pump OFF when moisture rises above WET_THRESHOLD_PERCENT.
  - Uses hysteresis + minimum on/off times to avoid rapid relay toggling.
*/

// ------------------------- Pin configuration -------------------------
const uint8_t SOIL_SENSOR_PIN = A0;
const uint8_t RELAY_PIN = 7;
const uint8_t LED_PIN = 8;

// ---------------------- Relay electrical behavior --------------------
// Many relay modules are ACTIVE LOW (LOW = relay ON).
// Set to true for active-low modules, false for active-high modules.
const bool RELAY_ACTIVE_LOW = true;

// ------------------------ Sensor calibration -------------------------
// Update these values after checking your sensor in dry and wet soil.
// - SENSOR_DRY_VALUE: analogRead value in dry air/soil
// - SENSOR_WET_VALUE: analogRead value in water/saturated soil
const int SENSOR_DRY_VALUE = 850;
const int SENSOR_WET_VALUE = 400;

// ---------------------- Watering control setup -----------------------
// Hysteresis thresholds (in moisture %):
// pump starts if moisture <= DRY_THRESHOLD_PERCENT
// pump stops if moisture >= WET_THRESHOLD_PERCENT
const int DRY_THRESHOLD_PERCENT = 35;
const int WET_THRESHOLD_PERCENT = 50;

// Debounce-like timing to protect relay/pump from rapid switching.
const unsigned long MIN_PUMP_ON_TIME_MS = 5000;   // keep on at least 5 seconds
const unsigned long MIN_PUMP_OFF_TIME_MS = 5000;  // keep off at least 5 seconds

// Sensor polling interval.
const unsigned long SAMPLE_INTERVAL_MS = 1000;

bool pumpOn = false;
unsigned long lastSampleTime = 0;
unsigned long lastPumpStateChangeTime = 0;

// Helper: map raw analog sensor reading to percentage.
// 0% = very dry, 100% = very wet.
int moisturePercentFromRaw(int raw) {
  // Constrain to calibration limits to avoid out-of-range percentages.
  raw = constrain(raw, SENSOR_WET_VALUE, SENSOR_DRY_VALUE);

  // Reverse map because many sensors read higher in dry soil.
  long percent = map(raw, SENSOR_DRY_VALUE, SENSOR_WET_VALUE, 0, 100);
  return (int)constrain(percent, 0, 100);
}

void setPump(bool turnOn) {
  pumpOn = turnOn;
  lastPumpStateChangeTime = millis();

  // Drive relay respecting module logic.
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(RELAY_PIN, turnOn ? LOW : HIGH);
  } else {
    digitalWrite(RELAY_PIN, turnOn ? HIGH : LOW);
  }

  // LED mirrors pump state.
  digitalWrite(LED_PIN, turnOn ? HIGH : LOW);
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SOIL_SENSOR_PIN, INPUT);

  // Ensure pump starts OFF for safety.
  setPump(false);

  Serial.begin(9600);
  Serial.println(F("Automatic Watering System started."));
  Serial.println(F("Calibrate SENSOR_DRY_VALUE/SENSOR_WET_VALUE for best results."));
}

void loop() {
  unsigned long now = millis();

  // Run the control loop at a fixed interval.
  if (now - lastSampleTime < SAMPLE_INTERVAL_MS) {
    return;
  }
  lastSampleTime = now;

  int raw = analogRead(SOIL_SENSOR_PIN);
  int moisturePercent = moisturePercentFromRaw(raw);

  // Serial monitoring output.
  Serial.print(F("Raw: "));
  Serial.print(raw);
  Serial.print(F(" | Moisture: "));
  Serial.print(moisturePercent);
  Serial.print(F("% | Pump: "));
  Serial.println(pumpOn ? F("ON") : F("OFF"));

  // Respect minimum on/off durations before changing state.
  unsigned long elapsedSinceChange = now - lastPumpStateChangeTime;

  if (!pumpOn) {
    bool canTurnOn = elapsedSinceChange >= MIN_PUMP_OFF_TIME_MS;
    if (canTurnOn && moisturePercent <= DRY_THRESHOLD_PERCENT) {
      setPump(true);
      Serial.println(F("Soil dry -> Pump ON"));
    }
  } else {
    bool canTurnOff = elapsedSinceChange >= MIN_PUMP_ON_TIME_MS;
    if (canTurnOff && moisturePercent >= WET_THRESHOLD_PERCENT) {
      setPump(false);
      Serial.println(F("Soil wet enough -> Pump OFF"));
    }
  }
}

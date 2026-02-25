/*
  Automatic Watering System for My Plants
  Board: Arduino Uno

  Hardware Mapping:
  - Soil Moisture Sensor: AO -> A0, VCC -> 5V, GND -> GND
  - Relay Module: IN -> D7, VCC -> 5V, GND -> GND
  - LED: D8 -> 220 ohm resistor -> LED -> GND
  - Pump: through relay COM/NO using external power supply

  Behavior:
  - Soil DRY  -> relay ON -> pump ON -> LED ON
  - Soil WET  -> relay OFF -> pump OFF -> LED OFF
  - Includes averaging, minimum pump runtime, and cooldown to avoid rapid toggling
*/

// ----------------------------- Pins -----------------------------
const uint8_t SOIL_SENSOR_PIN = A0;
const uint8_t RELAY_PIN = 7;
const uint8_t LED_PIN = 8;

// Most relay boards are active LOW (LOW = ON). Change if needed.
const bool RELAY_ACTIVE_LOW = true;

// ---------------------- Calibration support ---------------------
// Set these two values after calibration (steps in README/docs):
// dryValue = raw analog value in dry soil/air
// wetValue = raw analog value in wet soil/water
const int dryValue = 850;
const int wetValue = 420;

// Thresholds in moisture percentage.
const int DRY_THRESHOLD_PERCENT = 35;  // start watering at/below
const int WET_THRESHOLD_PERCENT = 55;  // stop watering at/above

// -------------------------- Safety timing -----------------------
const unsigned long SAMPLE_INTERVAL_MS = 500;
const unsigned long MIN_PUMP_RUN_MS = 5000;     // 5 seconds minimum ON time
const unsigned long PUMP_COOLDOWN_MS = 30000;   // 30 seconds minimum OFF time

// ------------------------- Averaging setup ----------------------
const uint8_t AVERAGE_SAMPLES = 10;
int samples[AVERAGE_SAMPLES];
uint8_t sampleIndex = 0;
bool sampleBufferFull = false;

bool pumpOn = false;
unsigned long lastSampleMs = 0;
unsigned long lastPumpChangeMs = 0;

int moisturePercentFromRaw(int rawValue) {
  // Keep value in calibration range before mapping
  rawValue = constrain(rawValue, wetValue, dryValue);

  // Reverse mapping (common for resistive soil sensors)
  long percent = map(rawValue, dryValue, wetValue, 0, 100);
  return (int)constrain(percent, 0, 100);
}

void setRelay(bool turnOn) {
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(RELAY_PIN, turnOn ? LOW : HIGH);
  } else {
    digitalWrite(RELAY_PIN, turnOn ? HIGH : LOW);
  }
}

void setPumpState(bool turnOn) {
  pumpOn = turnOn;
  lastPumpChangeMs = millis();

  setRelay(turnOn);
  digitalWrite(LED_PIN, turnOn ? HIGH : LOW);
}

void addSample(int rawValue) {
  samples[sampleIndex] = rawValue;
  sampleIndex++;

  if (sampleIndex >= AVERAGE_SAMPLES) {
    sampleIndex = 0;
    sampleBufferFull = true;
  }
}

int getAverageRaw() {
  uint8_t count = sampleBufferFull ? AVERAGE_SAMPLES : sampleIndex;
  if (count == 0) {
    return analogRead(SOIL_SENSOR_PIN);
  }

  long total = 0;
  for (uint8_t i = 0; i < count; i++) {
    total += samples[i];
  }
  return (int)(total / count);
}

void printStatus(int rawValue, int moisturePercent) {
  Serial.print(F("Raw="));
  Serial.print(rawValue);
  Serial.print(F(" | Moisture="));
  Serial.print(moisturePercent);
  Serial.print(F("% | Relay="));
  Serial.print(pumpOn ? F("ON") : F("OFF"));
  Serial.print(F(" | Pump="));
  Serial.println(pumpOn ? F("ON") : F("OFF"));
}

void setup() {
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  setPumpState(false);  // safe startup

  Serial.begin(9600);
  Serial.println(F("=== Automatic Watering System (Uno) ==="));
  Serial.println(F("Calibration: set dryValue/wetValue in code."));
  Serial.println(F("Safety: min runtime 5s, cooldown 30s."));

  // Prime average buffer for stable startup readings
  for (uint8_t i = 0; i < AVERAGE_SAMPLES; i++) {
    addSample(analogRead(SOIL_SENSOR_PIN));
    delay(20);
  }
}

void loop() {
  unsigned long now = millis();
  if (now - lastSampleMs < SAMPLE_INTERVAL_MS) {
    return;
  }
  lastSampleMs = now;

  int rawValue = analogRead(SOIL_SENSOR_PIN);
  addSample(rawValue);

  int averageRaw = getAverageRaw();
  int moisturePercent = moisturePercentFromRaw(averageRaw);
  unsigned long elapsed = now - lastPumpChangeMs;

  if (!pumpOn) {
    // Allow start only after cooldown period
    if (elapsed >= PUMP_COOLDOWN_MS && moisturePercent <= DRY_THRESHOLD_PERCENT) {
      setPumpState(true);
      Serial.println(F("ACTION: DRY soil detected -> Relay ON, Pump ON"));
    }
  } else {
    // Allow stop only after minimum run time
    if (elapsed >= MIN_PUMP_RUN_MS && moisturePercent >= WET_THRESHOLD_PERCENT) {
      setPumpState(false);
      Serial.println(F("ACTION: Moisture recovered -> Relay OFF, Pump OFF"));
    }
  }

  printStatus(averageRaw, moisturePercent);
}

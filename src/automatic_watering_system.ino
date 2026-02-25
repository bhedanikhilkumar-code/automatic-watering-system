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
  - Includes averaging, hysteresis, minimum runtime, and cooldown
  - Includes averaging, minimum pump runtime, and cooldown to avoid rapid toggling
*/

// ----------------------------- Pins -----------------------------
const uint8_t SOIL_SENSOR_PIN = A0;
const uint8_t RELAY_PIN = 7;
const uint8_t LED_PIN = 8;

// Most relay modules are active LOW (LOW = relay ON).
const bool RELAY_ACTIVE_LOW = true;

// ---------------------- Calibration support ---------------------
// dryValue: sensor raw value in dry soil/air (typically higher)
// wetValue: sensor raw value in wet soil/water (typically lower)
int dryValue = 850;
int wetValue = 420;

// Moisture thresholds in percentage.
const int DRY_THRESHOLD_PERCENT = 35;  // <= this: start watering
const int WET_THRESHOLD_PERCENT = 55;  // >= this: stop watering

// -------------------------- Safety timing -----------------------
const unsigned long SAMPLE_INTERVAL_MS = 500;
const unsigned long MIN_PUMP_RUN_MS = 5000;      // 5s minimum ON time
const unsigned long PUMP_COOLDOWN_MS = 30000;    // 30s minimum OFF time
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
  // Handle invalid calibration gracefully.
  if (dryValue <= wetValue) {
    return 0;
  }

  rawValue = constrain(rawValue, wetValue, dryValue);
  // Keep value in calibration range before mapping
  rawValue = constrain(rawValue, wetValue, dryValue);

  // Reverse mapping (common for resistive soil sensors)
  long percent = map(rawValue, dryValue, wetValue, 0, 100);
  return (int)constrain(percent, 0, 100);
}

void setRelayOutput(bool turnOn) {
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

  setRelayOutput(turnOn);
  setRelay(turnOn);
  digitalWrite(LED_PIN, turnOn ? HIGH : LOW);
}

void addSample(int rawValue) {
  samples[sampleIndex] = rawValue;
  sampleIndex = (sampleIndex + 1) % AVERAGE_SAMPLES;
  if (sampleIndex == 0) {
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

const __FlashStringHelper* relayStatusText() {
  return pumpOn ? F("ON") : F("OFF");
}

void printStatus(int avgRawValue, int moisturePercent) {
  Serial.print(F("Raw="));
  Serial.print(avgRawValue);
  Serial.print(F(" | Moisture="));
  Serial.print(moisturePercent);
  Serial.print(F("% | Relay="));
  Serial.print(relayStatusText());
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

void validateCalibration() {
  if (dryValue <= wetValue) {
    Serial.println(F("ERROR: Invalid calibration! dryValue must be greater than wetValue."));
    Serial.println(F("Set correct dryValue/wetValue in code before use."));
  } else {
    Serial.print(F("Calibration OK | dryValue="));
    Serial.print(dryValue);
    Serial.print(F(" wetValue="));
    Serial.println(wetValue);
  }
}

void setup() {
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Safe startup.
  setPumpState(false);

  // Allow pump to start immediately if soil is already dry after boot.
  lastPumpChangeMs = millis() - PUMP_COOLDOWN_MS;

  Serial.begin(9600);
  Serial.println(F("=== Automatic Watering System (Uno) ==="));
  Serial.println(F("Rules: DRY->Pump ON, WET->Pump OFF | MinRun=5s | Cooldown=30s"));
  validateCalibration();

  // Prime the moving average buffer.
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
    if (elapsed >= PUMP_COOLDOWN_MS && moisturePercent <= DRY_THRESHOLD_PERCENT) {
      setPumpState(true);
      Serial.println(F("ACTION: Soil DRY -> Relay ON | Pump ON | LED ON"));
    }
  } else {
    if (elapsed >= MIN_PUMP_RUN_MS && moisturePercent >= WET_THRESHOLD_PERCENT) {
      setPumpState(false);
      Serial.println(F("ACTION: Soil WET enough -> Relay OFF | Pump OFF | LED OFF"));
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

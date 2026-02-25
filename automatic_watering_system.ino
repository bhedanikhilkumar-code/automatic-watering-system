/*
  Automatic Watering System for My Plants
  Board: Arduino Uno

  Hardware mapping:
  - Soil Moisture Sensor: AO -> A0, VCC -> 5V, GND -> GND
  - Relay Module: IN -> D7, VCC -> 5V, GND -> GND
  - LED: D8 -> 220 ohm resistor -> LED -> GND
  - Pump: through relay COM/NO using external power

  Logic:
  - Soil DRY -> relay ON -> pump ON -> LED ON
  - Soil WET -> relay OFF -> pump OFF -> LED OFF
  - Uses moving average + minimum runtime + cooldown to prevent rapid toggling
*/

// ---------------------------- Pins ----------------------------
const uint8_t SOIL_SENSOR_PIN = A0;
const uint8_t RELAY_PIN = 7;
const uint8_t LED_PIN = 8;

// Relay behavior: many modules are active LOW.
// true  => LOW turns relay ON
// false => HIGH turns relay ON
const bool RELAY_ACTIVE_LOW = true;

// ---------------------- Calibration section -------------------
// Set these values from your own measurements:
// dryValue: sensor reading in dry soil/air (usually higher)
// wetValue: sensor reading in wet soil/water (usually lower)
const int dryValue = 850;
const int wetValue = 420;

// Moisture thresholds (%):
// <= DRY_THRESHOLD_PERCENT -> start watering
// >= WET_THRESHOLD_PERCENT -> stop watering
const int DRY_THRESHOLD_PERCENT = 35;
const int WET_THRESHOLD_PERCENT = 55;

// ---------------------- Timing parameters ---------------------
const unsigned long SAMPLE_INTERVAL_MS = 500;
const unsigned long MIN_PUMP_RUNTIME_MS = 5000;   // 5 seconds minimum ON time
const unsigned long PUMP_COOLDOWN_MS = 30000;     // 30 seconds minimum OFF time

// Moving average window for sensor smoothing
const uint8_t AVERAGE_SAMPLES = 10;
int rawSamples[AVERAGE_SAMPLES];
uint8_t sampleIndex = 0;
bool sampleBufferFull = false;

bool pumpOn = false;
unsigned long lastSampleTime = 0;
unsigned long lastPumpStateChangeMs = 0;

int moisturePercentFromRaw(int raw) {
  // Clamp to calibration range before mapping
  raw = constrain(raw, wetValue, dryValue);

  // Reverse map because dry soil often gives a higher analog value.
  long moisturePercent = map(raw, dryValue, wetValue, 0, 100);
  return (int)constrain(moisturePercent, 0, 100);
}

void setPumpAndLed(bool turnOn) {
  pumpOn = turnOn;
  lastPumpStateChangeMs = millis();

  if (RELAY_ACTIVE_LOW) {
    digitalWrite(RELAY_PIN, turnOn ? LOW : HIGH);
  } else {
    digitalWrite(RELAY_PIN, turnOn ? HIGH : LOW);
  }

  digitalWrite(LED_PIN, turnOn ? HIGH : LOW);
}

void addRawSample(int raw) {
  rawSamples[sampleIndex] = raw;
  sampleIndex++;

  if (sampleIndex >= AVERAGE_SAMPLES) {
    sampleIndex = 0;
    sampleBufferFull = true;
  }
}

int getAverageRawSample() {
  uint8_t count = sampleBufferFull ? AVERAGE_SAMPLES : sampleIndex;
  if (count == 0) {
    return analogRead(SOIL_SENSOR_PIN);
  }

  long total = 0;
  for (uint8_t i = 0; i < count; i++) {
    total += rawSamples[i];
  }
  return (int)(total / count);
}

void printStatus(int raw, int averageRaw, int moisturePercent) {
  Serial.print(F("Raw="));
  Serial.print(raw);
  Serial.print(F(" | AvgRaw="));
  Serial.print(averageRaw);
  Serial.print(F(" | Moisture="));
  Serial.print(moisturePercent);
  Serial.print(F("% | State="));
  Serial.println(pumpOn ? F("DRY -> PUMP ON, LED ON") : F("WET/OK -> PUMP OFF, LED OFF"));
}

void setup() {
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Safe startup: keep pump OFF
  setPumpAndLed(false);

  Serial.begin(9600);
  Serial.println(F("=== Automatic Watering System ==="));
  Serial.println(F("Calibrate dryValue/wetValue for your sensor."));
  Serial.println(F("Minimum pump runtime: 5s, cooldown: 30s."));

  // Prime moving average buffer
  for (uint8_t i = 0; i < AVERAGE_SAMPLES; i++) {
    addRawSample(analogRead(SOIL_SENSOR_PIN));
    delay(20);
  }
}

void loop() {
  unsigned long now = millis();
  if (now - lastSampleTime < SAMPLE_INTERVAL_MS) {
    return;
  }
  lastSampleTime = now;

  int raw = analogRead(SOIL_SENSOR_PIN);
  addRawSample(raw);

  int averageRaw = getAverageRawSample();
  int moisturePercent = moisturePercentFromRaw(averageRaw);
  unsigned long elapsedSinceStateChange = now - lastPumpStateChangeMs;

  if (!pumpOn) {
    // Pump can start only after cooldown period
    bool cooldownDone = elapsedSinceStateChange >= PUMP_COOLDOWN_MS;
    if (cooldownDone && moisturePercent <= DRY_THRESHOLD_PERCENT) {
      setPumpAndLed(true);
      Serial.println(F("ACTION: Soil is DRY -> Pump ON, LED ON"));
    }
  } else {
    // Pump can stop only after minimum runtime
    bool minimumRunDone = elapsedSinceStateChange >= MIN_PUMP_RUNTIME_MS;
    if (minimumRunDone && moisturePercent >= WET_THRESHOLD_PERCENT) {
      setPumpAndLed(false);
      Serial.println(F("ACTION: Soil is WET enough -> Pump OFF, LED OFF"));
    }
  }

  printStatus(raw, averageRaw, moisturePercent);
}

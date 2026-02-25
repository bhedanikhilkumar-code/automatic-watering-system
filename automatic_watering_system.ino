/*
  Automatic Watering System for My Plants
  Board: Arduino Uno
  Target board: Arduino Uno

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
  - Pump: wired through relay COM/NO with external power supply

  Logic:
  - If soil is DRY  -> Relay ON  -> Pump ON  -> LED ON
  - If soil is WET  -> Relay OFF -> Pump OFF -> LED OFF
  - Uses averaging + minimum pump runtime + cooldown to avoid rapid toggling
*/

// ---------------------- Pin configuration ----------------------
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
// Relay modules are often active LOW (LOW = relay ON).
// Set false if your relay is active HIGH.
const bool RELAY_ACTIVE_LOW = true;

// ------------------- Sensor calibration values ------------------
// IMPORTANT: update these values for your own sensor after testing.
// SENSOR_DRY_VALUE: analog reading in dry soil/air (usually higher)
// SENSOR_WET_VALUE: analog reading in wet soil/water (usually lower)
const int SENSOR_DRY_VALUE = 850;
const int SENSOR_WET_VALUE = 420;

// Moisture thresholds (%) for pump control
const int DRY_THRESHOLD_PERCENT = 35;  // start watering at/below this
const int WET_THRESHOLD_PERCENT = 55;  // stop watering at/above this

// Control timing (ms)
const unsigned long SAMPLE_INTERVAL_MS = 500;       // sample every 0.5s
const unsigned long MIN_PUMP_RUN_TIME_MS = 4000;    // keep pump on at least 4s
const unsigned long PUMP_COOLDOWN_MS = 4000;        // keep pump off at least 4s

// Averaging window (simple moving average)
const uint8_t AVERAGE_SAMPLES = 10;
int sampleBuffer[AVERAGE_SAMPLES];
uint8_t sampleIndex = 0;
bool bufferFilled = false;

bool pumpOn = false;
unsigned long lastSampleTime = 0;
unsigned long lastPumpChangeTime = 0;

// Convert raw analog value to moisture percentage.
// 0% = very dry, 100% = very wet.
int moisturePercentFromRaw(int raw) {
  raw = constrain(raw, SENSOR_WET_VALUE, SENSOR_DRY_VALUE);
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

void setRelayAndLed(bool on) {
  pumpOn = on;
  lastPumpChangeTime = millis();

  // Relay drive based on module logic
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(RELAY_PIN, on ? LOW : HIGH);
  } else {
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  }

  // LED follows pump state
  digitalWrite(LED_PIN, on ? HIGH : LOW);
}

void addSample(int value) {
  sampleBuffer[sampleIndex] = value;
  sampleIndex++;

  if (sampleIndex >= AVERAGE_SAMPLES) {
    sampleIndex = 0;
    sampleBufferFull = true;
  }
}

int getAverageRawSample() {
  uint8_t count = sampleBufferFull ? AVERAGE_SAMPLES : sampleIndex;
    bufferFilled = true;
  }
}

int getAverageRaw() {
  uint8_t count = bufferFilled ? AVERAGE_SAMPLES : sampleIndex;
  if (count == 0) {
    return analogRead(SOIL_SENSOR_PIN);
  }

  long total = 0;
  for (uint8_t i = 0; i < count; i++) {
    total += rawSamples[i];
    total += sampleBuffer[i];
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
  // Safe startup: pump OFF
  setRelayAndLed(false);

  Serial.begin(9600);
  Serial.println(F("Automatic Watering System started"));
  Serial.println(F("Use Serial Monitor @ 9600 baud"));

  // Prime average buffer with initial readings
  for (uint8_t i = 0; i < AVERAGE_SAMPLES; i++) {
    int raw = analogRead(SOIL_SENSOR_PIN);
    addSample(raw);
    delay(20);
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
  addSample(raw);

  int avgRaw = getAverageRaw();
  int moisturePercent = moisturePercentFromRaw(avgRaw);

  // Decide pump state with hysteresis + timing protection.
  unsigned long timeSincePumpChange = now - lastPumpChangeTime;

  if (!pumpOn) {
    bool cooldownDone = (timeSincePumpChange >= PUMP_COOLDOWN_MS);
    if (cooldownDone && moisturePercent <= DRY_THRESHOLD_PERCENT) {
      setRelayAndLed(true);  // DRY -> turn pump ON
    }
  } else {
    bool minRunDone = (timeSincePumpChange >= MIN_PUMP_RUN_TIME_MS);
    if (minRunDone && moisturePercent >= WET_THRESHOLD_PERCENT) {
      setRelayAndLed(false); // WET -> turn pump OFF
    }
  }

  // Serial monitor output
  Serial.print(F("Raw="));
  Serial.print(raw);
  Serial.print(F(" AvgRaw="));
  Serial.print(avgRaw);
  Serial.print(F(" Moisture="));
  Serial.print(moisturePercent);
  Serial.print(F("% Status="));
  Serial.println(pumpOn ? F("PUMP ON / LED ON") : F("PUMP OFF / LED OFF"));
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

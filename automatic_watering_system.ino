/*
  Automatic Watering System for My Plants
  Target board: Arduino Uno

  Hardware mapping:
  - Soil Moisture Sensor: AO -> A0, VCC -> 5V, GND -> GND
  - Relay Module: IN -> D7, VCC -> 5V, GND -> GND
  - LED: D8 -> 220 ohm resistor -> LED -> GND
  - Pump: wired through relay COM/NO with external power supply

  Logic:
  - If soil is DRY  -> Relay ON  -> Pump ON  -> LED ON
  - If soil is WET  -> Relay OFF -> Pump OFF -> LED OFF
  - Uses averaging + minimum pump runtime + cooldown to avoid rapid toggling
*/

// ---------------------- Pin configuration ----------------------
const uint8_t SOIL_SENSOR_PIN = A0;
const uint8_t RELAY_PIN = 7;
const uint8_t LED_PIN = 8;

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
    total += sampleBuffer[i];
  }
  return (int)(total / count);
}

void setup() {
  pinMode(SOIL_SENSOR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

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
}

void loop() {
  unsigned long now = millis();

  if (now - lastSampleTime < SAMPLE_INTERVAL_MS) {
    return;
  }
  lastSampleTime = now;

  int raw = analogRead(SOIL_SENSOR_PIN);
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
}

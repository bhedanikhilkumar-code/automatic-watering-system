// Soil Moisture Sensor
int soilMoisturePin = A0; // Analog input pin A0
int soilMoistureValue = 0; // Variable to store the moisture value

// Relay Module
int relayPin = 7; // Digital output pin 7
int pumpOn = HIGH; // Define the pump activation state
int pumpOff = LOW; // Define the pump deactivation state

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
 
  // Set relay pin as output
  pinMode(relayPin, OUTPUT);
}

void loop() {
  // Read the soil moisture value
  soilMoistureValue = analogRead(soilMoisturePin);
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoistureValue);
 
  // Check if the soil is too dry (adjust threshold as needed)
  if (soilMoistureValue < 700) {
    // Turn on the water pump
    digitalWrite(relayPin, pumpOn);
    Serial.println("Pump ON");
  } else {
    // Turn off the water pump
    digitalWrite(relayPin, pumpOff);
    Serial.println("Pump OFF");
  }
 
  // Delay before next reading
  delay(1000);
}



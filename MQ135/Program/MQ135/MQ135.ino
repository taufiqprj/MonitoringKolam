const int MQ135pin = 35;  // Define the pin for MQ135 sensor
bool isRunning = true;   // Flag to track if the system is running

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (isRunning) {
    int rawValue = analogRead(MQ135pin);
    float voltage = rawValue / 1024.0 * 5.0;
    float resistance = (5.0 - voltage) / voltage;

    // Calculate CO2
    float ppm = 1.0 / (0.03588 * pow(resistance, 1.336));

    // Print values to Serial Monitor
    // Serial.print("Raw Analog Value: ");
    // Serial.println(rawValue);
    // Serial.print("Voltage: ");
    // Serial.println(voltage, 2);    // Print with 2 decimal places
    // Serial.print("Sensor Resistance: ");
    // Serial.println(resistance, 2); // Print with 2 decimal places
    Serial.print("PPM: ");
    Serial.println(ppm, 2);        // Print with 2 decimal places

    delay(500); // Adjust the delay based on your application needs
  }
}

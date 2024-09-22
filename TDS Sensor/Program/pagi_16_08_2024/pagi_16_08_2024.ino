#include <Arduino.h>
#include <EEPROM.h>

#define TdsSensorPin 34
#define VREF 3.3      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30    // sum of sample point

int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, tdsValue = 0, temperature = 25;

// Calibration variables
float calibrationPPM = 0.0;
float calibrationReading = 0.0;
#define EEPROM_SIZE 8
#define CALIBRATION_PPM_ADDRESS 0
#define CALIBRATION_READING_ADDRESS 4

void setup() {
  Serial.begin(115200);
  pinMode(TdsSensorPin, INPUT);
  
  EEPROM.begin(EEPROM_SIZE);
  loadCalibrationData();
  
  if (calibrationPPM == 0.0 || calibrationReading == 0.0) {
    Serial.println("Calibration needed. Please enter the known PPM value:");
    while (!Serial.available()) {
      // Wait for input
    }
    calibrationPPM = Serial.parseFloat();
    Serial.print("Calibration PPM set to: ");
    Serial.println(calibrationPPM);
    
    calibrationReading = readRawTDSValue();
    Serial.print("Calibration reading: ");
    Serial.println(calibrationReading);
    
    // Save calibration data
    EEPROM.put(CALIBRATION_PPM_ADDRESS, calibrationPPM);
    EEPROM.put(CALIBRATION_READING_ADDRESS, calibrationReading);
    EEPROM.commit();
    
    Serial.println("Calibration complete!");
  } else {
    Serial.println("Calibration data loaded from memory.");
    Serial.print("Calibration PPM: ");
    Serial.println(calibrationPPM);
    Serial.print("Calibration reading: ");
    Serial.println(calibrationReading);
  }
}

void loop() {
  static unsigned long printTime = millis();
  if (millis() - printTime > 1000) {
    printTime = millis();
    float rawReading = readRawTDSValue();
    float calibratedTDS = (rawReading / calibrationReading) * calibrationPPM;
    
    Serial.print("TDS Value: ");
    Serial.print(calibratedTDS, 2);
    Serial.println(" ppm");
  }
}

float readRawTDSValue() {
  for(int i = 0; i < SCOUNT; i++) {
    analogBuffer[i] = analogRead(TdsSensorPin);
    delay(10);
  }
  
  averageVoltage = getMedianNum(analogBuffer, SCOUNT) * (float)VREF / 4096.0;
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensationVoltage = averageVoltage / compensationCoefficient;
  return (133.42 * compensationVoltage * compensationVoltage * compensationVoltage 
          - 255.86 * compensationVoltage * compensationVoltage 
          + 857.39 * compensationVoltage) * 0.5;
}

void loadCalibrationData() {
  EEPROM.get(CALIBRATION_PPM_ADDRESS, calibrationPPM);
  EEPROM.get(CALIBRATION_READING_ADDRESS, calibrationReading);
  if (isnan(calibrationPPM) || isnan(calibrationReading)) {
    calibrationPPM = 0.0;
    calibrationReading = 0.0;
  }
}

int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}
#include <Arduino.h>

const int pH_Pin = 34;  // Pin analog untuk sensor pH

float pH_Value;
float Voltage;

void setup() {
  Serial.begin(115200);
  pinMode(pH_Pin, INPUT);
}

void loop() {
  Voltage = analogRead(pH_Pin) * 3.3 / 4095.0;  
  pH_Value = 3.5 * Voltage + Offset;  

  Serial.print("Voltage: ");
  Serial.print(Voltage, 2);
  Serial.print(" V, pH value: ");
  Serial.println(pH_Value, 2);

  delay(1000);  
}

// Fungsi untuk mengkalibrasi sensor
float Offset = 0.00;
#define ArrayLenth 40
int pHArray[ArrayLenth];
int pHArrayIndex = 0;

double avergearray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    return 0;
  }
  if (number < 5) {
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0]; max = arr[1];
    } else {
      min = arr[1]; max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;
          max = arr[i];
        } else {
          amount += arr[i];
        }
      }
    }
    avg = (double)amount / (number - 2);
  }
  return avg;
}

void calibration() {
  Serial.println("Calibration starting...");
  for (int i = 0; i < ArrayLenth; i++) {
    pHArray[pHArrayIndex++] = analogRead(pH_Pin);
    if (pHArrayIndex == ArrayLenth) pHArrayIndex = 0;
    delay(20);
  }
  float voltage = avergearray(pHArray, ArrayLenth) * 3.3 / 4095.0;
  float pH_Value = 3.5 * voltage;
  Offset = 7.00 - pH_Value;
  Serial.print("Offset: ");
  Serial.println(Offset, 3);
  Serial.println("Calibration finished.");
}
#include <Wire.h>
#include "DCT532.h"

DCT532 sensor;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!sensor.begin(0x32)) { // your address
    Serial.println("DCT532 not found!");
    while (1);
  }
  Serial.println("DCT532 connected");
}

void loop() {
  float p = sensor.readPressure();
  float t = sensor.readTemperature();

  if (!isnan(p) && !isnan(t)) {
    Serial.print("Pressure: ");
    Serial.print(p, 3);
    Serial.print(" ");
    Serial.print(sensor.getPressureUnits());
    Serial.print(" Temp: ");
    Serial.print(t, 2);
    Serial.println(" °C");
  } else {
    Serial.println("Read error");
  }

  delay(1000);
}

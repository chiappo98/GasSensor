#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Scanning I2C...");
}

void loop() {
  byte error, address;

  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Trovato dispositivo a 0x");
      Serial.println(address, HEX);
    }
  }

  delay(2000);
}
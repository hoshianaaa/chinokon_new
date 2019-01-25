#include <Wire.h>

int bc = 0;
void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  Wire.requestFrom(1, 2);// request 1 bytes from Slave ID #8

  while (Wire.available()) {
    byte b = Wire.read();
    byte c = Wire.read();
    
    

    bc = b;
    bc <<= 8;
    bc |= c;

    Serial.println(bc);
    bc = 0;
    Serial.println("hello");
  }
  Serial.println("he");

  //delay(500);
}

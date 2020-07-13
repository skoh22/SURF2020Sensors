/*
  Communicate with BME280s with different I2C addresses
  Nathan Seidle @ SparkFun Electronics
  March 23, 2015

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14348 - Qwiic Combo Board
  https://www.sparkfun.com/products/13676 - BME280 Breakout Board

  This example shows how to connect two sensors on the same I2C bus.

  The BME280 has two I2C addresses: 0x77 (jumper open) or 0x76 (jumper closed)

  Hardware connections:
  BME280 -> Arduino
  GND -> GND
  3.3 -> 3.3
  SDA -> A4
  SCL -> A5
*/

#include <Wire.h>

#include "SparkFunBME280.h"
BME280 bme;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  bme.beginI2C(); 
}

void loop()
{
  Serial.print("Humidity: ");
  Serial.println(bme.readFloatHumidity());

  Serial.print(" Pressure: ");
  Serial.println(bme.readFloatPressure());

  Serial.print(" Temp: ");
  Serial.println(bme.readTempF());

  Serial.println();

  delay(50);
}

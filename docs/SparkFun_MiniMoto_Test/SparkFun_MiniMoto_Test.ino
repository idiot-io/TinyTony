
/*
  ##TinyTony##
  I2C address table
  +------+------+---------+
  |  A1  |  A0  | Address |
  +------+------+---------+
  | 0    | 0    | 0xC0    |
  | 0    | open | 0xC2    |
  | 0    | 1    | 0xC4    |
  | open | 0    | 0xC6    |
  | open | open | 0xC8    |
  | open | 1    | 0xCA    |
  | 1    | 0    | 0xCC    |
  | 1    | open | 0xCE    |
  | 1    | 1    | 0xD0    |
  +------+------+---------+

*/
#include "SparkFunMiniMoto.h"  // Include the MiniMoto library

// Create MiniMoto instances
MiniMoto motor0(0xC8); // A1 = 1, A0 = clear

#define POTPIN    3  // Potentiometer on A3


void setup()
{
  Serial.begin(115200);
  Serial.println("start");
  pinMode(POTPIN, INPUT);
}

void loop()
{
  // Read voltage on LDR
  int potValue = analogRead(POTPIN);

  //Serial.print(potValue);
  //Serial.print(" ");
  potValue=constrain(potValue, 150,790);
  potValue=map(potValue, 150, 790, -63, 63);
  Serial.println(potValue);
  
  // the magnitude can range from 6-63
  motor0.drive(potValue);

}


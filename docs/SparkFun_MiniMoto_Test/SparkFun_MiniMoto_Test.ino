
/*
  ##TinyTony##
  this example is for running an UNO+DRV8830
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
#define FAULTn  16     // Pin used for fault detection.

#define POTPIN    3  // Potentiometer on A3


void setup()
{
  Serial.begin(9600);
  Serial.println("start");
  pinMode(POTPIN, INPUT);
  pinMode(FAULTn, INPUT);
}

void loop()
{
  // Read voltage on LDR
  int potValue = analogRead(POTPIN);

  //Serial.print(potValue);
  //Serial.print(" ");
  potValue=constrain(potValue, 73,790);
  potValue=map(potValue, 73, 790, -63, 63);
  //Serial.println(potValue);
  
  // the magnitude can range from 6-63
  motor0.drive(50);
  delayUntil(50);

}

// delayUntil() is a little function to run the motor either for
//  a designated time OR until a fault occurs. Note that this is
//  a very simple demonstration; ideally, an interrupt would be
//  used to service faults rather than blocking the application
//  during motion and polling for faults.
void delayUntil(unsigned long elapsedTime)
{
  // See the "BlinkWithoutDelay" example for more details on how
  //  and why this loop works the way it does.
  unsigned long startTime = millis();
  while (startTime + elapsedTime > millis())
  {
    // If FAULTn goes low, a fault condition *may* exist. To be
    //  sure, we'll need to check the FAULT bit.
    if (digitalRead(FAULTn) == LOW)
    {
      // We're going to check both motors; the logic is the same
      //  for each...
      byte result = motor0.getFault();
      // If result masked by FAULT is non-zero, we've got a fault
      //  condition, and we should report it.
      if (result & FAULT)
      {
        Serial.print("Motor 0 fault: ");
        if (result & OCP) Serial.println("Chip overcurrent!");
        if (result & ILIMIT) Serial.println("Load current limit!");
        if (result & UVLO) Serial.println("Undervoltage!");
        if (result & OTS) Serial.println("Over temp!");
        break; // We want to break out of the motion immediately,
               //  so we can stop motion in response to our fault.
      }
    }
  }
}

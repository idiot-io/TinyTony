/*
  * ATTinyCore- https://github.com/SpenceKonde/ATTinyCore
  * drv8330 DRIVER- based on  https://github.com/sparkfun/SparkFun_MiniMoto_Arduino_Library
  
  ## I2C address table ##
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

#define SDA_PORT PORTB
#define SDA_PIN 0
#define SCL_PORT PORTB
#define SCL_PIN 2
#define _addr 0xC8
#define FAULTn  3     // Pin used for fault detection.
#include "SoftI2CMaster.h"

// Fault constants
#define FAULT 0x01
#define ILIMIT 0x10
#define OTS 0x08
#define UVLO 0x04
#define OCP 0x02

#define TXPIN     1 // tested on PB1/PB4
#include "SendOnlySoftwareSerial.h"
SendOnlySoftwareSerial softSerial (TXPIN);  // Tx pin

void setup()
{
  softSerial.begin(9600);
  softSerial.println("reboot");

#if I2C_CPUFREQ == (F_CPU/8)
  CPUSlowDown();
#endif

  i2c_init();
}

void loop()
{
  drive(50);
  delayUntil(500);
  //delay(5);//this is bad, replace with noDelay

}

// Send the drive command over I2C to the DRV8830 chip. Bits 7:2 are the speed
//  setting; range is 0-63. Bits 1:0 are the mode setting:
//  - 00 = HI-Z
//  - 01 = Reverse
//  - 10 = Forward
//  - 11 = H-H (brake)
void drive(int speedx)
{ 
  //This should clear the fault register.
  byte regValue = 0x08;
  //Clear the fault status, in case there is one.
  i2c_start(_addr | I2C_WRITE);
  i2c_write(0x01);   //0x01 = We want to write.
  i2c_write(regValue); //Write the clear bye for fault register.
  i2c_stop();  //Stop transmission.

  //Let's convert the integer given us into a byte.
  regValue = (byte)abs(speedx);     //Convert 0-63 to byte value to set speed.
  if (regValue > 63) regValue = 63; // Cap the value at 63.
  regValue = regValue << 2;         // Left shift to make room for bits 1:0
  if (speedx < 0) regValue |= 0x01;  // Set bits 1:0 based on sign of input.
  else           regValue |= 0x02;   //A negative number for reverse and positive for forward.

  //Now, let's move this sucker.
  //Sets the i2c slave _address
  i2c_start(_addr | I2C_WRITE);
  //0x00 = We want to write something.
  i2c_write(0x00);
  //Writes the byte which had been converted from an integer that was passed this function.  Annnnd! The motor moves!
  i2c_write(regValue);
  i2c_stop();

}

// Return the fault status of the DRV8830 chip. Also clears any existing faults.
byte getFault()
{
  byte buffer = 0;
  byte clearFault = 0x80;
  //I2CReadBytes(0x01, &buffer, 1);
  i2c_rep_start(_addr | I2C_READ);
  buffer = i2c_read(0x01);
  i2c_stop();
  
  //I2CWriteBytes(0x01, &clearFault, 1);
  i2c_start(_addr | I2C_WRITE);
  i2c_write(0x01);
  i2c_write(clearFault);
  i2c_stop();
  return buffer;
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
      byte result = getFault();
      // If result masked by FAULT is non-zero, we've got a fault
      //  condition, and we should report it.
      if (result & FAULT)
      {
        softSerial.print("Motor 0 fault: ");
        if (result & OCP) softSerial.println("Chip overcurrent!");
        if (result & ILIMIT) softSerial.println("Load current limit!");
        if (result & UVLO) softSerial.println("Undervoltage!");
        if (result & OTS) softSerial.println("Over temp!");
        break; // We want to break out of the motion immediately,
               //  so we can stop motion in response to our fault.
      }
    }
  }
}

void CPUSlowDown(int fac) {
  // slow down processor by a fac
  CLKPR = _BV(CLKPCE);
  CLKPR = _BV(CLKPS1) | _BV(CLKPS0);
}


/*
  TinyIdiot
  tiytony esp8266

  drv8830 code from https://jiwashin.blogspot.com/2016/09/drive-dc-motor-by-drv8830-and-esp8266.html
  pdf > http://www.tij.co.jp/jp/lit/ds/symlink/drv8830.pdf

  ##wiring
  ESP	        DRV 8830
  GPIO 4	|   SDA
  GPIO 5	|   SCL
  VIN	    |   Vcc
  GND	    |   Gnd

  * **connect to the ESP only with I2C pulled up with 3.3v.**
  * Motor (+) and OUT1, motor (-) and OUT2
  * A 1 nF ceramic capacitor between + and - of the motor
  * A resistance of 0.2 Ω between ISENSE and GND of the DRV 8830
  * 0.1 μF ceramic capacitor between VCC and GND of the DRV 8830
  * A 10 kΩ resistor between SDA and 3.3 v
  * A 10 kΩ resistor between SCL and 3.3 v
  * 5 V output such as USB serial and VIN of ESP - WROOM - 02 board
  * GND of USB serial etc. and GND of ESP - WROOM - 02 board
*/

#include <Arduino.h>
#include <Wire.h>

uint8_t readMotorStatus();
void resetMotorStatus();
void runMotor(int inVector);
void writeToDriver(byte inDirection, byte inVoltage);
void brakeMotor();

const int kDrv8830Address = 0x64;

const int kBitClear  = 0x80;
const int kBitILimit = 0x10;
const int kBitOTS    = 0x08;
const int kBitUVLO   = 0x04;
const int kBitOCP    = 0x02;
const int kBitFault  = 0x01;


//
//  Setup
//

void setup() {
  Serial.begin(115200);
  Wire.begin();
}


//
//  Loop
//

float r = 0;

void loop() {
  //
  //  Motor
  float s = sin(r) * 64.0;
  r += 0.1;
  if (r > 6.28) r = 0.0;

  int out = (s > 0) ? 0x01 : 0x02;
  int speed = s;

  runMotor(speed);

  int status = readMotorStatus();
  if (status & kBitFault) {
    Serial.print("Motor Fault : ");
    Serial.println(status, HEX);
    resetMotorStatus();
  }
  
  delay(100);
}


//
//  DRV8830 Controll
//
uint8_t readMotorStatus() {
  uint8_t result = 0x00;
    
  Wire.beginTransmission(kDrv8830Address);
  Wire.write(0x01); //  read register 0x01
  Wire.endTransmission();

  Wire.requestFrom(kDrv8830Address, 1);
  if (Wire.available()) {
    result = Wire.read();
  } else {
    Serial.println("No status data");
  }

  return result;
}

void resetMotorStatus() {
  Wire.beginTransmission(kDrv8830Address);
  Wire.write(0x01); //  fault
  Wire.write(0x80);  // clear
  Wire.endTransmission(true);
}


void runMotor(int inVector) {
  int direction;
  int voltage;

  if (inVector > 0) {
    direction = 0x01;
    voltage = inVector;
  } else if (inVector == 0) {
    direction = 0x00;
    voltage = 0;
  } else {
    direction = 0x02;
    voltage = -inVector;
  }

  writeToDriver(direction, voltage);
}

void brakeMotor() {
  writeToDriver(0x03, 0x00);
}


void writeToDriver(byte inDirection, byte inVoltage) {
  if (inVoltage <= 0x05) inVoltage = 0x06;  // minimum voltage value is 0x06.
  
  int outData = (inVoltage & 0x3f) << 2 | (inDirection & 0x03);
  Wire.beginTransmission(kDrv8830Address);
  Wire.write(0x00); //  control
  Wire.write(outData);  //
  Wire.endTransmission(true);

  delay(12);
}
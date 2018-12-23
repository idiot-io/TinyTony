
/********************************************************************
  TinyIdiot
  tiytony esp8266

  drv8830 code from https://jiwashin.blogspot.com/2016/09/drive-dc-motor-by-drv8830-and-esp8266.html
  pdf > http://www.ti.com/lit/ds/symlink/drv8830.pdf

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
  * 
 
  ==calibration==
  the system has an initial guess for calibration, 
  but you will need to fine tune it to compensate 
  for diffrent material and lighting
  [CH+] means press that key on remote
  
  *fine tune
  cover LDR #1       press the [CH+] to set maxval
  cover LDR #2       press the [CH-] to set minval
  uncover both LDRs  press the [CH] to set midval
  *trim
  press [-] and [+] to subtract/add to trim value
    after fine-tune you will have to trim the motor middle point
    press until the motor doesn't turn or twitch in uncovered state.
    usually no more then -/+10 is needed. 
  *store
  to put your settings in deep memory press the [200+] key
    the setting you provide will be stored in ATtoni 
    so they are remmbered and will  reboot.
  *info
  if DEBUG is on you can use the [EQ] button to watch your settings
  you will need a serial RX line connected to pin6 (TX)
  
  *sensitivity 
    [PREV] AND [NEXT] keys set the minimal motion trashold 
  == usage ==
  === setups ==
  *LDRx2 - voltage divider
  *LINEAR - fixed side
 *******************************************************************/


#include <Arduino.h>
#include <Wire.h>

int mapped(int input);
void printGraph();

uint8_t readMotorStatus(int addr);
void resetMotorStatus(int addr);
void runMotor(int addr, int inVector);
void writeToDriver(int addr, byte inDirection, byte inVoltage);
void brakeMotor(int addr);

const int kDrv8830Address1 = 0x64; //A0,A1 both open
const int kDrv8830Address2 = 0x61; //A0 open, A1 to GND
const int kBitClear  = 0x80;
const int kBitILimit = 0x10;
const int kBitOTS    = 0x08;
const int kBitUVLO   = 0x04;
const int kBitOCP    = 0x02;
const int kBitFault  = 0x01;

unsigned long  prevValue;

//2. calculat min and max from pos output
//1. get midval from reseting pos of y
struct settings {
  int minval;
  int midval;
  int maxval; 
  int trim;
  int senestivity;
};
settings set01 = {88,645,1024,20,5 };

int pos, potValue, speed, adc, adcc, speedc;

#define  graph_state 1


void setup() {
  Serial.begin(115200);
  //Wire.begin(int sda, int scl);
  Wire.begin(4, 5);
}



void loop() {
// Read voltage on LDR
  adc = analogRead(A0);
  //y = 0.10*adc +0.75*y;  //smoothing
  adcc = (adc-set01.midval)/4;
  speed = mapped(adcc);
  speedc=constrain(speed, -60,60);

  printGraph();
 
  runMotor(kDrv8830Address1, speedc);
 
  delay(100);
}

int mapped(int input){
	int output=0;
  if ( input >= -set01.senestivity && input <=  set01.senestivity ){
		  output = 0 ;
	} else if (input < -set01.senestivity){
    //if between -20 -200 then
		  output = map(input, -set01.senestivity,set01.minval , 0, 60   );
     // Serial.println("<");
  }else if(input > set01.senestivity){
    //if between 20 200 then
		  output = map(input, set01.senestivity, set01.maxval, 0, -60  );
     // Serial.println(">");

  }
	return output;
}
//
//  DRV8830 Controll
//

//void printGraph(){};
void printGraph(){
  if(graph_state){
   // Serial.print(adc);	  Serial.print("	");
    Serial.print(adcc);	  Serial.print("	");
	  Serial.print(speed);    Serial.println();
  }
}

void runMotor(int addr, int inVector) {
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

  writeToDriver(addr, direction, voltage);
}

void brakeMotor(int addr) {
  writeToDriver(addr, 0x03, 0x00);
}


void writeToDriver(int addr, byte inDirection, byte inVoltage) {
  if (inVoltage <= 0x05) inVoltage = 0x06;  // minimum voltage value is 0x06.
  
  int outData = (inVoltage & 0x3f) << 2 | (inDirection & 0x03);
  Wire.beginTransmission(addr);
  Wire.write(0x00); //  control
  Wire.write(outData);  //
  Wire.endTransmission(true);
  delay(12);
}



uint8_t readMotorStatus(int addr) {
  //add status read/write to loop i needed
  /*  
   int status = readMotorStatus();
  if (status & kBitFault) {
    //Serial.print("Motor Fault : ");
    //Serial.println(status, HEX);
    resetMotorStatus();
  }
  */
  uint8_t result = 0x00;
    
  Wire.beginTransmission(addr);
  Wire.write(0x01); //  read register 0x01
  Wire.endTransmission();

  Wire.requestFrom(addr, 1);
  if (Wire.available()) {
    result = Wire.read();
  } else {
    Serial.println("No status data");
  }

  return result;
}

void resetMotorStatus(int addr) {
  Wire.beginTransmission(addr);
  Wire.write(0x01); //  fault
  Wire.write(0x80);  // clear
  Wire.endTransmission(true);
}
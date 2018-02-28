/*******************************************************************

   words http://idiot.io
   code https://github.com/shenkarSElab/xxx

   pin out attiny85
   NC      -1+----+8-   VCC
   3/A3    -2|*   |7-   2/A1
   4/A4    -3|    |6-   1
   GND     -4+----+5-   0/PWM

  install:
  ATTinyCore- https://github.com/SpenceKonde/ATTinyCore
  sofServo https://github.com/adafruit/Adafruit_SoftServo

 *******************************************************************/

#define DEBUG 1

#define SERVO1PIN 0   // Servo control line (orange) on 0/PWM
#define POTPIN   A2  // Potentiometer on 2/A1 //for some reason this pysically mapsto 4/A2/PB$ on my attiny85 *shrug*
#define TXPIN 1

#include "SoftwareServo.h"
SoftwareServo myServo1;  // create servo object to control a servo

#include "SendOnlySoftwareSerial.h"
SendOnlySoftwareSerial mySerial (TXPIN);  // Tx pin

void setup() {

  mySerial.begin(9600);
  mySerial.println("hell");

  myServo1.attach(SERVO1PIN);
  myServo1.setMaximumPulse(2200);

  delay(15);
}

void loop()  {
  int servoPos;
  int potValue = analogRead(POTPIN);              // Read voltage on potentiometer

  // map the range of pot to limited map range on the servo.
  // we are only intrested in naoorw band of the full range.
  // in both directions, with a stop space in middle

  if ( potValue < 350 ) {
    servoPos = map(potValue, 332, 380, 55, 66);
  } else if (potValue > 650) {
    servoPos = map(potValue, 480, 430, 96, 83);
  } else {
    servoPos = 74;
  }
  mySerial.print(servoPos);
  mySerial.print(" ");
  mySerial.println(potValue);
  myServo1.write(servoPos);


  SoftwareServo::refresh();
  delay(15);                              // waits 15ms for the servo to reach the position


}

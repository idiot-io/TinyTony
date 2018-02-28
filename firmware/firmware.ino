/*******************************************************************

   words http://idiot.io
   code https://github.com/shenkarSElab/xxx

   pin out attiny85
   NC      -1+----+8-   VCC
   3/A3    -2|*   |7-   2/A1
   4/A4    -3|    |6-   1
   GND     -4+----+5-   0/PWM

  using:
  ATTinyCore- https://github.com/SpenceKonde/ATTinyCore
  sofServo http://playground.arduino.cc/ComponentLib/Servo
  SendOnlySoftwareSerial https://forum.arduino.cc/index.php?topic=112013.msg841582#msg841582

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
    servoPos = map(potValue, 0, 350, 43, 83);
  } else if (potValue > 650) {
    servoPos = map(potValue, 1024, 650, 130, 89);
  } else {
    servoPos = 86;
  }
  //servoPos = map(potValue, 0, 1024, 0, 179);
  
  mySerial.print(servoPos);
  mySerial.print(" ");
  mySerial.println(potValue);
  myServo1.write(servoPos);


  SoftwareServo::refresh();
  delay(15);                              // waits 15ms for the servo to reach the position


}

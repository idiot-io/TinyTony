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

#include <Servo.h>

#include <Adafruit_SoftServo.h>  // SoftwareServo (works on non PWM pins)
#define SERVO1PIN 9   // Servo control line (orange) on 0/PWM
#define POTPIN   A0   // Potentiometer on 2/A1
Servo myServo1;


void setup() {
  Serial.begin(9600);
  
  myServo1.attach(SERVO1PIN);
  delay(15);
}

void loop()  {
  int potValue;
  int servoPos;
  int state;
  potValue = analogRead(POTPIN);              // Read voltage on potentiometer
  int invar = map(potValue, 0, 1023, 0, 179);

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

  if (DEBUG) {
    Serial.print(potValue);
    Serial.print(" ");
    Serial.print(invar);
    Serial.print(" ");
    Serial.println(state);
  }
  myServo1.write(servoPos);
  delay(15);                              // waits 15ms for the servo to reach the position
}



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


#include <Servo.h>

#include <Adafruit_SoftServo.h>  // SoftwareServo (works on non PWM pins)
#define SERVO1PIN 9   // Servo control line (orange) on 0/PWM
#define POTPIN   A0   // Potentiometer on 2/A1
Servo myServo1;

int pos_mid = 74;
int pos_mid_jitter = 1;
int pos_fast = 55;
int pos_slow = 65;

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

  if ( potValue < 350 ) {
    servoPos = map(potValue, 332, 380, 55, 66);
    state = 2;

  } else if (potValue > 650) {
    servoPos = map(potValue, 480, 430, 96, 83);
    state = 1;

  } else {
    servoPos = 74;
    state = 0;
  }

  Serial.print(potValue);
  Serial.print(" ");
  Serial.print(invar);
  Serial.print(" ");
  Serial.println(state);

  myServo1.write(servoPos);
  delay(15);                              // waits 15ms for the servo to reach the position
}



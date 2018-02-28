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

#define DEBUG 0

#include <Adafruit_SoftServo.h>  // SoftwareServo (works on non PWM pins)
#define SERVO1PIN 0   // Servo control line (orange) on 0/PWM
#define POTPIN   2   // Potentiometer on 2/A1
Adafruit_SoftServo myServo1;


#include "SendOnlySoftwareSerial.h"
SendOnlySoftwareSerial mySerial (1);  // Tx pin

void setup() {
  // Set up the interrupt that will refresh the servo for us automagically
  OCR0A = 0xAF;            // any number is OK
  TIMSK |= _BV(OCIE0A);    // Turn on the compare interrupt (below!)


myServo1.attach(SERVO1PIN);

mySerial.begin(4800);
mySerial.println("hoo");

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

  myServo1.write(servoPos);
  delay(15);                              // waits 15ms for the servo to reach the position


}

volatile uint8_t counter = 0;
SIGNAL(TIMER0_COMPA_vect) {
  // this gets called every 2 milliseconds
  counter += 2;
  // every 20 milliseconds, refresh the servos!
  if (counter >= 20) {
    counter = 0;
    myServo1.refresh();
  }
}

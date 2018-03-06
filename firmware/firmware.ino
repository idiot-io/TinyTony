/*******************************************************************

   words http://idiot.io
   code https://github.com/shenkarSElab/tongue

   pin out attiny85
   RST    -1+----+8-   VCC
   3/LDR  -2|*   |7-   2/PWM
   4/IR   -3|    |6-   1/SERIAL
   GND    -4+----+5-   0/SERVO

  using:
  ATTinyCore- https://github.com/SpenceKonde/ATTinyCore
  sofServo http://playground.arduino.cc/ComponentLib/Servo
  SendOnlySoftwareSerial https://forum.arduino.cc/index.php?topic=112013.msg841582#msg841582
  tiny_ir https://gist.github.com/SeeJayDee/caa9b5cc29246df44e45b8e7d1b1cdc5
  ir_methods https://github.com/z3t0/Arduino-IRremote/wiki/Receiving-with-the-IRremote-library

  attiny85 pinout - https://camo.githubusercontent.com/081b569122da2244ff7de8bae15eb56947d05cc8/687474703a2f2f6472617a7a792e636f6d2f652f696d672f50696e6f7574543835612e6a7067
 
 
 
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

#include <EEPROM.h>

#define SERVO1PIN 0   // Servo control line (orange) on 0/PWM
#define TXPIN     1 // tested on PB1/PB4
#define LEDPIN    2 // led pin
#define POTPIN    3  // Potentiometer on A3
#define RECVPIN   4 //ir reciver data pin , tested on PB1/PB4

#include "SoftwareServo.h"
SoftwareServo softServo;  // create servo object to control a servo

#include "SendOnlySoftwareSerial.h"
SendOnlySoftwareSerial softSerial (TXPIN);  // Tx pin

#include "tiny_IRremote.h"
IRrecv irrecv(RECVPIN);
decode_results results;

unsigned long  prevValue;

struct settings {
  int minval;
  int midval;
  int maxval; 
  int trim;
  int senestivity;
  int MaximumPulse;
};
settings set01 = {50,200,400,0,20, 2200 };

int servoPos;
int potValue, tmpval;
  
float x_1, y;
int graph_state = 0;

void setup() {

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  irrecv.enableIRIn();

  softSerial.begin(9600);
  softSerial.println("reboot");
  
  EEPROM.get(0, set01);
  //make sure we dont get null values for maxpulse
  if(set01.MaximumPulse<=0 )
  {
    set01.MaximumPulse = 2100;
    printdebug();
  }
  

  softServo.attach(SERVO1PIN);
  softServo.setMaximumPulse(set01.MaximumPulse);
  //softServo.setMaximumPulse(2200);

  delay(15);
}

void loop()  {
  //read IR signals
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xFFA25D:
        //softSerial.println("CH-");
	      set01.minval = servoPos;
        printdebug();
        break;

      case 0xFF629D: 
       // softSerial.println("CH");
	      set01.midval = servoPos;
        printdebug();
        break;
        
      case 0xFFE21D:
        //softSerial.println("CH+");
        set01.maxval = servoPos;
        printdebug();
        break;

      case 0xFFE01F:
        //softSerial.println("-");
        set01.trim++;
        printdebug();
        break;

      case 0xFFA857:
        //softSerial.println("+");
        set01.trim--;
        printdebug();
        break;

      case 0xFF22DD:
        //softSerial.println("<");
        set01.senestivity -= 10;
        printdebug();
        break;
        
      case 0xFF02FD:
        ////softSerial.println(">");
        set01.senestivity += 10;
        printdebug();
        break;

      case 0xFF52AD:
        //softSerial.println("9");
        if (graph_state)
          graph_state=0;
        else 
          graph_state=1;    
          
        break;

      case 0xFF42BD:
        ////softSerial.println("7");
        set01.MaximumPulse -= 50;
        softServo.setMaximumPulse(set01.MaximumPulse);
        printdebug();
        break;
      case 0xFF4AB5:
        ////softSerial.println("8");
        set01.MaximumPulse += 50;
        softServo.setMaximumPulse(set01.MaximumPulse);
        printdebug();
        break;

      case 0xFF6897:
        //softSerial.println("0");
        set01 = {50,200,500,0, 20 };
        printdebug();	
        break;

      case 0xFF906F:
        //softSerial.println("EQ");
        printdebug();
        break;

      case 0xFFB04F:
        //softSerial.println("200");
        EEPROM.put(0, set01);
        printdebug();
        break;

      case 0xFFFFFFFF:
        break;

          }
            

    irrecv.resume();
  }

  // Read voltage on LDR
  potValue = analogRead(POTPIN);  
 
  //smoothing
  y = 0.10*potValue +0.75*y;
  servoPos = int(y)  ;
  servoPos = servoPos - set01.midval;
  tmpval = int(mapped(servoPos));
  tmpval = constrain(tmpval, 0, 179);

  if (millis() - prevValue >= 15) {
    prevValue = millis();
    softServo.write(tmpval);

    SoftwareServo::refresh();
    
	  //printdebug();
    printGraph();
  }

}


int mapped(int input){
	int output;
  if ( (input >= (set01.midval - set01.senestivity) ) && (input <= (set01.midval + set01.senestivity)) ){
		output = 90-set01.trim ;
    digitalWrite(LEDPIN, HIGH);
	  } else if (input < (set01.midval - set01.senestivity)){
		output = map(input, set01.minval, set01.midval, 0, 90-set01.trim   );
    digitalWrite(LEDPIN, LOW);
    }else if(input > (set01.midval + set01.senestivity)){
		output = map(input, set01.midval, set01.maxval, 179, 90-set01.trim  );
    digitalWrite(LEDPIN, LOW);	
  }
	return output;
}

//void printGraph(){};
void printGraph(){
  if(graph_state){
    softSerial.print(tmpval);
	  softSerial.print("	");
	  softSerial.print(int(servoPos));
	  //softSerial.print("	");
	  //softSerial.print(potValue);
    softSerial.println();
  }
}
void printdebug(){};

/*

/*
void printdebug(){
	//softSerial.println("minval  midval maxval  trim sensitivity servopos");
  /*
	softSerial.print(set01.minval);
	softSerial.print("  ");
	softSerial.print(set01.midval);
	softSerial.print("  ");
	softSerial.print(set01.maxval);
  softSerial.print("  ");
	softSerial.print(set01.trim);
  softSerial.print("  ");
	softSerial.print(set01.senestivity);
  softSerial.print("  ");
	softSerial.print(set01.MaximumPulse);
  softSerial.print("  ");
	softSerial.print(servoPos);
	//softSerial.println("tmpval	servoPos	potValue");
	//softSerial.print(tmpval);
	//softSerial.print("	");
	//softSerial.print("	");
	//softSerial.print(potValue);
  softSerial.println();

}
*/
  
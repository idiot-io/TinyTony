/*******************************************************************

   words http://idiot.io
   code https://github.com/idiot-io/TinyTony

   pin out attiny85
   RST    -1+----+8-   VCC
   3/LDR  -2|*   |7-   2/SCL(I2C)
   4/IR   -3|    |6-   1/SERIAL
   GND    -4+----+5-   0/SDA(I2C)

  using:
  ATTinyCore- https://github.com/SpenceKonde/ATTinyCore
  drv8330 DRIVER- based on  https://github.com/sparkfun/SparkFun_MiniMoto_Arduino_Library

  drv8330 I2C address table
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

  attiny85 pinout - https://i.imgur.com/gfLIGGz.jpg
*/

#define SDA_PORT PORTB
#define SDA_PIN 0
#define SCL_PORT PORTB
#define SCL_PIN 2
#include <SoftWire.h>
SoftWire Wire = SoftWire();
// Create drv8330 instances
//drv8330 motor0(0xC8); // A1 = 1, A0 = clear

#include <EEPROM.h>

#define TXPIN     1 // tested on PB1/PB4
#define SCL       2 // SCL(I2C)
#define SDA       0 // SDA(I2C)
#define POTPIN    3  // Potentiometer on A3
#define RECVPIN   4 //ir reciver data pin , tested on PB1/PB4

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
};
settings set01 = {150, 450, 790, 0, 20 };

int motoPos;
int potValue, tmpval;

float x_1, y;
int graph_state = 0;

void setup() {
  Wire.begin();
  
  irrecv.enableIRIn();

  softSerial.begin(9600);
  softSerial.println("reboot");

  EEPROM.get(0, set01);

  delay(15);
}

void loop()  {
  //read IR signals
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xFFA25D:
        //softSerial.println("CH-");
        set01.minval = motoPos;
        printdebug();
        break;

      case 0xFF629D:
        // softSerial.println("CH");
        set01.midval = motoPos;
        printdebug();
        break;

      case 0xFFE21D:
        //softSerial.println("CH+");
        set01.maxval = motoPos;
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
          graph_state = 0;
        else
          graph_state = 1;

        break;

      case 0xFF42BD:
        ////softSerial.println("7");
        printdebug();
        break;
      case 0xFF4AB5:
        ////softSerial.println("8");
        printdebug();
        break;

      case 0xFF6897:
        //softSerial.println("0");
        set01 = {50, 200, 500, 0, 20 };
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
  y = 0.10 * potValue + 0.75 * y;
  motoPos = int(y)  ;
  motoPos = motoPos - set01.midval;
  tmpval = int(mapped(motoPos));
  tmpval = constrain(tmpval, -63, 63);

  if (millis() - prevValue >= 15) {
    prevValue = millis();
   // motor0.drive(tmpval);

    //printdebug();
    printGraph();
  }

}


int mapped(int input) {
  int output;
  if ( (input >= (set01.midval - set01.senestivity) ) && (input <= (set01.midval + set01.senestivity)) ) {
    output = 90 - set01.trim ;
  } else if (input < (set01.midval - set01.senestivity)) {
    output = map(input, set01.minval, set01.midval, 0, 90 - set01.trim   );
  } else if (input > (set01.midval + set01.senestivity)) {
    output = map(input, set01.midval, set01.maxval, 179, 90 - set01.trim  );
  }
  return output;
}

//void printGraph(){};
void printGraph() {
  if (graph_state) {
    softSerial.print(tmpval);
    softSerial.print("	");
    softSerial.print(int(motoPos));
    //softSerial.print("	");
    //softSerial.print(potValue);
    softSerial.println();
  }
}
void printdebug() {};

/*

  /*
  void printdebug(){
	//softSerial.println("minval  midval maxval  trim sensitivity motoPos");
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
	softSerial.print(motoPos);
	//softSerial.println("tmpval	motoPos	potValue");
	//softSerial.print(tmpval);
	//softSerial.print("	");
	//softSerial.print("	");
	//softSerial.print(potValue);
  softSerial.println();

  }
*/


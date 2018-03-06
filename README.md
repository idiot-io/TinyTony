## TinyTony V1.0
words http://idiot.io   
code https://github.com/shenkarSElab/TinyTony 


#### TOC
* [Usage](#usage)
* [Calibration](#calibration)
* [Firmware](#firmware)
* [Hardware](#hardware)

### Usage 
to run Tony you need follow the connection diagram, and an ir remote for calibration.  **no coding is nessecery**

##### Setup 
* LDRx2 - voltage divider
* LINEAR - fixed side

### Calibration   
the system has an initial guess for calibration,   
but you will need to fine tune it to compensate   
for diffrent material and lighting  

*when discussed in this document, [CH+] means press that key on ir remote* 
 
##### Fine tune  
cover LDR #1       press the [CH+] to set `maxval`    
cover LDR #2       press the [CH-] to set `minval`   
uncover both LDRs  press the [CH] to set  `midval`  

##### Trim  
press [-] and [+] to subtract/add to `trimval` value  
  > after fine-tune you will have to trim the motor middle point  
  > press until the motor doesn't turn or twitch in uncovered state.  
  > usually no more then -/+10 is needed.   
  
##### Store  
to put your settings in deep memory press the [200+] key  
  > the setting you provide will be stored in TinyTony   
  > so they are remembered and will  reboot.  
  
##### Info
if DEBUG is on you can use the [EQ] button to watch your settings
you will need a serial RX line connected to pin6 (TX)

##### Sensitivity 
  [PREV] AND [NEXT] keys set the minimal motion threshold 
  



### Firmware



###### Re-flashing
to upload new firmware you will need
* arduino software runing on your machine and able to communicate with usb
* a programmer (it can be an arduino uno borad, or aspusb programmer)
* an SPI cable to connect tony to the programmer.

##### Libs
most of the libraries are included and referanced from the firmware folder.

[ATTinyCore](https://github.com/SpenceKonde/ATTinyCore)  
EEPROM -used supplied one.   
[sofServo](http://playground.arduino.cc/ComponentLib/Servo)  
[SendOnlySoftwareSerial](https://forum.arduino.cc/index.php?topic=112013.msg841582#msg841582)  
[tiny_ir](https://gist.github.com/SeeJayDee/caa9b5cc29246df44e45b8e7d1b1cdc5) , and [ir_methods](https://github.com/z3t0/Arduino-IRremote/wiki/Receiving-with-the-IRremote-library)  


### Hardware
check the PDF folder in this git to get datasheetsfor parts


#### pinout of ATtiny85  
```
RST    -1+----+8-   VCC  
3/LDR  -2|    |7-   2/PWM  
4/IR   -3|    |6-   1/SERIAL  
GND    -4+----+5-   0/SERVO  
```
[attiny85 pinout complete](https://camo.githubusercontent.com/081b569122da2244ff7de8bae15eb56947d05cc8/687474703a2f2f6472617a7a792e636f6d2f652f696d672f50696e6f7574543835612e6a7067  )

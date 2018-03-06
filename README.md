# TinyTony V1.0
words http://idiot.io  
code https://github.com/shenkarSElab/tongue  

## pinout of ATtiny85  
```
RST    -1+----+8-   VCC  
3/LDR  -2|    |7-   2/PWM  
4/IR   -3|    |6-   1/SERIAL  
GND    -4+----+5-   0/SERVO  
```
## Code
to run tony you only need tho follow the connection diagram, and an ir remote for calibration. **no coding is nessecery**

## calibration   
the system has an initial guess for calibration,   
but you will need to fine tune it to compensate   
for diffrent material and lighting  

*when discussed in this document, [CH+] means press that key on ir remote* 
 
### fine tune  
cover LDR #1       press the [CH+] to set `maxval`    
cover LDR #2       press the [CH-] to set `minval`   
uncover both LDRs  press the [CH] to set  `midval`  

### trim  
press [-] and [+] to subtract/add to `trimval` value  
  > after fine-tune you will have to trim the motor middle point  
  > press until the motor doesn't turn or twitch in uncovered state.  
  > usually no more then -/+10 is needed.   
  
###store  
to put your settings in deep memory press the [200+] key  
  > the setting you provide will be stored in TinyTony   
  > so they are remembered and will  reboot.  
  
###info
if DEBUG is on you can use the [EQ] button to watch your settings
you will need a serial RX line connected to pin6 (TX)

###sensitivity 
  [PREV] AND [NEXT] keys set the minimal motion threshold 
  
## usage 
### setups 
* LDRx2 - voltage divider
* LINEAR - fixed side


## re-flashing
to upload new firmware you will need
* arduino software runing on your machine and able to communicate with usb
* a programmer (it can be an arduino uno borad, or aspusb programmer)
* an SPI cable to connect tony to the programmer.

## firmware
most of the libraries are included and referanced from the firmware folder.

[ATTinyCore](https://github.com/SpenceKonde/ATTinyCore)  
[EEPROM] -used supplied one. 
[sofServo](http://playground.arduino.cc/ComponentLib/Servo)  
[SendOnlySoftwareSerial](https://forum.arduino.cc/index.php?topic=112013.msg841582#msg841582)  
[tiny_ir](https://gist.github.com/SeeJayDee/caa9b5cc29246df44e45b8e7d1b1cdc5) , and [ir_methods](https://github.com/z3t0/Arduino-IRremote/wiki/Receiving-with-the-IRremote-library)  

[attiny85 pinout](https://camo.githubusercontent.com/081b569122da2244ff7de8bae15eb56947d05cc8/687474703a2f2f6472617a7a792e636f6d2f652f696d672f50696e6f7574543835612e6a7067  )

## hardware
check the PDF folder in this git to get datasheetsfor parts

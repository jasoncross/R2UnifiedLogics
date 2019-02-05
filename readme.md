# RSERIES UNIFIED LOGIC ENGINE 0.8
Code originally by Joymonkey, updated by IOIIOOO  
With portions inspired by / lifted from Curiousmarc, Mowee, Flthymcnasty, and others!  

Works with both AVR and Teensy Boards  
AVR Boards don't support Mic effects due to a lack of analog input  

Now supports scrolling text!

**********

# REQUIREMENTS

Arduino IDE: https://www.arduino.cc/en/Main/Software  
Teensyduino (if using Teensy board):  http://www.pjrc.com/teensy/td_download.html  
FastLED: https://github.com/FastLED/FastLED/releases  

**********

# COMMAND INPUT
The RSeries use a command input string to tell it what effects combination should be run.
This is comprised of a 8 digit long int.

TLEECSNN

## T - the text message designator - if not provided, defaults 0 (first message)
   0 - "Astromech"
   1 - "Astromech"
   2 - "Excuse me sir, but that R2-D2 is in prime condition, a real bargain."
   3 - "That malfunctioning little twerp."
   4 - "The city's central computer told me."
   5 - "Beep"
   6 - "Beep-bee-bee-boop-bee-doo-weep"
   7 - "R2-D2"
   8 - "Beep Boop"
   9 - "Bite my shiny metal ... Beep ... Boop ...";

## L - the logic designator - if not provided, defaults to 0 (all)
*** NOT YET SUPPORTED ***  
   0 - All  
   1 - Front  
   2 - Rear  

## EE - the effect - use two digits if logic designator provided
   01 - Alarm - flips between color and red with mic effects  
   02 - Failure - cycles colors and brightness fading - roughly timed to 128 screa-3.mp3  
   03 - Leia - pale green with mic effects  
   04 - March - sequence timed to Imperial March  
   05 - Single Color - single hue shown  
   06 - Flashing Color - single hue on and off  
   07 - Flip Flop Color - boards flip back and forth - similar to march  
   08 - Flip Flop Alt - other direction of flips on back board, front is same to flip flop  
   09 - Color Swap - switches between color specified and inverse compliment color  
   10 - Rainbow - rotates through colors over time  
   11 - Red Alert - shows color specified based on mic input  
   12 - Mic Bright - brightness of color specified back on mic input  
   13 - Mic Rainbow - color goes from default specified through color range based on mic input  
   14 - Text Scrolling Left
   98 - Displays Off - turns off displays  
   00 - Reset to Normal  
## C - color designator
   1 - Red  
   2 - Orange  
   3 - Yellow  
   4 - Green  
   5 - Cyan (Aqua)  
   6 - Blue  
   7 - Purple  
   8 - Magenta  
   9 - Pink  
   0 - Default color on alarm / default to red on many effects / color cycle on march / ignored on failure and rainbow  
## S - speed or sensitivity (1-9 scale) with 5 generally considered default for speed
   Flip Flop and Rainbow - 200ms x speed  
   Flash - 250ms x speed  
   March - 150ms x speed  
   Color Swap - 350ms x speed  
   Red Alert - sets mic sensitivity - as a fraction of speed / 10 - we recommend 3  
   Mic Bright - sets minimum brightness - fraction of speed / 10  
## NN - 2 digit time length in seconds
   00 for continuous use on most  
   00 for default length on Leia  
   Not used on March or Failure  

 ## Some sequence examples:
 Note: Leading 0s drop off as these are long ints  
 Solid Red:  51000  
 Solid Orange: 52000  
 Solid Yellow:  53000  
 Solid Green:  54000  
 Solid Cyan:  55000  
 Solid Blue:  56000  
 Solid Purple:  57000  
 Solid Magenta:  58000  
 Solid Pink: 59000  
 Alarm (default):  10500  
 Failure: 20000  
 Leia: 30000  
 March:  40500  
 March (Red Only):  41500  
 Flash (Yellow): 63500  
 Color Swap (pink): 99500  
 Rainbow: 100500  
 Red Alert: 111300  
 Mic Bright (Green): 124200  
 Mic Rainbow (Cyan): 135000  

 Text Scroll Left (Cyan): 40155118 [message #4 "The city's central computer told me." for 18 seconds]

 54008 - solid green for 8 seconds  
 63315 - flashing yellow at slightly higher speed for 15 seconds  
 30008 - leia effect for only 8 seconds  

See some of these effects with our release demo video by clicking below:  
 [![Demo Video](http://img.youtube.com/vi/xLywBaNr-SY/0.jpg)](http://www.youtube.com/watch?v=xLywBaNr-SY)

**********

# AVR BOARDS

Use jumper on S4 to indicate if rear (jumper on) or front (jumper off) logic display

**********

# USING TRIMPOTS FOR ADJUSTMENTS

Teensy:  jumper on Front or Rear pins  
AVR:  jumper on S3  
Boot with jumper in place to reset EEPROM to defaults and then go into adjustment mode. Adjust trimpots.  
Place jumper on after boot to skip EEPROM reset and just go into adjustment mode directly.  
Remove jumper with power on to save adjustments made so they will stay in effect after power off.  

**********

# MARCDUINOS / JAWALITE / TEECES

Input from Marcduinos work with either I2C (if using the Marcduino v2 firmware) or JAWALITE (default serial commands)

Marcduinos use a sequence of &<i2caddress>,"<i2ccommand>\r for I2C commands
So for example, to send to the default Teensy a command for static green, you would use:
```
   &10,"54000\r
```
If sending more than one command in a sequence, put a few \p in between for pauses
```
   &10,"54000\r\p\p\p&11"54000\r
```
The above would send the static green to both front and read AVR boards on the default I2C assignment

To pass commands via Jawalite (default Marcduino commands) connect the serial out on the Marcduino (Teeces out)
to the UART Rx input on the Teensy board.

NOTE: Much of the Teeces communication code came directly from CuriousMarc's Teeces sketch at http://www.curiousmarc.com/teeces-logics-dome-lights
It's highly likely there are some functions in here not currently used or available.

Text input for example is NOT currently supported.

**********

# SOME OTHER USEFUL LINKS

JawaLite Command Information: https://github.com/joymonkey/logicengine/wiki/JawaLite-Commands

The Logic of Logic Displays:  https://github.com/joymonkey/logicengine/wiki/The-Logic-of-Logic-Displays

Developer Notes: https://github.com/joymonkey/logicengine/wiki/Developer-Notes

Calculate HSV Color Values:  http://rseries.net/logic2/hsv/

Explanation of how "Tween" colors are implimented: http://rseries.net/logic2/color/?keys=4&tweens=4

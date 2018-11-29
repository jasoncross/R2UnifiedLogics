/**
 * RSERIES UNIFIED LOGIC ENGINE 0.7 
 * Code originally by Joymonkey, updated by IOIIOOO
 * With portions inspired by / lifted from Curiousmarc, Mowee, Flthymcnasty, and others!
 * 
 * Works with both AVR and Teensy Boards
 * AVR Boards don't support Mic effects due to a lack of analog input
 * 
 * Now supports I2C Input!
 * 
 * **********
 * 
 * REQUIREMENTS
 * 
 * Arduino IDE: https://www.arduino.cc/en/Main/Software  
 * Teensyduino (if using Teensy board):  http://www.pjrc.com/teensy/td_download.html 
 * FastLED: https://github.com/FastLED/FastLED/releases
 * 
 * **********
 * 
 * COMMAND INPUT
 * The RSeries use a command input string to tell it what effects combination should be run.
 * This is comprised of a 7 digit long int.
 * 
 * LEECSNN
 * 
 * L - the logic designator - if not provided, defaults to 0 (all)
 * *** NOT YET SUPPORTED ***
 *    0 - All
 *    1 - Front
 *    2 - Rear
 * EE - the effect - use two digits if logic designator provided
 *    01 - Alarm - flips between color and red with mic effects
 *    02 - Failure - cycles colors and brightness fading - roughly timed to 128 screa-3.mp3
 *    03 - Leia - pale green with mic effects
 *    04 - March - sequence timed to Imperial March
 *    05 - Single Color - single hue shown
 *    06 - Flashing Color - single hue on and off
 *    07 - Flip Flop Color - boards flip back and forth - similar to march
 *    08 - Flip Flop Alt - other direction of flips on back board, front is same to flip flop
 *    09 - Color Swap - switches between color specified and inverse compliment color
 *    10 - Rainbow - rotates through colors over time
 *    11 - Red Alert - shows color specified based on mic input
 *    12 - Mic Bright - brightness of color specified back on mic input
 *    13 - Mic Rainbow - color goes from default specified through color range based on mic input
 *    98 - Displays Off - turns off displays
 *    00 - Reset to Normal
 * C - color designator
 *    1 - Red
 *    2 - Orange
 *    3 - Yellow
 *    4 - Green
 *    5 - Cyan (Aqua)
 *    6 - Blue
 *    7 - Purple
 *    8 - Magenta
 *    9 - Pink
 *    0 - Default color on alarm / default to red on many effects / color cycle on march / ignored on failure and rainbow
 * S - speed or sensitivity (1-9 scale) with 5 generally considered default for speed
 *    Flip Flop and Rainbow - 200ms x speed
 *    Flash - 250ms x speed
 *    March - 150ms x speed
 *    Color Swap - 350ms x speed
 *    Red Alert - sets mic sensitivity - as a fraction of speed / 10 - we recommend 3
 *    Mic Bright - sets minimum brightness - fraction of speed / 10
 * NN - 2 digit time length in seconds
 *    00 for continuous use on most
 *    00 for default length on Leia
 *    Not used on March or Failure
 *    
 *  Some sequence examples:
 *  Note: Leading 0s drop off as these are long ints
 *  Solid Red:  51000
 *  Solid Orange: 52000
 *  Solid Yellow:  53000
 *  Solid Green:  54000
 *  Solid Cyan:  55000
 *  Solid Blue:  56000
 *  Solid Purple:  57000
 *  Solid Magenta:  58000
 *  Solid Pink: 59000
 *  Alarm (default):  10500
 *  Failure: 20000
 *  Leia: 30000
 *  March:  40500
 *  March (Red Only):  41500
 *  Flash (Yellow): 63500
 *  Color Swap (pink): 99500
 *  Rainbow: 100500
 *  Red Alert: 111300
 *  Mic Bright (Green): 124200
 *  Mic Rainbow (Cyan): 135000
 *
 *  54008 - solid green for 8 seconds
 *  63315 - flashing yellow at slightly higher speed for 15 seconds
 *  30008 - leia effect for only 8 seconds
 * 
 * **********
 * 
 * AVR BOARDS
 * 
 * Use jumper on S4 to indicate if rear (jumper on) or front (jumper off) logic display
 * 
 * **********
 * 
 * USING TRIMPOTS FOR ADJUSTMENTS
 * 
 * Teensy:  jumper on Front or Rear pins 
 * AVR:  jumper on S3
 * Boot with jumper in place to reset EEPROM to defaults and then go into adjustment mode. Adjust trimpots.
 * Place jumper on after boot to skip EEPROM reset and just go into adjustment mode directly.
 * Remove jumper with power on to save adjustments made so they will stay in effect after power off.
 * 
 * **********
 * 
 * MARCDUINOS / JAWALITE / TEECES
 * 
 * Input from Marcduinos work with either I2C (if using the Marcduino v2 firmware) or JAWALITE (default serial commands)
 * 
 * Marcduinos use a sequence of &<i2caddress>,"<i2ccommand>\r for I2C commands
 * So for example, to send to the default Teensy a command for static green, you would use:
 *    &10,"54000\r
 * If sending more than one command in a sequence, put a few \p in between for pauses
 *    &10,"54000\r\p\p\p&11"54000\r
 * The above would send the static green to both front and read AVR boards on the default I2C assignment
 * 
 * To pass commands via Jawalite (default Marcduino commands) connect the serial out on the Marcduino (Teeces out)
 * to the UART Rx input on the Teensy board.
 * 
 * NOTE: Much of the Teeces communication code came directly from CuriousMarc's Teeces sketch at http://www.curiousmarc.com/teeces-logics-dome-lights
 * It's highly likely there are some functions in here not currently used or available.
 * 
 * Text input for example is NOT currently supported.
 * 
 * **********
 * 
 * SOME OTHER USEFUL LINKS
 * 
 * JawaLite Command Information: https://github.com/joymonkey/logicengine/wiki/JawaLite-Commands
 *    
 * The Logic of Logic Displays:  https://github.com/joymonkey/logicengine/wiki/The-Logic-of-Logic-Displays
 * 
 * Developer Notes: https://github.com/joymonkey/logicengine/wiki/Developer-Notes
 * 
 * Calculate HSV Color Values:  http://rseries.net/logic2/hsv/
 * 
 * Explanation of how "Tween" colors are implimented: http://rseries.net/logic2/color/?keys=4&tweens=4
 * 
 **/


#define PCBVERSION 0 //what kind of LED PCBs are they? 0 = Originals (with Naboo logo on backs of Front and Rear Logic)
                     //                                1 = 2014 Version (with Kenny & McQuarry art on Rear, C3PO on Fronts)
                     //                                2 = 2016 Version (with Deathstar plans on back of Rear Logic)

#define MAX_BRIGHTNESS 225  //can go up to 255, but why? this limit keeps current and heat down, and not noticeably dimmer than 255
#define MIN_BRIGHTNESS 1   //minimum brightness for standard logic patterns that adjustment pots can go down to

#define DEBUG 0 //debug mode slows down startup and prints some extra info to serial, only really helpful for development
                //be warned - likely won't work on AVR due to memory limitations

#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
  #define BOARDTYPE 1 // Teensy
#else
  #define BOARDTYPE 0 // AVR
#endif

#include <FastLED.h>
#include <Wire.h>
#include <avr/pgmspace.h> //to save SRAM, some constants get stored in flash memory
#include "EEPROM.h"       //used to store user settings in EEPROM (settings will persist on reboots)

//a struct that holds the current color number and pause value of each LED (when pause value hits 0, the color number gets changed)
//to save SRAM, we don't store the "direction" anymore, instead we pretend that instead of having 16 colors, we've got 31 (16 that cross-fade up, and 15 "bizarro" colors that cross-fade back in reverse)
struct LEDstat { byte colorNum; byte colorPause; };

//adjustable settings
struct userSettings { byte maxBri; byte frontFade; byte frontDelay; byte frontHue; byte rearFade; byte rearDelay; byte rearHue; byte frontPalNum; byte rearPalNum; byte frontBri; byte rearBri; };
//default settings (will be overwritten from stored EEPROM data during setup(), or stored in EEPROM if current EEPROM values look invalid)
//to-do: add a version number
#define DFLT_FRONT_FADE 1
#define DFLT_FRONT_DELAY 40
#define DFLT_FRONT_HUE 0
#define DFLT_REAR_FADE 3
#define DFLT_REAR_DELAY 200
#define DFLT_REAR_HUE 0
#define DFLT_FRONT_PAL 0
#define DFLT_REAR_PAL 1
#define DFLT_FRONT_BRI 200
#define DFLT_REAR_BRI 200
userSettings settings[]={ MAX_BRIGHTNESS, DFLT_FRONT_FADE,DFLT_FRONT_DELAY,DFLT_FRONT_HUE,
                                          DFLT_REAR_FADE, DFLT_REAR_DELAY, DFLT_REAR_HUE,
                                          DFLT_FRONT_PAL, DFLT_REAR_PAL, DFLT_FRONT_BRI, DFLT_REAR_BRI }; 

#if (PCBVERSION==0)
  #define FRONT_LED_COUNT 96
#else
  #define FRONT_LED_COUNT 80
#endif

byte adjMode=0; // 0 for no adjustments, 1 for front, 3 for rear. if adjMode>0, then trimpots will be enabled
byte prevAdjMode=0;
byte prevBrightness=0;
byte prevPalNum=0;

//microphone stuff...
#define MIC_PERIOD 100 //the sample window for saving mic data (milliseconds)
unsigned int micVal;
unsigned int micStartMillis;
byte peakVal = 0; //microphone highest level for the current period
byte peakToPeak = 0; //microphone amplitude for the current period (difference between higest and lowest, a good way to detect )
unsigned int signalMax = 0;
unsigned int signalMin = 1024;

  //some serial related stuff...
  #if (BOARDTYPE==1)
    #define JEDI_SERIAL Serial3   //use Teensy's third hardware serial port to listen for Jawalite commands
  #else
    #define JEDI_SERIAL Serial // use standard?
  #endif
  #define DEBUG_SERIAL Serial
  #define CMD_MAX_LENGTH 64 // maximum number of characters in a command (63 chars since we need the null termination)
  #define MAXSTRINGSIZE 64  // maximum number of letters in a logic display message
  char cmdString[CMD_MAX_LENGTH];
  char logicText[3][MAXSTRINGSIZE+1]; // memory for the text strings to be displayed, add one for the NULL character at the end
  char ch;
  byte command_available; 
  // alphabet 0= english, 1= aurabesh
  #define LATIN 0
  #define AURABESH 1
  byte alphabetType[3];
  // display mode
  #define NORM 0
  #define ALARM 1
  #define MARCH 2
  #define LEIA 3
  #define FAILURE 4
  #define SINGLE 5
  #define FLASH 6
  #define FLIPFLOP 7
  #define FLIPFLOPALT 8
  #define COLORSWAP 9
  #define RAINBOW 10
  #define REDALERT 11
  #define MICBRIGHT 12
  #define MICRAINBOW 13
  #define LIGHTSOUT 98
  byte displayEffect; // 0 = normal, 1 = alarm, 2 = march, 3 = leia, 4 = failure
  byte previousEffect;
  
  // display state for the logics, 0=normal random, 1=text display, 2=bargraph, 3=test, 4=off 
  #define RANDOM 0
  #define TEXT 1
  #define BARGRAPH 2
  #define TEST 3
  #define OFF 4
  byte displayState[3];
  // display state for the PSI, 0=normal random, 1=color1, 2=color2, 3=test, 4=off (0, 3 and 4 are shared with displays above)
  byte psiState[2];
  #define COLOR1 1
  #define COLOR2 2
  #define BLUE         COLOR1
  #define RED          COLOR2
  #define YELLOW       COLOR1
  #define GREEN        COLOR2
  #define LOGICRandomStyle 4
  byte randomStyle[3]={LOGICRandomStyle, LOGICRandomStyle, LOGICRandomStyle}; // start with the default display random mode. This can be altered via extended JawaLite commands.
  byte effectRunning; // tracks if we are in the midst of running an effect

  //some default effect parameters
  #define LEIAduration  34000
  #define ALARMduration 4000
  #define MARCHduration 48300
  unsigned int marchSegment[8]={ 0,9800,14500,19300,28800,38300,45300,48300 };
  #define FAILUREduration 10000

  long displayEffectVal = 0; // if you want to boot with a different effect, set it here
  #define NORMVAL 0 // set the default effect string for "normal" running

  boolean logicInput = 0;

  //some LED and pattern related stuff...  
  #if (BOARDTYPE==0)
    // Pins specific to AVR
    #define delayPin A0 //analog pin to read keyPause value
    #define fadePin A1 //analog pin to read tweenPause value
    #define briPin A2 //analog pin to read Brightness value
    #define huePin A3 //analog pin to read Color/Hue shift value  
    #define FRONT_PIN 6
    #define REAR_PIN 6
    #define TOGGLEPIN 4 // pin to detect which logic is front vs back on AVR boards
    #define MIC_PIN 23 //pin used to for microphone preamp
    #define FJUMP_PIN 3  //front jumper
    #define RJUMP_PIN 3  //rear jumper 
    #define STATUSLED_PIN 13 //status LED is connected to pin 10, incorrectly labelled 9 on the PCB!!    
  #else
    // Pins Specific to Teensy
    #define delayPin A1 //15analog pin to read keyPause value
    #define fadePin A2 //16analog pin to read tweenPause value
    #define briPin A3 //17analog pin to read Brightness value
    #define huePin A6 //20analog pin to read Color/Hue shift value  
    #define FRONT_PIN 21
    #define REAR_PIN 22
    #define MIC_PIN 23 //pin used to for microphone preamp
    #define FJUMP_PIN 0  //front jumper
    #define RJUMP_PIN 1  //rear jumper 
    #define STATUSLED_PIN 10 //status LED is connected to pin 10, incorrectly labelled 9 on the PCB!!  
    #define TOGGLEPIN 0 // not used  
  #endif
  #define TWEENS 14 //lower=faster higher=smoother color crossfades, closely related to the Fade setting
  #define PAL_PIN 2  //pin used to switch palettes in ADJ mode 

  #define REAR_I2C 10 // A in hex - I2C Address for Rear or BOTH (if Teensy)
  #define FRONT_I2C 11  // B in hex - I2C Address for Front
  byte I2CAddress;
  #define I2CMAXCOMMANDLENGTH 8
  char I2CInput[I2CMAXCOMMANDLENGTH] = {};  // a char array to hold incoming data
  // String I2CInput = "";                       // a string to hold incoming data
  volatile boolean I2CComplete = false;       // whether the serial string is complete


  CRGB frontLEDs[FRONT_LED_COUNT];
  CRGB rearLEDs[96];
  CRGB statusLED[1];
  #define TOTALCOLORS (4+(TWEENS*(3)))
  #define TOTALCOLORSWBIZ ((TOTALCOLORS*2)-2)
  byte allColors[2][TOTALCOLORS][3]; //the allColor array will comprise of two color palettes on a Teensy

  LEDstat ledStatus[FRONT_LED_COUNT + 96]; //status array will cover both front and rear logics on a Teensy

#if (PCBVERSION<2)
  #define LED_TYPE WS2812
#else
  #define LED_TYPE SK6812
#endif 

#define JEDI_BAUDRATE 2400

//////////////////////////////////////////////////////////////////////////////////////////////////////
  // FRONT LED MAPPING...
  #if (PCBVERSION==0)
    //mapping for FLD boards from first run (with 48 LEDs per PCB)
    const byte frontLEDmap[]PROGMEM = {
     0, 1, 2, 3, 4, 5, 6, 7,
    15,14,13,12,11,10, 9, 8,
    16,17,18,19,20,21,22,23,
    31,30,29,28,27,26,25,24,
    32,33,34,35,36,37,38,39,
    47,46,45,44,43,42,41,40, //
    
    88,89,90,91,92,93,94,95, //
    87,86,85,84,83,82,81,80,
    72,73,74,75,76,77,78,79,
    71,70,69,68,67,66,65,64,
    56,57,58,59,60,61,62,63,
    55,54,53,52,51,50,49,48}; 
  #else
    //mapping for newer FLD PCBs (40 LEDs per PCB, lower FLD upside-down)...
    const byte frontLEDmap[]PROGMEM = { 
     0, 1, 2, 3, 4, 5, 6, 7,
    15,14,13,12,11,10, 9, 8,
    16,17,18,19,20,21,22,23,
    31,30,29,28,27,26,25,24,
    32,33,34,35,36,37,38,39,    
    79,78,77,76,75,74,73,72,
    64,65,66,67,68,69,70,71,
    63,62,61,60,59,58,57,56,
    48,49,50,51,52,53,54,55,
    47,46,45,44,43,42,41,40};  
  #endif
  //REAR LED MAPPING...
  #if (PCBVERSION==0)
    //mapping for first RLD (two PCBs soldered together)
    const byte rearLEDmap[]PROGMEM = {
     0, 1, 2, 3, 4, 5, 6, 7,  48,49,50,51,52,53,54,55,
    15,14,13,12,11,10, 9, 8,  63,62,61,60,59,58,57,56,
    16,17,18,19,20,21,22,23,  64,65,66,67,68,69,70,71,
    31,30,29,28,27,26,25,24,  79,78,77,76,75,74,73,72,
    32,33,34,35,36,37,38,39,  80,81,82,83,84,85,86,87,
    47,46,45,44,43,42,41,40,  95,94,93,92,91,90,89,88 };
  #elif (PCBVERSION==1)
    //mapping for single RLD PCB (second parts run on)...
    const byte rearLEDmap[]PROGMEM = {
     0, 1, 2,28, 3,27, 4,26, 5,25, 6, 7, 8, 9,22,10,21,11,20,12,19,13,14,15,
    31,30,29,32,33,34,35,36,37,38,39,24,23,40,41,42,43,44,45,46,47,18,17,16,
    64,65,66,63,62,61,60,59,58,57,56,71,72,55,54,53,52,51,50,49,48,77,78,79,
    95,94,93,67,92,68,91,69,90,70,89,88,87,86,73,85,74,84,75,83,76,82,81,80 };
  #else  
    //mapping for 2016 RLD (Death Star on back, direct mapping)...
    const byte rearLEDmap[]PROGMEM = {
     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,
    47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,
    48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
    95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72 };
  #endif
//////////////////////////////////////////////////////////////////////////////////////////////////////  


//we define both all palettes, in case the user wants to try out rear colors on the front etc
// note that these are not RGB colors, they're HSV
// for help calculating HSV color values see http://joymonkey.com/logic/
#define PAL_COUNT 4
const byte keyColors[PAL_COUNT][4][3]PROGMEM = { { {170,255,0} , {170,255,136} , {170,255,255} , {170,0,255}  } ,  //front colors
                                         { {87,206,0}    , {87,206,105}  , {45,255,184}  , {0,255,250}  } ,  //rear colors (hues: 87=bright green, 79=yellow green, 45=orangey yellow, 0=red)
                                         { {0,255,0}   , {0,255,85}    , {0,255,170}   , {0,255,255}  } ,  //monotone (black to solid red)
                                         { {255,0,255} , {0,255,255}   , {85,255,255}  , {170,255,255}} }; //rainbow (white, red, green, blue) THIS PALETTE IS HORRIBLE, CHANGE IT TO SOMETHING MORE USEFUL
                                  
//some variables used for status display and timekeeping...
unsigned long currentMillis = millis();
unsigned long effectStartMillis = millis();
unsigned int effectMillis;
unsigned long statusMillis = millis(); //last time status LED was changed, or effect sequence was started
unsigned long adjMillis = millis(); //last time we entered an adj mode
unsigned long statusDelay=1500;
bool flipflop; 
bool flipfloplast;

//////////////////////////////////////////////////////////////////////////////////////////////////////  
////////////////////////////////////////////////////////////////////////////////////////////////////// 

//function to calculate all colors based on the chosen colorPalNum
//this will get called during setup(), and also if we've remotely commanded to change color palettes
//NEW: added a "bright" parameter, to allow front and rear brightness to be adjusted independantly 
void calculateAllColors(byte colorPalNum, bool frontRear=0, byte brightVal=MAX_BRIGHTNESS) {
  //take a set of 4 key colors from keyColors[3][4][3] and generate 16 colors in allColors[frontRear]
  // 328P will only have one set of full colors, Teensy will have two
      for(byte kcol=0;kcol<(4);kcol++) { //go through each Key color
        byte workColor[3]={ pgm_read_byte(&keyColors[colorPalNum][kcol][0]) , pgm_read_byte(&keyColors[colorPalNum][kcol][1]) , pgm_read_byte(&keyColors[colorPalNum][kcol][2])  };      
        allColors[frontRear][kcol+(TWEENS*kcol)][0]=workColor[0];
        allColors[frontRear][kcol+(TWEENS*kcol)][1]=workColor[1];
        #if (DEBUG==1)
        //DEBUG_SERIAL.println( "Key color "+String(kcol)+" val is "+String( map8(workColor[2], 0, brightVal)+ "( was "+String( workColor[2] )+", max would be "+String(brightVal) ) );
        #endif
        allColors[frontRear][kcol+(TWEENS*kcol)][2]=map8(workColor[2], MIN_BRIGHTNESS, brightVal);  //Value (V) is adjusted down to whatever brightness setting we've specified
        if (kcol+(TWEENS*kcol)+1!=TOTALCOLORS) {
            for(byte el=0;el<3;el++) { //loop through H, S and V from this key to the next
                int perStep=int(pgm_read_byte(&keyColors[colorPalNum][kcol+1][el])-workColor[el])/(TWEENS+1);
                if (perStep!=0) {
                    for (byte tweenCount=1; tweenCount<=TWEENS; tweenCount++) {
                        byte val= pgm_read_byte(&keyColors[colorPalNum][kcol][el]) + (tweenCount*perStep);
                        if (el==2) allColors[frontRear][kcol+(TWEENS*kcol)+tweenCount][el]=map8(val, MIN_BRIGHTNESS, brightVal);
                        else allColors[frontRear][kcol+(TWEENS*kcol)+tweenCount][el]=val;
                    }
                }
                else {
                    //tweens for this element (h,s or v) don't change between this key and the next, fill em up
                    for (byte tweenCount=1; tweenCount<=TWEENS; tweenCount++) {
                        /*if (el==2) {
                          //allColors[frontRear][kcol+(TWEENS*kcol)+tweenCount][el]=map(workColor[el], 0, 255, 0, bright);
                          allColors[frontRear][kcol+(TWEENS*kcol)+tweenCount][el]=map(workColor[el], 0, 255, 0, bright);  //Value (V) is adjusted down to whatever brightness setting we've specified
                        }
                        else allColors[frontRear][kcol+(TWEENS*kcol)+tweenCount][el]=workColor[el];*/
                        if (el==2) allColors[frontRear][kcol+(TWEENS*kcol)+tweenCount][el]=map8(workColor[el], MIN_BRIGHTNESS, brightVal); //map V depending on brightness setting
                        else allColors[frontRear][kcol+(TWEENS*kcol)+tweenCount][el]=workColor[el];
                    }
                }              
            }
        }    
    }
    #if (DEBUG==1)
    /*DEBUG_SERIAL.println("Logic "+String(frontRear));
    for(byte colorNum=0;colorNum<TOTALCOLORS;colorNum++) {
      DEBUG_SERIAL.println( String( allColors[frontRear][colorNum][0] )+","+String( allColors[frontRear][colorNum][1] )+","+String( allColors[frontRear][colorNum][2] ) );
    }*/
    #endif
}

//function takes a color number (that may be bizarro) and returns an actual color number
int actualColorNum(int x) {
      if (x>=TOTALCOLORS) x=(TOTALCOLORS-2)-(x-TOTALCOLORS);
      return(x);
}


void updateLED(byte LEDnum , byte hueVal, bool frontRear, byte briVal=255) {
    //frontRear is 0 for front, 1 for rear
    //briVal is optional, typically used to change individual LED brightness during effect sequences
    //remember that ledStatus[] has 176/192 LEDs to track, frontLEDs is an 80/96 LED object, rearLEDs is a 96 LED object
    //current front pallete is settings[0].frontPalNum
    //current rear pallete is settings[0].rearPalNum
      if (frontRear==0) {
          if (ledStatus[LEDnum].colorPause!=0)  ledStatus[LEDnum].colorPause--; //reduce the LEDs pause number and check back next loop
          else {
              ledStatus[LEDnum].colorNum++;
              if (ledStatus[LEDnum].colorNum>=TOTALCOLORSWBIZ) ledStatus[LEDnum].colorNum=0; //bring it back to color zero
              byte realColor=actualColorNum(ledStatus[LEDnum].colorNum);
              if (ledStatus[LEDnum].colorNum%(5)==0) ledStatus[LEDnum].colorPause=random8(settings[0].frontDelay); //color is a key, assign random pause
              else ledStatus[LEDnum].colorPause=settings[0].frontFade; //color is a tween, assign a quick pause
              //
              if (briVal==255) frontLEDs[LEDnum].setHSV( (allColors[0][realColor][0]+hueVal), allColors[0][realColor][1], allColors[0][realColor][2] );
              else frontLEDs[LEDnum].setHSV( (allColors[0][realColor][0]+hueVal), allColors[0][realColor][1], map8( briVal, 0, allColors[0][realColor][2] ) );
          }
      }
      if (frontRear==1) {
        LEDnum=LEDnum+FRONT_LED_COUNT; //because ledStatus also holds status of the fronts (0-79 or 0-95), we need to start at 80/96
        if (ledStatus[LEDnum].colorPause!=0) ledStatus[LEDnum].colorPause--; //reduce the LEDs pause number and check back next loop          
        else {
            ledStatus[LEDnum].colorNum++;
            if (ledStatus[LEDnum].colorNum>=TOTALCOLORSWBIZ) ledStatus[LEDnum].colorNum=0; //bring it back to color zero
            byte realColor=actualColorNum(ledStatus[LEDnum].colorNum);
            if (ledStatus[LEDnum].colorNum%(5)==0) ledStatus[LEDnum].colorPause=random8(settings[0].rearDelay); //color is a key, assign random pause
            else ledStatus[LEDnum].colorPause=settings[0].rearFade; //color is a tween, assign a quick pause
            //        
            if (briVal==255) rearLEDs[LEDnum-FRONT_LED_COUNT].setHSV( (allColors[1][realColor][0]+hueVal), allColors[1][realColor][1], allColors[1][realColor][2] );
            else rearLEDs[LEDnum-FRONT_LED_COUNT].setHSV( (allColors[1][realColor][0]+hueVal), allColors[1][realColor][1], map8( briVal, 0, allColors[1][realColor][2] ) );
        }
      }
    #if (DEBUG==1)
    /*if (LEDnum==0) {
      DEBUG_SERIAL.print(String(LEDnum)+": "+String(frontLEDs[0][0])+","+String(frontLEDs[0][1])+","+String(frontLEDs[0][2])+".");
    }*/
    #endif
}  

//microphone preamp function
void microphoneRead(unsigned int micPeriod=100) {
  micVal=analogRead(MIC_PIN);
  if (currentMillis-micStartMillis<micPeriod) {
      if (micVal > signalMax) signalMax = micVal; //things were louder than before, so save this value as the highest so far
      if (micVal < signalMin) signalMin = micVal; //things were quieter than before, so save this value as the lowest so far
  }
  else {    
    //we've waited long enough, save the highest micVal for the last period as peakVal and biggest difference to peakToPeak
    peakVal = map(signalMax, 525, 1024, 0, 255);
    peakToPeak = map((signalMax-signalMin), 0, 1024, 0, 255);
    //reset timer and signal measures...
    micStartMillis=currentMillis;
    signalMin = 1024; signalMax = 0;
  }
}

//quick funciton to write all the current++++ settings to EEPROM in order
void writeSettingsToEEPROM() {
    EEPROM.write(0, settings[0].maxBri);
    EEPROM.write(1, settings[0].frontFade);
    EEPROM.write(2, settings[0].frontDelay);
    EEPROM.write(3, settings[0].frontHue);
    EEPROM.write(4, settings[0].rearFade);
    EEPROM.write(5, settings[0].rearDelay);
    EEPROM.write(6, settings[0].rearHue);
    EEPROM.write(7, settings[0].frontPalNum);
    EEPROM.write(8, settings[0].rearPalNum);
    EEPROM.write(9, settings[0].frontBri);
    EEPROM.write(10, settings[0].rearBri);
}
void readSettingsFromEEPROM() {
  settings[0]={EEPROM.read(0), EEPROM.read(1),EEPROM.read(2), EEPROM.read(3), EEPROM.read(4), EEPROM.read(5), EEPROM.read(6), EEPROM.read(7), EEPROM.read(8), EEPROM.read(9), EEPROM.read(10)};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    

void setup() {
  delay(50);

  #if (DEBUG==1)
    delay(5000); //long delay to give you time to open a serial monitor
  #endif
  
  // start listening for serial
  JEDI_SERIAL.begin(JEDI_BAUDRATE); //start whichever serial device we're using for Jawalite commands on this board
  DEBUG_SERIAL.begin(9600);
  if (BOARDTYPE == 0) {
    #if (DEBUG==1)
    DEBUG_SERIAL.println("AVR Board");
    #endif
  } else {
    #if (DEBUG==1)
    DEBUG_SERIAL.println("Teensy Reactor");
    #endif
  }

  // Determine which board is running and act accordingly
  if (BOARDTYPE == 0) {
    pinMode(TOGGLEPIN, INPUT);
    logicInput = digitalRead(TOGGLEPIN);
    if (logicInput == 1) {
      I2CAddress = REAR_I2C;
      #if (DEBUG==1)
        DEBUG_SERIAL.println("RLD"); 
      #endif
    } else {
      I2CAddress = FRONT_I2C;
      #if (DEBUG==1)
        DEBUG_SERIAL.println("FLD");
      #endif
    }
  } else {
    I2CAddress = REAR_I2C;
    logicInput = 0;
    #if (DEBUG==1)
      DEBUG_SERIAL.println("Teensy");
    #endif
  }

  // start listening for I2C
  Wire.begin(I2CAddress);   // Start I2C Bus as Master I2C Address
  #if (DEBUG==1)
    DEBUG_SERIAL.print("I2C Start: ");
    DEBUG_SERIAL.println(I2CAddress);
  #endif
  Wire.onReceive(i2cEvent);            // register event so when we receive something we jump to receiveEvent();

  if (BOARDTYPE == 1 || logicInput == 1) {
    FastLED.addLeds<LED_TYPE, REAR_PIN, GRB>(rearLEDs, 96);
    fill_solid( rearLEDs, 96, CRGB(0,0,0));
  }
  if (BOARDTYPE == 1 || logicInput == 0) {  
    FastLED.addLeds<LED_TYPE, FRONT_PIN, GRB>(frontLEDs, FRONT_LED_COUNT);
    fill_solid( frontLEDs, FRONT_LED_COUNT, CRGB(0,0,0));
  }

  // turn on status LED for Teensy
  if (BOARDTYPE == 1) {
    FastLED.addLeds<SK6812, STATUSLED_PIN, GRB>(statusLED, 1);
    statusLED[0] = 0x220000; FastLED.show(); //status LED dark red
    pinMode(13, OUTPUT); digitalWrite(13, HIGH); //turn on Teensy LED
    delay(100);
  }

  //jumper is used to set front or rear adjustment mode (pull low to adjust)
  pinMode(FJUMP_PIN,INPUT_PULLUP); //use internal pullup resistors of Teensy
  pinMode(RJUMP_PIN,INPUT_PULLUP);
  pinMode(PAL_PIN,INPUT_PULLUP); 

  //if eeprom settings look invalid (frontFade>100), or front jumper is pulled low on Teensy or S3 in place on AVR, reset eeprom values to defaults
  if ((EEPROM.read(1)>100)||(BOARDTYPE == 1 && digitalRead(FJUMP_PIN)==0)||(BOARDTYPE == 0 && digitalRead(FJUMP_PIN)==HIGH)) {
    if (EEPROM.read(1)>100) {
      #if (DEBUG==1) 
        DEBUG_SERIAL.println("Bad eeprom value. 001="+String(EEPROM.read(1)));
      #endif
    } else {
      #if (DEBUG==1) 
        DEBUG_SERIAL.println("Front Jumper pin is low");
      #endif
    }
    #if (DEBUG==1) 
      DEBUG_SERIAL.println("Writing EEPROM defaults");
    #endif
    if (BOARDTYPE == 1) {
      for (byte i=0; i<5; i++){ statusLED[0] = 0x222200; FastLED.delay(200); statusLED[0] = 0x000000; FastLED.delay(200);} //blinky
    } else if (BOARDTYPE == 0) {
      for (byte i=0; i<5; i++){ digitalWrite(STATUSLED_PIN, HIGH); FastLED.delay(200); digitalWrite(STATUSLED_PIN, LOW); FastLED.delay(200);} //blinky
    }
    writeSettingsToEEPROM();    
    if (BOARDTYPE == 1) {
      statusLED[0] = 0x110022; FastLED.show(); // Teensy LED
    } else if (BOARDTYPE == 0) { // AVR flutter the status LED to show we are done
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
    }
    delay(1000); //
  }
  
  //read settings from eeprom...
  readSettingsFromEEPROM();
  //settings[0]={EEPROM.read(0), EEPROM.read(1),EEPROM.read(2), EEPROM.read(3), EEPROM.read(4), EEPROM.read(5), EEPROM.read(6), EEPROM.read(7), EEPROM.read(8), EEPROM.read(9), EEPROM.read(10)};
  #if (DEBUG==1) 
  DEBUG_SERIAL.println("bri,frontFade,frontDelay,frontHue,rearFade,rearDelay,rearHue,frontPalNum,rearPalNum,frontBrightness,rearBrightness");
  DEBUG_SERIAL.print(F("eeprom: "));
  for (byte i=0; i<11; i++) { DEBUG_SERIAL.print(String(EEPROM.read(i))); if (i<10) DEBUG_SERIAL.print(F(",")); }
  DEBUG_SERIAL.println(".");
  #endif
  #if (DEBUG==1)
  DEBUG_SERIAL.println("Calculating all colors using max brightness value of "+String(settings[0].frontBri));
  #endif
  calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
  for( byte i = 0; i < FRONT_LED_COUNT; i++) {
    ledStatus[i]={random8(TOTALCOLORSWBIZ),random8()};
    updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0);
    #if (DEBUG==1)
    FastLED.delay(10);
    DEBUG_SERIAL.print(String(ledStatus[i].colorNum)+" ");
    if (((i+1)%8)==0) DEBUG_SERIAL.println();
    if (((i+1)%40)==0) DEBUG_SERIAL.println();
    #endif
  }
  #if (DEBUG==1)
    FastLED.delay(250);
  #endif
  calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);  
  for( byte i = 0; i < 96; i++) {
    ledStatus[i+FRONT_LED_COUNT]={random8(TOTALCOLORSWBIZ),random8()}; //assign the LED a random color number
    updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1); //update that LED to actually set its color
    #if (DEBUG==1)
    FastLED.delay(10);
    DEBUG_SERIAL.print(String(ledStatus[i+FRONT_LED_COUNT].colorNum)+" ");
    if (((i+1)%24)==0) DEBUG_SERIAL.println();
    #endif
  }

  if (BOARDTYPE == 1) {
    statusLED[0] = 0x002200; FastLED.show(); //status LED dark green
    //delay(50);
  }
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    


////////////////////////////////////////////////////////
// Command language - JawaLite emulation
///////////////////////////////////////////////////////


////////////////////////////////
// command line builder, makes a valid command line from the input
byte buildCommand(char ch, char* output_str) {
  static uint8_t pos=0;
  switch(ch) {
    case '\r':                          // end character recognized
      output_str[pos]='\0';   // append the end of string character
      pos=0;        // reset buffer pointer
      return true;      // return and signal command ready
      break;
    default:        // regular character
      output_str[pos]=ch;   // append the  character to the command string
      if(pos<=CMD_MAX_LENGTH-1)pos++; // too many characters, discard them.
      break;
  }
  // DEBUG_SERIAL.println(pos);
  // DEBUG_SERIAL.println(ch);
  return false;
}

///////////////////////////////////
// command parser and switcher, 
// breaks command line in pieces, 
// rejects invalid ones, 
// switches to the right command
void parseCommand(char* inputStr) {
  byte hasArgument=false;
  long argument=0;
  int address;
  byte pos=0;
  byte length=strlen(inputStr);
  if(length<2) goto beep;   // not enough characters

  // If it starts with I, treat as an I2C command
  if (inputStr[0]=='I') {
    pos++; 
    if(!length>pos) hasArgument=false; // end of string reached, no arguments
    else  {
      for(byte i=pos; i<length; i++) if(!isdigit(inputStr[i])) goto beep; // invalid, end of string contains non-numerial arguments
      argument=atol(inputStr+pos);    // that's the numerical argument after the command character
      hasArgument=true;
    }
    if(!hasArgument) goto beep;       // invalid, no argument after command
    //DEBUG_SERIAL.print(F("Arg: "));
    //DEBUG_SERIAL.println(argument);
    doIcommand(argument);  
    return; // exit    
  }
  
  // get the address, one or two digits
  char addrStr[3];
  if(!isdigit(inputStr[pos])) goto beep;  // invalid, first char not a digit
  addrStr[pos]=inputStr[pos];
  pos++;                              // pos=1
  if(isdigit(inputStr[pos])) {        // add second digit address if it's there 
    addrStr[pos]=inputStr[pos];
    pos++;                            // pos=2
  }
  addrStr[pos]='\0';                  // add null terminator
  address= atoi(addrStr);        // extract the address
  
  // check for more
  if(!length>pos) goto beep;            // invalid, no command after address

  // echo the command out, so the other logic gets it
  JEDI_SERIAL.println(inputStr);
  
  // special case of M commands, which take a string argument
  if(inputStr[pos]=='M') {
    pos++;
    if(!length>pos) goto beep;     // no message argument
    doMcommand(address, inputStr+pos);   // pass rest of string as argument
    return;                     // exit
  }

  // other commands, get the numerical argument after the command character

  pos++;                             // need to increment in order to peek ahead of command char
  if(!length>pos) hasArgument=false; // end of string reached, no arguments
  else  {
    for(byte i=pos; i<length; i++) if(!isdigit(inputStr[i])) goto beep; // invalid, end of string contains non-numerial arguments
    argument=atoi(inputStr+pos);    // that's the numerical argument after the command character
    hasArgument=true;
  }
  
  // switch on command character
  switch(inputStr[pos-1]) {             // 2nd or third char, should be the command char
    case 'T':
      if(!hasArgument) goto beep;       // invalid, no argument after command
      doTcommand(address, argument);      
      break;
    case 'F':                           // adjust logic fade setting
      if(!hasArgument) goto beep;       // invalid, no argument after command
      doFcommand(address, argument);      
      break;
    case 'G':                           // adjust logic delay setting
      if(!hasArgument) goto beep;       // invalid, no argument after command
      doGcommand(address, argument);      
      break;            
    case 'H':                           // adjust logic hue setting
      if(!hasArgument) goto beep;       // invalid, no argument after command
      doHcommand(address, argument);      
      break;
    case 'J':                           // adjust logic brightness setting
      if(!hasArgument) goto beep;       // invalid, no argument after command
      doJcommand(address, argument);      
      break;  
    case 'D':                           // holos,D command is weird, does not need an argument, ignore if it has one
      //doDcommand(address);
      break;
    case 'P':                           // custom parameter (alphabet switching, palette cycling, settings saving etc)
      if(!hasArgument) goto beep;       // invalid, no argument after command
      doPcommand(address, argument);
      break;
    case 'R':                           // random styles for Logics (extra CuriousMarc command)
      if(!hasArgument) goto beep;       // invalid, no argument after command
      //doRcommand(address, argument);
      break;
    case 'S':                           // PSI State (extra CuriousMarc command)
      if(!hasArgument) goto beep;       // invalid, no argument after command
      //doScommand(address, argument);
      break;
    default:
      goto beep;                        // unknown command
      break;
  }
  
  return;                               // normal exit
  
  beep:                                 // error exit
    JEDI_SERIAL.write(0x7);             // beep the terminal, if connected
    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Special Effect Routines
/////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////
// Reset Utilities

// resets text scrolling except alphabet
void resetText(byte display) {
  // reset text
  //scrollPositions[display]= (display==2? 27 : 9);
  //textScrollCount[display]=0;  
}

// same for all the displays
void resetAllText() {
  //for(byte disp=0; disp<3; disp++) resetText(disp);
}

// forces exit from effects immediately
void exitEffects() {
  displayEffect=NORM;
  effectRunning=0;
}

// exit effects, reset scrolling, alphabet back to latin, mode to random
void resetDisplays() {
  resetAllText();
  exitEffects();
  for(byte disp=0; disp<3; disp++) {
    alphabetType[disp]=LATIN;
    displayState[disp]=NORM;
  }
}


//////////////////////////////////////
// Text Display Routines
/////////////////////////////////////

//////////////////////
// Set String
void setText(byte disp, const char* message)
{
  strncpy(logicText[disp], message, MAXSTRINGSIZE);
  logicText[disp][MAXSTRINGSIZE]=0; // just in case
}

//////////////////////
// command executers

// set text command
void doMcommand(int address, char* message){
  DEBUG_SERIAL.print(F("\nCmd: M Addr: "));
  DEBUG_SERIAL.print(address);
  DEBUG_SERIAL.print(F(" Arg: "));
  DEBUG_SERIAL.print(message);  
  /*if(address==0) {setText(0, message); setText(1, message); setText(2, message); resetAllText();}
  if(address==1) {setText(0, message); resetText(0);}
  if(address==2) {setText(1, message); resetText(1);}
  if(address==3) {setText(2, message); resetText(2);}  */  
}

/////////////////////////////////////////////////////////
///*****            I2C Event Function           *****///
/////////////////////////////////////////////////////////
///  This routine is run when an onRecieve event is   ///
///     triggered.  Multiple bytes of data may be     ///
///                    available.                     ///
/////////////////////////////////////////////////////////            
      
void i2cEvent(int howMany)
{  
  #if(DEBUG==1)
    DEBUG_SERIAL.println("I2C Event Received.");
  #endif
  I2CInput[I2CMAXCOMMANDLENGTH] = {};                                     // Ensures I2CInput is empty
  int i=0;
  while(Wire.available())  {                            // loop through all incoming bytes
    char inChar = (char)Wire.read();                    // Receive each byte as a character
    //DEBUG_SERIAL.print("Char: ");
    //DEBUG_SERIAL.println(inChar);
    if(i<I2CMAXCOMMANDLENGTH) {
       //I2CInput += inChar;                           // Add each Character to I2CInput
       if (isDigit(inChar)) {
        I2CInput[i] = inChar;
        //DEBUG_SERIAL.println(I2CInput[i]);
       } else {
        I2CInput[i] = 0; // empty out the rest of the array
        //DEBUG_SERIAL.println(I2CInput[i]);
       }
    }
    i++; 
  }
  for (i=i;i<I2CMAXCOMMANDLENGTH;i++)
    I2CInput[i] = 0;  // empty out the rest of the array
  //DEBUG_SERIAL.println(I2CInput);
  I2CComplete = true;                                // Once done, set a flag so the main loop can do something about it. 
}

// I2C commands
void doIcommand(long commandInput) {
  //DEBUG_SERIAL.print(F("I2C Command: "));
  //DEBUG_SERIAL.println(commandInput);
  updateEffect(commandInput);
}


// various commands for states and effects
void doTcommand(int address, int argument) {
  JEDI_SERIAL.print(F("\nCmd: T Addr: "));
  JEDI_SERIAL.print(address);
  JEDI_SERIAL.print(F(" Arg: "));
  JEDI_SERIAL.print(argument);    
  FastLED.show(); 
  //***  
  switch(argument) {
    case 0:    // test mode
      exitEffects();
      if(address==0) {displayState[0]=displayState[1]=displayState[2]=psiState[0]=psiState[1]=TEST; resetAllText();}
      /*if(address==1) {displayState[0]=TEST; resetText(0);}
      if(address==2) {displayState[1]=TEST; resetText(1);}
      if(address==3) {displayState[2]=TEST; resetText(2);}
      if(address==4) {psiState[0]=TEST;}
      if(address==5) {psiState[1]=TEST;}*/
      break;
    case 1:    // normal random mode, cancel effects too
      exitEffects();
      if(address==0) {displayState[0]=displayState[1]=displayState[2]=psiState[0]=psiState[1]=RANDOM; resetAllText();}
      updateEffect(0);// default sequence
      /*if(address==1) {displayState[0]=RANDOM; resetText(0);}
      if(address==2) {displayState[1]=RANDOM; resetText(1);}
      if(address==3) {displayState[2]=RANDOM; resetText(2);}
      if(address==4) {psiState[0]=RANDOM;}
      if(address==5) {psiState[1]=RANDOM;}*/
      break;
    case 2: case 3: case 5:    // alarm
      exitEffects();
      //displayEffect=ALARM;
      updateEffect(10503);// default sequence for Alarm
      break;   
    case 4:   // short circuit
      exitEffects();
      //displayEffect=FAILURE;
      updateEffect(20507);// default sequence for Failure
      break;
    case 6:   // leia
      exitEffects();
      //displayEffect=LEIA;
      updateEffect(30000);// default sequence for Leia
      break;   
    case 10:  // star wars
      if (BOARDTYPE != 0) {  // only works on new boards
        exitEffects();
        // reset text
        for(byte disp=0; disp<3; disp++) {
          resetText(disp);
          alphabetType[disp]=0;
          displayState[disp]=TEXT;
        }
        setText(0,"STAR    ");
        setText(1,"    WARS");
        setText(2,"STAR WARS   ");  
      }
      break;
    case 11:   // March
      exitEffects();
      updateEffect(40500);// default sequence for March
      break; 
    case 20:    // extra CuriousMarc command, to turn displays off.
      exitEffects();
      if(address==0) {displayState[0]=displayState[1]=displayState[2]=psiState[0]=psiState[1]=OFF; resetAllText();}
      if(address==1) {displayState[0]=OFF; resetText(0);}
      if(address==2) {displayState[1]=OFF; resetText(1);}
      if(address==3) {displayState[2]=OFF; resetText(2);}
      if(address==4) {psiState[0]=OFF;}
      if(address==5) {psiState[1]=OFF;}
      break;
    case 92:    // bargraph mode, does not cancel effects, but resets text
      exitEffects();
      if(address==0) {displayState[0]=displayState[1]=displayState[2]=BARGRAPH; resetAllText();}
      if(address==1) {displayState[0]=BARGRAPH; resetText(0);}
      if(address==2) {displayState[1]=BARGRAPH; resetText(1);}
      if(address==3) {displayState[2]=BARGRAPH; resetText(2);}
      break;
    case 100:    // text mode, cancel effects too
      exitEffects();
      if(address==0) {displayState[0]=displayState[1]=displayState[2]=TEXT; resetAllText();}
      if(address==1) {displayState[0]=TEXT; resetText(0);}
      if(address==2) {displayState[1]=TEXT; resetText(1);}
      if(address==3) {displayState[2]=TEXT; resetText(2);}
      break;
    default:
      exitEffects(); // default stops any running effect
      updateEffect(0);// default sequence for normal
      break;
  }
}

// holos commands
void doDcommand(int address) {
  JEDI_SERIAL.print(F("\nCmd: D Addr: "));
  JEDI_SERIAL.print(address); 
  // for turning off holos, not implemented
}

// logic fade setting
void doFcommand(int address, int argument) {
  JEDI_SERIAL.print(F("\nCmd: F Addr: "));
  JEDI_SERIAL.print(address);
  JEDI_SERIAL.print(F(" Arg: "));
  JEDI_SERIAL.print(argument);  
  if(address==3 || address==0) settings[0].rearFade=argument;
  if(address==1 || address==2 ||  address==0) settings[0].frontFade=argument;
}

// logic delay setting
void doGcommand(int address, int argument) {
  JEDI_SERIAL.print(F("\nCmd: G Addr: "));
  JEDI_SERIAL.print(address);
  JEDI_SERIAL.print(F(" Arg: "));
  JEDI_SERIAL.print(argument);  
  if(address==3 || address==0) settings[0].rearDelay=argument;
  if(address==1 || address==2 || address==0) settings[0].frontDelay=argument;
}

// logic brightness setting
void doJcommand(int address, int argument) {
  JEDI_SERIAL.print(F("\nCmd: J Addr: "));
  JEDI_SERIAL.print(address);
  JEDI_SERIAL.print(F(" Arg: "));
  JEDI_SERIAL.print(argument);  
  if(address==3 || address==0) { settings[0].rearBri=argument; calculateAllColors(settings[0].rearPalNum, 1, settings[0].rearBri); }
  if(address==1 || address==2 ||  address==0) { settings[0].frontBri=argument; calculateAllColors(settings[0].frontPalNum, 0, settings[0].frontBri);}
}

// logic hue
void doHcommand(int address, int argument) {
  JEDI_SERIAL.print(F("\nCmd: H Addr: "));
  JEDI_SERIAL.print(address);
  JEDI_SERIAL.print(F(" Arg: "));
  JEDI_SERIAL.print(argument);  
  if(address==3 || address==0) settings[0].rearHue=argument;
  if(address==1 || address==2 ||  address==0) settings[0].frontHue=argument;
}

// misc commands
void doPcommand(int address, int argument) {
  JEDI_SERIAL.print(F("\nCmd: P Addr: "));
  JEDI_SERIAL.print(address);
  JEDI_SERIAL.print(F(" Arg: "));
  JEDI_SERIAL.print(argument);  
  switch(argument) {
    case 60:    // latin
      if(address==0) {alphabetType[0]=alphabetType[1]=alphabetType[2]=LATIN;}
      if(address==1) {alphabetType[0]=LATIN;}
      if(address==2) {alphabetType[1]=LATIN;}
      if(address==3) {alphabetType[2]=LATIN;}
      break;
    case 61:    // Aurabesh
      if(address==0) {alphabetType[0]=alphabetType[1]=alphabetType[2]=AURABESH;}
      if(address==1) {alphabetType[0]=AURABESH;}
      if(address==2) {alphabetType[1]=AURABESH;}
      if(address==3) {alphabetType[2]=AURABESH;}
      break;
    case 70:    // Reset to default logic colors and speeds
        if(address==1||address==2||address==0) {
          settings[0].frontFade=DFLT_FRONT_FADE;
          settings[0].frontDelay=DFLT_FRONT_DELAY;
          settings[0].frontHue=DFLT_FRONT_HUE;
          settings[0].frontPalNum=DFLT_FRONT_PAL;
          settings[0].frontBri=DFLT_FRONT_BRI;
          calculateAllColors(settings[0].frontPalNum, 0, settings[0].frontBri);
        }
        if(address==3||address==0) {
          settings[0].rearFade=DFLT_REAR_FADE;
          settings[0].rearDelay=DFLT_REAR_DELAY;
          settings[0].rearHue=DFLT_REAR_HUE;
          settings[0].rearPalNum=DFLT_REAR_PAL;
          settings[0].rearBri=DFLT_REAR_BRI;
          calculateAllColors(settings[0].rearPalNum, 1, settings[0].rearBri);
        }
      break;  
    case 71:    // Load logic colors and speeds from EEPROM
      readSettingsFromEEPROM();
      break; 
    case 72:    // Save logic colors and speeds to EEPROM
      writeSettingsToEEPROM();
      break;     
    case 73:    // Cycle to next color palette
      if(address==1||address==2||address==0) {
        settings[0].frontPalNum++;
        if (settings[0].frontPalNum>=PAL_COUNT) settings[0].frontPalNum=0;
        JEDI_SERIAL.println(F("calc front pal"));
        calculateAllColors(settings[0].frontPalNum, 0, settings[0].frontBri);
      }
      if(address==3||address==0) {
        settings[0].rearPalNum++;
        if (settings[0].rearPalNum>=PAL_COUNT) settings[0].rearPalNum=0;
        calculateAllColors(settings[0].rearPalNum, 1, settings[0].rearBri);
      }
      break; 
    default:
      // should I do back to latin on default argument?
      break;
  }  
}

// random styles for Logics (extra CuriousMarc command)
void doRcommand(int address, int argument) {
  JEDI_SERIAL.print(F("\nCmd: R Addr: "));
  JEDI_SERIAL.print(address);
  JEDI_SERIAL.print(F(" Arg: "));
  JEDI_SERIAL.print(argument);    
  if(address==0) {randomStyle[0]=randomStyle[1]=randomStyle[2]=argument;}
  if(address==1) {randomStyle[0]=argument;}
  if(address==2) {randomStyle[1]=argument;}
  if(address==3) {randomStyle[2]=argument;} 
}

// PSI State (extra CuriousMarc command)
void doScommand(int address, int argument) {
  JEDI_SERIAL.print(F("\nCmd: S Addr: "));
  JEDI_SERIAL.print(address);
  JEDI_SERIAL.print(F(" Arg: "));
  JEDI_SERIAL.print(argument);  
  switch(argument) {
    case 0:    // test, all PSI leds on
      if(address==0) {psiState[0]=psiState[1]=TEST;}
      if(address==4) {psiState[0]=TEST;}
      if(address==5) {psiState[1]=TEST;}
      break;
    case 1:    // normal, random mode
      if(address==0) {psiState[0]=psiState[1]=RANDOM;}
      if(address==4) {psiState[0]=RANDOM;}
      if(address==5) {psiState[1]=RANDOM;}
      break;
    case 2:    // color 1
      if(address==0) {psiState[0]=psiState[1]=COLOR1;}
      if(address==4) {psiState[0]=COLOR1;}
      if(address==5) {psiState[1]=COLOR1;}
      break;
    case 3:    // color 2
      if(address==0) {psiState[0]=psiState[1]=COLOR2;}
      if(address==4) {psiState[0]=COLOR2;}
      if(address==5) {psiState[1]=COLOR2;}
      break;
    case 4:    // off 
      if(address==0) {psiState[0]=psiState[1]=OFF;}
      if(address==4) {psiState[0]=OFF;}
      if(address==5) {psiState[1]=OFF;}
      break;      
    default:
      // should I do back to latin on default argument?
      break;
  }  
}



// =======================================================================================
// This runs in the loop to get the displayEffectVal currently set and process accordingly

void updateEffect(long inputNum) {
  displayEffectVal = inputNum;
}

void runDisplayEffect() {
  if (displayEffectVal == 0)
    displayEffectVal = NORMVAL;
  
  int selectLogic = displayEffectVal / 1000000;
  int selectSequence;
  int selectColor;
  int selectSpeed;
  int selectLength;
  int effectHue;

  selectLogic = displayEffectVal / 1000000;
  selectSequence = (displayEffectVal % 1000000) / 10000;
  selectColor = (displayEffectVal % 10000) / 1000;
  selectSpeed = (displayEffectVal % 1000) / 100;
  selectLength = (displayEffectVal % 100);

  if (selectSequence == 1)
    displayEffect = ALARM;
  else if (selectSequence == 2)
    displayEffect = FAILURE;
  else if (selectSequence == 3)
    displayEffect = LEIA;
  else if (selectSequence == 4)
    displayEffect = MARCH;
  else if (selectSequence == 5)
    displayEffect = SINGLE;
  else if (selectSequence == 6)
    displayEffect = FLASH;
  else if (selectSequence == 7)
    displayEffect = FLIPFLOP;
  else if (selectSequence == 8)
    displayEffect = FLIPFLOPALT;
  else if (selectSequence == 9)
    displayEffect = COLORSWAP;
  else if (selectSequence == 10)
    displayEffect = RAINBOW;
  else if (selectSequence == 11)
    displayEffect = REDALERT;   
  else if (selectSequence == 12)
    displayEffect = MICBRIGHT;  
  else if (selectSequence == 13)
    displayEffect = MICRAINBOW; 
  else if (selectSequence == 98)
    displayEffect = LIGHTSOUT;
  else if (selectSequence == 0)
    displayEffect = NORM;
  else
    displayEffect = NORM;

  // DEBUG_SERIAL.println(displayEffect);

  if (selectColor == 1)
    effectHue = 0;
  else if (selectColor == 2)
    effectHue = 26;
  else if (selectColor == 3)
    effectHue = 42;
  else if (selectColor == 4)
    effectHue = 85;
  else if (selectColor == 5)
    effectHue = 128;
  else if (selectColor == 6)
    effectHue = 170;
  else if (selectColor == 7)
    effectHue = 202;
  else if (selectColor == 8)
    effectHue = 213;
  else if (selectColor == 9)
    effectHue = 228;
  else {
    // default to red
    effectHue = 0;
    // should put a default here
  }
    
  //DEBUG_SERIAL.print(F("displayEffect "));
  //DEBUG_SERIAL.println(displayEffect);
  //DEBUG_SERIAL.print(F("length "));
  //DEBUG_SERIAL.println(selectLength);

  if (displayEffect==NORM) {
        if (previousEffect!=NORM) {
          //we were doing something else last time around, go back to our normal settings to be on the safe side
          readSettingsFromEEPROM();
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          statusDelay=1500;
        }
        //update all them thar LEDs for standard logic patterns...
        if (PCBVERSION == 0) {
          for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
          for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        } else {
          for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        }
        for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,settings[0].rearBri);
        previousEffect=NORM;
  }
  
  else if (displayEffect==ALARM) {
        if (previousEffect!=ALARM) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          settings[0].frontPalNum=2; settings[0].frontHue=170; settings[0].rearPalNum=2; settings[0].rearHue=72;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          effectStartMillis=currentMillis;
          statusDelay=250*selectSpeed;; //we'll flipflop hues
        }
        if (flipflop==0) {
          if (selectColor == 0) {
            readSettingsFromEEPROM();
            calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
            calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          } else {
            settings[0].frontPalNum=2; settings[0].frontHue=effectHue; settings[0].rearPalNum=2; settings[0].rearHue=effectHue;
            calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
            calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          }
        }
        else {
          settings[0].rearHue=0;
          settings[0].frontHue=0;
          settings[0].frontPalNum=2; settings[0].frontHue=0; settings[0].rearPalNum=2; settings[0].rearHue=0;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
        }
        //maybe link brightness to the microphone?
        microphoneRead();
        if (PCBVERSION == 0) {
          for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,peakVal); 
          for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,peakVal); 
        } else {
          for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,peakVal); 
        }
        for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,peakVal);
        
        if (selectLength > 0)
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=ALARM;
  } 
  else if (displayEffect==MARCH) {
        //Imperial March (non-beep version) can be divided into a few distinct segments...
        //note: version used (from Padawan sound archive) includes 500ms of silence at start
        //     0-09800 (9800) = intro
        // 09800-14500 (4700)
        // 14500-19300 (4800) //notes every 700ms
        // 19300-28800 (9500)
        // 28800-38300 (9500)
        // 38300-45300 (7000)
        // 45300-48300 (3000) = wind down  
        //unsigned int marchSegment[]={ 0,9800,14500,19300,28800,38300,45300,48300  };
        //we'll split the front and rear logics into 2 sections (0-39 and 40-79 for fronts, 0-47 and 48-95 for rear)
        //and alternate brightness of each section as the song hits each "step" (every 700ms or so during each segment)   
        if (previousEffect!=MARCH) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          //change both palettes to monotone red, and hue shift to 0 (so it actually is red)
          if (selectColor==0) effectHue = 0; // default to red to start if nothing set
          settings[0].frontPalNum=2; settings[0].frontHue=effectHue; settings[0].rearPalNum=2; settings[0].rearHue=effectHue;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          effectStartMillis=currentMillis;
          statusDelay=150*selectSpeed;
        }
        effectMillis=currentMillis-effectStartMillis;
        //change color hue for each marchSegment...
        for (byte i=1; i<8; i++) {
          if (effectMillis>marchSegment[i-1] && effectMillis<marchSegment[i]) {
            if (selectColor == 0) {
              // switch colors on segments if no specific color set
              settings[0].frontHue=((i-1)*32);
              settings[0].rearHue=((i-1)*32);
            } else {
              settings[0].frontHue=effectHue;
              settings[0].rearHue=effectHue;
            }
            if (i==7) statusDelay=175; //make the flipflop really fast for the last segment
          }
        }
        //alternate brightness of sections(re-uses the flipFlop value from our statusLED)...
        /*
        if (flipflop==0) {
          for ( byte i = 0; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50); 
          for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255);         
          for ( byte i = 0; i < 48; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 48; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
        } else {
          for ( byte i = 0; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
          for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50);         
          for ( byte i = 0; i < 48; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 48; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);  
        }*/
        if (flipflop==0) {
          if (PCBVERSION == 0) {
            for ( byte i = 8; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255);
          } else {
            for ( byte i = 0; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255);   
          }
          for ( byte i = 0; i < 8; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 8; i < 16; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 16; i < 24; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 24; i < 32; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 32; i < 40; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 40; i < 48; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 48; i < 56; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 56; i < 64; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 64; i < 72; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 72; i < 80; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 80; i < 88; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 88; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
        } else {
          if (PCBVERSION == 0) {
            for ( byte i = 8; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50);
          } else {
            for ( byte i = 0; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50);   
          }
          for ( byte i = 0; i < 8; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 8; i < 16; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 16; i < 24; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 24; i < 32; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 32; i < 40; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 40; i < 48; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 48; i < 56; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 56; i < 64; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 64; i < 72; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 72; i < 80; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 80; i < 88; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 88; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50); 
        }
        //
        if (currentMillis-effectStartMillis>=MARCHduration) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=MARCH;
  }
  else if (displayEffect==LEIA) {
        // for the Leia message we'll change both logics to hologram colors (palette 2 with a hue shift of 60)
        // then we'll do something with the microphone
        if (previousEffect!=LEIA) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          settings[0].frontPalNum=2; settings[0].frontHue=60; settings[0].rearPalNum=2; settings[0].rearHue=60;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          effectStartMillis=currentMillis;
        }
        microphoneRead();
        if (peakVal>127) {
          // DEBUG_SERIAL.println(peakVal);
          settings[0].frontHue=60+(map(peakVal,127,255,0,20)); //shift the hue very slightly
          settings[0].rearHue=60+(map(peakVal,127,255,0,20));
          if (PCBVERSION == 0) {
            for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
            for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
          } else {
            for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
          }
          for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
        }
        else {
          settings[0].frontHue=60;
          settings[0].rearHue=60;
          if (PCBVERSION == 0) {
            for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,127); 
            for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,127); 
          } else {
            for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,127); 
          }
          for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,127);
        }    
        if (selectLength > 0) {
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        } else {   
          if (currentMillis-effectStartMillis>=LEIAduration) updateEffect(NORMVAL); //go back to normal operation if its time
        }
        previousEffect=LEIA;
  }
  else if (displayEffect==FAILURE) {
        //start off with Fade and Delay settings low (logic patterns fast), slow them down towards the end and then fade out
        if (previousEffect!=FAILURE) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          settings[0].frontFade=0; settings[0].frontDelay=0; settings[0].rearFade=0; settings[0].rearDelay=0;
          effectStartMillis=currentMillis;
        }
        effectMillis=currentMillis-effectStartMillis;
        // FAILUREduration is 10000 milliseconds by default, and the effect sequence should be roughly timed to the "128 screa-3.mp3" file
        //   0-1800 = scream
        //1800-5500 = glitch
        //5500-6500 = fade out
        if (effectMillis>1800 && effectMillis<5500) {
          //during this 'glitch' period, we'll cycle the color hues
          settings[0].frontHue++;
          settings[0].rearHue++;
        }
        else if (effectMillis>5500 && effectMillis<6500) {
          //briVal starts at around 255, and drops down to 0 as we approach 6500 millis.
          //this portion lasts 1000 millis, so we'll scale brightness of both logics to a value related to this period
          settings[0].frontBri= map( (effectMillis-5500), 1000, 0 , 0, 255 );
          settings[0].rearBri= map( (effectMillis-5500), 1000, 0 , 0 , 255 );
        }
        else if (effectMillis>=FAILUREduration) updateEffect(NORMVAL); //go back to normal operation if its time    
        previousEffect=FAILURE;
        if (PCBVERSION == 0) {
          for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
          for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        } else {
          for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        }; 
        for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,settings[0].rearBri);
  }
  else if (displayEffect==SINGLE) {
        if (previousEffect!=SINGLE) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          settings[0].frontPalNum=2; settings[0].frontHue=effectHue; settings[0].rearPalNum=2; settings[0].rearHue=effectHue;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          effectStartMillis=currentMillis;
        }
        settings[0].frontHue=effectHue;
        settings[0].rearHue=effectHue;
        if (PCBVERSION == 0) {
          for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
          for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        } else {
          for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        }  
        for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,settings[0].rearBri);  
        // DEBUG_SERIAL.println(currentMillis-effectStartMillis);      
        if (selectLength > 0)
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=SINGLE;
  }
  else if (displayEffect==FLIPFLOP) {
        //we'll split the front and rear logics into 2 sections 
        //and alternate brightness of each section as the song hits each "step" (every 700ms or so during each segment)   
        if (previousEffect!=FLIPFLOP) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          //change both palettes to selected color
          settings[0].frontPalNum=2; settings[0].frontHue=effectHue; settings[0].rearPalNum=2; settings[0].rearHue=effectHue;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          effectStartMillis=currentMillis;
          statusDelay=200*selectSpeed;
        }
        effectMillis=currentMillis-effectStartMillis;
        settings[0].frontHue=effectHue;
        settings[0].rearHue=effectHue;
        //alternate brightness of sections(re-uses the flipFlop value from our statusLED)...
        if (flipflop==0) {
          if (PCBVERSION == 0) {
            for ( byte i = 8; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255);
          } else {
            for ( byte i = 0; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255);   
          }
          for ( byte i = 0; i < 48; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 48; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
        } else {
          if (PCBVERSION == 0) {
            for ( byte i = 8; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50);
          } else {
            for ( byte i = 0; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50);   
          }  
          for ( byte i = 0; i < 48; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 48; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);  
        }
        //
        if (selectLength > 0)
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=FLIPFLOP;
  }
  else if (displayEffect==FLIPFLOPALT) {
        //we'll split the front and rear logics into 2 sections 
        //and alternate brightness of each section as the song hits each "step"   
        if (previousEffect!=FLIPFLOPALT) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          //change both palettes to selected color
          settings[0].frontPalNum=2; settings[0].frontHue=effectHue; settings[0].rearPalNum=2; settings[0].rearHue=effectHue;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          effectStartMillis=currentMillis;
          statusDelay=200*selectSpeed;
        }
        effectMillis=currentMillis-effectStartMillis;
        settings[0].frontHue=effectHue;
        settings[0].rearHue=effectHue;
        //alternate brightness of sections(re-uses the flipFlop value from our statusLED)...
        if (flipflop==0) {
          if (PCBVERSION == 0) {
            for ( byte i = 8; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255);
          } else {
            for ( byte i = 0; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255);   
          }
          for ( byte i = 0; i < 8; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 8; i < 16; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 16; i < 24; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 24; i < 32; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 32; i < 40; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 40; i < 48; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 48; i < 56; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 56; i < 64; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 64; i < 72; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 72; i < 80; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 80; i < 88; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 88; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
        } else {
          if (PCBVERSION == 0) {
            for ( byte i = 8; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50);
          } else {
            for ( byte i = 0; i < (FRONT_LED_COUNT / 2); i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
            for ( byte i = (FRONT_LED_COUNT / 2); i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50);   
          }        
          for ( byte i = 0; i < 8; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 8; i < 16; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 16; i < 24; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 24; i < 32; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 32; i < 40; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 40; i < 48; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 48; i < 56; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 56; i < 64; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 64; i < 72; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 72; i < 80; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
          for ( byte i = 80; i < 88; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
          for ( byte i = 88; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50); 
        }
        //
        if (selectLength > 0)
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=FLIPFLOPALT;
  }
  else if (displayEffect==FLASH) {
        // swap between color at full brightness and off every flipflop  
        if (previousEffect!=FLASH) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          //change both palettes to selected color
          settings[0].frontPalNum=2; settings[0].frontHue=effectHue; settings[0].rearPalNum=2; settings[0].rearHue=effectHue;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          effectStartMillis=currentMillis;
          statusDelay=250*selectSpeed;
        }
        effectMillis=currentMillis-effectStartMillis;
        settings[0].frontHue=effectHue;
        settings[0].rearHue=effectHue;
        //alternate brightness of sections(re-uses the flipFlop value from our statusLED)...
        if (flipflop==0) {
          if (PCBVERSION == 0) {
            for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
            for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255);
          } else {
            for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,255); 
          }       
          for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,255);
        } else {
          if (PCBVERSION == 0) {
            for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50); 
            for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50); 
          } else {
            for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,50); 
          }          
          for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,50);
        }
        //
        if (selectLength > 0)
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=FLASH;
  }
  else if (displayEffect==COLORSWAP) {
        if (previousEffect!=COLORSWAP) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          settings[0].frontPalNum=2; settings[0].frontHue=0; settings[0].rearPalNum=2; settings[0].rearHue=0;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          effectStartMillis=currentMillis;
          statusDelay=350*selectSpeed;; //we'll flipflop hues 
        }
        if (flipflop==0) {
          if (effectHue >= 128) {
            settings[0].rearHue=effectHue - 128;
            settings[0].frontHue=effectHue - 128;
          } else {
            settings[0].rearHue=effectHue + 128;
            settings[0].frontHue=effectHue + 128;
          }
        }
        else {
          settings[0].rearHue=effectHue;
          settings[0].frontHue=effectHue;
        } 
        if (PCBVERSION == 0) {
          for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
          for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        } else {
          for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        }        
        for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,settings[0].rearBri);
        if (selectLength > 0)
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=COLORSWAP;
  }
  else if (displayEffect==RAINBOW) {
        if (previousEffect!=RAINBOW) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          settings[0].frontPalNum=2; settings[0].frontHue=effectHue; settings[0].rearPalNum=2; settings[0].rearHue=effectHue;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
          effectStartMillis=currentMillis;
          statusDelay=200*selectSpeed; //speed of hue swap
          flipfloplast = 0;
        }
        effectMillis=currentMillis-effectStartMillis;
        if (flipflop != flipfloplast) {
          settings[0].frontHue=settings[0].frontHue + 20;
          settings[0].rearHue=settings[0].rearHue + 20;  
          flipfloplast = flipflop;
        }
        // DEBUG_SERIAL.println(settings[0].frontBri);
        if (PCBVERSION == 0) {
          for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
          for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        } else {
          for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        } 
        for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,settings[0].rearBri);  
        // DEBUG_SERIAL.println(currentMillis-effectStartMillis);      
        if (selectLength > 0)
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=RAINBOW;
  }
  else if (displayEffect==REDALERT) {
        if (previousEffect!=REDALERT) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
        }
        microphoneRead();
        // DEBUG_SERIAL.println((selectSpeed*256)/10);
        if (peakVal < (selectSpeed*256)/10) {
          readSettingsFromEEPROM();
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
        } else { // show color while loud
          settings[0].rearHue=effectHue;
          settings[0].frontHue=effectHue;
          settings[0].frontPalNum=2; settings[0].frontHue=effectHue; settings[0].rearPalNum=2; settings[0].rearHue=effectHue;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
        }
        if (PCBVERSION == 0) {
          for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
          for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        } else {
          for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        }
        for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,settings[0].rearBri);
        
        if (selectLength > 0)
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=REDALERT;
  }
  else if (displayEffect==MICBRIGHT) {
        if (previousEffect!=MICBRIGHT) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
        }
        microphoneRead();
        if (peakVal < (selectSpeed*256)/10) {
          settings[0].frontBri = (selectSpeed*256)/10;
          settings[0].rearBri = (selectSpeed*256)/10;
        } else {
          settings[0].frontBri = peakVal;
          settings[0].rearBri = peakVal;
        }
        if (selectColor == 0) { // use default colors
          readSettingsFromEEPROM();
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
        } else { // show color
          settings[0].rearHue=effectHue;
          settings[0].frontHue=effectHue;
          settings[0].frontPalNum=2; settings[0].frontHue=effectHue; settings[0].rearPalNum=2; settings[0].rearHue=effectHue;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
        }
        if (PCBVERSION == 0) {
          for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
          for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        } else {
          for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        }
        for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,settings[0].rearBri);
        
        if (selectLength > 0)
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=MICBRIGHT;
  }
  else if (displayEffect==MICRAINBOW) {
        if (previousEffect!=MICRAINBOW) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          settings[0].frontPalNum=2; settings[0].frontHue=effectHue; settings[0].rearPalNum=2; settings[0].rearHue=effectHue;
          calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
          calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
        }
        microphoneRead();
        settings[0].rearHue=(peakVal+effectHue)-256;
        settings[0].frontHue=(peakVal+effectHue)-256;
        if (PCBVERSION == 0) {
          for ( byte i = 8; i < FRONT_LED_COUNT / 2; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
          for ( byte i = FRONT_LED_COUNT / 2; i < FRONT_LED_COUNT - 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        } else {
          for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,settings[0].frontBri); 
        }
        for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,settings[0].rearBri);
        
        if (selectLength > 0)
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=MICRAINBOW;
  }
  else if (displayEffect==LIGHTSOUT) {
        if (previousEffect!=LIGHTSOUT) {
          readSettingsFromEEPROM(); // reset to defaults for a clean slate
          settings[0].frontPalNum=2; settings[0].frontHue=effectHue; settings[0].rearPalNum=2; settings[0].rearHue=effectHue;
          calculateAllColors(settings[0].frontPalNum,0,0);
          calculateAllColors(settings[0].rearPalNum,1,0);
          effectStartMillis=currentMillis;
        }
        settings[0].frontHue=effectHue;
        settings[0].rearHue=effectHue;
        for ( byte i = 0; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,0); 
        for ( byte i = 0; i < 96; i++) updateLED(pgm_read_byte(&rearLEDmap[i]),settings[0].rearHue,1,0);  
        // DEBUG_SERIAL.println(currentMillis-effectStartMillis);      
        if (selectLength > 0)
          if (currentMillis-effectStartMillis>=selectLength * 1000) updateEffect(NORMVAL); //go back to normal operation if its time
        previousEffect=LIGHTSOUT;
  }
  //  Turn off any stray LEDs on the front that may be on for older boards
  if (PCBVERSION == 0) {
    for ( byte i = 0; i < 8; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,0);
    for ( byte i = FRONT_LED_COUNT - 8; i < FRONT_LED_COUNT; i++) updateLED(pgm_read_byte(&frontLEDmap[i]),settings[0].frontHue,0,0);
  }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    

void loop() {
  currentMillis = millis();

    //STATUS LED
    //check current status, indicate it using the onboard status LED
    if (currentMillis-statusMillis>=statusDelay) {
      statusMillis=currentMillis;     
      if (flipflop==0) flipflop=1;       
      else flipflop=0; 
      if (adjMode==0) {
        //for standard mode, blink back & forth between blue and red (like a very basic front PSI)  
        if (flipflop==0) { 
          if (BOARDTYPE == 1) {
            statusLED[0] = 0x000022; FastLED.show();
            //statusLED[0] = 0x220000; FastLED.show();
          }
          else if (BOARDTYPE == 0)
            digitalWrite(STATUSLED_PIN,HIGH);
        } else {  
          if (BOARDTYPE == 1) {
            statusLED[0] = 0x220000; FastLED.show();
          }
          else if (BOARDTYPE == 0)
            digitalWrite(STATUSLED_PIN,LOW);
        }      
      }      
      else if (adjMode==1) {
        //for front adjustment mode on teensy, blink back & forth between blue and white    
        if (flipflop==0) { 
          if (BOARDTYPE == 1)
            statusLED[0] = 0x000022;
          else if (BOARDTYPE == 0) { // blink LED faster
            digitalWrite(STATUSLED_PIN, HIGH);
            delay(50);
            digitalWrite(STATUSLED_PIN, LOW);
            delay(50); 
          }
        }
        else { statusLED[0] = 0x222222; }      
      }
      else if (adjMode==3) {
        //for rear adjustment mode, blink back & forth between orange and green   
        if (flipflop==0) { statusLED[0] = 0x002200;  }
        else { statusLED[0] = 0x220800; }       
      }      
    }

  microphoneRead(100); //if you're going to ever use the microphone input, this should be run every loop (updates peakVal)
  //settings[0].frontHue=peakVal; //to test out the mic
  /*settings[0].frontBri=peakVal;
  calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);*/
  //sideScroll(0,0);

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// START ADJUSTMENTS
  //Where's me jumper? Set adjMode...
  // On the AVR Board, the presence of a jumper *ON* S3 goes to adjustment mode, on the Teensy it's a jumper *OFF* of FJUMP or RJUMP
  // 0 for no adjustments, 1 for front, 2 for rear
  if ((BOARDTYPE == 1 && digitalRead(FJUMP_PIN)==0) || (BOARDTYPE == 0 && digitalRead(FJUMP_PIN)==HIGH)) { //go to adjMode 1 (adj pots will tweak the front logic)        
     if (adjMode!=1) {
       delay(250); //dumb delay 
       if ((BOARDTYPE == 1 && digitalRead(FJUMP_PIN)==0) || (BOARDTYPE == 0 && digitalRead(FJUMP_PIN)==HIGH)) { //double check after the delay (incase jumped by mistake)
         #if (DEBUG==1)
          DEBUG_SERIAL.println("adjMode 1");
         #endif
         adjMode=1; statusDelay=500; //set the adjMode and delay time for the status LED
         adjMillis = millis(); //note when we start making adjustments
       }
     }
  }
  else if (BOARDTYPE == 1 && digitalRead(RJUMP_PIN)==0) { //go to adjMode 3 (adj pots will tweak the rear logic) (Teensy Only)
    if (adjMode!=3) {
      delay(250); //dumb delay  
      if (BOARDTYPE == 1 && digitalRead(RJUMP_PIN)==0) { //double check after the delay (incase jumped by mistake)  
        #if (DEBUG==1)
          DEBUG_SERIAL.println("adjMode 3");
        #endif
        adjMode=3; statusDelay=500;
        adjMillis = millis();
      }
    }
  }
  if ((adjMode!=0) && ((BOARDTYPE == 1 && digitalRead(FJUMP_PIN)==1 && digitalRead(RJUMP_PIN)==1) || (BOARDTYPE == 0 && digitalRead(FJUMP_PIN)!=HIGH))) { //go back to normal mode (adj pots disabled)
      delay(250); //delay for debounce
      if ((BOARDTYPE == 1 && digitalRead(FJUMP_PIN)==1 && digitalRead(RJUMP_PIN)==1) || (BOARDTYPE == 0 && digitalRead(FJUMP_PIN)!=HIGH)) { //double check after the delay (incase jumped by mistake)  
        //adjMode just went back to 0
        #if (DEBUG==1)
          DEBUG_SERIAL.println("adjMode 0");
        #endif
        statusLED[0] = 0x110022; FastLED.show();
        delay(1000);
        adjMode=0;
        statusDelay=1500;
      }   
  }
  if (adjMode>0) {
    //are we adjusting front or rear? check it and then set to settings[0] values to match the trimpots current states
    /*if (currentMillis-adjMillis>60000) {
      //we've been in adj mode too long, save and freeze things. this is to force the user to remove the jumper from the adj pin (that'd complicate things on next startup)
      writeSettingsToEEPROM();
      while (1==1) { //blink everything on and off forever and ever...
        FastLED.setBrightness(0);
        FastLED.delay(1000);
        FastLED.setBrightness(MAX_BRIGHTNESS);
        FastLED.delay(1000);
      }
    }*/
    if (prevAdjMode==0) prevBrightness=map(analogRead(briPin), 0, 1023, MIN_BRIGHTNESS, settings[0].maxBri);
    if (adjMode==1) {
      prevPalNum=settings[0].frontPalNum;
      settings[0].frontBri=map(analogRead(briPin), 0, 1023, MIN_BRIGHTNESS, settings[0].maxBri); //we want this to map to the settings[0].maxBri value (which is MAX_BRIGHTNESS by default)     
      settings[0].frontDelay=analogRead(delayPin)/4; //0-255
      settings[0].frontFade=analogRead(fadePin)/128; //0-8    
      settings[0].frontHue=analogRead(huePin)/4;     //0-255
      //if the PAL_PIN was low, switch the palette number
      if (!digitalRead(PAL_PIN)) {
        delay(100);
        settings[0].frontPalNum++;
        if (settings[0].frontPalNum>=PAL_COUNT) settings[0].frontPalNum=0;
      }      
      //if brightness or palette setting changed from this loop to last, recalculate allColors... (this is done sparingly so it doesn't delay the patterns)
      if (settings[0].frontBri!=prevBrightness||settings[0].frontPalNum!=prevPalNum) calculateAllColors(settings[0].frontPalNum,0,settings[0].frontBri);
      prevBrightness=settings[0].frontBri;
    }
    else if (adjMode==3) {
      prevPalNum=settings[0].rearPalNum;
      settings[0].rearBri=map(analogRead(briPin), 0, 1023, MIN_BRIGHTNESS, settings[0].maxBri);
      settings[0].rearDelay=analogRead(delayPin)/4; //0-255
      settings[0].rearFade=analogRead(fadePin)/128; //0-8     
      settings[0].rearHue=analogRead(huePin)/4;     //0-255
      //if the PAL_PIN was low, switch the palette number
      if (!digitalRead(PAL_PIN)) {
        delay(500);
        settings[0].rearPalNum++;
        if (settings[0].rearPalNum>=PAL_COUNT) settings[0].rearPalNum=0;
      } 
      //if brightness setting changed from this loop to last, recalculate allColors
      if (settings[0].rearBri!=prevBrightness||settings[0].rearPalNum!=prevPalNum) calculateAllColors(settings[0].rearPalNum,1,settings[0].rearBri);
      prevBrightness=settings[0].rearBri;
    }    
  }
  else if (adjMode==0 && prevAdjMode!=0) {
    //adjMode just went back to 0, write settings to eeprom
    statusLED[0] = 0x110022; FastLED.show();
    #if (DEBUG==1)
      DEBUG_SERIAL.println("Writing EEPROM"); delay(100);
    #endif
    for (byte i=0; i<5; i++){ statusLED[0] = 0x222200; FastLED.delay(200); statusLED[0] = 0x000000; FastLED.delay(200);}
    writeSettingsToEEPROM();
    if (BOARDTYPE == 1) {
      statusLED[0] = 0x110022; FastLED.delay(1000); //Teensy - a purple status LED tells us that we're all done
    } else if (BOARDTYPE == 0) { // AVR flutter the status LED to show we are done
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
      digitalWrite(STATUSLED_PIN, HIGH);
      delay(20);
      digitalWrite(STATUSLED_PIN, LOW);
      delay(20);
    }
  }
  prevAdjMode=adjMode;
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// END ADJUSTMENTS

  // check for completed I2C commands
  if(I2CComplete) {
    //I2CInput.trim();    // Trim any white space off off inputString, including \r and \n as they are unecessary now.
    #if(DEBUG==1)
      DEBUG_SERIAL.print(F("I2C Command Received: "));
      DEBUG_SERIAL.println(I2CInput);
    #endif
    // do it
    doIcommand(atol(I2CInput));
    // doIcommand(I2CInput.toInt());
    // Clean up
    I2CInput[I2CMAXCOMMANDLENGTH];
    I2CComplete = false;
  }

  // listen to commands through the serial port
  if (JEDI_SERIAL.available() > 0) {         
    ch=JEDI_SERIAL.read();  // get input
    JEDI_SERIAL.print(ch);  // echo back
    delay(20);
    // DEBUG_SERIAL.print(ch);
    command_available=buildCommand(ch, cmdString);  // build command line
    if (command_available) {
      //DEBUG_SERIAL.print(F("Command: "));
      //DEBUG_SERIAL.println(cmdString);
      parseCommand(cmdString);  // interpret the command
      JEDI_SERIAL.print(F("\n> "));  // prompt again
      // DEBUG_SERIAL.print(F("\n> ")); // prompt again
    }
    // else JEDI_SERIAL.println("nope"); 
  }

  // run display effects here

  runDisplayEffect();
  
  FastLED.show();


  
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  finn 

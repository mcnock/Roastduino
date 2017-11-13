#include <Time.h>
#include <TimeLib.h>
#include <Adafruit_TFTLCD.h>
#include <max6675.h>
#include  <Adafruit_GFX.h>
#include <gfxfont.h>
#include "libraries\Average.h"
#include <EEPROM.h>
#include <Wire.h>
#include "Chrono.h"
#include "TouchScreen.h"
#include "B_MyTypes.h"

// ===========
//  DEFINES
// ===========

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define ORANGE  0xFD20
#define WHITE   0xFFFF

#define TFT_CSp 39 // Chip Select goes to Analog 3
#define TFT_CDp 41 // Command/Data goes to Analog 2
#define TFT_WRp 43 // LCD Write goes to Analog 1
#define TFT_RDp 45 // LCD Read goes to Analog 0
#define TFT_RSTp 47 // Can alternately just connect to Arduino's reset pin
//   D0 connects to digital pin 28  (Notice these are
//   D1 connects to digital pin 29   NOT in order!)
//   D2 connects to digital pin 22
//   D3 connects to digital pin 23
//   D4 connects to digital pin 24
//   D5 connects to digital pin 25
//   D6 connects to digital pin 26
//   D7 connects to digital pin 27
// These are the four touchscreen analog pins
#define YM A3   // can be a digital pin
#define XM A2  // must be an analog pin, use "An" notation!
#define YP A1  // xx must be an analog pin, use "An" notation!
#define XP A0   // can be a digital pin

// This is calibration data for the raw touch data to the screen coordinates

#define MINTOUCHPRESSURE 5
#define MAXTOUCHPRESSURE 1000



//THERMOCOUPLES
//#define SDIp    11
#define TSD1p    5
#define TCS1p    6
#define TSCKp    7
#define TSD2p    8
#define TCS2p    9
#define TSD3p    10
#define TCS3p    11
#define TSD4p    12
#define TCS4p    13

#define SSR1p  2
#define SSR2p  3
#define Buzzerp   4
#define LEDp      52
//annalog
#define CURFANap    A7
#define CURHEAT2ap  A6
#define CURHEAT1ap  A5
#define VOLT5ap     A4

//capacitive buttons
#define CP1p    30
#define CP2p    32
#define CP3p    34
#define CP4p    36

#define VIBRELAYp    31
#define FANRELAYp    33

#define INTEGRAL_EP 0
#define TEMPSCREENTOP_EP 1
#define GAIN_EP 2
#define TEMPLASTENDOFROAST_EP 3
//these EEPROM memoary are also used
//MySetpointsEprom[6];   {4,5,6,7,8,9}
//MySpanM  inutesEprom[6];   {10,11,12,13,14,15}
#define PROFILETIMESTAMP_EP 16
//17,18,19

#define OVERHEATFAN     250
#define OVERHEATCOIL    1100
#define BAUD            57600
#define TEMPCOOLINGDONE 170

#define AMROASTING          1
#define AMSTOPPED           2
#define AMAUTOCOOLING       3
#define AMOVERHEATEDCOIL    4
#define AMOVERHEATEDFAN     5
#define AMFANONLY           6
#define STATECHANGED        8

#define SETPOINTLINEID 0
#define ROLLAVGLINEID 1
#define AVGLINEID 2
#define COILLINEID 3

#define RELAYON    LOW
#define RELAYOFF  HIGH
// ===========
// DEFINITIONS
// ===========
Adafruit_TFTLCD tft(TFT_CSp, TFT_CDp, TFT_WRp, TFT_RDp, TFT_RSTp);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// ===============================================================================================================================================================================
// GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS
// ======================================================================================================================================================================================
int State;
int newState;
MAX6675 thermocouple1(TSCKp, TCS1p,  TSD1p);
MAX6675 thermocouple2(TSCKp, TCS2p,  TSD2p);
MAX6675 thermocouple3(TSCKp, TCS3p,  TSD3p);
MAX6675 thermocouple4(TSCKp, TCS4p,  TSD4p);
//global variablers for temp control
int Gain = 10; //read from eeprom



point TGainValue;
point TGainIncrease;
point TGainDecrease;
point TIntegralValue;
point TIntegralIncrease;
point TIntegralDecrease;

rect TouchExtent;
rect MappedExtent;
rect TsSetting;


int TTextClickBuffer = 5;
const point TTextClickBufferOffset =  { 4 , -5 }  ;


boolean ShowTouch = false;
double Integral = 0.1;  //read from eeprom


long unsigned IntegralLastTime = 0;
long unsigned IntegralSum = 0;
unsigned long PIDWindowStartTime;
boolean PIDNewWindow;
int ErrI = 0;
int Err = 0;
double Duty ;
double Setpoint ;
int PIDIntegralUdateTimeValue ;
int PIDWindowSize ;
int ManagingSSR ;


point MySetpoints[6];
int MySpanAccumulatedMinutes[6];
int MyMinuteSetpoints[20];
int MySetpointsEprom[]    = {3, 4, 5, 6, 7, 8};  //these are EEprom memory locations - not data
int MySpanMinutesEprom[]  = {10, 11, 12, 13, 14, 15};  //these are EEprom memory locations - not data
int MySpanMinutes[6] =       {  0,   4,   2,   7,    1,   1}; //actual values read from eeprom
int MyBaseSetpoints[6] =     {120, 390, 430, 450,  470, 500}; //actual values read from eeprom
unsigned long MyProfileTimeStamp;
int SetPointCount = 6;
double TempRoastDone = 0;


unsigned long TempScreenTop = 700;
const double TempYMax = 700; //actual value is set in program
const double PixelYSplit = 90;
const double TempYSplit = 400;
const double PixelYSplit2 = 170;
const double TempYSplit2 = 440;

  //we have 240 units...
  //240-120 is for < 400 >> 400/120  3.333 degrees per pixel
  //120 - 0 is for < 400 - 600 >> 200/120 1.66 degrees per pixel
const double TempPerPixL = TempYSplit/PixelYSplit;
const double TempPerPixM = (TempYSplit2 - TempYSplit)/(PixelYSplit2 - PixelYSplit);
const double TempPerPixH = (TempYMax - TempYSplit2)/(240.00 - PixelYSplit2);
  

int TimeScreenLeft = 14;
int TempLastEndOfRoast;
double TimeLastEndOfRoast;


int SetpointbeingAdjusted ;

char RoastName[10] = "Default";
byte RoastNumber = 0;

long IYscale;

int LoopsPerSecond;

Chrono RoastTime(Chrono::SECONDS);
Chrono SecondTimer(Chrono::MILLIS);
Chrono FlashTimer(Chrono::MILLIS);
//int SecondTimerValue = 1000;
Chrono LcdUdateTime(Chrono::MILLIS);
Chrono PIDIntegralUdateTime(Chrono::MILLIS);


int CapButActive = 0;

Average<float> AvgFanCurrent(10);
Average<float> AvgCoil1Amp(20);
Average<float> AvgCoil2Amp(20);
Average<int> TBeanAvgRoll(5);
Average<int> TCoilRoll(20);
int OVERHEATFANCount;
int OVERHEATCOILCount;
int TempReachedCount;
int TempSpikeCount;
int Readingskipped;

long PixelsPerMin;


buttonsetdef myButtonControl;
//buttondef* myButtonControl = 0;
//int myButtonControlCount = 0;
//rect myButtonControlrect;

buttonsetdef myButtonMenu1;

//buttondef* myButtonMenu1 = 0;
//int myButtonMenu1Count = 0;
//rect myButtonMenu1rect;

buttonsetdef myButtonMenu2;

//buttondef* myButtonMenu2 = 0;
//int myButtonMenu2Count = 0;
//rect myButtonMenu2rect;
boolean Menu2Showing;

int DUMMY;

//used when drawing lines. We support up to 3 lines (see line ID constants)
uint16_t LastXforLineID[4];
uint16_t LastYforLineID[4];
uint16_t LineColorforLineID[4];
int myLastGraph[320];



double RoastMinutes;
int TCoil;
int TBean1;
int TBean2;
int TFan;
double MaxVoltage, MaxVread, CurrentFan, CurrentHeat1, CurrentHeat2;
int TBeanAvg;

char Buf5[5];
char Buf6[6];
char Buf7[7];

boolean Flasher;
//PROGRAM

// =====================================================================================================================================================================
// SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP
// =====================================================================================================================================================================
void setup() {
  // Pin Configuration
  pinMode(VIBRELAYp, OUTPUT); pinMode(FANRELAYp, OUTPUT);
  pinMode(TFT_RSTp, INPUT_PULLUP);
  pinMode(SSR1p, OUTPUT); pinMode(SSR2p, OUTPUT);
  pinMode(Buzzerp, OUTPUT); pinMode(LEDp, OUTPUT);
  digitalWrite(Buzzerp, HIGH);

  pinMode(VOLT5ap, INPUT); pinMode(CURFANap, INPUT);
  pinMode(CURHEAT1ap, INPUT); pinMode(CURHEAT2ap, INPUT);
  //define CURFANap    7///#define CURHEAT2ap  6


  //set relays to high - cause that means off
  digitalWrite(FANRELAYp, RELAYOFF); digitalWrite(VIBRELAYp, RELAYOFF);
  Serial.begin(BAUD);
  delay(1000);
  MaxVread = 512; //this is the expected half way value
  RoastTime.stop();
  Gain =      EEPROM.read(GAIN_EP);
  Integral =  (double)EEPROM.read(INTEGRAL_EP) / 100;
  if (Integral > 1) Integral = 0.1 ;
//Serial.print("read Gain:");Serial.print(Gain);Serial.print(" Integral:");Serial.println(Integral);

  EEPROM.get(PROFILETIMESTAMP_EP , MyProfileTimeStamp);
//Serial.print("Read profile time stamp:");Serial.println(MyProfileTimeStamp);
  if (MyProfileTimeStamp == 0)   {
    MyProfileTimeStamp = millis();
    EEPROM.put(PROFILETIMESTAMP_EP , MyProfileTimeStamp);
  //Serial.print("Saving time stamp:");Serial.println(MyProfileTimeStamp);
  }
  TempLastEndOfRoast = ReadTempEprom (TEMPLASTENDOFROAST_EP, 0);
  for (int X = 1; X < SetPointCount; X++) {
    MySpanMinutes[X]  =  ReadIntEprom(MySpanMinutesEprom[X], X , 12, MySpanMinutes[X]);
    MyBaseSetpoints[X] = ReadTempEprom(MySetpointsEprom[X] , MyBaseSetpoints[X]);
  }
//Serial.println (LastXforLineID[2]);
  PixelsPerMin =  (int)(320 / TimeScreenLeft);
  
  
  
  
//Serial.println (LastXforLineID[2]);
  //TempScreenTop = ReadIntEprom(TEMPSCREENTOP_EP, 100, 500, 460);
 //TempScreenTop  = 450;
//Serial.print("  tempscreentop:");Serial.println(TempScreenTop);

  tft.reset();
  uint16_t identifier = tft.readID();
  if (identifier == 0x9325) {
  //Serial.println(F("Found ILI9325 LCD driver"));
  } else if (identifier == 0x9328) {
  //Serial.println(F("Found ILI9328 LCD driver"));
  } else if (identifier == 0x7575) {
  //Serial.println(F("Found HX8347G LCD driver"));
  } else if (identifier == 0x9341) {
  //Serial.println(F("Found ILI9341 LCD driver"));
  } else if (identifier == 0x8357) {
  //Serial.println(F("Found HX8357D LCD driver"));
  } else {
  //Serial.print(F("Unknown LCD driver chip: "));
    return;
  }

  SecondTimer.restart(0);
  FlashTimer.restart(0);
  tft.begin(identifier);

  graphProfile();

  ManagingSSR = 1;

  State = AMSTOPPED;
}

void loop()
{
  theloop();

}



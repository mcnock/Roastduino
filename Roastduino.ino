#include <Chrono.h>
#include <Time.h>
#include <TimeLib.h>
//#include <Adafruit_TFTLCD.h>
#include <max6675.h>
//#include <Adafruit_GFX.h>
//#include <gfxfont.h>
#include "Average.h"
#include <EEPROM.h>
#include <Wire.h>
#include "TouchScreen.h"
#include "B_MyTypes.h"
#include <UTFT.h>

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41

// Remember to change the model parameter to suit your display module!
UTFT myGLCD(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)


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

#define STATEROASTING          1
#define STATESTOPPED           2
#define STATECOOLING       3
#define STATEOVERHEATED    4

#define STATEFANONLY           6
#define STATENOFANCURRENT   7
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

// ===============================================================================================================================================================================
// GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS
// ======================================================================================================================================================================================
int State;
char StateName[9] = "12345678";
int newState;
MAX6675 thermocouple1(TSCKp, TCS1p,  TSD1p);
MAX6675 thermocouple2(TSCKp, TCS2p,  TSD2p);
MAX6675 thermocouple3(TSCKp, TCS3p,  TSD3p);
MAX6675 thermocouple4(TSCKp, TCS4p,  TSD4p);
//global variablers for temp control
int Gain = 10; //read from eeprom



point TGainValue;
point TIntegralValue;
point TIntegralReset;

rect TouchExtent;
rect MappedExtent;
rect TsSetting;

char Commandsp1[7] = "xxxxx ";
int iCommandsp1 = 0;

int spSelected = 0;

char Commandsp[7] = "xxxxx ";
int iCommandsp = 0;


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
double Dutyraw ;

int PIDIntegralUdateTimeValue ;
int PIDWindowSize ;

point MySetpointsTouchXY[6];

double MyMinuteTemperature[30];
int MySetpointsEprom[]    = {3, 4, 5, 6, 7, 8};  //these are EEprom memory locations - not data
int MySpanMinutesEprom[]  = {10, 11, 12, 13, 14, 15};  //these are EEprom memory locations - not data
setpoint MySetPoints[6] = {{0, 120}, {3, 410}, {5, 420}, {7, 425}, {10, 430}, {12, 450}};
int SetPointCount = 6;  //0,1,2,3,4,5
int TimeScreenLeft = 15;
int EndingSetPoint = 4;

double TempYMax = 800;

double TempYSplit2 = 440;

//double PixelYSplit2 = 150;
double PixelYSplit2 = 300;

double TempYSplit = 390;

//double PixelYSplit = 90;
double PixelYSplit = 180;

//we have 240 units...
//240-120 is for < 400 >> 400/120  3.333 degrees per pixel
//120 - 0 is for < 400 - 600 >> 200/120 1.66 degrees per pixel
//we have 480 units...
//480-240 is for < 400 >> 400/120  6.66 degrees per pixel
//240 - 0 is for < 400 - 600 >> 200/120 3.32 degrees per pixel


double TempPerPixL = TempYSplit / PixelYSplit;
double TempPerPixM = (TempYSplit2 - TempYSplit) / (PixelYSplit2 - PixelYSplit);
double TempPerPixH = (TempYMax - TempYSplit2) / (240.00 - PixelYSplit2);

int CurrentSetPointTemp = 0;
int BeforeTemp = 0;
int BeforeTime = 0;

long IYscale;

int LoopsPerSecond;

Chrono RoastTime(Chrono::SECONDS);
Chrono SecondTimer(Chrono::MILLIS);
Chrono FlashTimer(Chrono::MILLIS);
Chrono SerialInputTimer(Chrono::MILLIS);
Chrono Serial1InputTimer(Chrono::MILLIS);

//int SecondTimerValue = 1000;
Chrono LcdUdateTime(Chrono::MILLIS);
Chrono PIDIntegralUdateTime(Chrono::MILLIS);

//int CapButActive = 0;
//current is read every loop (200 per second) so 30  very short
Average<float> AvgFanCurrent(30);
Average<float> AvgCoil1Amp(30);
Average<float> AvgCoil2Amp(30);

//temps are read  once per second
Average<int> TBeanAvgRoll(5);
Average<int> TCoilRoll(60); //this is minute avg

int OVERHEATFANCount;
int OVERHEATCOILCount;
int TempReachedCount;
int TempSpikeCount;
int Readingskipped;

long PixelsPerMin;


buttonsetdef myHorizontalButtonControl;

buttonsetdef myButtonVertMenu1;

int DUMMY;

//used when drawing lines. We support up to 3 lines (see line ID constants)
uint16_t LastXforLineID[4];
uint16_t LastYforLineID[4];
uint16_t LineColorforLineID[4];
int myLastGraphYPixels[320];
int myLastGraphTemps[320];

double RoastMinutes = 0;
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
  Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println ("setup starting");

  // Pin Configuration
  pinMode(VIBRELAYp, OUTPUT); pinMode(FANRELAYp, OUTPUT);
  pinMode(SSR1p, OUTPUT); pinMode(SSR2p, OUTPUT);
  pinMode(Buzzerp, OUTPUT); pinMode(LEDp, OUTPUT);
  digitalWrite(Buzzerp, HIGH);

  pinMode(VOLT5ap, INPUT); pinMode(CURFANap, INPUT);
  pinMode(CURHEAT1ap, INPUT); pinMode(CURHEAT2ap, INPUT);
  //define CURFANap    7///#define CURHEAT2ap  6

  //set relays to high - cause that means off
  digitalWrite(FANRELAYp, RELAYOFF); digitalWrite(VIBRELAYp, RELAYOFF);

  delay(1000);
  MaxVread = 512; //this is the expected half way value
  RoastTime.stop();
  Gain =      EEPROM.read(GAIN_EP);
  Integral =  (double)EEPROM.read(INTEGRAL_EP) / 100;
  if (Integral > 1) Integral = 0.1 ;
  Serial.print("read Gain:");Serial.print(Gain);Serial.print(" Integral:");Serial.println(Integral);

  int accumulated = 0;
  for (int X = 0; X < SetPointCount; X++) {
    Serial.println(X); Serial.println( " ");
    if (X > 0) {
      //  =  ReadIntEprom(MySpanMinutesEprom[X], 1 , 10, 2);
      MySetPoints[X].SpanMinutes =  MySetPoints[X].Minutes -MySetPoints[X-1].Minutes;
      accumulated = accumulated + MySetPoints[X].SpanMinutes;
      MySetPoints[X].Minutes = accumulated;
    }
    else
    { MySetPoints[X].SpanMinutes = 0;
      MySetPoints[X].Minutes = 0;
    }
    Serial.println(MySetPoints[X].SpanMinutes);
    Serial.println( " ");

    Serial.println(MySetPoints[X].Minutes);
    Serial.println( " ");

    if (X > 0) {
      //MySetPoints[X].Temperature = ReadTempEprom(MySetpointsEprom[X] , MySetPoints[X].Temperature);
      if (MySetPoints[X].Temperature < MySetPoints[X - 1].Temperature) {
        MySetPoints[X].Temperature = MySetPoints[X - 1].Temperature * 1.1;
        Serial.println (" correcting low value of "); Serial.println (X); Serial.println(" to ");
        Serial.println (MySetPoints[X].Temperature);
      }
    }
    else
    {
      MySetPoints[X].Temperature = 150;
      
    }
    Serial.println(MySetPoints[X].Temperature);


  }
  //TempRoastDone = MySetPoints[EndingSetPoint].Temperature;
  //TimeRoastDone = MySetPoints[EndingSetPoint].Minutes;

  //Serial.println (LastXforLineID[2]);
  PixelsPerMin =  (int)(800 / TimeScreenLeft);

  SecondTimer.restart(0);
  FlashTimer.restart(0);

  myGLCD.InitLCD();
 // -------------------------------------------------------------
  pinMode(8, OUTPUT);  //backlight 
  digitalWrite(8, HIGH);//on
// -------------------------------------------------------------
  myGLCD.setFont(SmallFont);

  Serial.print("DisplayX:");Serial.print(myGLCD.getDisplayXSize());Serial.print(" DisplayY:");Serial.println(myGLCD.getDisplayYSize());

  graphProfile();


  

  State = STATESTOPPED;
  // Serial.println ("setup complete");
  //  Serial.flush();
}


void loop()
{
 delay(10000);
 
 graphProfile();
  
  return;
  theloop();

}

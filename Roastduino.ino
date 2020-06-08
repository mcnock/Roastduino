#include <EEPROM.h>

#include <UTouchCD.h>
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
#include <UTouch.h>

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41

// Remember to change the model parameter to suit your display module!
UTFT myGLCD(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch  myTouch(43, 42, 44, 45, 46);  //byte tclk, byte tcs, byte din, byte dout, byte irq


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



//#5 inch display shield  does not  use 30- 34, 10,12, or 13 


#define SSR1p       2
#define SSR2p       3
#define VIBRELAYp    4
#define FANRELAYp    5
#define FanPWM       6
#define LEDp        7


#define TSD1p    A15
#define TCS1p    A14
#define TSCKp    A13
#define TSD2p    A12
#define TCS2p    A11
#define TSD3p    A10
#define TCS3p    A9
#define TSD4p    A8
#define TCS4p    A7
#define CURFANap    A6
#define CURHEAT2ap  A5
#define CURHEAT1ap  A4
#define FANPOTp     A3
//#define			    A2
//#define			    A1
//#define			    A0






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





char Commandsp1[7] = "xxxxx ";
int iCommandsp1 = 0;

int spSelected = 0;

char Commandsp[7] = "xxxxx ";
int iCommandsp = 0;



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
int MySetpointsEprom[]    = {5, 10, 15, 20, 25, 30};  //these are EEprom memory locations - not data
setpoint MySetPoints[6] = {{0, 0}, {3, 390}, {5, 420}, {7, 425}, {10, 430}, {12, 450}};
int SetPointCount = 6;  //0,1,2,3,4,5
int TimeScreenLeft = 15;
int EndingSetPoint = 5;

double TempYMax = 800;

double TempYSplit2 = 440;

//double PixelYSplit2 = 150;
double PixelYSplit2 = 300;

double TempYSplit = 390;

//double PixelYSplit = 90;
double PixelYSplit = 180;



double TempPerPixL = 0;
double TempPerPixM = 0;
double TempPerPixH = 0;

int CurrentSetPointTemp = 0;
int BeforeTemp = 0;
int BeforeTime = 0;

//long IYscale;

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

buttonsetdef myButtonVertMenus[6];
int VerticalMenuShowing = 0;

int DUMMY;

//used when drawing lines. We support up to 3 lines (see line ID constants)
uint16_t LastXforLineID[4];
uint16_t LastYforLineID[4];
uint16_t LineColorforLineID[4];
int myLastGraphYPixels[320];
int myLastGraphTemps[320];
int  moveamount = -1;
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
  pinMode(LEDp, OUTPUT);
  
   pinMode(CURFANap, INPUT);
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

  PixelsPerMin =  (int)(800 / TimeScreenLeft);

  SecondTimer.restart(0);
  FlashTimer.restart(0);

  myGLCD.InitLCD();
 
  // -------------------------------------------------------------
  pinMode(8, OUTPUT);  //backlight 
  digitalWrite(8, HIGH);//on
// -------------------------------------------------------------
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  myGLCD.setFont(SmallFont);

  initializeButtonDefs();

  graphProfile();

  

  State = STATESTOPPED;
  // Serial.println ("setup complete");
  //  Serial.flush();
}


void loop()
{
 
  theloop();

}

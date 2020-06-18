#include <EEPROM.h>
#include <UTouchCD.h>
#include <Chrono.h>
#include <Time.h>
#include <TimeLib.h>
#include <max6675.h>
#include "Average.h"
#include <EEPROM.h>
#include <Wire.h>
#include "B_MyTypes.h"
#include <UTFT.h>
#include <UTouch.h>

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];


// Remember to change the model parameter to suit your display module!
UTFT myGLCD(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch  myTouch(43, 42, 44, 45, 46);  //byte tclk, byte tcs, byte din, byte dout, byte irq

// Assign human-readable names to some common 16-bit color values:

#define BLACK    0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define ORANGE   0xFBE0
#define GREEN   0x0400
#define BLUE    0x001F
#define SILVER    0xC618
#define GRAY    0x8410
#define MAROON    0x8000
#define YELLOW    0xFFE0
#define OLIVE   0x8400
#define LIME    0x07E0
#define AQUA    0x07FF
#define TEAL    0x0410
#define NAVY    0x0010
#define FUCHSIA   0xF81F
#define PURPLE    0x8010
#define VGA_TRANSPARENT 0xFFFFFFFF

//PIN  definitions
//#5 inch display shield  does not  use 30- 34, 10,12, or 13 
#define VIBRELAYp    2
#define FANRELAYp     3
#define FanPWMp       4
//#define burned out 5
#define SSR2p         6
#define SSR1p         7


//#define available 10
//#define available 12
//#define available 13

//#define available 30
//#define available 31
//#define available 32
//#define available 33
//#define available 34

#define TSD1p    A8 
#define TCS1p    A9
#define TSD2p    A10  //could share A8
#define TCS2p    A11
#define TSD3p    A12  //could share A8
#define TCS3p    A13  
#define TSD4p    A14  //could share A8
#define TCS4p    A15  

#define TSCKp    A7

#define CURFANap    A6
#define CURHEAT2ap  A5
#define CURHEAT1ap  A4 
#define FanOutIncp	A3
#define FanOutDirp  A2
#define FanOutCsp   A1
#define	fanPressurep A0


#define LEDp		 5  //bad pin will not do anthing


//EPROM MEMORORY
int SETPOINTTEMP_EP[]    = {5, 10, 15, 20, 25, 30};  //these are EEprom memory locations - not data
#define INTEGRAL_EP 0
#define GAIN_EP 2
#define FANSPEED_EP 1



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
#define FANSPEEDLINEID 4

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
int Gain = 100; //read from eeprom


//for python serial  commands
char Commandsp1[7] = "xxxxx ";
int iCommandsp1 = 0;
char Commandsp[7] = "xxxxx ";
int iCommandsp = 0;

int spSelected = 0;

int FanSpeedPWM=0;
int FanSpeedPWMStart=0;
int FanSpeedPWMAutoEnd=0;
int FanSpeedPWMAutoDecrease = 20;
bool FanSpeedPWMAutoMode = false;

int FanSpeedPWNDecreaseByMinutes = 6;

int FanSpeedResistanceLast=0;
int FanSpeedResistanceCurrent = 0;


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

double MyMinuteTemperature[30];
setpoint MySetPoints[6] = {{0, 100}, {3, 390}, {5, 420}, {7, 425}, {10, 430}, {12, 450}};
int SetPointCount = 6;  //0,1,2,3,4,5
int TimeScreenLeft = 15;
int EndingSetPoint = 5;

double TempYMax = 800;
double TempYSplit2 = 440;
double TempYSplit = 390;

double PixelYSplit2;
double PixelYSplit;
long PixelsPerMin;
double TempPerPixL = 0;
double TempPerPixM = 0;
double TempPerPixH = 0;

int CurrentSetPointTemp = 0;
int BeforeTemp = 0;
int BeforeTime = 0;


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
float AmpFanOffset;
float AmpCoil1Offset;
float AmpCoil2Offset;

//temps are read  once per second
Average<float> FanPressureRoll(5);
Average<int> TBeanAvgRoll(5);
Average<int> TCoilRoll(60); //this is minute avg

int OVERHEATFANCount;
int OVERHEATCOILCount;
int TempReachedCount;
int TempSpikeCount;
int Readingskipped;

buttonsetdef myControlMenuDef;
buttonsetdef myFanButtonControl;
buttonsetdef myButtonVertMenus[6];
int VerticalMenuShowing = 0;


//used when drawing lines. We support up to 3 lines (see line ID constants)
uint16_t LastXforLineID[5];
uint16_t LastYforLineID[5];
uint16_t LineColorforLineID[5];

int myLastGraphYPixels[800];
int myLastGraphTemps[800];
int  moveamount = -1;
double RoastMinutes = 0;
int TCoil;
int TBean1;
int TBean2;
int TFan;
double MaxVoltage, MaxVread, CurrentFan, CurrentHeat1, CurrentHeat2;
int TBeanAvg;


boolean Flasher;

// =====================================================================================================================================================================
// SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP
// =====================================================================================================================================================================
void setup() {
//Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println ("setup starting");

  // Pin Configuration
  pinMode(VIBRELAYp, OUTPUT); pinMode(FANRELAYp, OUTPUT);
  pinMode(SSR1p, OUTPUT); pinMode(SSR2p, OUTPUT);
  pinMode(LEDp, OUTPUT);
 
  pinMode(CURFANap, INPUT);
  pinMode(CURHEAT1ap, INPUT); pinMode(CURHEAT2ap, INPUT);
  pinMode(FanOutIncp, OUTPUT);
  pinMode(FanOutDirp, OUTPUT);
  pinMode(FanOutCsp, OUTPUT);

  pinMode(TSD1p, INPUT_PULLUP);
  pinMode(TSD2p, INPUT_PULLUP);
  pinMode(TSD3p, INPUT_PULLUP);
  pinMode(TSD4p, INPUT_PULLUP);


  pinMode(TCS1p, OUTPUT);
  pinMode(TCS2p, OUTPUT);
  pinMode(TCS3p, OUTPUT);
  pinMode(TCS4p, OUTPUT);


  pinMode(TSCKp, OUTPUT);
  //#define FanPWMp     5

  
  //set fan and vibrator relays to high - cause that means off
  digitalWrite(FANRELAYp, RELAYOFF); digitalWrite(VIBRELAYp, RELAYOFF);

  delay(1000);
  MaxVread = 512; //this is the expected half way value
  RoastTime.stop();

  
  Gain =      EEPROM.read(GAIN_EP);
  Integral =  (double)EEPROM.read(INTEGRAL_EP) / 100;
  if (Integral > 1) Integral = 0.1 ;
  Integral = 0.1;
  Gain = 100;
  SecondTimer.restart(0);
  FlashTimer.restart(0);


  // -------------------------------------------------------------
  myGLCD.InitLCD();
  pinMode(8, OUTPUT);  //backlight 
  digitalWrite(8, HIGH);//on
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
// -------------------------------------------------------------

  initializeVerticalMenus();

  State = STATESTOPPED;

   
  
  graphProfile();

  FanSpeedPWM = EEPROM.read(FANSPEED_EP);
  FanSpeedPWMStart = FanSpeedPWM;
  analogWrite(FanPWMp, FanSpeedPWM);
  updateFanOutputResistance();
  UpdateFanPWMBut();
  
  

}


void loop()
{
  theloop();

}

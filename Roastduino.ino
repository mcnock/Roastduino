//MCP4725
//line 91. Change names of fan analog output port
//line 350 set values of the analog output ports
//wire 4725 vcc and gnd to anolog output ports
//wire 4725 scl and sda to 20 and 20
//wire 4725 out and gnd to inputs of dc cd power modulator


//Lm386 (3)
//check voltage of ground to white and join.
//wire lm386 4 - 12 to 5 v
//wire lm386 input to input from shunt
//check if grounds are all connected
//wire ground to ground
//wire output ++ to annalog inputs 4,5,6


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
#include <Adafruit_MCP4725.h>
#include <stdio.h>  // for function sprintf

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegmentFull[];
extern uint8_t Grotesk24x48[];

// Remember to change the model parameter to suit your display module!
UTFT myGLCD(SSD1963_800480, 38, 39, 40, 41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch myTouch(43, 42, 44, 45, 46);           //byte tclk, byte tcs, byte din, byte dout, byte irq

Adafruit_MCP4725 dac;

// Assign human-readable names to some common 16-bit color values:

#define BLACK 0x0000
#define WHITE 0xFFFF

#define BLUE 0xF800  //255 0 0  red
#define RED 0x001F   //0  0  255  blud

#define ORANGE 0x0339   //0,102,204 med blue
#define BLUEMED 0xCB20  //204,102, 0 orange

#define LIME 0x07E0   //0,255,0
#define GREEN 0x05E0  //0,190,0

#define YELLOW 0x0FFF  //pair 255,255,0 (aqua)
#define AQUA 0x07FF    //pair 0,255,255 (yellow)

#define TAN 0x9E7F     // 152,204,255
#define LGBLUE 0xFE73  // 255,204,152 (tan)

#define PINK 0xFB3F  // 255,100,255

#define MAROON 0x0010    //128,0,255
#define BLUEDEEP 0xF810  //255,0,128

#define VGA_TRANSPARENT 0xFFFFFFFF

//#define MCP4725_ADDR 0x62
//#define MCP4725_ADDR 0x61
#define MCP4725_ADDR 0x60

//For devices with A0 pulled HIGH, use 0x61

//PIN  definitions
//#5 inch display shield  does not  use 30- 34, 10,12, or 13
#define VIBRELAYp 2
#define FANRELAYp 3

#define FanPWMp 4

//#define burned out 5
#define SSR2p 6
#define SSR1p 7


//#define available 10
//#define available 12
//#define available 13

//#define available 30
//#define available 31
//#define available 32
//#define available 33
//#define available 34

#define TSD1p A8
#define TCS1p A9
#define TSD2p A10  //could share A8
#define TCS2p A11
#define TSD3p A12  //could share A8
#define TCS3p A13
#define TSD4p A14  //could share A8
#define TCS4p A15

#define TSCKp A7

#define CURFAN_A6 A6
#define CURHEAT2_A5 A5
#define CURHEAT1_A4 A4

#define FanOutVcc_A3 A3
#define FanOutG_A4 A2
//#define FanOutIncp	A3
//define FanOutDirp  A2
//define FanOutCsp   A1

#define fanPressurep A0


#define LEDp 5  //bad pin will not do anthing


#define VBUT0 0
#define VBUT1 1
#define VBUT2 2
#define VBUT3 3
#define VBUT4 4
#define VBUT5 5
#define VBUT6 6
#define VBUT7 7
#define VBUT8 8


//EPROM MEMORORY
int SETPOINTTEMP_EP[] = { 5, 10, 15, 20, 25, 30 };  //these are EEprom memory locations - not data
#define FanSpeedPWMStart_EP 1
#define INTEGRAL_EP 0
#define GAIN_EP 2
#define FanSpeedPWNDelayDecreaseByMinutes_EP 3
#define FanSpeedPWNDecreaseByMinutes_EP 4
#define FanSpeedPWMAutoDecrease_EP 35
#define RoastLength_EP 36


#define OVERHEATFAN 250
#define BAUD 57600
#define TEMPCOOLINGDONE 250

#define STATEROASTING 1
#define STATESTOPPED 2
#define STATECOOLING 3
#define STATEOVERHEATED 4
#define STATEFANONLY 6
#define STATENOFANCURRENT 7
#define STATECHANGED 8
#define DEBUGTOGGLE 9
#define DEBUGDUTY 10
#define DEBUGCOIL 11


double manualtemp = 300;
boolean usemanualtemp = true;

#define PROFILELINEID 0
#define ROLLAVGLINEID 1
#define AVGLINEID 2
#define COILLINEID 3
#define FANSPEEDLINEID 4

#define RELAYON LOW
#define RELAYOFF HIGH

#define ValuesOnly true
#define All false


#define VmenuNone -1
#define Vmenubase 0
#define VmenuSetPointSelect 1
#define VmenuSetPointValue 2
#define VmenuDebug 3
#define VmenuZeroAmps 4
#define VmenuOnOff 5
#define VmenuAjd_01 6
#define VMenuAdj_1_5_10_V 7
#define VmenuFan 8
#define VmenuCount 9

// ===========
// DEFINITIONS
// ===========

// ===============================================================================================================================================================================
// GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS
// ======================================================================================================================================================================================
int State;
char StateName[9] = "12345678";
int newState;
int lastStateUpdated;
String errmsg;
boolean newerrmsg;
int lenlasterrmsg;
int TEMPCOILTOOHOT = 700;


MAX6675 thermocouple1(TSCKp, TCS1p, TSD1p);
MAX6675 thermocouple2(TSCKp, TCS2p, TSD2p);
MAX6675 thermocouple3(TSCKp, TCS3p, TSD3p);
MAX6675 thermocouple4(TSCKp, TCS4p, TSD4p);
//global variablers for temp control
int Gain = 100;  //read from eeprom

//for python serial  commands
char Commandsp1[7] = "xxxxx ";
int iCommandsp1 = 0;
char Commandsp[7] = "xxxxx ";
int iCommandsp = 0;

int spSelected = -1;

int FanSpeedPWM = 0;
int FanSpeedPWMStart = 0;

//value to store initial fan profile line
int XStartFan_Last;
int X2Fan_Last;
int X3Fan_Last;
int XEndFan_Last;
int YBotFan_Last;
int YTopFan_Last;


//int FanSpeedPWMAutoEnd=0;
int FanSpeedPWMAutoDecrease = 90;
int FanSpeedPWMAutoDecreaseStart = 90;
bool FanSpeedPWMAutoMode = false;
int FanSpeedPWNDelayDecreaseByMinutes = 2;
int FanSpeedPWNDecreaseByMinutes = 8;

int FanSpeedResistanceLast = 0;
int FanSpeedResistanceCurrent = 0;

double Integral = 0.1;  //read from eeprom
long unsigned IntegralLastTime = 0;
long unsigned IntegralSum = 0;
unsigned long PIDWindowStartTime;
boolean PIDNewWindow;
int ErrI = 0;
int Err = 0;
double Duty;
double Dutyraw;

int PIDIntegralUdateTimeValue;
int PIDWindowSize;

boolean setpointschanged = true;
double MyMinuteTemperature[30];

setpoint MySetPoints[6] = { { 0, 100 }, { 4, 390 }, { 7, 420 }, { 10, 425 }, { 13, 430 }, { 16, 450 } };
setpoint MySetPoints_Last[6] = { { 0, 100 }, { 4, 390 }, { 7, 420 }, { 10, 425 }, { 13, 430 }, { 16, 450 } };

//                 20             4         4/8         4/12         4/16        4/20

//                 16             4         3/7         3/10         3/13        3/16

//                 12             4         2/7         2/8          2/10        2/12


int SetPointCount = 6;  //0,1,2,3,4,5
int EndingSetPoint = 5;
int TimeScreenLeft = 0;

double TempYMax = 800;
double TempSplitHigh = 460;
double TempSplitLow = 390;

double PixelYSplit2;
double PixelYSplit;
long PixelsPerMin;
double TempPerPixL = 0;
double TempPerPixM = 0;
double TempPerPixH = 0;

int CurrentSetPointTemp = 0;
int BeforeTemp = 0;
int BeforeTime = 0;

int badLastTempCount = 0;
int LoopsPerSecond;

Chrono RoastTime(Chrono::SECONDS);
Chrono SecondTimer(Chrono::MILLIS);
Chrono SerialInputTimer(Chrono::MILLIS);
Chrono Serial1InputTimer(Chrono::MILLIS);

//int SecondTimerValue = 1000;
Chrono LcdUdateTime(Chrono::MILLIS);
Chrono PIDIntegralUdateTime(Chrono::MILLIS);

//int CapButActive = 0;
//current is read every loop (200 per second) so 30  very short
Average<float> AvgFanCurrent(30);
Average<int> AvgCoil1Amp(30);
Average<int> AvgCoil2Amp(30);

//temps are read once per second
Average<float> FanPressureRoll(5);
Average<double> TBeanAvgRoll(5);
Average<double> TCoilRoll(5);  //this is minute avg

int OVERHEATFANCount;
int TEMPCOILTOOHOTCount;
int TempReachedCount;
int TempSpikeCount;
int Readingskipped;

buttonsetdef myHorControlMenuDef;
buttonsetdef myHorFanButtonControl;
buttonsetdef myButtonVertMenus[VmenuCount];
int VerticalMenuShowing = 0;
int VerticalMenuPrior = 0;

char s7[7];

char s6[6];
char s5[5];

char spFormat[5] = "%6.2F";
//used when drawing lines. We support up to 3 lines (see line ID constants)
uint16_t LastXforLineID[5];
uint16_t LastYforLineID[5];
uint16_t LineColorforLineID[5];

int myLastGraphYPixels[800];
int myLastGraphXPixels[800];
int moveamount = -1;

double RoastMinutes = 0;

int TCoil;
int TBean1;
int TBean2;
int TBean3;
double MaxVoltage, MaxVread;
double CurrentFan;
double CurrentHeat1, CurrentHeat2;

double CurrentHeat1Offset, CurrentHeat2Offset;
double CurrentFanOffset = 0;

int TBeanAvg;


boolean Flasher;

// =====================================================================================================================================================================
// SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP
// =====================================================================================================================================================================
void setup() {
  //Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("setup starting");
  delay(2000);

  dac.begin(0x60);


  // Pin Configuration
  pinMode(VIBRELAYp, OUTPUT);
  pinMode(FANRELAYp, OUTPUT);
  pinMode(SSR1p, OUTPUT);
  pinMode(SSR2p, OUTPUT);
  pinMode(LEDp, OUTPUT);

  pinMode(CURFAN_A6, INPUT);
  pinMode(CURHEAT1_A4, INPUT);
  pinMode(CURHEAT2_A5, INPUT);

  pinMode(FanOutVcc_A3, OUTPUT);
  pinMode(FanOutG_A4, OUTPUT);
  digitalWrite(FanOutVcc_A3, LOW);  //0V
  digitalWrite(FanOutG_A4, HIGH);   //5V

  //pinMode(FanOutIncp, OUTPUT);
  //pinMode(FanOutDirp, OUTPUT);
  //pinMode(FanOutCsp, OUTPUT);

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
  digitalWrite(FANRELAYp, RELAYOFF);
  digitalWrite(VIBRELAYp, RELAYOFF);

  delay(1000);
  MaxVread = 512;  //this is the expected half way value
  RoastTime.stop();


  Gain = EEPROM.read(GAIN_EP);
  Integral = (double)EEPROM.read(INTEGRAL_EP) / 100;
  if (Integral > 1) Integral = 0.00;
  if (Gain > 100) Gain = 75;
  if (Gain < 10) Gain = 10;


  //  Integral = 0.01;
  // Gain = 75;
  //
  SecondTimer.restart(0);



  // -------------------------------------------------------------
  myGLCD.InitLCD();
  pinMode(8, OUTPUT);     //backlight
  digitalWrite(8, HIGH);  //on
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  // -------------------------------------------------------------
  delay(2000);

  intializeVMenus();

  State = STATESTOPPED;
  setpointschanged = true;
  ;

  // Serial.println ("setup B");

  //graphProfile();

  //Serial.println ("setup C");

  FanSpeedPWMStart = EEPROM.read(FanSpeedPWMStart_EP);


  FanSpeedPWNDelayDecreaseByMinutes = EEPROM.read(FanSpeedPWNDelayDecreaseByMinutes_EP);

  if (FanSpeedPWNDelayDecreaseByMinutes < 0 or FanSpeedPWNDelayDecreaseByMinutes > 5) {
    FanSpeedPWNDelayDecreaseByMinutes = 2;
    EEPROM.write(FanSpeedPWNDelayDecreaseByMinutes_EP, FanSpeedPWNDelayDecreaseByMinutes);
  }

  FanSpeedPWNDecreaseByMinutes = EEPROM.read(FanSpeedPWNDecreaseByMinutes_EP);
  if (FanSpeedPWNDecreaseByMinutes < 0 or FanSpeedPWNDecreaseByMinutes > 10) {
    FanSpeedPWNDecreaseByMinutes = 8;
    EEPROM.write(FanSpeedPWNDecreaseByMinutes_EP, FanSpeedPWNDecreaseByMinutes);
  }

  FanSpeedPWMAutoDecrease = EEPROM.read(FanSpeedPWMAutoDecrease_EP);
  if (FanSpeedPWMAutoDecrease < 0 or FanSpeedPWMAutoDecrease > 200) {
    FanSpeedPWMAutoDecrease = 80;
    EEPROM.write(FanSpeedPWMAutoDecrease_EP, FanSpeedPWMAutoDecrease);
  }

  if (FanSpeedPWMAutoDecrease > FanSpeedPWMStart) {
    FanSpeedPWMAutoDecrease = FanSpeedPWMStart;
    EEPROM.write(FanSpeedPWMAutoDecrease_EP, FanSpeedPWMAutoDecrease);
  }
  FanSpeedPWMAutoDecrease = EEPROM.read(FanSpeedPWMAutoDecrease_EP);
  FanSpeedPWMAutoDecreaseStart = FanSpeedPWMAutoDecrease;

  Wire.begin();
  StopAndSendFanPWM();
  delay(2000);

  DrawFanGraph();
  graphProfile();
}


void loop() {
  theloop();
}

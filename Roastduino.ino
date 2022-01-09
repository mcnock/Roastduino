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
//#include <Adafruit_MCP4725.h>
#include <stdio.h>  // for function sprintf
   

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegmentFull[];
extern uint8_t Grotesk24x48[];

// Remember to change the model parameter to suit your display module!
UTFT myGLCD(SSD1963_800480, 38, 39, 40, 41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch myTouch(43, 42, 44, 45, 46);           //byte tclk, byte tcs, byte din, byte dout, byte irq

//Adafruit_MCP4725 dac;

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

#define MCP4725_ADDR 0x60
//#define MCP4725_ADDR 0x61
//define MCP4725_ADDR 0x62

//For devices with A0 pulled HIGH, use 0x61

//PIN  definitions
//#5 inch display shield  does not  use 30- 34, 10,12, or 13
#define FANRELAYVCCp_3 3
#define FANRELAYp_2 2

#define FanPWMp_4 4

//#define burned out 5
#define SSR2_p6 6
#define SSR1_p7 7
#define SSRgr_14 14

//#define available 10
//#define available 12
//#define available 13

//#define available 30
//#define available 31
//#define available 32
//#define available 33
//#define available 34

#define TCS_G_p7  A7
#define TCS_5v_p8 A8
#define TSCKp A9
#define TSD1p A10  //could share A8
#define TCS1p A11
#define TSD2p A12  //could share A8
#define TCS2p A13
#define TSD3p A14  //could share A8
#define TCS3p A15
#define TCS_5v A8
#define CURHEAT2_A5 A5


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

boolean serialOutPutTempsBySecond ;
boolean serialOutPutTempsBy3Seconds ;
boolean serialOutPutStatusBy3Seconds;
boolean serialOutPutStatusBySecond;


MAX6675 thermocouple1(TSCKp, TCS1p, TSD1p);
MAX6675 thermocouple2(TSCKp, TCS2p, TSD2p);
MAX6675 thermocouple3(TSCKp, TCS3p, TSD3p);
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
float IntegralSum = 0;
unsigned long PIDWindowStartTime;
boolean PIDNewWindow;
float ErrI = 0;
float Err = 0;
double Duty;

int PIDIntegralUdateTimeValue;
int PIDWindowSize;

boolean setpointschanged = true;
double MyMinuteTemperature[30];

setpoint MySetPoints[6] = { { 0, 100 }, { 4, 390 }, { 7, 420 }, { 10, 425 }, { 13, 430 }, { 16, 450 } };
setpoint MySetPoints_Last[6] = { { 0, 100 }, { 4, 390 }, { 7, 420 }, { 10, 425 }, { 13, 430 }, { 16, 450 } };

//                 20             4         4/8         4/12         4/16        4/20

//                 16             4         3/7         3/10         3/13        3/16

//                 12             4         2/7         2/8          2/10        2/12


             



int SetPointCount = 6;  //0,1,2,3,4,t0t5
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

double CurrentSetPointTemp = 0;
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
//Average<float> AvgFanCurrent(30);
//Average<int> AvgCoil1Amp(30);
//Average<int> AvgCoil2Amp(30);

//temps are read once per second
Average<double> TBeanAvgRoll(5);
Average<double> TCoilRoll(5);  //this is minute avg

int OVERHEATFANCount;
int TEMPCOILTOOHOTCount;
int TempReachedCount;
int TempSpikeCount;
int Readingskipped[3];

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

boolean HasDisplay = true ;
// =====================================================================================================================================================================
// SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP
// =====================================================================================================================================================================
void setup() {
  //Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("setup starting");
  delay(2000);



  // Pin Conffaiguration
  pinMode(FANRELAYVCCp_3, OUTPUT);
  pinMode(FANRELAYp_2, OUTPUT);
  digitalWrite(FANRELAYVCCp_3, HIGH);   //5V

  
  pinMode(SSRgr_14,OUTPUT);
  digitalWrite(SSRgr_14, LOW); 
  pinMode(SSR1_p7, OUTPUT);
  pinMode(SSR2_p6, OUTPUT);


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

  pinMode(TCS1p, OUTPUT);
  pinMode(TCS2p, OUTPUT);
  pinMode(TCS3p, OUTPUT);

  pinMode(TCS_G_p7,OUTPUT);
  digitalWrite(TCS_G_p7, LOW);   //0V

  pinMode(TCS_5v_p8,OUTPUT);
  digitalWrite(TCS_5v_p8, HIGH);   //5V

  pinMode(TSCKp, OUTPUT);
  

  //set fan and vibrator relays to high - cause that means off
  digitalWrite(FANRELAYp_2, RELAYOFF);
  //digitalWrite(VIBRELAYp, RELAYOFF);

  delay(1000);
  MaxVread = 512;  //this is the expected half way value
  RoastTime.stop();
int testiffirstrun;
EEPROM.get(SETPOINTTEMP_EP[1],testiffirstrun);
if (testiffirstrun == 0)
{
   //first run on this mega
    EEPROM.put(SETPOINTTEMP_EP[0], (int)390);             
    EEPROM.put(SETPOINTTEMP_EP[1], (int)390);
    EEPROM.put(SETPOINTTEMP_EP[2], (int)400);
    EEPROM.put(SETPOINTTEMP_EP[3], (int)410);
    EEPROM.put(SETPOINTTEMP_EP[4], (int)430);
    EEPROM.put(SETPOINTTEMP_EP[5], (int)440);
    EEPROM.put(RoastLength_EP,14);
    EEPROM.update(INTEGRAL_EP , (int)(.04 * 100));
    EEPROM.update(GAIN_EP , 30);
    EEPROM.write(FanSpeedPWNDelayDecreaseByMinutes_EP, 2);
    EEPROM.write(FanSpeedPWNDecreaseByMinutes_EP, 8);
    EEPROM.write(FanSpeedPWMAutoDecrease_EP, 50);
}
//EEPROM.write(FanSpeedPWMAutoDecrease_EP, 50);
//EEPROM.write(FanSpeedPWNDelayDecreaseByMinutes_EP, 1);
//EEPROM.write(FanSpeedPWNDecreaseByMinutes_EP, 10);
    


        
  Gain = EEPROM.read(GAIN_EP);
  Integral = (double)EEPROM.read(INTEGRAL_EP) / 100;
  if (Integral > 1) Integral = 0.00;
  if (Gain > 75) Gain = 75;
  if (Gain < 10) Gain = 10;
  
  SecondTimer.restart(0);



  // -------------------------------------------------------------
  if (HasDisplay == true){
    Serial.println("Initializing LCD");

    myGLCD.InitLCD();
    pinMode(8, OUTPUT);     //backlight
    digitalWrite(8, HIGH);  //on
    myTouch.InitTouch();
    myTouch.setPrecision(PREC_MEDIUM);
    // -------------------------------------------------------------
  } 
  delay(2000);

  intializeVMenus();

  State = STATESTOPPED;
  setpointschanged = true;
  ;

  
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

  Serial.println("wire i2c for fan begin");

  
  Wire.begin();  
  
  StopAndSendFanPWM();
  
  DrawFanGraph();
  graphProfile();
  Serial.println("loop is starting...");
  
}


void loop() {
  theloop();
}


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
#include <stdio.h>  // for function sprintf
#include <stdarg.h>
#include <Arduino.h>
#include "src/Bitcraze_PMW3901.h"

extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegmentFull[];
extern uint8_t Grotesk24x48[];
extern uint8_t Retro8x16[];

UTFT myGLCD(SSD1963_800480, 38, 39, 40, 41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch myTouch(43, 42, 44, 45, 46);           //byte tclk, byte tcs, byte din, byte dout, byte irq

// Assign human-readable names to some common 16-bit color values:


#define LGBLUE 0xFE73

#define DARKBLUE 0x0804
#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x0400
#define BLUE 0x001F
#define GRAY 0x630C
#define LIGHTGRAY 0x3186
#define YELLOW 0xC781
#define PALEYELLOW 0xDFD4
#define ORANGE 0xF241
#define AQUA 0x1D5C

#define MCP4725_ADDR 0x60

#define SpDebug Serial.println


//PIN  definitions
//note: the 5 TFT inch display shield has pins inserted, but does not actuall use digital pins 30-34, 10 , 12,  13 or i2c pins 20,21

#define FANRELAYVCCp_3 3
#define FANRELAYp_2 2
//#define available 4
//#define available 5
#define SSR2_p6 6
#define SSR1_p7 7
//#define available 10
//#define available 12
//#define available 13
#define SSRgr_14 14


//#define available 30
//#define available 31
//#define available 32
//#define available 33
//#define available 34


//#define available A0
//#define available A1
#define FanOutG_A4 A2
#define FanOutVcc_A3 A3
//#define available A4
//#define available A5
//#define available A6

#define TC_G_A7 A7
#define TC_5v_A8 A8
#define TC_SCK_A9 A9
#define TC_SD1_A10 A10  //could share A8
#define TC_CS1_A11 A11
#define TC_SD2_A12 A12  //could share A8
#define TC_CS2_A13 A13
#define TC_SD3_A14 A14  //could share A8
#define TC_CS3_A15 A15

#define VBUT0 0
#define VBUT1 1
#define VBUT2 2
#define VBUT3 3
#define VBUT4 4
#define VBUT5 5
#define VBUT6 6
#define VBUT7 7
#define VBUT8 8

#define TCoilID 0
#define TBean1ID 1
#define TBean2ID 2

//EPROM MEMORORY

#define INTEGRALTEMP_EP 0
#define GAINTEMP_EP 2

const int SETPOINTTEMP_EP[] = { 5, 10, 15, 20, 25, 30 };  //these are EEprom memory locations - not data
#define RoastLength_EP 36
#define FanGraphMinPWM_EP 37
#define FanGraphMaxPWM_EP 38
const int FanSetPoints_EP[] = { 50, 55, 60, 65 };  //these are EEprom memory locations - not data
#define INTEGRALFLOW_EP 70
#define GAINFLOW_EP 72
#define SETPOINTFLOW_EP 76




#define TEMPCOOLINGDONE 250

#define STATEROASTING 1
#define STATESTOPPED 2
#define STATECOOLING 3
#define STATEOVERHEATED 4
#define STATEFANONLY 5
#define STATENOFANCURRENT 6
#define STATECHANGED 7
#define DEBUGTOGGLE 8
#define DEBUGDUTY 9
#define DEBUGCOIL 10
#define STATERESTARTROASTING 11



const char Sname0[] = "";
const char Sname1[] = "Roasting  ";
const char Sname2[] = "Stopped   ";
const char Sname3[] = "Cooling   ";
const char Sname4[] = "Overheated";
const char Sname5[] = "FanOnly   ";
const char Sname6[] = "NoFanCurre";
const char Sname7[] = "Changed   ";
const char Sname8[] = "DebugTog  ";
const char Sname9[] = "DebugDuty ";
const char Sname10[] = "DebugCoil";
const char Sname11[] = "STATERESTARTROASTING";

bool RoastRestartNeeded = false;

const uint16_t StateColor[] = {
  0,
  GREEN,
  RED,
  BLUE,
  YELLOW,
  YELLOW,
  YELLOW,
  YELLOW,
  YELLOW,
  YELLOW,
  YELLOW,
  GREEN
};

const char* StateName[] = {
  Sname0,
  Sname1,
  Sname2,
  Sname3,
  Sname4,
  Sname5,
  Sname6,
  Sname7,
  Sname8,
  Sname9,
  Sname10,
  Sname11
};



Bitcraze_PMW3901 beanflow(10);
int16_t deltaXflow, deltaYflow, setpointflow;




#define PROFILELINEID 0
#define ROLLAVGLINEID 1
#define ROLLMAXLINEID 2
#define COILLINEID 3
#define FANSPEEDLINEID 4
#define ROLLMINLINEID 5

#define GRAPHLINECOUNT 6

const uint16_t LineColorforLineID[GRAPHLINECOUNT] = { WHITE, YELLOW, RED, RED, YELLOW, ORANGE };

const boolean LineBoldforLineID[GRAPHLINECOUNT] = { false, false, false, false, true, false };


//800 pixels / every 4 seconds = 200

#define RELAYON LOW
#define RELAYOFF HIGH

#define ValuesOnly true
#define All false

#define Vmenubase 0
#define VmenuSetPointSelect 1
#define VmenuAdj_1_3_5 2
#define VmenuDebug 3
#define VmenuOnOff 4
#define VmenuAjd_01 5
#define VMenuAdj_1_5_10_V 6
#define VmenuFan 7
#define VmenuEmpty 8

#define HmenuCTRL 9
#define HmenuFAN 10


const char *Mname0 = "Vmenubase";
const char *Mname1 = "VmenuSetPointSelect";
const char *Mname2 = "VmenuAdj_1_3_5";
const char *Mname3 = "VmenuDebug";
const char *Mname4 = "VmenuOnOff";
const char *Mname5 = "VmenuAjd_01";
const char *Mname6 = "VMenuAdj_1_5_10_V";
const char *Mname7 = "VmenuFan";
const char *Mname8 = "VmenuEmpty";
const char *Mname9 = "HmenuCTRL";
const char *Mname10 = "HmenuFAN";

const char* menunames[] = {
  Mname0,
  Mname1,
  Mname2,
  Mname3,
  Mname4,
  Mname5,
  Mname6,
  Mname7,
  Mname8,
  Mname9,
  Mname10,
};


char debug = 'a';

#define VmenuCount 9
#define MaxButtonCount 9
const buttontext PROGMEM Vmenutext[][MaxButtonCount] = {
  { { 0, ">>", "forward", "to", "next", GREEN },
    { 1, "<<", "back", "to", "prior", GREEN },
    { 2, "GainT", "Ajdust", "Temp Gain", "Value", YELLOW },
    { 3, "IntT", "Ajdust", "Intergal", "Value", YELLOW },
    { 4, "SPs", "Select", "Setpoints", "to adjust", YELLOW },
    { 5, "Fan", "Adjust", "fan auto", "decrease", YELLOW },
    { 6, "Adv", "Advance", "roast", "by 1 min", YELLOW },
    { 7, "Rtd", "Retard ", "roast", "by 1 min", YELLOW },
    { 8, "cCut", "Adjust", "Hightemp", "Cut out", YELLOW } },
  { { 10, "<<", "back", "to prior", "menu", GREEN },
    { 11, "sp1", "Adjust", "setpoint", "#1", YELLOW },
    { 12, "sp2", "Adjust", "setpoint", "#2", YELLOW },
    { 13, "sp3", "Adjust", "setpoint", "#3", YELLOW },
    { 14, "sp4", "Adjust", "setpoint", "#4", YELLOW },
    { 15, "sp5", "Adjust", "setpoint", "#5", YELLOW },
    { 16, "ls3", "Adjust", "last 3", "setpoint", YELLOW },
    { 17, "T+1", "Increase", "roast len", "by 1 min", YELLOW },
    { 18, "T-1", "Decrease", "roast len", "by 1 min", YELLOW } },
  { { 20, "<<", "go back", "to", "prior", GREEN },
    { 21, "", "go back", "to", "prior", YELLOW },
    { 22, "+1", "go back", "to", "prior", ORANGE },
    { 23, "+3", "go back", "to", "prior", ORANGE },
    { 24, "+5", "go back", "to", "prior", ORANGE },
    { 25, "-1", "go back", "to", "prior", ORANGE },
    { 26, "-3", "go back", "to", "prior", ORANGE },
    { 27, "-5", "go back", "to", "prior", ORANGE },
    { 28, "SAVE", "go back", "to", "prior", YELLOW } },
  { { 30, ">>", "foward", "to", "next", GREEN },
    { 31, "<<", "back", "to", "prior", GREEN },
    { 32, "DBG", "subject", "of", "menu", YELLOW },
    { 33, "C1", "toggle", "coil 1 SSR", "on and off", YELLOW },
    { 34, "C2", "toggle", "coil 2 SSR", "on and off", YELLOW },
    { 35, "", "go back", "to", "prior", YELLOW },
    { 36, "Fan", "toggle", "fan relay", "on and off", YELLOW },
    { 37, "Duty", "Manually", "set", "duty", YELLOW },
    { 38, "Temp", "go back", "to", "prior", YELLOW } },
  { { 40, "<<", "go to", "prior", "menu", GREEN },
    { 41, "", "selected", "device", "to debug", GREEN },
    { 42, "ON", "turn", "device", "on", ORANGE },
    { 43, "OFF", "turn", "device", "off", ORANGE },
    { 44, "", "go back", "to", "prior", BLACK },
    { 45, "", "go back", "to", "prior", BLACK },
    { 46, "", "go back", "to", "prior", BLACK },
    { 47, "", "go back", "to", "prior", BLACK },
    { 48, "", "go back", "to", "prior", BLACK } },
  { { 50, "<<", "go back", "to", "prior", GREEN },
    { 51, "", "go back", "to", "prior", GREEN },
    { 52, "+.01", "go back", "to", "prior", ORANGE },
    { 53, "+.05", "go back", "to", "prior", ORANGE },
    { 54, "+.10", "go back", "to", "prior", ORANGE },
    { 55, "-.01", "go back", "to", "prior", ORANGE },
    { 56, "-.05", "go back", "to", "prior", ORANGE },
    { 57, "-.10", "go back", "to", "prior", ORANGE },
    { 58, "Save", "Save", "and", "close", GREEN } },
  { { 60, "<<", "go to", "prior", "menu", GREEN },
    { 61, "", "go back", "to", "prior", GREEN },
    { 62, "+1", "go back", "to", "prior", ORANGE },
    { 63, "+5", "go back", "to", "prior", ORANGE },
    { 64, "+10", "go back", "to", "prior", ORANGE },
    { 65, "-1", "go back", "to", "prior", ORANGE },
    { 66, "-5", "go back", "to", "prior", ORANGE },
    { 67, "-10", "go back", "to", "prior", ORANGE },
    { 68, "Save", "Save", "and", "close", GREEN } },
  { { 70, "<<", "go to", "prior", "menu", GREEN },
    { 71, "", "Gain", "of", "Fan", AQUA },
    { 72, "", "Int", "of", "Fan", AQUA },
    { 73, "A PWM", "Adjust", "A", "PWM", AQUA },
    { 74, "B PWM", "Adjust", "B", "PWM", AQUA },
    { 75, "C PWM", "Adjust", "C", "PWM", AQUA },
    { 76, "D PWM", "Adjust", "D", "PWM", AQUA },
    { 77, "C+1", "Add 1 min", "to C", "period", AQUA },
    { 78, "C-1", "Rmv 1 min", "to C", "period", AQUA } },
  { { 80, ">>", "go to", "next", "menu", GREEN },
    { 81, "", "go back", "to", "prior", AQUA },
    { 82, "", "go", "to", "prior", AQUA },
    { 83, "", "go back", "to", "prior", AQUA },
    { 84, "", "go back", "to", "prior", AQUA },
    { 85, "", "go back", "to", "prior", GREEN },
    { 86, "", "go back", "to", "prior", GREEN },
    { 87, "", "go back", "to", "prior", GREEN },
    { 88, "", "go back", "to", "prior", GREEN } },
  { { 90, "Start", "Start", "Roast", "", GREEN },
    { 91, "Stop", "End Roast", "or Fan", "", RED },
    { 92, "Fan", "Start", "Fan", "", AQUA },
    { 93, "rfs", "Redraw", "screen", "", ORANGE },
    { 94, "", "go back", "to", "prior", GREEN },
    { 95, "", "go back", "to", "prior", GREEN },
    { 96, "", "go back", "to", "prior", GREEN },
    { 97, "", "go back", "to", "prior", GREEN },
    { 98, "", "go back", "to", "prior", GREEN }

  },
  { { 100, "-5", "Decrease", "fan 10", "prior", AQUA },
    { 101, "-1", "Decrease", "fan 3", "prior", AQUA },
    { 102, "+1", "Increase", "fan 3", "prior", AQUA },
    { 103, "+5", "Increase", "fan 10", "prior", AQUA },
    { 104, "", "save as", "start", "prior", AQUA },
    { 105, "", "go back", "to", "prior", GREEN },
    { 106, "", "go back", "to", "prior", GREEN },
    { 107, "", "go back", "to", "prior", GREEN },
    { 108, "", "go back", "to", "prior", GREEN } 
    }
};

buttontext myArrayLocal;

int State;
int newState;
int lastStateUpdated;
String errmsg;
String errmsglast;
boolean newerrmsg;
int lenlasterrmsg;

int TEMPCOILTOOHOT = 800;

boolean serialOutPutTempsBySecond;
boolean serialOutPutTempsBy3Seconds;
boolean serialOutPutStatusBy3Seconds;
boolean serialOutPutStatusBySecond;


MAX6675 thermocouple1(TC_SCK_A9, TC_CS1_A11, TC_SD1_A10);
MAX6675 thermocouple2(TC_SCK_A9, TC_CS2_A13, TC_SD2_A12);
MAX6675 thermocouple3(TC_SCK_A9, TC_CS3_A15, TC_SD3_A14);

//for python serial  commands
char Commandsp1[7] = "xxxxx ";
int iCommandsp1 = 0;
char Commandsp[7] = "xxxxx ";
int iCommandsp = 0;

int spSelected = -1;

char _debug = 'a';


PWMSetpoint FanSetPoints[4];
int FanSpeedPWM = 0;
//int FanSpeedPWMStart = 0;
//int FanSpeedPWMA = 0;
//int FanSpeedPWMC = 0;
//int FanSpeedPWMEnd = 90;
//int FanSpeedPWMAutoDecreaseStart = 90;
//bool FanSpeedPWMAutoMode = false;
//int FanSpeedPWNMinutesToA = 2;
//int FanSpeedPWNMinutesToC = 8;
int FanSpeedResistanceLast = 0;
int FanSpeedResistanceCurrent = 0;



point TempPixelHistory[160];
point FanPixelHistory[160];

//LineID;SkipCount;SkipLimit;PixelsP;ArraySize;Array
graphhistory GraphHistory[] = {
  { ROLLAVGLINEID, 0, 5, 0, 160, TempPixelHistory },
  { FANSPEEDLINEID, 0, 5, 0, 160, FanPixelHistory }
};


const int FanGraphXStart = 410;  //starting col of fan graph - a little past half
//int FanGraphXWidth = 390;  //uses 1/4 of screen width
const int FanGraphHeight = 165;  //uses 1/4 of screen width
const int FanGraphHorGridSpacingPWM = 15;
const int FanGraphBottom = 455;
int FanGraphMinPWM = 100;
int FanGraphMaxPWM = 254;

float PixelsPerMinFan;

int GainTemp = 0;           //read from eeprom
double IntegralTemp = 0.1;  //read from eeprom
long unsigned IntegralLastTimeTemp = 0;
float IntegralSumTemp = 0;
unsigned long PIDWindowStartTimeTemp;
boolean PIDNewWindowTemp;
float ErrITemp = 0;
float ErrTemp = 0;
double DutyTemp;
unsigned int PIDIntegralUdateTimeValueTemp;
unsigned int PIDWindowSizeTemp;

int GainFlow = 0;           //read from eeprom
double IntegralFlow = 0.1;  //read from eeprom
long unsigned IntegralLastTimeFlow = 0;
float IntegralSumFlow = 0;
unsigned long PIDWindowStartTimeFlow;
boolean PIDNewWindowFlow;
float ErrIFlow = 0;
float ErrFlow = 0;
double DutyFlow;
unsigned int PIDIntegralUdateTimeValueFlow;
unsigned int PIDWindowSizeFlow;




boolean setpointschanged = true;
long MyMinuteTemperature[30];


//read from eprom
setpoint MySetPoints[6] = { { 0, 100 }, { 4, 390 }, { 7, 420 }, { 10, 425 }, { 13, 430 }, { 16, 450 } };
const int SetPointCount = 6;  //0,1,2,3,4,t0t5
int EndingSetPoint = 5;
int TimeScreenLeft = 0;


int HorScaleLineY[20];
int HorScaleLineYCount = 0;
double CurrentSetPointTemp = 0;

int LoopsPerSecond;

boolean TouchDetected;
boolean LongPressDetected;
buttonsetdef* TouchButtonSet;
ClickHandler* TouchButtonHandler;
int TouchButton;

Chrono TouchTimer(Chrono::MILLIS);
Chrono RoastTime(Chrono::SECONDS);
Chrono SecondTimer(Chrono::MILLIS);
Chrono SerialInputTimer(Chrono::MILLIS);
Chrono Serial1InputTimer(Chrono::MILLIS);
Chrono LcdUdateTime(Chrono::MILLIS);
Chrono PIDIntegralUdateTimeTemp(Chrono::MILLIS);
Chrono PIDIntegralUdateTimeFlow(Chrono::MILLIS);
Chrono MeasureTempTimer(Chrono::MILLIS);

//temps are read once per second
Average<double> TBeanAvgRoll(3);
Average<double> TCoilRoll(3);  //this is minute avg

//int OVERHEATFANCount;
int TEMPCOILTOOHOTCount;
int TempReachedCount;
int TempSpikeCount;
int TempReadingskipped[3];

buttonsetdef myHorControlMenuDef;
buttonsetdef myHorFanButtonControl;
buttonsetdef myButtonVertMenus[VmenuCount];

int VerticalMenuShowing = 0;
int VerticalMenuPrior = 0;

char s7[7];
char s6[6];
char s5[5];

char spFormat[6] = "%6.2F";
//used when drawing lines. We support up to 3 lines (see line ID constants)
point LastforLineID[GRAPHLINECOUNT];

int moveamount = -1;

double RoastMinutes = 0;

int FanDeviation = 0;

boolean bNewTempsAvailable = false;
int ReadTempFlag = -1;
int TCoil;
int TBean1;
int TBean2;

double MaxVoltage, MaxVread;


int TBeanAvg;


boolean HasDisplay = true;
// =====================================================================================================================================================================
// SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP
// =====================================================================================================================================================================
void setup() {
  //Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("setup starting");
  Serial1.begin(9600);
  Serial1.println("setup starting");


  // Pin Conffaiguration
  pinMode(FANRELAYVCCp_3, OUTPUT);
  pinMode(FANRELAYp_2, OUTPUT);
  digitalWrite(FANRELAYVCCp_3, HIGH);  //5V


  pinMode(SSRgr_14, OUTPUT);
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

  pinMode(TC_SD1_A10, INPUT_PULLUP);
  pinMode(TC_SD2_A12, INPUT_PULLUP);
  pinMode(TC_SD3_A14, INPUT_PULLUP);

  pinMode(TC_CS1_A11, OUTPUT);
  pinMode(TC_CS2_A13, OUTPUT);
  pinMode(TC_CS3_A15, OUTPUT);

  pinMode(TC_G_A7, OUTPUT);
  digitalWrite(TC_G_A7, LOW);  //0V

  pinMode(TC_5v_A8, OUTPUT);
  digitalWrite(TC_5v_A8, HIGH);  //5V

  pinMode(TC_SCK_A9, OUTPUT);



  //set fan and vibrator relays to high - cause that means off
  digitalWrite(FANRELAYp_2, RELAYOFF);
  //digitalWrite(VIBRELAYp, RELAYOFF);

  delay(1000);
  MaxVread = 512;  //this is the expected half way value
  RoastTime.stop();
  int testiffirstrun;
  EEPROM.get(SETPOINTTEMP_EP[1], testiffirstrun);
  if (testiffirstrun == 0) {
    //first run on this mega
    EEPROM.put(SETPOINTTEMP_EP[0], (int)390);
    EEPROM.put(SETPOINTTEMP_EP[1], (int)390);
    EEPROM.put(SETPOINTTEMP_EP[2], (int)400);
    EEPROM.put(SETPOINTTEMP_EP[3], (int)410);
    EEPROM.put(SETPOINTTEMP_EP[4], (int)430);
    EEPROM.put(SETPOINTTEMP_EP[5], (int)440);
    EEPROM.put(RoastLength_EP, 14);
    EEPROM.update(INTEGRALTEMP_EP, (int)(.04 * 100));
    EEPROM.update(GAINTEMP_EP, 30);
    EEPROM.update(INTEGRALFLOW_EP, (int)(.04 * 100));
    EEPROM.update(GAINFLOW_EP, 30);
    EEPROM.update(SETPOINTFLOW_EP, 30); 
    FanSetPoints[0].PWM = 185;
    FanSetPoints[0].Minutes = 0;
    FanSetPoints[1].PWM = 160;
    FanSetPoints[1].Minutes = 2;
    FanSetPoints[2].PWM = 145;
    FanSetPoints[2].Minutes = 8;
    FanSetPoints[3].PWM = 135;
    FanSetPoints[3].Minutes = 14;
  }


  for (int i = 0; i < 4; i++) {
    EEPROM.get(FanSetPoints_EP[i], FanSetPoints[i]);
  }

  if (FanSetPoints[0].PWM == 0 || FanSetPoints[0].PWM == 255) {
    Serial.println(F("Setting FanSetPointEprom"));
    FanSetPoints[0].PWM = 185;
    FanSetPoints[0].Minutes = 0;
    FanSetPoints[1].PWM = 160;
    FanSetPoints[1].Minutes = 2;
    FanSetPoints[2].PWM = 145;
    FanSetPoints[2].Minutes = 8;
    FanSetPoints[3].PWM = 135;
    FanSetPoints[3].Minutes = 14;
    EEPROM.put(FanSetPoints_EP[0], FanSetPoints[0]);
    EEPROM.put(FanSetPoints_EP[1], FanSetPoints[1]);
    EEPROM.put(FanSetPoints_EP[2], FanSetPoints[2]);
    EEPROM.put(FanSetPoints_EP[3], FanSetPoints[3]);
    for (int i = 0; i < 4; i++) {
      EEPROM.get(FanSetPoints_EP[i], FanSetPoints[i]);
    }
  }

  for (int i = 0; i < 4; i++) {
    //SpDebug("FanSP " + String(i) + " min:" + String(FanSetPoints[i].Minutes) + " pwm:" + String(FanSetPoints[i].PWM));
  }

  GainTemp = EEPROM.read(GAINTEMP_EP);
  IntegralTemp = (double)EEPROM.read(INTEGRALTEMP_EP) / 100;
  if (IntegralTemp > 1) IntegralTemp = 0.00;
  if (GainTemp > 75) GainTemp = 75;
  if (GainTemp < 10) GainTemp = 10;

  GainFlow = EEPROM.read(GAINFLOW_EP);
  IntegralFlow = (double)EEPROM.read(INTEGRALFLOW_EP) / 100;
  setpointflow = EEPROM.read(SETPOINTFLOW_EP);
  if (IntegralFlow > 1) IntegralFlow = 0.00;
  if (GainFlow > 75) GainFlow = 75;
  if (GainFlow < 10) GainFlow = 10;


  SecondTimer.restart(0);



  // -------------------------------------------------------------
  if (HasDisplay == true) {
    Serial.println("Initializing LCD");

    //myGLCD.InitLCD();
    myGLCD.InitLCD(PORTRAIT);
    pinMode(8, OUTPUT);     //backlight
    digitalWrite(8, HIGH);  //on
    myTouch.InitTouch();
    myTouch.setPrecision(PREC_MEDIUM);
    // -------------------------------------------------------------
  }
  delay(2000);

  intializeMenus();

  State = STATESTOPPED;
  setpointschanged = true;
  ;



  FanSpeedPWM = FanSetPoints[0].PWM;

  Serial.println(F("wire i2c for fan begin"));


  Wire.begin();

  StopAndSendFanPWM();

  VerticalMenuShowing = VmenuEmpty;
  graphProfile();
  Serial.println("loop is starting...");
}


void loop() {
  theloop();
}

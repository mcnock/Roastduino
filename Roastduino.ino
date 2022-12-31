#include <SPI.h>
#include <EEPROM.h>
//#include <UTouchCD.h>
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
#include "Bitcraze_PMW3901.h"

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else   // __ARM__
extern char* __brkval;
#endif  // __arm__

#define Font8x9 mykefont2
#define Font8x12 SmallFont
#define Font8x16 Retro8x16

#define Font16x16 BigFont
#define Font16x32 Grotesk16x32
#define Font16x48 Grotesk16x48


extern uint8_t SmallFont[];  //8 x 12
extern uint8_t Retro8x16[];  //8 x 16


extern uint8_t BigFont[];       //16 x 16
extern uint8_t arial_normal[];  //16 x 16
extern uint8_t arial_bold[];    //16 x 16
extern uint8_t Grotesk16x32[];  //16 x 32
extern uint8_t Grotesk24x48[];  //24 x 48

UTFT myGLCD(SSD1963_800480, 38, 39, 40, 41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch myTouch(43, 42, 44, 45, 46);           //byte tclk, byte tcs, byte din, byte dout, byte irq

// Assign human-readable names tox12 some common 16-bit color values:


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
#define spDebug Serial.println
#define spDebugxClose }
#define spDebug1 if(_debugbyte==1){Serial.println
#define spDebug2 if(_debugbyte==2){Serial.println
#define spDebug3 if(_debugbyte==3){Serial.println
#define spDebug4 if(_debugbyte==4){Serial.println

byte _debugbyte = 0;

//PIN  definitions
//note: the 5 TFT inch display shield has pins inserted, but does not actuall use digital pins 30-34, 10 , 12,  13 or i2c pins 20,21

#define FANRELAYVCCp_3 3
#define FANRELAYp_2 2
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

#define COILCURRENT_SPI_SSp53 53 
#define BEAN_OPTICAL_FLOW_SPI_SSp48 48
#define BEAN_OPTICAL_FLOW2_SPI_SSp49 49


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

#define SSRFULL 1
#define SSRPWM 2
#define SSROFF 3
byte SSR1Status = 0;
byte SSR2Status = 0;

const ssrstatus SSRStatus[4] = {" NA"," ON","PWM","OFF"};


//EPROM MEMORORY

#define INTEGRALTEMP_EP 0
#define SETDEFAULT_EP 4
const int SETPOINTTEMP_EP[] = { 5, 10, 15, 20, 25, 30 };  //these are EEprom memory locations - not data
#define RoastLength_EP 36
//#define FanGraphMinPWM_EP 37
//#define FanGraphMaxPWM_EP 38

const int FanSetPoints_EP[] = { 50, 55, 60, 65 };  //these are EEprom memory locations - not data
#define INTEGRALFLOW_EP 70
#define SETPOINTFLOW_EP 76
#define FANGRAPHBOTTOM_EP 78
#define FANGRAPHHEIGHT_EP 80
#define COILGRAPHTEMPOFFSET_EP 82
#define COOLDOWNTEMP_EP 84
#define TOOHOTTEMP_EP 86
#define GAINTEMP_EP 88
#define GAINFLOW_EP 90
#define OPERDETAILDISPLAY_X_EP 92
#define OPERDETAILDISPLAY_Y_EP 94
#define CONFIGURATIONDISPLAY_X_EP 96
#define CONFIGURATIONDISPLAY_Y_EP 98

error_status ErrorStatus;
const char ErrDisplay[6] = "ERROR";
const int NoError = -1;
const int ErrorNeedFanFirst = 0;
const int ErrorCoilTooHot = 1;
const int ErrorPleaseRestart = 2;
const error errorlist[] = {
  { 0, "!!Turn on fan before you start!!" },
  { 1, "!!Coil exceeds Too Hot Temp!!" },
  { 2, "Repower to load defaults.  Click 'Reset' again to cancel" }
};

byte DisplayMessageToShow = 0;
const int DisplayDefault = 0;
const int DisplayMsgAdvanced = 1;
const int DisplayMsgRetarded = 2;


int TEMPCOOLINGDONE = 250;

int FreeMemory = 0;

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
int16_t deltaYflow, setpointflow;

#define PROFILELINEID 0
#define ROLLAVGLINEID 1
#define ROLLMAXLINEID 2
#define COILLINEID 3
#define FANSPEEDLINEID 4
#define ROLLMINLINEID 5

#define GRAPHLINECOUNT 6

const uint16_t LineColorforLineID[GRAPHLINECOUNT] = { WHITE, YELLOW, RED, RED, YELLOW, ORANGE };

const boolean LineBoldforLineID[GRAPHLINECOUNT] = { false, false, false, false, true, false };

#define RELAYON LOW
#define RELAYOFF HIGH
#define ValuesOnly true
#define All false
#define BeingMoved true
#define Regular false


#define Values_d01_d03_d05 0
#define Values_1_3_5 1
#define Values_1_3_10 2
#define Values_1_0_0 3
#define Values_5_10_20 4
#define Values_d5_d1_d01 5

const adjustmentlabels AdustmentValuesLabels[][3]{
  { ".05", ".03", ".01" },
  { "5", "3", "1" },
  { "10", "5", "1" },
  { "1", "1", "1" },
  { "20", "5", "1" },
  { ".5", ".1", ".01" },

};

const float AdustmentValues[][3] = {
  { .05, .03, .01 },
  { 5, 3, 1 },
  { 10, 5, 1 },
  { 1, 1, 1 },
  { 20, 5, 1 },
  { .5, .1, .01}
};

activeadjustment ActiveAdjustment;


#define ActionShowSetpointSelectMenu 30
#define ActionShowSetpointFanMenu 31
#define ActionGetLableFromPrior 32
#define ActionSelectAdustmentValue 33
#define ActionResetToDefaultOnNextStart 34

#define ActionAdjustments 40
#define ActionAdjustIntegralTemp 41
#define ActionAdjustGainTemp 42
#define ActionAdjustSetpointTemp 43
#define ActionAdjustSetpointFan 44
#define ActionAdjustRoastLength 45
#define ActionAdjustTempToHot 46
#define ActionAdjustCoolDownTemp 47
#define ActionAdjustCoilGraphTempOffset 48
#define ActionAdjustFanGraphPixelBottom 48
#define ActionAdjustTempDuty 50
#define ActionAdjustIntegralFlow 51
#define ActionAdjustGainFlow 52
#define ActionAdjustSetpointFlow 53

#define VmenuCount 7
#define MaxButtonCount 9

#define VmenuBase 0
#define VmenuSetPointSelect 1
#define VmenuDebug 2
#define VmenuOnOff 3
#define VmenuAdjustValue 4
#define VmenuFan 5
#define VmenuEmpty 6
#define HmenuCTRL 7
#define HmenuFAN 8

#define VmenuBase0 0
#define VmenuSetPointSelect0 10
#define VmenuDebug0 20
#define VmenuOnOff0 30
#define VmenuAdjustValue0 40
#define VmenuFan0 50
#define VmenuEmpty0 60
#define HmenuCTRL0 70
#define HmenuFAN0 80

#define VmenuFindPrior 11

byte DrawLablesRotated = 60;
byte GetLablesFromPrior = 41;


const buttontext PROGMEM Vmenutext[][MaxButtonCount] = {
  { { VmenuBase0, ">>", "Show", "debug", "menu", GREEN, VmenuDebug },
    { 1, "SPs", "Show", "Temp", "menu", YELLOW, ActionShowSetpointSelectMenu },
    { 2, "GainT", "Adjust", "T PID", "Gain", YELLOW, ActionAdjustGainTemp, Values_1_3_5 },
    { 3, "IntT", "Adjust", "T PID", "Interg", YELLOW, ActionAdjustIntegralTemp, Values_d01_d03_d05 },
    { 4, "Fan", "Show", "Fan", "menu", YELLOW, ActionShowSetpointFanMenu },
    { 5, "ThT", "Adjust", "TooHot", "Temp", YELLOW, ActionAdjustTempToHot, Values_1_3_10 },
    { 6, "CdT", "Adjust", "CoolDwn", "Temp", YELLOW, ActionAdjustCoolDownTemp, Values_1_3_10 },
    { 7, "Coil", "Adjust", "CoilTmp", "Offset", YELLOW, ActionAdjustCoilGraphTempOffset, Values_1_3_10 },
    { 8, "Reset", "Load", "Default", "Configs", RED, ActionResetToDefaultOnNextStart } },
  { { VmenuSetPointSelect0, "<<", "back", "to prior", "menu", GREEN, VmenuBase },
    { 11, "T sp0", "Adjust", "point", "#0", YELLOW, ActionAdjustSetpointTemp, Values_1_3_5 },
    { 12, "T sp1", "Adjust", "point", "#1", YELLOW, ActionAdjustSetpointTemp, Values_1_3_5 },
    { 13, "T sp2", "Adjust", "point", "#2", YELLOW, ActionAdjustSetpointTemp, Values_1_3_5 },
    { 14, "T sp3", "Adjust", "point", "#3", YELLOW, ActionAdjustSetpointTemp, Values_1_3_5 },
    { 15, "T sp4", "Adjust", "point", "#4", YELLOW, ActionAdjustSetpointTemp, Values_1_3_5 },
    { 16, "Last 2", "Adjust", "last 2", "points", YELLOW, ActionAdjustSetpointTemp, Values_1_3_5 },
    { 17, "Last 3", "Adjust", "last 3", "points", YELLOW, ActionAdjustSetpointTemp, Values_1_3_5 },
    { 18, "", "", "", "", BLACK } },
  { { VmenuDebug0, ">>", "show", "empty", "menu", GREEN, VmenuEmpty },
    { 21, "SetDut", "Set", "Duty", "Manua", GREEN , ActionAdjustTempDuty ,Values_d5_d1_d01},
    { 22, "", "subject", "of", "menu", YELLOW },
    { 23, "C1", "toggle", "coil 1 SSR", "on and off", YELLOW },
    { 24, "C2", "toggle", "coil 2 SSR", "on and off", YELLOW },
    { 25, "FanG", "Adjust", "F PID", "Gain", AQUA, ActionAdjustGainFlow, Values_1_3_5},
    { 26, "FanI" , "Adjust", "F PID", "Interg", AQUA , ActionAdjustIntegralFlow , Values_d01_d03_d05},
    { 27, "FanSP", "Adjust", "F", "Setpnt", AQUA , ActionAdjustSetpointFlow, Values_1_3_5 },
    { 28, "Tem", "go back", "to", "prior", YELLOW } },
  { { VmenuOnOff0, "<<", "go to", "prior", "menu", GREEN, VmenuFindPrior },
    { 31, "", "selected", "device", "to debug", GREEN },
    { 32, "ON", "turn", "device", "on", ORANGE },
    { 33, "OFF", "turn", "device", "off", ORANGE },
    { 34, "", "go back", "to", "prior", BLACK },
    { 35, "", "go back", "to", "prior", BLACK },
    { 36, "", "go back", "to", "prior", BLACK },
    { 37, "", "go back", "to", "prior", BLACK },
    { 38, "", "go back", "to", "prior", BLACK } },
  { { VmenuAdjustValue0, "<<", "go back", "to", "prior", GREEN, VmenuFindPrior },
    { 41, "", "What's", "being", "edited", GREEN, ActionGetLableFromPrior },
    { 42, "x", "Select", "this", "amount", ORANGE, ActionSelectAdustmentValue },
    { 43, "x", "Select", "this", "amount", ORANGE, ActionSelectAdustmentValue },
    { 44, "x", "Select", "this", "amount", ORANGE, ActionSelectAdustmentValue },
    { 45, "x", "Select", "this", "amount", ORANGE, ActionSelectAdustmentValue },
    { 46, "x", "Select", "this", "amount", ORANGE, ActionSelectAdustmentValue },
    { 47, "x", "Select", "this", "amount", ORANGE, ActionSelectAdustmentValue },
    { 48, "", "", "", "", BLACK } },
  { { VmenuFan0, "<<", "go to", "prior", "menu", GREEN, VmenuBase },
    { 51, "A PWM", "Adjust", "A", "PWM", AQUA, ActionAdjustSetpointFan, Values_1_3_5 },
    { 52, "B PWM", "Adjust", "B", "PWM", AQUA, ActionAdjustSetpointFan, Values_1_3_5 },
    { 53, "C PWM", "Adjust", "C", "PWM", AQUA, ActionAdjustSetpointFan, Values_1_3_5 },
    { 54, "D PWM", "Adjust", "D", "PWM", AQUA, ActionAdjustSetpointFan, Values_1_3_5 },
    { 55, "Btime", "Adjust", "B", "Time", AQUA, ActionAdjustSetpointFan, Values_1_0_0 },
    { 56, "Ctime", "Adjust", "C", "Time", AQUA, ActionAdjustSetpointFan, Values_1_0_0 },
    { 57, "GrBot", "Raise", "Fan", "Graph", AQUA, ActionAdjustFanGraphPixelBottom, Values_5_10_20 },
    { 58, "", "", "", "", BLACK } },
  { { VmenuEmpty0, ">>", "go to", "next", "menu", GREEN, VmenuBase },
    { -61, "", "go back", "to", "prior", AQUA },
    { -62, "", "go", "to", "prior", AQUA },
    { -63, "", "go back", "to", "prior", AQUA },
    { -64, "", "go back", "to", "prior", AQUA },
    { -65, "", "go back", "to", "prior", GREEN },
    { -66, "", "go back", "to", "prior", GREEN },
    { -67, "", "go back", "to", "prior", GREEN },
    { -68, "", "go back", "to", "prior", GREEN } },
  { { HmenuCTRL0, "Start", "Start", "the", "Roast", GREEN },
    { 71, "Stop", "Stop", "the", "roast", RED },
    { 72, "Fan", "Start", "the", "fan", AQUA },
    { 73, "rfs", "Refresh", "the", "screen", ORANGE },
    { 74, "Fwd", "Advance", "1", "minute", GREEN },
    { 75, "Back", "Go back", "1", "minute", GREEN },
    { 76, "", "go back", "to", "prior", GREEN },
    { -77, "", "go back", "to", "prior", GREEN },
    { -78, "", "go back", "to", "prior", GREEN } },
  { { HmenuFAN0, "FAN", "Decrease", "fan 10", "prior", AQUA },
    { 81, "-5", "Decrease", "fan 3", "prior", AQUA },
    { 82, "-1", "Increase", "fan 3", "prior", AQUA },
    { 83, "+1", "Increase", "fan 10", "prior", AQUA },
    { 84, "+5", "save as", "start", "prior", AQUA },
    { 85, "", "go back", "to", "prior", GREEN },
    { 86, "", "go back", "to", "prior", GREEN },
    { -87, "", "go back", "to", "prior", GREEN },
    { -88, "", "go back", "to", "prior", GREEN } }
};
rect OpProgessDisplay = { 80, 0, 0, 0 };
rect OpDetailDisplay = { 610, 200, 0, 0 };
rect ConfigDisplay = { 205, 385, 0, 0 };
buttontext myLocalbuttontext;

const byte PressNone = 0;
const byte PressMenu = 1;
const byte PressCongurationBox = 2;
const byte PressOperDetailBox = 3;

touchstatus TouchStatus;

int lastStateUpdated = -1;
int newState;
int State;
int StateDebug;


int TEMPCOILTOOHOT = 800;

boolean serialOutPutTempsBySecond;
boolean serialOutPutTempsBy3Seconds;
boolean serialOutPutStatusBy3Seconds;
boolean serialOutPutStatusBySecond;


MAX6675 thermocouple1(TC_SCK_A9, TC_CS1_A11, TC_SD1_A10);
MAX6675 thermocouple2(TC_SCK_A9, TC_CS2_A13, TC_SD2_A12);
MAX6675 thermocouple3(TC_SCK_A9, TC_CS3_A15, TC_SD3_A14);
const int ThermoCoil = 0;
const int ThermoBean1 = 1;
const int ThermoBean2 = 2;
MAX6675 thermocouples[] = {thermocouple1,thermocouple2,thermocouple3};

//for python serial  commands
char Commandsp1[7] = "xxxxx ";
int iCommandsp1 = 0;
char Commandsp[7] = "xxxxx ";
int iCommandsp = 0;
PWMSetpoint FanSetPoints[4];
int FanSpeed254PWM = -1;
point_byte TempPixelHistory[160];
point_byte FanPixelHistory[160];
point_byte CoilPixelHistory[160];
//800/160 = 5
graphhistory GraphHistory[] = {
  { ROLLAVGLINEID, 5,0,160, TempPixelHistory },
  { FANSPEEDLINEID, 5, 0,160, FanPixelHistory },
  { COILLINEID, 5, 0,160, CoilPixelHistory }
};

const int FanGraphXStart = 0;    //starting col of fan graph - a little past half
const int FanGraphXWidth = 720;  //uses 1/4 of screen width
const int FanGraphHeight = 340;  //uses 1/4 of screen width
const int FanGraphHorGridSpacingPWM = 15;
int FanGraphBottom = 450;
const int DisplayWidth = 800;
int FanGraphMinPWM = 100;
int FanGraphMaxPWM = 254;
float PixelsPerMinFan;

int GainTemp = 0;           //read from eeprom
double IntegralTemp = 0.1;  //read from eeprom
long unsigned IntegralLastTimeTemp = 0;
float IntegralSumTemp = 0;
unsigned long PIDWindowStartTimeTemp = 1000;
boolean PIDNewWindowTemp;
float ErrITemp = 0;
float ErrTemp = 0;
double DutyDebug = 0.0;
double DutyTemp;
unsigned int PIDIntegralUdateTimeValueTemp = 5000;
unsigned int PIDDutyWindowSizeTemp = 1000;

int GainFlow = 0;           //read from eeprom
double IntegralFlow = 0.1;  //read from eeprom
double PercentChangeFlow = .01;
long unsigned IntegralLastTimeFlow = 0;
float IntegralSumFlow = 0;
unsigned long PIDWindowStartTimeFlow;
boolean PIDNewWindowFlow;
float ErrIFlow = 0;
float ErrFlow = 0;
double DutyFan = - 99;
unsigned int PIDIntegralUdateTimeValueFlow = 1000;
unsigned int PIDWindowSizeFlow;

boolean setpointschanged = true;
int MyMinuteTemperature[30];
setpoint MySetPoints[6];
 int SetPointCount = 5;  //0,1,2,3,4,t0t5
//EndingSetPoint = 3;
int TimeScreenLeft = 0;
double TempPerPixL = 0;
double TempPerPixM = 0;
double TempPerPixH = 0;

int HorScaleLineY[30];
int HorScaleLineYValue[30];
int HorScaleLineYCount = 0;
double CurrentSetPointTemp = 0;

int LoopsPerSecond;
int LoopsPerSecondCalcing;

menustatus MenuStatus;
buttonsetdef* TouchButtonSet;
ClickHandler* TouchButtonHandler;

boolean TouchDetected;
boolean LongPressDetected;
int TouchButton;
char _debug;
Chrono TouchTimer(Chrono::MILLIS);
Chrono RoastTime(Chrono::SECONDS);
Chrono SecondTimer(Chrono::MILLIS);
double TimeSubSecondNext = 0;
double TimeSubSecondDuration = 50;
double TimeReadThermoNext = 0;
double TimeReadThermoDuration = 100;
Chrono SerialInputTimer(Chrono::MILLIS);
Chrono Serial1InputTimer(Chrono::MILLIS);
Chrono ThreeSecondTimer(Chrono::MILLIS);
Chrono PIDIntegralUdateTimeTemp(Chrono::MILLIS);
Chrono PIDIntegralUdateTimeFlow(Chrono::MILLIS);
Chrono MeasureTempTimer(Chrono::MILLIS);


//temps are read once per second
Average<int> TBeanAvgRoll(50,10);
Average<int> TCoilRoll(50,10);  //this is minute avg


//int OVERHEATFANCount;
int TEMPCOILTOOHOTCount;
int TempReachedCount;
int TempSpikeCount;
int TempReadingskipped[3];

buttonsetdef myHorControlMenuDef;
buttonsetdef myHorFanButtonControl;
buttonsetdef myButtonVertMenus[VmenuCount];

//int VmenuShowing = -1;
//int VmenuPrior = -1;

char s7[7];
char s6[6];
char s5[5];

char spFormat[6] = "%6.2F";
//used when drawing lines. We support up to 3 lines (see line ID constants)

point LastforLineID[GRAPHLINECOUNT];


double RoastMinutes = 0;
unsigned int RoastAcumHeat = 0;

int FanDeviation = 0;

boolean bNewTempsAvailable = false;
int ReadSensorInSequenceFlag = -1;
int TCoil;
int TBean1;
int TBean2;

bool ReadCoilCurrentFlag = false;
bool ReadBeanOpticalFlowRateFlag = false;

Bitcraze_PMW3901 BeanOpticalFlowSensor(BEAN_OPTICAL_FLOW_SPI_SSp48);

char debugflag = ' ';

byte CoilAmps = 0;

boolean ReadBeanFlowRate = false;

Average<int16_t> deltaYflow_avg(100,20);

int fastloopmillseconds = 50;

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

  pinMode(COILCURRENT_SPI_SSp53,OUTPUT);
  digitalWrite(COILCURRENT_SPI_SSp53, HIGH);

  pinMode(BEAN_OPTICAL_FLOW_SPI_SSp48,OUTPUT);
  digitalWrite(BEAN_OPTICAL_FLOW_SPI_SSp48, HIGH);
  
  //set fan and vibrator relays to high - cause that means off
  digitalWrite(FANRELAYp_2, RELAYOFF);
  //digitalWrite(VIBRELAYp, RELAYOFF);

  delay(1000);
  MaxVread = 512;  //this is the expected half way value
  RoastTime.stop();
  byte SetDefaults;
  EEPROM.get(SETDEFAULT_EP, SetDefaults);
  if (SetDefaults != 1) {
    //spDebug("Setting defaults with SetDefault value:" + String(SetDefaults));
    MySetPoints[0].Temperature = 250;
    MySetPoints[1].Temperature = 390;
    MySetPoints[2].Temperature = 410;
    MySetPoints[3].Temperature = 430;
    MySetPoints[4].Temperature = 440;
    for (int i = 0; i < 5; i++) {
      EEPROM.update(SETPOINTTEMP_EP[i], MySetPoints[i].Temperature);
    }
    FanSetPoints[0].PWM = 185;
    FanSetPoints[0].Minutes = 0;
    FanSetPoints[1].PWM = 160;
    FanSetPoints[1].Minutes = 2;
    FanSetPoints[2].PWM = 150;
    FanSetPoints[2].Minutes = 5;
    FanSetPoints[3].PWM = 135;
    FanSetPoints[3].Minutes = 14;
    for (int i = 0; i < 4; i++) {
      EEPROM.put(FanSetPoints_EP[i], FanSetPoints[i]);
    }
    EEPROM.put(RoastLength_EP, (int)14);
    GainTemp = 30;
    EEPROM.put(GAINTEMP_EP, GainTemp);
    IntegralTemp = .03;
    EEPROM.put(INTEGRALTEMP_EP, IntegralTemp);
    IntegralFlow = 0;
    EEPROM.put(INTEGRALFLOW_EP, IntegralFlow);
    GainFlow = 1;
    EEPROM.put(GAINFLOW_EP, GainFlow);
    setpointflow = 0;
    EEPROM.put(SETPOINTFLOW_EP, setpointflow);
    FanGraphBottom = 450;
    EEPROM.put(FANGRAPHBOTTOM_EP, FanGraphBottom);
    TEMPCOILTOOHOT = 850;
    EEPROM.put(TOOHOTTEMP_EP, TEMPCOILTOOHOT);
    EEPROM.put(OPERDETAILDISPLAY_X_EP, OpDetailDisplay.x);
    EEPROM.put(OPERDETAILDISPLAY_Y_EP, OpDetailDisplay.y);
    EEPROM.put(CONFIGURATIONDISPLAY_X_EP, ConfigDisplay.x);
    EEPROM.put(CONFIGURATIONDISPLAY_Y_EP, ConfigDisplay.y);

    SetDefaults = 1;
    EEPROM.put(SETDEFAULT_EP, SetDefaults);
    //spDebug("Done Setting defaults  SetDefault value to be saved is:" + String(SetDefaults));
  } else {
    for (int i = 0; i < 4; i++) {
      EEPROM.get(FanSetPoints_EP[i], FanSetPoints[i]);
    }
    EEPROM.get(GAINTEMP_EP, GainTemp);
    EEPROM.get(INTEGRALTEMP_EP, IntegralTemp);
    EEPROM.get(GAINFLOW_EP, GainFlow);
    EEPROM.get(INTEGRALFLOW_EP, IntegralFlow);
    EEPROM.get(SETPOINTFLOW_EP, setpointflow);
    EEPROM.get(FANGRAPHBOTTOM_EP, FanGraphBottom);
    EEPROM.get(TOOHOTTEMP_EP, TEMPCOILTOOHOT);
    EEPROM.get(OPERDETAILDISPLAY_X_EP, OpDetailDisplay.x);
    EEPROM.get(OPERDETAILDISPLAY_Y_EP, OpDetailDisplay.y);
    EEPROM.get(CONFIGURATIONDISPLAY_X_EP, ConfigDisplay.x);
    EEPROM.get(CONFIGURATIONDISPLAY_Y_EP, ConfigDisplay.y);
  }



  //if (IntegralTemp > 1) IntegralTemp = 0.00;
  //if (GainTemp > 75) GainTemp = 75;
  //if (GainTemp < 10) GainTemp = 10;


  //FanGraphBottom = 450;
  //EEPROM.put(FANGRAPHBOTTOM_EP, FanGraphBottom);


  SecondTimer.restart(0);



  // -------------------------------------------------------------
  if (HasDisplay == true) {
    //Serial.println("Initializing LCD");

    myGLCD.InitLCD();
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




  FanSpeed254PWM = FanSetPoints[0].PWM;

  //Serial.println(F("wire i2c for fan begin"));


  Wire.begin();

  StopAndSendFanPWM();


  graphProfile();
  Serial.println("loop is starting...");
}


void loop() {
  theloop();
}

#include "DefaultValues.h"
#include <digitalWriteFast.h>
#include <SPI.h>
#include <EEPROM.h>
//#include <UTouchCD.h>
#include <Chrono.h>
#include <Time.h>
#include <TimeLib.h>
#include "src/max6675.h"
#include "src/PMW3901.h"
#include "Average.h"
#include <EEPROM.h>
#include <Wire.h>
#include "B_MyTypes.h"
#include <UTFT.h>
#include <UTouch.h>
#include <stdio.h>  // for function sprintf
#include <stdarg.h>
#include <Arduino.h>
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else                           // __ARM__
extern char* __brkval;
#endif                          // __arm__
extern uint8_t SmallFont[];     //8 x 12
extern uint8_t Retro8x16[];     //8 x 16
extern uint8_t BigFont[];       //16 x 16
extern uint8_t arial_normal[];  //16 x 16
extern uint8_t arial_bold[];    //16 x 16
extern uint8_t Grotesk16x32[];  //16 x 32
extern uint8_t Grotesk24x48[];  //24 x 48


UTFT myGLCD(SSD1963_800480, 38, 39, 40, 41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch myTouch(43, 42, 44, 45, 46);           //byte tclk, byte tcs, byte din, byte dout, byte irq

//#define Font8x12 SmallFont
//#define Font8x16 Retro8x16
#define FONT16X16 BigFont
//#define Font16x32 Grotesk16x32
//#define Font16x48 Grotesk16x48
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
#define SPDEBUG Serial.println
#define SPDEBUGXCLOSE }
#define SPDEBUG1 \
  if (Debugbyte == 1) { \
  Serial.println
#define SPDEBUG2 \
  if (Debugbyte == 2) { \
  Serial.println
#define SPDEBUG3 \
  if (Debugbyte == 3) { \
  Serial.println
#define SPDEBUG4 \
  if (Debugbyte == 4) { \
  Serial.println
#define SPDEBUG5 \
  if (Debugbyte == 5) { \
  Serial.println
byte Debugbyte = 0;
//PIN  definitions
//AJN 5 TFT inch display shield does not use digital pins 30-34, 10 , 12,  13,  i2c pins 20,21, or miso pins 39 to 54
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
#define BEAN_OPTICAL_FLOW_SPI_SSp48 48
#define BEAN_OPTICAL_FLOW2_SPI_SSp49 49
#define COILCURRENT_SPI_SSp53 53
//#define available A0
//#define available A1
#define FANOUTG_A2 A2    // can be replaced with wire to add-on CB
#define FANOUTVCC_A3 A3  // can be replaced with wire to add-on CB
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
#define TCOILID 0
#define TBEAN1ID 1
#define TBEAN2ID 2
#define SSRFULL 1
#define SSRPWM 2
#define SSROFF 3
//EPROM MEMORORY
#define INTEGRALTEMP_EP 0
#define LOADDEFAULTS_EP 4
#define RoastLength_EP 36
//#define FanGraphMinPWM_EP 37
//#define FanGraphMaxPWM_EP 38
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

#define PROFILELINEID 0
#define ROLLAVGLINEID 1
#define ROLLMAXLINEID 2
#define COILLINEID 3
#define FANSPEEDLINEID 4
#define ROLLMINLINEID 5
#define GRAPHLINECOUNT 6


error_status ErrorStatus;
const char ErrDisplay[6] = "ERROR";
const int NoError = -1;
const int ErrorNeedFanFirst = 0;
const int ErrorCoilTooHot = 1;
const int ErrorPleaseRestart = 2;
const error ErrorList[] = {
  { 0, "!!Turn on fan before you start!!" },
  { 1, "!!Coil exceeds Too Hot Temp!!" },
  { 2, "Repower to load defaults.  Click 'Reset' again to cancel" }
};
byte DisplayMessageToShow = 0;
const int DisplayDefault = 0;
const int DisplayMsgAdvanced = 1;
const int DisplayMsgRetarded = 2;
byte SSR1Status = 0;
byte SSR2Status = 0;
const ssrstatus SSRStatus[4] = { " NA", " ON", "PWM", "OFF" };
const int FanSetPoints_EP[] = { 50, 55, 60, 65 };         //these are EEprom memory locations - not data
const int SetpointTemp_EP[] = { 5, 10, 15, 20, 25, 30 };  //these are EEprom memory locations - not data
int TempCoolingDone = 250;
int FreeMemory = 0;
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

int16_t BeanYflow, BeanYFlowSetpoint;
const uint16_t LineColorforLineID[GRAPHLINECOUNT] = { WHITE, YELLOW, RED, RED, YELLOW, ORANGE };
const boolean LineBoldforLineID[GRAPHLINECOUNT] = { false, false, false, false, true, false };

#define RELAYON LOW
#define RELAYOFF HIGH
#define VALUESONLY true
#define ALL false
#define BEINGMOVED true
#define REGULAR false


#define VALUESD01_D03_D05 0
#define VALUES1_3_5 1
#define VALUES1_3_10 2
#define VALUES1_0_0 3
#define VALUES5_10_20 4
#define VALUESD5_D1_D01 5
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
  { .5, .1, .01 }
};

activeadjustment ActiveAdjustment;
#define ACTIONSHOWSETPOINTSELECTMENU 30
#define ACTIONSHOWSETPOINTFANMENU 31
#define ACTIONGETLABLEFROMPRIOR 32
#define ACTIONSELECTADUSTMENTVALUE 33
#define ACTIONRESETTODEFAULTONNEXTSTART 34
#define ACTIONADJUSTMENTS 40
#define ACTIONADJUSTINTEGRALTEMP 41
#define ACTIONADJUSTGAINTEMP 42
#define ACTIONADJUSTSETPOINTTEMP 43
#define ACTIONADJUSTSETPOINTFAN 44
#define ACTIONADJUSTROASTLENGTH 45
#define ACTIONADJUSTTEMPTOHOT 46
#define ACTIONADJUSTCOOLDOWNTEMP 47
#define ActionAdjustCoilGraphTempOffset 48
#define ActionAdjustFanGraphPixelBottom 48
#define ACTIONADJUSTTEMPDUTY 50
#define ACTIONADJUSTINTEGRALFLOW 51
#define ACTIONADJUSTGAINFLOW 52
#define ACTIONADJUSTSETPOINTFLOW 53
#define VMENUCOUNT 7
#define MAXBUTTONCOUNT 9
#define VMENUBASE 0
#define VMENUSETPOINTSELECT 1
#define VMENUDEBUG 2
#define VMENUONOFF 3
#define VMENUADJUSTVALUE 4
#define VMENUFAN 5
#define VMENUEMPTY 6
#define HMENUCTRL 7
#define HMENUFAN 8
#define VMENUBASE0 0
#define VMENUSETPOINTSELECT0 10
#define VMENUDEBUG0 20
#define VMENUONOFF0 30
#define VMENUADJUSTVALUE0 40
#define VMENUFAN0 50
#define VMENUEMPTY0 60
#define HMENUCTRL0 70
#define HMENUFAN0 80
#define VMENUFINDPRIOR 11

byte DrawLablesRotated = 60;
byte GetLablesFromPrior = 41;

const buttontext PROGMEM Vmenutext[][MAXBUTTONCOUNT] = {
  { { VMENUBASE0, ">>", "Show", "debug", "menu", GREEN, VMENUDEBUG },
    { 1, "SPs", "Show", "Temp", "menu", YELLOW, ACTIONSHOWSETPOINTSELECTMENU },
    { 2, "GainT", "Adjust", "T PID", "Gain", YELLOW, ACTIONADJUSTGAINTEMP, VALUES1_3_5 },
    { 3, "IntT", "Adjust", "T PID", "Interg", YELLOW, ACTIONADJUSTINTEGRALTEMP, VALUESD01_D03_D05 },
    { 4, "Fan", "Show", "Fan", "menu", YELLOW, ACTIONSHOWSETPOINTFANMENU },
    { 5, "ThT", "Adjust", "TooHot", "Temp", YELLOW, ACTIONADJUSTTEMPTOHOT, VALUES1_3_10 },
    { 6, "CdT", "Adjust", "CoolDwn", "Temp", YELLOW, ACTIONADJUSTCOOLDOWNTEMP, VALUES1_3_10 },
    { 7, "Coil", "Adjust", "CoilTmp", "Offset", YELLOW, ActionAdjustCoilGraphTempOffset, VALUES1_3_10 },
    { 8, "Reset", "Load", "Default", "Configs", RED, ACTIONRESETTODEFAULTONNEXTSTART } },
  { { VMENUSETPOINTSELECT0, "<<", "back", "to prior", "menu", GREEN, VMENUBASE },
    { 11, "T sp0", "Adjust", "point", "#0", YELLOW, ACTIONADJUSTSETPOINTTEMP, VALUES1_3_5 },
    { 12, "T sp1", "Adjust", "point", "#1", YELLOW, ACTIONADJUSTSETPOINTTEMP, VALUES1_3_5 },
    { 13, "T sp2", "Adjust", "point", "#2", YELLOW, ACTIONADJUSTSETPOINTTEMP, VALUES1_3_5 },
    { 14, "T sp3", "Adjust", "point", "#3", YELLOW, ACTIONADJUSTSETPOINTTEMP, VALUES1_3_5 },
    { 15, "T sp4", "Adjust", "point", "#4", YELLOW, ACTIONADJUSTSETPOINTTEMP, VALUES1_3_5 },
    { 16, "Last 2", "Adjust", "last 2", "points", YELLOW, ACTIONADJUSTSETPOINTTEMP, VALUES1_3_5 },
    { 17, "Last 3", "Adjust", "last 3", "points", YELLOW, ACTIONADJUSTSETPOINTTEMP, VALUES1_3_5 },
    { 18, "", "", "", "", BLACK } },
  { { VMENUDEBUG0, ">>", "show", "empty", "menu", GREEN, VMENUEMPTY },
    { 21, "SetDut", "Set", "Duty", "Manua", GREEN, ACTIONADJUSTTEMPDUTY, VALUESD5_D1_D01 },
    { 22, "", "subject", "of", "menu", YELLOW },
    { 23, "C1", "toggle", "coil 1 SSR", "on and off", YELLOW },
    { 24, "C2", "toggle", "coil 2 SSR", "on and off", YELLOW },
    { 25, "FanG", "Adjust", "F PID", "Gain", AQUA, ACTIONADJUSTGAINFLOW, VALUES1_3_5 },
    { 26, "FanI", "Adjust", "F PID", "Interg", AQUA, ACTIONADJUSTINTEGRALFLOW, VALUESD01_D03_D05 },
    { 27, "FanSP", "Adjust", "F", "Setpnt", AQUA, ACTIONADJUSTSETPOINTFLOW, VALUES1_3_5 },
    { 28, "Tem", "go back", "to", "prior", YELLOW } },
  { { VMENUONOFF0, "<<", "go to", "prior", "menu", GREEN, VMENUFINDPRIOR },
    { 31, "", "selected", "device", "to debug", GREEN },
    { 32, "ON", "turn", "device", "on", ORANGE },
    { 33, "OFF", "turn", "device", "off", ORANGE },
    { 34, "", "go back", "to", "prior", BLACK },
    { 35, "", "go back", "to", "prior", BLACK },
    { 36, "", "go back", "to", "prior", BLACK },
    { 37, "", "go back", "to", "prior", BLACK },
    { 38, "", "go back", "to", "prior", BLACK } },
  { { VMENUADJUSTVALUE0, "<<", "go back", "to", "prior", GREEN, VMENUFINDPRIOR },
    { 41, "", "What's", "being", "edited", GREEN, ACTIONGETLABLEFROMPRIOR },
    { 42, "x", "Select", "this", "amount", ORANGE, ACTIONSELECTADUSTMENTVALUE },
    { 43, "x", "Select", "this", "amount", ORANGE, ACTIONSELECTADUSTMENTVALUE },
    { 44, "x", "Select", "this", "amount", ORANGE, ACTIONSELECTADUSTMENTVALUE },
    { 45, "x", "Select", "this", "amount", ORANGE, ACTIONSELECTADUSTMENTVALUE },
    { 46, "x", "Select", "this", "amount", ORANGE, ACTIONSELECTADUSTMENTVALUE },
    { 47, "x", "Select", "this", "amount", ORANGE, ACTIONSELECTADUSTMENTVALUE },
    { 48, "", "", "", "", BLACK } },
  { { VMENUFAN0, "<<", "go to", "prior", "menu", GREEN, VMENUBASE },
    { 51, "A PWM", "Adjust", "A", "PWM", AQUA, ACTIONADJUSTSETPOINTFAN, VALUES1_3_5 },
    { 52, "B PWM", "Adjust", "B", "PWM", AQUA, ACTIONADJUSTSETPOINTFAN, VALUES1_3_5 },
    { 53, "C PWM", "Adjust", "C", "PWM", AQUA, ACTIONADJUSTSETPOINTFAN, VALUES1_3_5 },
    { 54, "D PWM", "Adjust", "D", "PWM", AQUA, ACTIONADJUSTSETPOINTFAN, VALUES1_3_5 },
    { 55, "Btime", "Adjust", "B", "Time", AQUA, ACTIONADJUSTSETPOINTFAN, VALUES1_0_0 },
    { 56, "Ctime", "Adjust", "C", "Time", AQUA, ACTIONADJUSTSETPOINTFAN, VALUES1_0_0 },
    { 57, "GrBot", "Raise", "Fan", "Graph", AQUA, ActionAdjustFanGraphPixelBottom, VALUES5_10_20 },
    { 58, "", "", "", "", BLACK } },
  { { VMENUEMPTY0, ">>", "go to", "next", "menu", GREEN, VMENUBASE },
    { -61, "", "go back", "to", "prior", AQUA },
    { -62, "", "go", "to", "prior", AQUA },
    { -63, "", "go back", "to", "prior", AQUA },
    { -64, "", "go back", "to", "prior", AQUA },
    { -65, "", "go back", "to", "prior", GREEN },
    { -66, "", "go back", "to", "prior", GREEN },
    { -67, "", "go back", "to", "prior", GREEN },
    { -68, "", "go back", "to", "prior", GREEN } },
  { { HMENUCTRL0, "Start", "Start", "the", "Roast", GREEN },
    { 71, "Stop", "Stop", "the", "roast", RED },
    { 72, "Fan", "Start", "the", "fan", AQUA },
    { 73, "rfs", "Refresh", "the", "screen", ORANGE },
    { 74, "Fwd", "Advance", "1", "minute", GREEN },
    { 75, "Back", "Go back", "1", "minute", GREEN },
    { 76, "", "go back", "to", "prior", GREEN },
    { -77, "", "go back", "to", "prior", GREEN },
    { -78, "", "go back", "to", "prior", GREEN } },
  { { HMENUFAN0, "FAN", "Decrease", "fan 10", "prior", AQUA },
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

int LastStateUpdated = -1;
int NewState;
int State;
int StateDebug;


boolean SerialOutPutTempsBySecond;
boolean SerialOutPutTempsBy3Seconds;
boolean SerialOutPutStatusBy3Seconds;
boolean SerialOutPutStatusBySecond;

const int ThermoCoil = 0;
const int ThermoBean1 = 1;
const int ThermoBean2 = 2;

MAX6675 Thermocouple1(TC_SCK_A9, TC_CS1_A11, TC_SD1_A10);
MAX6675 Thermocouple2(TC_SCK_A9, TC_CS2_A13, TC_SD2_A12);
MAX6675 Thermocouple3(TC_SCK_A9, TC_CS3_A15, TC_SD3_A14);
thermocouple thermocouples[] = { { Thermocouple1, 0 }, { Thermocouple2, 0 }, { Thermocouple3, 0 } };
PWMSetpoint FanSetPoints[4];
int FanSpeed254PWM = -1;

point_byte TempPixelHistory[160];
point_byte FanPixelHistory[160];
point_byte CoilPixelHistory[160];
//800/160 = 5
graphhistory GraphHistory[] = {
  { ROLLAVGLINEID, 5, 0, 160, TempPixelHistory },
  { FANSPEEDLINEID, 5, 0, 160, FanPixelHistory },
  { COILLINEID, 5, 0, 160, CoilPixelHistory }
};
//used when drawing lines. We support up to 3 lines (see line ID constants)
point LastforLineID[GRAPHLINECOUNT];
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
float ErrITemp = 0;
float ErrTemp = 0;
double DutyDebug = 0.0;
double DutyTemp;
unsigned int PIDIntegralUdateTimeValueTemp = _PIDIntegralUdateTimeValueTemp;
int GainFlow = 0;           //read from eeprom
double IntegralFlow = 0.1;  //read from eeprom
double MaxPercentChangeFlow = _MaxPercentChangeFlow;
long unsigned IntegralLastTimeFlow = 0;
float IntegralSumFlow = 0;
unsigned long PIDWindowStartTimeFlow;
boolean PIDNewWindowFlow;
float ErrIFlow = 0;
float ErrFlow = 0;
double DutyFan = -99;
unsigned int PIDIntegralUdateTimeValueFlow = _PIDIntegralUdateTimeValueFlow;
unsigned int PIDWindowSizeFlow;
boolean Setpointschanged = true;
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
long TimeOpticalFlowNext = 0;
long TimeOpticalFlowDuration = _TimeOpticalFlowDuration;
long TimeReadThermoNext = 0;
long TimeReadThermoDuration = _TimeReadThermoDuration;
Chrono SerialInputTimer(Chrono::MILLIS);
Chrono Serial1InputTimer(Chrono::MILLIS);
Chrono ThreeSecondTimer(Chrono::MILLIS);
Chrono PIDIntegralUdateTimeTemp(Chrono::MILLIS);
Chrono PIDIntegralUdateTimeFlow(Chrono::MILLIS);
Chrono MeasureTempTimer(Chrono::MILLIS);
//temps are read once per second
Average<int> TBeanAvgRoll(_BeanYflow_avg_sizemax);
Average<int> TCoilAvgRoll(_TCoilAvgRoll_sizemax);  //this is minute avg
int TempCoilTooHotCount;
int TempCoilTooHot;
int TempReachedCount;
buttonsetdef HorControlMenuDef;
buttonsetdef HorFanMenuDef;
buttonsetdef VertMenuDefs[VMENUCOUNT];
char s7[7];
char s6[6];
char s5[5];
char s4[4];
char s3[3];
char s2[2];
char s1[1];
char s0[0];
char* CharArrays[] = {
  s0,
  s1,
  s2,
  s3,
  s4,
  s5,
  s6,
  s7
};
char spFormat[6] = "%6.2F";
double RoastMinutes = 0;
unsigned int RoastAcumHeat = 0;
int FanDeviation = 0;
boolean bNewTempsAvailable = false;
int ReadSensorInSequenceFlag = -1;
int CoilTempOffSet = _CoilTempOffSet;
int TCoil;
int TBean1;
int TBean2;
float FanCoolingBoostPercent = _FanCoolingBoostPercent;
bool ReadCoilCurrentFlag = false;
bool ReadBeanOpticalFlowRateFlag = false;
bool FanLegacy = false;
PMW3901 BeanOpticalFlow1(BEAN_OPTICAL_FLOW_SPI_SSp48);
PMW3901 BeanOpticalFlow2(BEAN_OPTICAL_FLOW2_SPI_SSp49);
opticalflow BeanOpticalFlowSensors[] = { { BeanOpticalFlow1, 0 }, { BeanOpticalFlow2, 0 } };
byte BeanOpticalFlowReadsPerSecond;
byte BeanOpticalFlowReadsPerSecondCalcing;
boolean ReadBeanFlowRate = false;
Average<byte> BeanYflow_avg(_BeanYflow_avg_sizemax);
byte TempSensorReadsPerSecond;
byte TempSensorReadsPerSecondCalcing;
char debugflag = ' ';
byte CoilAmps = 0;
int TBeanAvg;
boolean HasDisplay = true;
// =====================================================================================================================================================================
// SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP
// =====================================================================================================================================================================
void setup() {
  //Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("setup starting");
  //set pmw for  pins 5 and 6 36 hrz
  TCCR4B = TCCR4B & B11111000 | B00000101;
  pinMode(SSR1_p7, OUTPUT);
  pinMode(SSR2_p6, OUTPUT);
  pinMode(FANRELAYVCCp_3, OUTPUT);
  pinMode(FANRELAYp_2, OUTPUT);
  digitalWrite(FANRELAYVCCp_3, HIGH);  //5V
  pinMode(SSRgr_14, OUTPUT);
  digitalWrite(SSRgr_14, LOW);
  pinMode(FANOUTVCC_A3, OUTPUT);
  pinMode(FANOUTG_A2, OUTPUT);
  digitalWrite(FANOUTVCC_A3, LOW);  //0V
  digitalWrite(FANOUTG_A2, HIGH);   //5V
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
  pinMode(COILCURRENT_SPI_SSp53, OUTPUT);
  digitalWrite(COILCURRENT_SPI_SSp53, HIGH);
  pinMode(BEAN_OPTICAL_FLOW_SPI_SSp48, OUTPUT);
  digitalWrite(BEAN_OPTICAL_FLOW_SPI_SSp48, HIGH);
  digitalWrite(FANRELAYp_2, RELAYOFF);
  //digitalWrite(VIBRELAYp, RELAYOFF);
  delay(1000);
  RoastTime.stop();
  BeanYflow_avg._size = _BeanYflow_avg_size;
  TBeanAvgRoll._size = _TBeanAvgRoll_size;
  TCoilAvgRoll._size = _TCoilAvgRoll_size;
  bool loadDefaults;
  EEPROM.get(LOADDEFAULTS_EP, loadDefaults);  //will be 255 (T) on first run.. then set 1 (T) or 0 (F)
  if (loadDefaults == true) {
    //SPDEBUG("Setting defaults with SetDefault value:" + String(SetDefaults));
    MySetPoints[0].Temperature = _MySetPoints_0_Temperature;
    MySetPoints[1].Temperature = _MySetPoints_1_Temperature;
    MySetPoints[2].Temperature = _MySetPoints_2_Temperature;
    MySetPoints[3].Temperature = _MySetPoints_3_Temperature;
    MySetPoints[4].Temperature = _MySetPoints_4_Temperature;
    for (int i = 0; i < 5; i++) {
      EEPROM.update(SetpointTemp_EP[i], MySetPoints[i].Temperature);
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
    GainTemp = _GainTemp;
    EEPROM.put(GAINTEMP_EP, _GainTemp);
    IntegralTemp = _IntegralTemp;
    EEPROM.put(INTEGRALTEMP_EP, IntegralTemp);
    IntegralFlow = _IntegralFlow;
    EEPROM.put(INTEGRALFLOW_EP, IntegralFlow);
    GainFlow = _GainFlow;
    EEPROM.put(GAINFLOW_EP, GainFlow);
    BeanYFlowSetpoint = _BeanYFlowSetpoint;
    EEPROM.put(SETPOINTFLOW_EP, BeanYFlowSetpoint);
    FanGraphBottom = 450;
    EEPROM.put(FANGRAPHBOTTOM_EP, FanGraphBottom);
    TempCoilTooHot = _TempCoilTooHot;
    EEPROM.put(TOOHOTTEMP_EP, TempCoilTooHot);
    OpDetailDisplay.x = _OpDetailDisplay_x;
    OpDetailDisplay.y = _OpDetailDisplay_y;
    ConfigDisplay.x = _ConfigDisplay_x;
    ConfigDisplay.y = _ConfigDisplay_y;
    EEPROM.put(OPERDETAILDISPLAY_X_EP, OpDetailDisplay.x);
    EEPROM.put(OPERDETAILDISPLAY_Y_EP, OpDetailDisplay.y);
    EEPROM.put(CONFIGURATIONDISPLAY_X_EP, ConfigDisplay.x);
    EEPROM.put(CONFIGURATIONDISPLAY_Y_EP, ConfigDisplay.y);
    loadDefaults == false;
    EEPROM.put(LOADDEFAULTS_EP, loadDefaults);
    //SPDEBUG("Done Setting defaults  SetDefault value to be saved is:" + String(SetDefaults));
  }
  //temp set points are read  in graph screen function
  for (int i = 0; i < 4; i++) {
    EEPROM.get(FanSetPoints_EP[i], FanSetPoints[i]);
  }
  EEPROM.get(GAINTEMP_EP, GainTemp);
  EEPROM.get(INTEGRALTEMP_EP, IntegralTemp);
  EEPROM.get(GAINFLOW_EP, GainFlow);
  EEPROM.get(INTEGRALFLOW_EP, IntegralFlow);
  EEPROM.get(SETPOINTFLOW_EP, BeanYFlowSetpoint);
  EEPROM.get(FANGRAPHBOTTOM_EP, FanGraphBottom);
  EEPROM.get(TOOHOTTEMP_EP, TempCoilTooHot);
  EEPROM.get(OPERDETAILDISPLAY_X_EP, OpDetailDisplay.x);
  EEPROM.get(OPERDETAILDISPLAY_Y_EP, OpDetailDisplay.y);
  EEPROM.get(CONFIGURATIONDISPLAY_X_EP, ConfigDisplay.x);
  EEPROM.get(CONFIGURATIONDISPLAY_Y_EP, ConfigDisplay.y);
  SecondTimer.restart(0);
  // -------------------------------------------------------------
  if (HasDisplay == true) {
    myGLCD.InitLCD();
    pinMode(8, OUTPUT);     //backlight
    digitalWrite(8, HIGH);  //on
    myTouch.InitTouch();
    myTouch.setPrecision(PREC_MEDIUM);
  }
  delay(1000);
  intializeMenus();
  State = STATESTOPPED;
  Setpointschanged = true;
  FanSpeed254PWM = FanSetPoints[0].PWM;
  Wire.begin();
  StopAndSendFanPWM();
  graphProfile();
  Serial.println("loop is starting...");
}
void loop() {
  theloop();
}

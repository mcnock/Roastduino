
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
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegmentFull[];
extern uint8_t Grotesk24x48[];
extern uint8_t Retro8x16[];

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
#define ROLLMAXLINEID 2
#define COILLINEID 3
#define FANSPEEDLINEID 4
#define ROLLMINLINEID 5
#define GRAPHLINECOUNT 6

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

const char Mname0  = "Vmenubase";
const char Mname1  = "VmenuSetPointSelect";
const char Mname2  = "VmenuAdj_1_3_5";
const char Mname3  = "VmenuDebug";
const char Mname4  = "VmenuOnOff";
const char Mname5  = "VmenuAjd_01";
const char Mname6  = "VMenuAdj_1_5_10_V";
const char Mname7  = "VmenuFan";
const char Mname8  = "VmenuEmpty";
const char Mname9  = "HmenuCTRL";
const char Mname10  = "HmenuFAN";

const char*  menunames[] = {
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
  { {0,">>" ,"go back"  ,"to"     ,"prior",GREEN},
    {1,"" ,"go back"  ,"to"     ,"prior",GREEN},
    {2,"Gain" ,"Ajdust" ,"Gain"   ,"Value",YELLOW},
    {3,"Int" ,"Ajdust"  ,"Intergal","Value",YELLOW},
    {4,"SPs" ,"Select"  ,"Setpoints","to adjust",YELLOW},
    {5,"Fan" ,"Adjust"  ,"fan auto","decrease",YELLOW},
    {6,"rT+1" ,"Add 1"    ,"minute to" ,"this roast",YELLOW},
    {7,"rT-1" ,"Remove 1" ,"minute tp" ,"this roast",YELLOW},
    {8,"cCut" ,"Adjust" ,"Hightemp","Cut out",YELLOW}
  },
  { {10,"<<"  ,"back"       ,"to prior" ,"menu",GREEN},
    {11,"sp1" ,"Adjust"     ,"setpoint" ,"#1",YELLOW},
    {12,"sp2" ,"Adjust"     ,"setpoint" ,"#2",YELLOW},
    {13,"sp3" ,"Adjust"     ,"setpoint" ,"#3",YELLOW},
    {14,"sp4" ,"Adjust"     ,"setpoint" ,"#4",YELLOW},
    {15,"sp5" ,"Adjust"     ,"setpoint" ,"#5",YELLOW},
    {16,"ls3" ,"Adjust"     ,"last 3"   ,"setpoint",YELLOW},
    {17,"T+1" ,"Increase"   ,"roast len"    ,"by 1 min",YELLOW},
    {18,"T-1" ,"Decrease"   ,"roast len"    ,"by 1 min",YELLOW}
  },
  { {20,"<<" ,"go back","to","prior",GREEN},
    {21,"" ,"go back","to","prior",YELLOW},
    {22,"+1" ,"go back","to","prior",ORANGE},
    {23,"+3" ,"go back","to","prior",ORANGE},
    {24,"+5" ,"go back","to","prior",ORANGE},
    {25,"-1" ,"go back","to","prior",ORANGE},
    {26,"-3" ,"go back","to","prior",ORANGE},
    {27,"-5" ,"go back","to","prior",ORANGE},
    {28,"SAVE" ,"go back","to","prior",YELLOW}
  },
  { {30,">>" ,"go to","next","menu",GREEN},
    {31,"" ,"go back","to","prior",GREEN},
    {32,"DBG" ,"subject","of","menu",YELLOW},
    {33,"C1" ,"toggle","coil 1 SSR","on and off",YELLOW},
    {34,"C2" ,"toggle","coil 2 SSR","on and off",YELLOW},
    {35,"" ,"go back","to","prior",YELLOW},
    {36,"Fan" ,"toggle","fan relay","on and off",YELLOW},
    {37,"Duty" ,"Manually","set","duty",YELLOW},
    {38,"Temp" ,"go back","to","prior",YELLOW}
  },  
  { {40,"<<" ,"go to","prior","menu",GREEN},
    {41,"" ,"selected","device","to debug",GREEN},
    {42,"ON" ,"turn","device","on",ORANGE},
    {43,"OFF" ,"turn","device","off",ORANGE},
    {44,"" ,"go back","to","prior",BLACK},
    {45,"" ,"go back","to","prior",BLACK},
    {46,"" ,"go back","to","prior",BLACK},
    {47,"" ,"go back","to","prior",BLACK},
    {48,"" ,"go back","to","prior",BLACK}
  }
  ,
  { {50,"<<" ,"go back","to","prior",GREEN},
    {51,"" ,"go back","to","prior",GREEN},
    {52,"+.01" ,"go back","to","prior",ORANGE},
    {53,"+.05" ,"go back","to","prior",ORANGE},
    {54,"+.10" ,"go back","to","prior",ORANGE},
    {55,"-.01" ,"go back","to","prior",ORANGE},
    {56,"-.05" ,"go back","to","prior",ORANGE},
    {57,"-.10" ,"go back","to","prior",ORANGE},
    {58,"Save" ,"Save","and","close",GREEN}
  }
 ,
  { {60,"<<" ,"go to","prior","menu",GREEN},
    {61,"" ,"go back","to","prior",GREEN},
    {62,"+1" ,"go back","to","prior",ORANGE},
    {63,"+5" ,"go back","to","prior",ORANGE},
    {64,"+10" ,"go back","to","prior",ORANGE},
    {65,"-1" ,"go back","to","prior",ORANGE},
    {66,"-5" ,"go back","to","prior",ORANGE},
    {67,"-10" ,"go back","to","prior",ORANGE},
    {68,"Save" ,"Save","and","close",GREEN}
  }
 ,
  { {70,"<<" , "go to"      ,"prior"      ,"menu",GREEN},
    {71,"A+1" ,"Add 1 min"  ,"to start"   ,"period",ORANGE},
    {72,"A-1" ,"Rmv 1 min"  ,"to start"   ,"period",ORANGE},
    {73,"A" ,"Adjust" ,"start" ," PWM" ,ORANGE},
    {74,"B" ,"Adjust" ,"drop in"," PWM" ,ORANGE},
    {75,"C+1" ,"Rmv 1 unit" ,"to fan"     ,"Drop" ,ORANGE},
    {76,"C-1" ,"Rmv 5 unit" ,"to fan"     ,"Drop" ,ORANGE},
    {77,"" ,"Add 1 min"  ,"to middle"  ,"Period" ,ORANGE},
    {78,"" ,"Rmv 1 min"  ,"to middle"  ,"period",ORANGE}
  }
,
  { {80,">>" ,"go to","next","menu",GREEN},
    {81,""  ,"go back","to","prior",AQUA},
    {82,""   ,"go","to","prior",   AQUA},
    {83,"" ,"go back","to","prior",AQUA},
    {84,""  ,"go back","to","prior",AQUA},
    {85,"" ,"go back","to","prior",GREEN},
    {86,"" ,"go back","to","prior",GREEN},
    {87,"" ,"go back","to","prior",GREEN},
    {88,"" ,"go back","to","prior",GREEN}
  }
,  
  
  { 
    {90,"Start" ,"Start"      ,"Roast"  ,"",GREEN},
    {91,"Stop"  ,"End Roast"  ,"or Fan" ,"" ,RED},
    {92,"Fan"   ,"Start"      ,"Fan"    ,""  ,AQUA},
    {93, "rfs"  ,"Redraw"     ,"screen" ,"",WHITE},
    {94,"^" ,"go back","to","prior",GREEN},
    {95,"" ,"go back","to","prior",GREEN},
    {96,"" ,"go back","to","prior",GREEN},
    {97,"" ,"go back","to","prior",GREEN},
    {98,"" ,"go back","to","prior",GREEN}
    
  }
  ,
  { {100,"-10" ,"Decrease","fan 10","prior",AQUA},
    {101,"-3"  ,"Decrease","fan 3","prior",AQUA},
    {102,"+3"   ,"Increase","fan 3","prior",AQUA},
    {103,"+10" ,"Increase","fan 10","prior",AQUA},
    {104,"SAVE" ,"save as","start","prior",AQUA},
    {105,"" ,"go back","to","prior",GREEN},
    {106,"" ,"go back","to","prior",GREEN},
    {107,"" ,"go back","to","prior",GREEN},
    {108,"" ,"go back","to","prior",GREEN}
  }
  
};
 

buttontext myArrayLocal;

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

char _debug ='a';
int FanSpeedPWM = 0;
int FanSpeedPWMStart = 0;



//int FanSpeedPWMAutoEnd=0;
int FanSpeedPWMAutoDecrease = 90;
int FanSpeedPWMAutoDecreaseStart = 90;
bool FanSpeedPWMAutoMode = false;
int FanSpeedPWNDelayDecreaseByMinutes = 2;
int FanSpeedPWNDecreaseByMinutes = 8;
int FanSpeedResistanceLast = 0;
int FanSpeedResistanceCurrent = 0;


const int LastPixelArrayCount = 200;
point myLastTempGraphPixels[LastPixelArrayCount];
point myLastFanGraphPixels[LastPixelArrayCount];

int SkipTempCount;
const int SkipTempLimit = 6;  
int SkipFanCount;
const int SkipFanLimit = 6;

int myLastTempGraphPixelsP = 0;
int myLastFanGraphPixelsP = 0;
const int FanGraphXStart = 410;//starting col of fan graph - a little past half
const int FanGraphXWidth = 310;  //uses 1/4 of screen width
const int FanGraphHeight = 125;  //uses 1/4 of screen width
const int FanGraphHorGridSpacingPWM = 15;
const int FanGraphBottom = 455;
const int FanGraphMinPWM = 150;
const int FanGraphMaxPWM = 254;

long PixelsPerMinFan;
   //Y for a temp, for values under 300 is linear. So we are going graph the 0-254 values of fan as if they where temps   

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


const int SetPointCount = 6;  //0,1,2,3,4,t0t5
 int EndingSetPoint = 5;
int TimeScreenLeft = 0;

double TempYMax = 800;
double TempSplitHigh = 460;
double TempSplitLow = 390;

double PixelYSplit2;
double PixelYSplit;
long   PixelsPerMin;
double TempPerPixL = 0;
double TempPerPixM = 0;
double TempPerPixH = 0;

double CurrentSetPointTemp = 0;

//int BeforeTemp = 0;
//int BeforeTime = 0;

int badLastTempCount = 0;
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
Chrono PIDIntegralUdateTime(Chrono::MILLIS);

//temps are read once per second
Average<double> TBeanAvgRoll(3);
Average<double> TCoilRoll(3);  //this is minute avg

int OVERHEATFANCount;
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

char spFormat[5] = "%6.2F";
//used when drawing lines. We support up to 3 lines (see line ID constants)
int LastXforLineID[GRAPHLINECOUNT];
int LastYforLineID[GRAPHLINECOUNT];
uint16_t LineColorforLineID[GRAPHLINECOUNT];

int moveamount = -1;

double RoastMinutes = 0;

int FanDeviation = 0;

boolean bNewTempsAvailable =false;
int ReadTempFlag = -1;
int TCoil;
int TBean1;
int TBean2;

double MaxVoltage, MaxVread;


int TBeanAvg;


boolean HasDisplay = true ;
// =====================================================================================================================================================================
// SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP
// =====================================================================================================================================================================
void setup() {
  //Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("setup starting");
  Serial2.begin(9600);
  Serial2.println("setup starting");


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

  intializeMenus();

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
  FanSpeedPWM = FanSpeedPWMStart;
  FanSpeedPWMAutoDecrease = EEPROM.read(FanSpeedPWMAutoDecrease_EP);
  FanSpeedPWMAutoDecreaseStart = FanSpeedPWMAutoDecrease;

  Serial.println(F("wire i2c for fan begin"));
   
  
  Wire.begin();  
  
  StopAndSendFanPWM();
  
  graphProfile();
  Serial.println("loop is starting...");
  
}


void loop() {
  theloop();
}

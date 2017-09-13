//#include <arduino2.h>

#include <EEPROM.h>

#include "libraries\Time-master\Time.h"
#include "libraries\Time-master\TimeLib.h"
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include "libraries\MAX6675\max6675.h"
#include <Adafruit_GFX.h>
#include <gfxfont.h>
//#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include "libraries\Average.h"
#include "Chrono.h"

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
//#define TS_MINX 150
//#define TS_MINY 120
//#def   ine TS_MAXX 920
//#define TS_MAXY 940

#define MINTOUCHPRESSURE 10
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
#

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
#define BAUD            115200
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
MAX6675 thermocouple1(TSCKp, TCS1p,  TSD1p);
MAX6675 thermocouple2(TSCKp, TCS2p,  TSD2p);
MAX6675 thermocouple3(TSCKp, TCS3p,  TSD3p);
MAX6675 thermocouple4(TSCKp, TCS4p,  TSD4p);
//global variablers for temp control
int Gain = 10; //read from eeprom
int GainX = 0;
int GainY = 0;
int GainUX = 0;
int GainUY = 0;
int GainDX = 0;
int GainDY = 0;
double Integral = 0.1;  //read from eeprom
int IntegralX = 0;
int IntegralY = 0;
int IntegralUX = 0;
int IntegralUY = 0;
int IntegralDX = 0;
int IntegralDY = 0;

long unsigned IntegralLastTime = 0;
long unsigned IntegralSum = 0;
unsigned long PIDWindowStartTime;
unsigned long SSR2_OffTime;
unsigned long SSR1_OnTime;
boolean PIDNewWindow;
int ErrI = 0;

int MySetpointsX[6];
int MySetpointsY[6];
int MySpanAccumulatedMinutes[6];
int MyMinuteSetpoints[20];
int MySetpointsEprom[]    = {3, 4, 5, 6, 7, 8};  //these are EEprom memory locations - not data
int MySpanMinutesEprom[]  = {10, 11, 12, 13, 14, 15};  //these are EEprom memory locations - not data
int MySpanMinutes[] =      {  0,   4,   2,   7,    1,   1}; //actual values read from eeprom
int MyBaseSetpoints[] =     {120, 390, 430, 450,  470, 500}; //actual values read from eeprom
unsigned long MyProfileTimeStamp;
int SetPointCount = 6;
int TempEnd = 450; //actual value is set in program
int TimeScreenLeft = 14;
int TempLastEndOfRoast;
double TimeLastEndOfRoast;
int AdjustmentDisplayTop = 140;
int AdjustmentSetpoint = 0;



unsigned long TempScreenTop = 455;
long IYscale;

int LoopsPerSecond;

Chrono RoastTime(Chrono::SECONDS);
Chrono SecondTimer(Chrono::MILLIS);
//int SecondTimerValue = 1000;
Chrono LcdUdateTime(Chrono::MILLIS);
Chrono PIDIntegralUdateTime(Chrono::MILLIS);


int CapButActive = 0;

Average<float> AvgFanCurrent(10);
Average<float> AvgCoil1Amp(60);
Average<float> AvgCoil2Amp(60);
Average<int> TBeanAvgRoll(4);
int OVERHEATFANCount;
int OVERHEATCOILCount;
int TempReachedCount;
int TempSpikeCount;
int Readingskipped;

long PixelsPerMin;

int DUMMY;

//used when drawing lines. We support up to 3 lines (see line ID constants)
uint16_t LastXforLineID[3];
uint16_t LastYforLineID[3];
uint16_t LineColorforLineID[3];
int myLastGraph[320];


int TCoil;
int TBean1;
int TBean2;
int TFan;
double MaxVoltage, MaxVread, CurrentFan, CurrentHeat1, CurrentHeat2;
int TBeanAvg;

char Buf5[5];

double Duty ;
double Setpoint ;
int PIDIntegralUdateTimeValue ;
int PIDWindowSize ;
int ManagingSSR ;

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
  //define CURFANap    7///#define CURHEAT2ap  6


  //set relays to high - cause that means off
  digitalWrite(FANRELAYp, RELAYOFF); digitalWrite(VIBRELAYp, RELAYOFF);
  Serial.begin(BAUD);
  delay(1000);
  MaxVread = 512; //this is the expected half way value
  RoastTime.stop();
  Gain =      EEPROM.read(GAIN_EP);
  Integral =  (double)EEPROM.read(INTEGRAL_EP) / 10;
  if (Integral > 1) Integral = 0.1 ;
  Serial.print("read Gain:"); Serial.print(Gain); Serial.print(" Integral:"); Serial.println(Integral);

  EEPROM.get(PROFILETIMESTAMP_EP , MyProfileTimeStamp);
  Serial.print("Read profile time stamp:"); Serial.println(MyProfileTimeStamp);
  if (MyProfileTimeStamp == 0)   {
    MyProfileTimeStamp = millis();
    EEPROM.put(PROFILETIMESTAMP_EP , MyProfileTimeStamp);
    Serial.print("Saving time stamp:"); Serial.println(MyProfileTimeStamp);
  }
  TempLastEndOfRoast = ReadTempEprom (TEMPLASTENDOFROAST_EP, 0);
  for (int X = 1; X < SetPointCount; X++) {
    MySpanMinutes[X]  =  ReadIntEprom(MySpanMinutesEprom[X], X , 12, MySpanMinutes[X]);
    MyBaseSetpoints[X] = ReadTempEprom(MySetpointsEprom[X] , MyBaseSetpoints[X]);
  }
  Serial.println (LastXforLineID[2]);
  PixelsPerMin =  (int)(320 / TimeScreenLeft);
  Serial.println (LastXforLineID[2]);
  TempScreenTop = ReadIntEprom(TEMPSCREENTOP_EP, 100, 500, 460);
  TempScreenTop = 450;
  Serial.print("  tempscreentop:"); Serial.println(TempScreenTop);

  tft.reset();
  uint16_t identifier = tft.readID();
  if (identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if (identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if (identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if (identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    return;
  }

  SecondTimer.restart(0);

  tft.begin(identifier);

  graphProfile();

  ManagingSSR = 1;

  State = AMSTOPPED;
}

// **************************************************************************************************************************************************************
// LOOP A   LOOP A   LOOP A   LOOP  A  LOOP A   LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP
// **************************************************************************************************************************************************************
void loop () {

  //per loop variables
  int newState = 0;
  int minuteToStart = 0;
  boolean bNewSecond = false;
  double roastMinutes = ((double)RoastTime.elapsed()) / 60;
  //Serial.println(roastMinutes);
  if (SecondTimer.elapsed() > 1000) {
    bNewSecond = true;// Serial.print("LoopsPerSec:");Serial.println(LoopsPerSecond);
    LoopsPerSecond = 0;
    SecondTimer.restart(0);
  }
  else {
    LoopsPerSecond ++;
  }
  TSPoint p = ts.getPoint();
  if (p.z > MINTOUCHPRESSURE && p.z < MAXTOUCHPRESSURE)
  {
    //Serial.print("touch x:");Serial.print (p.x);Serial.println(" y:");Serial.println(p.y);
    int x = 0; int y = 0;
    //        A        B
    x = map(p.y, 80, 880, 0, tft.width());
    //lower A to increase report X pixes at left side of screen 3 ~ 3px
    //lower D to increase report X pixel at right side of scrreen
    //        C       D
    y = map(p.x, 140, 890, 0, tft.height());
    //lower D to increase Reporter Y Pixels at top of screen 5 ~ 1px
    //lower C to increase reportd Y pixels at bottom of screen 2 ~ 1px
    //Serial.print ("x:");Serial.print (x);Serial.print (" y:");Serial.println (y);
    ProcessTouch(x, y);
  }

  //**********************************************************************************************************************************
  //read temperatures and amps    B         read temperatures and amps   B             read temperatures and amps     B         read temperatures and amps
  //********************************************************************************************************************************
  float tempraw;
 
//we read current all the time since
 int tempread;
    
  MaxVread = analogRead(VOLT5ap);
  MaxVoltage = (((double)MaxVread) / 1024) * 5;
  // the center of the max voltage is 0
  tempread = analogRead(CURFANap);
  //Serial.print("volt:");Serial.print(maxVread);Serial.print("fan:");Serial.println(tempread);
  //185 millamps per volt
  CurrentFan = (((double)(tempread - (MaxVread / 2) ) * 5)) / 125;
  if (CurrentFan < 0) {
    CurrentFan = 0;
  }
  AvgFanCurrent.push(CurrentFan); CurrentFan = AvgFanCurrent.mean();
  CurrentHeat1 = (((double)(analogRead(CURHEAT1ap) - (MaxVread / 2)) * 5)) / 100; AvgCoil1Amp.push(CurrentHeat1 * CurrentHeat1);
  CurrentHeat2 = (((double)(analogRead(CURHEAT2ap) - (MaxVread / 2)) * 5)) / 100; AvgCoil2Amp.push(CurrentHeat2 * CurrentHeat2);
  CurrentHeat1 = sqrt( AvgCoil1Amp.mean());
  CurrentHeat2 = sqrt( AvgCoil2Amp.mean());

 
  if (bNewSecond) { //we speed up by reading temps once per second.  they are very slow.
    TCoil =  getCleanTemp(thermocouple1.readFahrenheit(), 1);
    //Serial.print("Coil teamp:");Serial.println(TCoil);
    TFan =   getCleanTemp(thermocouple4.readFahrenheit(), 4);
    TBean1 = getCleanTemp(thermocouple2.readFahrenheit(), 2);
    TBean2 = getCleanTemp(thermocouple3.readFahrenheit(), 3);
    TBeanAvg = getBeanAvgTemp(TBean1, TBean2);

    //Serial.print(TBean1);Serial.print (TBean2); Serial.print(TCoil); Serial.println(TFan);
    double newtempratiotoaverage;
    if (TBeanAvgRoll.getCount() > 1) {
      newtempratiotoaverage = TBeanAvg / TBeanAvgRoll.mean();
    }
    else {
      newtempratiotoaverage = 1;
      Readingskipped++;
    }

    if ( newtempratiotoaverage > .7 &&  newtempratiotoaverage < 1.2) {
      TBeanAvgRoll.push(TBeanAvg);
    }
    else {
      Readingskipped++;
      Serial.println("out of range:");
      Serial.print("TBean2:"); Serial.print(TBean2); Serial.print(" TBean1:"); Serial.print(TBean1);
      Serial.print(" avg:"); Serial.print(TBeanAvgRoll.mean()); Serial.print(" TCoil:");
      Serial.print(TCoil); Serial.print(" TFan:"); Serial.println(TFan);

    }
  }
 

  //*******************************************************************************************************************************************************
  //Input buttons      C          Input buttons        C        Input buttons       C         Input buttons      C          Input buttons
  //*******************************************************************************************************************************************************
  //look for pressed buttons being released
  if (CapButActive > 0 ) {
    if (digitalRead(CapButActive) == LOW) {
      digitalWrite(Buzzerp, HIGH); digitalWrite(LEDp, LOW);
      Serial.print ("Cap Button released:" ); Serial.println (CapButActive);
      CapButActive = 0;
    }
  }
  //find newly pressed  button
  //int capButGoneHigh = 0;

  boolean capbuttonpressed = false;
  if (CapButActive == 0 ) {
    if (digitalRead(CP1p) == HIGH) {
      CapButActive = CP1p;
    }
    else if ( digitalRead(CP2p) == HIGH) {
      CapButActive = CP2p;
    }
    else if ( digitalRead(CP3p) == HIGH) {
      CapButActive = CP3p;
    }
    else if ( digitalRead(CP4p) == HIGH) {
      CapButActive = CP4p;
    }

    if (CapButActive > 0) {
      capbuttonpressed = true;
      digitalWrite(Buzzerp, LOW); digitalWrite(LEDp, HIGH);
      Serial.print ("Cap Button pressed:" ); Serial.println (CapButActive);
    }
  }


  //**************************************************************************************************
  //DETERIM NEW STATE   D    DETERIM NEW STATE   D    DETERIM NEW STATE  D     DETERIM NEW STATE  D     DETERIM NEW STATE
  //*****************************************************************************************************


  //look at button inputs
  if (capbuttonpressed) {
    Serial.print ("Doing button work:" ); Serial.println (CapButActive);
    switch (CapButActive) {
      case CP1p:
        if (State == AMSTOPPED || State == AMFANONLY  ) {
          newState = AMROASTING;
          Serial.println("Start Detected!");
        }
        else  if (State == AMOVERHEATEDCOIL || State == AMOVERHEATEDFAN || State == AMAUTOCOOLING ) {
          newState = AMROASTING;
          Serial.println("ReStart Detected!");
        }
        else {
          newState = AMSTOPPED;
        }
        break;
      case CP2p:
        break;
      case CP3p:
        if ( State == AMSTOPPED) {
          Serial.println("Fan On Detected!");
          newState = AMFANONLY;
        }
        else if (State == AMFANONLY) {
          Serial.println("Fan Off Detected!");
          newState = AMSTOPPED;
        }
        else {
          Serial.println("Fan on off detected by state not right to turn on or off");
        }
        break;
      case CP4p:
        if (State == AMOVERHEATEDCOIL || State == AMOVERHEATEDFAN) {
          Serial.println("Manually Resetting overheat");
          newState = AMROASTING;
        }
        graphProfile();
        Serial.println("cp4");
        break;
      default:
        break;
    }
  }

  //look at temp inputs
  if (State == AMROASTING && TCoil > OVERHEATCOIL) {
    OVERHEATCOILCount++;
    Serial.print("Overheat coil Detected! count:"); Serial.println(OVERHEATCOILCount);
    if (OVERHEATCOILCount > 20) {
      newState = AMOVERHEATEDCOIL;
    }
  }
  else if (State == AMROASTING && TFan > OVERHEATFAN) {
    OVERHEATFANCount ++;
    Serial.print("Overheat fan Detected! count:"); Serial.println(OVERHEATFANCount);
    if (OVERHEATFANCount > 20) {
      newState = AMOVERHEATEDFAN;
    }
  }
  else {
    OVERHEATFANCount = 0;  //reset counts whenever we see neither overheat condition
    OVERHEATCOILCount = 0;
    if (State == AMROASTING && TBeanAvgRoll.mean() > TempEnd ) {
      TempReachedCount ++;
      if (TempReachedCount > 20) {
        newState = AMAUTOCOOLING;
        Serial.print("Roast Temp Reached. Cooling starting End:"); Serial.print(TempEnd); Serial.print("  Tempavg"); Serial.println(TBeanAvgRoll.mean());
      }
    }
    else if (State == AMAUTOCOOLING && TBeanAvgRoll.mean() < TEMPCOOLINGDONE ) {
      newState = AMSTOPPED;
      Serial.println("Auto Cooling Complete ");
    }
    else if (State == AMROASTING && roastMinutes > TimeScreenLeft ) {
      newState = AMAUTOCOOLING;
      Serial.println("Max time reached. Cooling starting");
    }
    else {
      TempReachedCount = 0;
    }
  }


  // **************************************************************************************************************************************************************
  //APPLY NEW STATE     E      APPLY NEW STATE      E     APPLY NEW STATE   E        APPLY NEW STATE      E     APPLY NEW STATE   E        APPLY NEW STATE           APPLY NEW STATE
  // **************************************************************************************************************************************************************
  if (newState == AMSTOPPED) {
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    if (TBeanAvgRoll.mean() < TEMPCOOLINGDONE  && TFan < TEMPCOOLINGDONE ) {
      State = AMSTOPPED;
      digitalWrite(FANRELAYp, RELAYOFF); digitalWrite(VIBRELAYp, RELAYOFF);
      RoastTime.stop();
    }
    else {
      newState = AMAUTOCOOLING;
      State = AMAUTOCOOLING;
    }
  }
  else if (newState == AMROASTING) {
    digitalWrite(FANRELAYp, RELAYON); digitalWrite(VIBRELAYp, RELAYON);
    if (State == AMSTOPPED || State == AMFANONLY) {

      Serial.println("Starting Fans and Vibration - and waiting 5 seconds");
      delay(2000);
      Readingskipped = 0;
      StartLinebyTimeAndTemp(0, 0, AVGLINEID , BLUE);
      StartLinebyTimeAndTemp(0, 0, ROLLAVGLINEID , YELLOW);
      graphProfile();
      delay(2000);
      Serial.println("2 Starting Heaters ");
      RoastTime.restart(minuteToStart * 60);
      roastMinutes = ((double)RoastTime.elapsed()) / 60;
    }
    State = AMROASTING;
  }
  else if (newState == AMAUTOCOOLING) {
    State = AMAUTOCOOLING;
    TempLastEndOfRoast = TBeanAvgRoll.mean();
    TimeLastEndOfRoast = roastMinutes;
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    LoadORSaveToHistory(false);
    delay(1000);
  }
  else if (newState == AMFANONLY) {
    State = AMFANONLY;
    digitalWrite(FANRELAYp, RELAYON); digitalWrite(VIBRELAYp, RELAYON);
  }
  else if (newState == AMOVERHEATEDCOIL) {
    State = AMOVERHEATEDCOIL;
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    delay(1000);
  }
  else if (State == AMOVERHEATEDFAN) {
    State = AMOVERHEATEDFAN;
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
  }


  //***************************************************************************************************************************************************************
  // CALCULATING PID VALUES     F    CALCULATING PID VALUES     F    CALCULATING PID VALUES    F     CALCULATING PID VALUES    F     CALCULATING PID VALUES
  //*************************************************************************************************************************************************************
  int err = 0;
  //pid window size should vary based on duty cycle. 10 millsec and 50% would to 20 millisecond.  10% would be 100 milliseconds.  01 % would be 1 seconds
  //but we run 9 times per second, so shortest on off is 1 cycles or ~ 100 millseconds.   50% means .2 seconds, 10% means 1 seconds  .05% means 2 seconds
  Duty = 0;
  Setpoint = 0;

  if (State == AMROASTING) {
    //CALC THE ERR AND INTEGRAL
    Setpoint =  calcSetpoint(roastMinutes);
    err = Setpoint - TBeanAvgRoll.mean();  //negative if temp is over setpoint
    PIDIntegralUdateTimeValue = 3000;
    if (roastMinutes > MySpanAccumulatedMinutes[1] ) { //only calc intergral error if we are above the 1st setup
      if (PIDIntegralUdateTime.elapsed() > PIDIntegralUdateTimeValue) { //every 3 seconds we add the err to be a sum of err
        IntegralSum =  IntegralSum + double(err);
        ErrI = (IntegralSum * Integral) ; //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
        //Serial.print("Isum:");Serial.print(IntegralSum);Serial.print("ErrI:");    Serial.println(ErrI);
        PIDIntegralUdateTime.restart(0);
      }
    }
    else { //clear out the integral corrected after each set point.  Not sure we need this but it seem to keep integral for being overdominant.
      PIDIntegralUdateTime.restart(0);
      IntegralSum = 0;
      IntegralLastTime = 0;
      ErrI = 0;
    }
    Duty =  ((double)(err + ErrI) / (double)Gain)    ; //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty decrease during pid window - we kill heater.  If duty increases during window we may miss the turn on.
    //Serial.print("sp:");Serial.print(setpoint);Serial.print(" err:"), Serial.print(err);Serial.print("duty:");Serial.print(duty);Serial.print (" Gain:");Serial.println (Gain);
    //100% means never set low.  0% means set low right away

    //APPLY THE ERROR WITH THE PID WINDOW
    PIDWindowSize = 1000;
    unsigned long now = millis();
    boolean ExceedsWholePidWindow = (PIDWindowStartTime == 0) || (now - PIDWindowStartTime > PIDWindowSize);
    if (ExceedsWholePidWindow) { //keep checking if we need to start a new PID window
      PIDWindowStartTime = now;
      PIDNewWindow = true;
      Serial.println("new pid window");
    }
    boolean AmInOnPortionOfWindow = (PIDNewWindow == true && Duty > 0) ||  ((now - PIDWindowStartTime) <=  (Duty * PIDWindowSize));
    if (AmInOnPortionOfWindow  ) {
      //note when it was turned on.  If it was just turned on, it is our management relay
      if (SSR1_OnTime == 0) {
        //if pid 1 is not on, turn it on
        SSR1_OnTime = now;
        ManagingSSR = 1;

      }
      //SSR1 should always be on
      digitalWrite(SSR1p, HIGH);

      //if ssr has been on for more than 5 seconds, kick in SSR2.  It is now our managing relay
      if ((now - SSR1_OnTime) > 5000 ) {
        //if where a still below setpoint after 5 seconds of contious ssr1 on, turn on ssr2, if not allready on
        //Serial.println ("ssd 2 is on for 5 seconds");
        digitalWrite(SSR2p, HIGH);
        ManagingSSR = 2;
        SSR2_OffTime = 0;
      }

    }
    else { //we are in the OffPortion of the Window
      if (ManagingSSR == 2) {
        //if pid 2 is managing relay - turn it off and begin tracking how long it has been off
        SSR2_OffTime = now;
        digitalWrite(SSR2p, LOW);

      }
      // if r2 has been off more than 5 seconds OR we in manage turn off relay 1 and swtich to managing relay 1
      if ((now - SSR2_OffTime) > 5000 || ManagingSSR == 1  ) {

        digitalWrite(SSR1p, LOW);
        SSR1_OnTime = 0;
        ManagingSSR == 1;
      }
    }
    //cancel new window if it had been true
    if (PIDNewWindow == true) {
      PIDNewWindow = false;
    }
  }
  else {
    //Serial.println("not roastine is off");
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    SSR1_OnTime = 0; SSR2_OffTime = 0;
  }

  //********************************************************************************************************************************
  //LCD AND GRAPH DISPLAY    G     LCD AND GRAPH DISPLAY   G      LCD AND GRAPH DISPLAY    G     LCD AND GRAPH DISPLAY    G     LCD AND GRAPH DISPLAY
  //*******************************************************************************************************************************
  //its slow to update th

  if (newState > 0) {
    Serial.println("newstate detected  Will update tft immediately");
    displayState(State);  newState = 0;
  }
  //we update the area when we get new temps
  if (bNewSecond) {
    Serial.println("update after reach new temp");
    UpdateGraphA(roastMinutes, Duty, Setpoint, err, ErrI);
    AddLinebyTimeAndTemp(roastMinutes, TBeanAvg, AVGLINEID);
    AddLinebyTimeAndTemp(roastMinutes, TBeanAvgRoll.mean(), ROLLAVGLINEID);
  }
  if (LcdUdateTime.elapsed() > 3000) {
    //Serial.print("slow update. once per:");Serial.println(3000);Serial.println(" millseconds");
    // void UpdateGraphB(int temp1, int temp2, int tempCoil, double ampHeater1, double ampHeater2, int tempFan, double ampFan, double volts)
    //   Serial.print("TBean2:");Serial.print(TBean2);Serial.print("TBean1:");Serial.print (TBean1);Serial.print("TCoil:"); Serial.print(TCoil); Serial.print("TFan:");Serial.println(TFan);
    UpdateGraphB(TBean2, TBean1, TCoil, CurrentHeat1, CurrentHeat2, TFan, CurrentFan, MaxVoltage);
    LcdUdateTime.restart(0);
  }

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
//      END LOOP  H      END LOOP      H    END LOOP    H      END LOOP    H      END LOOP   H       END LOOP     H     END LOOP    H      END LOOP
//----------------------------------------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------
// Get the temperature
int getCleanTemp(double temperature, int myID) {
  if (isnan(temperature)) {
    Readingskipped++;
    Serial.print (myID); Serial.print ("nan temp:"); Serial.println(temperature);
    return -1;
  }
  else if (temperature > 1000) {
    Readingskipped++;
    Serial.print (myID); Serial.print ("too high temp:"); Serial.println(temperature);
    return -1;
  }
  else if (temperature < 40) {
    Readingskipped++;
    Serial.print (myID); Serial.print ("too low temp:"); Serial.println(temperature);
    return -1;
  }
  else {
    int r = temperature;
    //  Serial.print (myID);Serial.print ("clean temp returned:");Serial.println(r);
    return r;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ReadIntEprom(int loc, int min, int max, int def) {
  int t;
  t = EEPROM.read(loc);
  if (t >= 0 && t < 100) {
    return t ;
  }  else {
    return def ;
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ReadTempEprom(int loc, int def) {
  int t;
  t = EEPROM.read(loc) + 254;
  if (t > 254 && t < 495) {
    return ( t);
  }  else {
    return def ;
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SaveTempEprom(int loc, int temp) {
  EEPROM.update(loc, (temp - 254));
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double getBeanAvgTemp(double t1, double t2) {
  if (t1 != -1 && t2 != -1) {
    return (t1 + t2) / 2;
  }
  else {
    if     (t1 != -1) {
      return t1;
    }
    else {
      return t2;
    }
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double calcSetpoint(double roastminutes) {
  int setpoint;
  double r = (roastminutes - (int)roastminutes);
  setpoint = MyMinuteSetpoints[(int)roastminutes] + ((MyMinuteSetpoints[int(roastminutes) + 1] - MyMinuteSetpoints[(int)roastminutes]) * r);
  return setpoint;
}

//-------------------------------------------------------------------------------------------------------------------------------
//GRAPHING             GRAPHING             GRAPHING             GRAPHING             GRAPHING             GRAPHING
//-------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void graphProfile() {
  tft.setRotation(3);
  tft.fillScreen(BLACK);


  IYscale = ((TempScreenTop) * (TempScreenTop) * (TempScreenTop)) / 240;
  //IYscale = IYscale * 1000;
  //Serial.print("IYscale:");Serial.print(TempScreenTop);Serial.print("  ");Serial.println(IYscale);
  //draw grid
  int color = tft.color565(125, 125, 125);
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.setCursor(1 * PixelsPerMin, 230); tft.println("1");
  tft.drawFastVLine(3 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor(3 * PixelsPerMin, 230); tft.println("3");
  tft.drawFastVLine(6 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor(6 * PixelsPerMin, 230); tft.println("6");
  tft.drawFastVLine(9 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor(9 * PixelsPerMin, 230); tft.println("9");
  tft.drawFastVLine(12 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor(12 * PixelsPerMin, 230); tft.println("12");
  tft.drawFastVLine(12 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor(15 * PixelsPerMin, 230); tft.println("15");


  tft.setCursor(0 , Y(200)); tft.println("200");
  tft.drawFastHLine(0,  Y(300), tft.width(), color);
  tft.setCursor(0 , Y(300)); tft.println("300");
  tft.drawFastHLine(0,  Y(375), tft.width(), color);
  tft.setCursor(0 , Y(375)); tft.println("375");
  tft.drawFastHLine(0,  Y(400), tft.width(), color);
  tft.setCursor(0 , Y(400)); tft.println("400");
  tft.drawFastHLine(0,  Y(425), tft.width(), color);
  tft.setCursor(0 , Y(425)); tft.println("425");
  tft.drawFastHLine(0,  Y(440), tft.width(), color);
  tft.setCursor(0 , Y(440)); tft.println("440");
  tft.drawFastHLine(0,  Y(460), tft.width(), color);
  tft.setCursor(0 , Y(460)); tft.println("460");
  tft.drawFastHLine(0,  Y(475), tft.width(), color);
  tft.setCursor(0 , Y(475)); tft.println("475");
  tft.setCursor(0 , Y(490)); tft.println("490");

  //draw steamp
  tft.setCursor(200, 0); tft.print("TS:");
  tft.setCursor(220, 0); tft.print(day(MyProfileTimeStamp)); tft.print("/"); tft.print(month(MyProfileTimeStamp)); tft.print(" ");
  tft.print(hour(MyProfileTimeStamp)); tft.print(":"); tft.println(minute(MyProfileTimeStamp));



  //the first setpoint is the left y axis
  //the last setpoint is the right y axis
  //so we have setpoint - 1 spans on the graph

  StartLinebyTimeAndTemp (0, MyBaseSetpoints[0], SETPOINTLINEID , WHITE);
  MySpanAccumulatedMinutes[0] = 0;


  for (int X = 1; X < SetPointCount; X++) {
    MySpanAccumulatedMinutes[X] = MySpanAccumulatedMinutes[X - 1] + MySpanMinutes[X];

    for (int Y = MySpanAccumulatedMinutes[X - 1] ; Y < MySpanAccumulatedMinutes[X]; Y++) {
      MyMinuteSetpoints[Y] =  MyBaseSetpoints[X - 1] + ((MyBaseSetpoints[X] - MyBaseSetpoints[X - 1]) * (double)(Y - MySpanAccumulatedMinutes[X - 1]) / (double)MySpanMinutes[X]);
      //Serial.print("minutesp:");Serial.print(Y);Serial.print("  ");Serial.println(MyMinuteSetpoints[Y]);
      //Serial.print (MyBaseSetpoints[X]);Serial.print (" - ");Serial.print (MyBaseSetpoints[X-1]);Serial.print (" - ");Serial.print ( Y );Serial.print (" - ");Serial.println ( MyMinuteSetpoints[Y]);
    }
    MySetpointsX[X] = MySpanAccumulatedMinutes[X] * PixelsPerMin;
    MySetpointsY[X] = 240 - Y(MyBaseSetpoints[X]);

    AddLinebyTimeAndTemp(MySpanAccumulatedMinutes[X], MyBaseSetpoints[      X], SETPOINTLINEID);

    tft.drawFastVLine(MySpanAccumulatedMinutes[X] * PixelsPerMin, Y(MyBaseSetpoints[X] + 10), 20, WHITE);
  }

  //create setpoint array for last segment
  for (int Y = MySpanAccumulatedMinutes[SetPointCount - 1] ; Y <=     TimeScreenLeft; Y++) {
    MyMinuteSetpoints[Y] = MyBaseSetpoints[SetPointCount - 1] ;
    //Serial.print("minutesp:");Serial.print(Y);Serial.print("  ");Serial.println(MyMinuteSetpoints[Y]);
  }


  TempEnd = MyBaseSetpoints[SetPointCount - 2];
  //Serial.print ("TempEnd:");Serial.println (TempEnd);
  tft.setCursor(tft.width() / 2, Y(TempEnd) - 10);  tft.println(TempEnd);
  tft.drawFastHLine(((tft.width() / 2) + 20)    ,  Y(TempEnd), tft.width(), BLUE);
  tft.drawFastVLine(PixelsPerMin * MySpanAccumulatedMinutes[SetPointCount - 2], 0, tft.height() / 2, BLUE);


  //tft.fillRect(0,0,5,5, BLUE);
  //tft.fillRect(0,235,5,5, RED);
  //tft.fillRect(315,0,5,5,YELLOW);
  //tft.fillRect(315,235,5,5,WHITE);

  tft.drawFastVLine(PixelsPerMin * MySpanAccumulatedMinutes[SetPointCount - 2], 0, tft.height() / 2, BLUE);
  //delay(1000);

  ReDrawROLLAVGLINEFromArray(ORANGE);
  if (AdjustmentSetpoint > 0) DrawAdjustMentBoxes(AdjustmentSetpoint);
}


void UpdateAdjustMentText()
{
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.setCursor(225, AdjustmentDisplayTop + 25);  tft.println("New:"); tft.setCursor(260, AdjustmentDisplayTop + 25) ;   tft.println( MyBaseSetpoints[AdjustmentSetpoint]);
  tft.setCursor(225, AdjustmentDisplayTop + 55 ); tft.println("New:"); tft.setCursor(260, AdjustmentDisplayTop + 55);  tft.println( MySpanAccumulatedMinutes[AdjustmentSetpoint]);
}
void HideAdjustMentBoxes()
{
  AdjustmentSetpoint = 0;
  tft.fillRect(150, AdjustmentDisplayTop - 10, 150, 80, BLACK);
}

void DrawAdjustMentBoxes(int setpoint)
{
  // make the color selection boxes
  AdjustmentSetpoint = setpoint;

  tft.fillRect(150, AdjustmentDisplayTop - 10, 150, 80, BLACK);
  tft.drawRect(150, AdjustmentDisplayTop - 10, 150, 80, BLUE);

  tft.fillRect(180, AdjustmentDisplayTop      , 20,  20, GREEN); //TOP
  tft.fillRect(200, AdjustmentDisplayTop + 20 , 20, 20, GREEN); //RIGHT
  tft.fillRect(180, AdjustmentDisplayTop + 40 , 20, 20, GREEN); //BOTTOM
  tft.fillRect(160, AdjustmentDisplayTop + 20 , 20, 20, GREEN); //LEFT

  tft.fillRect(280, AdjustmentDisplayTop - 10 , 20, 20,  BLUE); //LEFT
  tft.drawLine(280, AdjustmentDisplayTop + 10 , 300 ,  AdjustmentDisplayTop - 10, BLACK);
  tft.drawLine(280, AdjustmentDisplayTop - 10 , 300 ,  AdjustmentDisplayTop + 10, BLACK);

  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(2);
  tft.setCursor(184, AdjustmentDisplayTop + 23 ); tft.println(setpoint);

  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.setCursor(225, AdjustmentDisplayTop + 15);  tft.println("Temp:"); tft.setCursor(260, AdjustmentDisplayTop + 15) ;   tft.println( MyBaseSetpoints[AdjustmentSetpoint]);
  tft.setCursor(225, AdjustmentDisplayTop + 40 ); tft.println("Mins:"); tft.setCursor(260, AdjustmentDisplayTop + 40);  tft.println( MySpanAccumulatedMinutes[AdjustmentSetpoint]);

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ProcessTouch(int Xtouch, int Ytouch)
{

  Serial.print ("x:"); Serial.print (Xtouch); Serial.print (" y:"); Serial.println (Ytouch);
  for (int X = 1; X < SetPointCount; X++) {
    if (abs(Xtouch - MySetpointsX[X]) < 20 && abs(Ytouch - MySetpointsY[X]) < 20 ) {
      DrawAdjustMentBoxes(X);
      return;
    }
  }

  if (abs(Xtouch - GainUX) < 5 && abs(Ytouch - GainUY) < 5 ) {
    Gain = Gain + 1;
    tft.setTextSize(1);
    tft.setCursor(GainX , GainY); tft.println(Gain);
    Serial.print("Gain U"); Serial.println(Gain);
    EEPROM.update(GAIN_EP, Gain);
    return;
  }
  if (abs(Xtouch - GainDX) < 5 && abs(Ytouch - GainDY) < 5 ) {
    Gain = Gain - 1;
    tft.setTextSize(1);
    tft.setCursor(GainX , GainY); tft.println(Gain);
    Serial.print("Gain D"); Serial.println(Gain);
    EEPROM.update(GAIN_EP, Gain);
    return;
  }
  if (abs(Xtouch - IntegralUX) < 5 && abs(Ytouch - IntegralUY) < 5 ) {
    Integral = Integral + 0.1;
    tft.setTextSize(1);
    tft.setCursor(IntegralX , IntegralY); tft.println(Integral);
    Serial.print("Integral U"); Serial.println(Integral);
    EEPROM.update(INTEGRAL_EP , (int)(Integral * 10));
    return;
  }
  if (abs(Xtouch - IntegralDX) < 5 && abs(Ytouch - IntegralDY) < 5 ) {
    Integral = Integral - 0.1;
    if (Integral < 0) Integral = 0;
    tft.setTextSize(1);
    tft.setCursor(IntegralX , IntegralY); tft.println(Integral);
    Serial.print("Integral D"); Serial.println(Integral);
    EEPROM.update(INTEGRAL_EP , (int)(Integral * 10));
    return;
  }

  if (AdjustmentSetpoint > 0) {
    int Ytouch2 = 240 - Ytouch;
    int beforeTemp = MyBaseSetpoints[AdjustmentSetpoint];
    int beforeTime = MySpanAccumulatedMinutes[AdjustmentSetpoint];
    if (Xtouch > 180 && Xtouch < 200 && Ytouch2 > AdjustmentDisplayTop && Ytouch2 < AdjustmentDisplayTop + 20) {
      MyBaseSetpoints[AdjustmentSetpoint] = MyBaseSetpoints[AdjustmentSetpoint] + 1;
      //Serial.println ("up");
      SaveTempEprom(MySetpointsEprom[AdjustmentSetpoint] , MyBaseSetpoints[AdjustmentSetpoint]);
    }
    if (Xtouch > 200 && Xtouch < 220 && Ytouch2 > AdjustmentDisplayTop + 20 && Ytouch2 < AdjustmentDisplayTop + 40) {
      if (AdjustmentSetpoint > 0) {
        MySpanMinutes[AdjustmentSetpoint] = MySpanMinutes[AdjustmentSetpoint] + 1;
        MySpanMinutes[AdjustmentSetpoint + 1] = MySpanMinutes[AdjustmentSetpoint + 1] - 1;
        MySpanAccumulatedMinutes[AdjustmentSetpoint] =  MySpanAccumulatedMinutes[AdjustmentSetpoint] + 1;
        EEPROM.update( MySpanMinutesEprom[AdjustmentSetpoint] ,  MySpanMinutes[AdjustmentSetpoint] );
        EEPROM.update(MySpanMinutesEprom[AdjustmentSetpoint + 1] , MySpanMinutes[AdjustmentSetpoint + 1]);
        //Serial.println ("right");
      }
    }
    if (Xtouch > 180 && Xtouch < 200 && Ytouch2 > AdjustmentDisplayTop + 40 && Ytouch2 < AdjustmentDisplayTop + 60) {
      MyBaseSetpoints[AdjustmentSetpoint] = MyBaseSetpoints[AdjustmentSetpoint] - 1;
      SaveTempEprom(MySetpointsEprom[AdjustmentSetpoint] , MyBaseSetpoints[AdjustmentSetpoint]);
      Serial.println ("botton");
    }
    if (Xtouch > 160 && Xtouch < 180 && Ytouch2 > AdjustmentDisplayTop + 20 && Ytouch2 < AdjustmentDisplayTop + 40) {
      if (AdjustmentSetpoint > 0) {

        MySpanMinutes[AdjustmentSetpoint] = MySpanMinutes[AdjustmentSetpoint] - 1;
        MySpanMinutes[AdjustmentSetpoint + 1] = MySpanMinutes[AdjustmentSetpoint + 1] + 1;
        MySpanAccumulatedMinutes[AdjustmentSetpoint] =  MySpanAccumulatedMinutes[AdjustmentSetpoint] - 1;
        EEPROM.update(MySpanMinutesEprom[AdjustmentSetpoint] , MySpanMinutes[AdjustmentSetpoint]);
        EEPROM.update(MySpanMinutesEprom[AdjustmentSetpoint + 1] , MySpanMinutes[AdjustmentSetpoint + 1]);
        Serial.println ("left");
      }
    }
    if (Xtouch > 275 && Xtouch < 305 && Ytouch2 > AdjustmentDisplayTop - 15  && Ytouch2 < AdjustmentDisplayTop + 15) {
      HideAdjustMentBoxes();
      Serial.println ("close");
      return;
    }
    else {
      DrawLinebyTimeAndTemp(MySpanAccumulatedMinutes[AdjustmentSetpoint - 1], MyBaseSetpoints[AdjustmentSetpoint - 1], beforeTime, beforeTemp, BLUE);
      DrawLinebyTimeAndTemp(beforeTime, beforeTemp, MySpanAccumulatedMinutes[AdjustmentSetpoint + 1], MyBaseSetpoints[AdjustmentSetpoint + 1], BLUE);
      DrawLinebyTimeAndTemp(MySpanAccumulatedMinutes[AdjustmentSetpoint - 1], MyBaseSetpoints[AdjustmentSetpoint - 1], MySpanAccumulatedMinutes[AdjustmentSetpoint], MyBaseSetpoints[AdjustmentSetpoint ], WHITE);
      DrawLinebyTimeAndTemp(MySpanAccumulatedMinutes[AdjustmentSetpoint], MyBaseSetpoints[AdjustmentSetpoint ], MySpanAccumulatedMinutes[AdjustmentSetpoint + 1], MyBaseSetpoints[AdjustmentSetpoint + 1], WHITE);
      UpdateAdjustMentText();
    }
  }

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------


int Y(int temp)
{

  //460 x 460 x 460 /240  = 379.687
  return (240 -  ((double)temp * (double)temp * double(temp)) / IYscale);
}

void tftPrintDouble(double num)
{
  //tft.println("  ");
  tft.println(num, 1);
}
void tftPrintIntTo5Char(int num)
{
  //  Serial.print("print:");Serial.println(num);
  sprintf(Buf5, "% 5d", num);
  tft.println(Buf5);
  // Serial.print("printbuf:");Serial.println(num);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void UpdateGraphA(double roastMinutes, double duty, int setpoint, double err, double errI) {
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  int row = 0;
  // tft.setCursor(40 ,row); tft.println("F:"); tft.setCursor(50 , row); tft.println(TempEnd);
  // tft.setCursor(100 , row); tft.println("Last:"); tft.setCursor(130 , row); tft.println(TempLastEndOfRoast);
  row = 20;
  tft.setCursor(40 , row); tft.print("sp:"); tft.setCursor(75 , row); tft.println(setpoint); tft.setCursor(100 , row); tft.print("SSD:"); tft.setCursor(130 , row); tft.println(ManagingSSR);
  row = 35;
  tft.setCursor(40 , row); tft.print("Time:");   tft.setCursor(75 , row); tft.println(roastMinutes);
  row = 50;
  if (duty > 1) {
    tft.setCursor(40 , row);
    tft.println("Duty:");
    tft.setCursor(75 , row);
    tft.println("1.00");
  }
  else if (duty < 0) {
    tft.setCursor(40 , row);
    tft.println("Duty:");
    tft.setCursor(75 , row);
    tft.println("0.00");
  }
  else {
    tft.setCursor(40 , row);
    tft.println("Duty:");
    tft.setCursor(75 , row);
    tft.println(duty);
  }
  row = 65;
  tft.setCursor(40 , row); tft.print("Err:"); tft.setCursor(75 , row); tft.println(-err);
  row = 80;
  tft.setCursor(40 , row); tft.print("IEr:"); tft.setCursor(75 , row); tft.println(-errI);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void UpdateGraphB(int temp2, int temp1, int tempCoil, double ampHeater1, double ampHeater2, int tempFan     , double ampFan, double volts)
{
  //Serial.print("TBean2:");Serial.print(temp2);Serial.print("TBean1:");Serial.print (temp1);Serial.print("TCoil:"); Serial.print(tempCoil); Serial.print("TFan:");Serial.println(tempFan);
  if (AdjustmentSetpoint > 0 ) return;


  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  int row = 135;
  tft.setCursor(150 , row); tft.println("Tvg:"); tft.setCursor(190 , row); tftPrintIntTo5Char(TBeanAvgRoll.mean());
  tft.setCursor(240 , row); tft.println("Skip"); tft.setCursor(270 , row); tftPrintIntTo5Char(Readingskipped);

  row = 150;
  tft.setCursor(150 , row); tft.println("T1:"); tft.setCursor(190 , row); tftPrintIntTo5Char(temp1);
  tft.setCursor(240 , row); tft.println("T2"); tft.setCursor(270 , row); tftPrintIntTo5Char(temp2);
  row = 165;
  tft.setCursor(150 , row); tft.println("Fan  T:"); tft.setCursor(190 , row); tftPrintIntTo5Char(tempFan);
  tft.setCursor(240 , row); tft.println("Amp:"); tft.setCursor(270 , row); tftPrintDouble(ampFan);
  row = 180;
  tft.setCursor(050 , row); tft.println("G:"); tft.setCursor(60 , row); tft.println(Gain); tft.setCursor(100 , row); tft.println("U D");
  GainX =  60; GainY = 180;
  GainUX = 102; GainUY = 55;
  GainDX = 114; GainDY = 55;
  tft.setCursor(150 , row); tft.println("Heat T:"); tft.setCursor(190 , row); tftPrintIntTo5Char(tempCoil);
  tft.setCursor(240 , row); tft.println("Amp1:"); tft.setCursor(270 , row); tftPrintDouble(ampHeater1);
  row = 195;
  tft.setCursor(050 , row); tft.println("I:"); tft.setCursor(60 , row);
  tft.println(Integral) ; tft.setCursor(100 , row); tft.println("U D");
  IntegralX  = 60; IntegralY  = 195;
  IntegralUX = 102; IntegralUY = 38;
  IntegralDX = 114; IntegralDY = 38;
  tft.setCursor(150 , row); tft.println("Volts:"); tft.setCursor(190 , row); tft.println(volts);
  tft.setCursor(240 , row); tft.println("Amp2:"); tft.setCursor(270 , row); tftPrintDouble(ampHeater2);
  row = 210;
  tft.setCursor(050 , row); tft.println("ps:"); tft.setCursor(60 , row); tftPrintIntTo5Char(LoopsPerSecond) ;

  // tft.setCursor(240 , row); tft.println("Amp:"); tft.setCursor(270 , row); tft.println(ampHeater);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void StartLinebyTimeAndTemp(double timemins, int temp, int lineID, uint16_t color)
{
  Serial.print ("StartLineTandT id:"); Serial.print (lineID); Serial.print(" color:"); Serial.println(color);
  LastXforLineID[lineID] = (PixelsPerMin * timemins);
  if (temp > 0) {
    LastYforLineID[lineID] = Y(temp);
  }
  else {
    LastYforLineID[lineID] = 240;
  }
  LineColorforLineID[lineID] = color;
  if (lineID == ROLLAVGLINEID) {
    for (int X = 0; X < 320; X++) {
      myLastGraph[X] = -1;
    }
  }
}


//void DrawPointByTimeAndTemp(double timemins, int temp, int lineID)
//{
//  int newX = (PixelsPerMin * timemins);
//  int newY = Y(temp);
//  //Serial.print ("AddLineTandT i:");Serial.print (i);Serial.print(" color:");Serial.println(LineColorforLineID[i]);
//  //Serial.print ("AddLineTandT i:");Serial.print (i);Serial.print(" time:");Serial.print(timemins);Serial.print(" temp:");Serial.println(temp);
//
//  tft.drawPixel(newX, newY, LineColorforLineID[lineID]);
//}
void AddLinebyTimeAndTemp(double timemins, int temp, int lineID)
{
  uint16_t newX = (uint16_t)(PixelsPerMin * timemins);
  int newY = Y(temp);
  //Serial.print ("AddLineTandT line iD:");Serial.print (lineID);Serial.print(" time:");Serial.print(timemins);Serial.print("temp:");Serial.print(temp);Serial.print(" color:");Serial.println(LineColorforLineID[lineID]);
  //Serial.print ("newX:");Serial.print (newX); Serial.print ("pixelspermin:");Serial.print (PixelsPerMin); Serial.print(" newY:");Serial.println(newY);
  tft.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY, LineColorforLineID[lineID]);
  LastXforLineID[lineID] = newX;
  LastYforLineID[lineID] = newY;
  if (lineID == ROLLAVGLINEID) {
    BoldLine(LastXforLineID[lineID], LastYforLineID[lineID] + 1, newX, newY + 1, LineColorforLineID[lineID]);
    myLastGraph[newX] = newY;
  }
}

void LoadORSaveToHistory(boolean Load) {
  int number = 1;
  //300 to 306
  int address = number * 100;
  if (!Load) {
    unsigned long timestamp = millis();
    EEPROM.put(address, timestamp);
    address = address + 4;
    EEPROM.write(address, TempLastEndOfRoast);
    address++;
    EEPROM.write(address, (TimeLastEndOfRoast * 10));
  }
  //312 - 318 & 319 to 325
  address = (number * 100) + 12;
  if (!Load) {
    for (int X = 0; X < SetPointCount; X++) {
      address++;
      EEPROM.write(address, MySpanMinutes[X]);
      SaveTempEprom(address + SetPointCount, MyBaseSetpoints[X]);
    }
  }
  else {
    for (int X = 0; X < SetPointCount; X++) {
      address++;
      MySpanMinutes[X] = EEPROM.read(address);
      MyBaseSetpoints[X] = ReadTempEprom(address + SetPointCount , 0);
    }
  }
  //330 to 394 (320 pixes wide/5 = 64)
  address = (number * 100) + 30;
  int avgwidth = 5;
  if (!Load) {
    int total; int count;
    for (int X = 0; X < 320; X++) {
      total = total + myLastGraph[X];
      count++;
      if (count >= avgwidth) {
        address++;
        int avg = total / avgwidth;
        EEPROM.write(address, avg);
      }
    }
  }
  else {
    int Xlast = 0;
    int Ylast = EEPROM.read(address);
    address++;
    for (int X = 1; X < 64; X++) {
      int Ynext =  EEPROM.read(address);
      double Ydelta = (double)(Ynext - Ylast) / (double)avgwidth;
      for (int k = 1;  k <= avgwidth; X++) {
        myLastGraph[(X - 1) + k] = Ylast + (Ydelta * k) ;
      }
      address++;
    }
  }
  if (Load) {

  }
}
void ReDrawROLLAVGLINEFromArray(int color) {
  LastXforLineID[1] = 0;
  LastYforLineID[1] = 240;
  for (int X = 0; X < 320; X++) {
    if (myLastGraph[X] > 0 ) {
      //  Serial.print ("DrawRealTime ");Serial.print(" color:");Serial.println(color);
      tft.drawLine(LastXforLineID[1], LastYforLineID[1] , X, myLastGraph[X], color);
      BoldLine(LastXforLineID[1], LastYforLineID[1] , X, myLastGraph[X], color);
      LastXforLineID[1] = X;
      LastYforLineID[1] = myLastGraph[X];
    }
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BoldLine(int x, int y, int newX, int newY, int color) {
  tft.drawLine(x, y + 1, newX, newY + 1, color);
  tft.drawLine(x + 1, y + 1, newX + 1, newY + 1, color);
  tft.drawLine(x + 1, y, newX + 1, newY, color);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawLinebyTimeAndTemp(double timemins1, int temp1, double timemins2, int temp2, int color) {
  tft.drawLine(PixelsPerMin * timemins1, Y(temp1), PixelsPerMin * timemins2, Y(temp2), color);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  displayState(int state) {
  tft.setCursor(40, 0);
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  switch (state) {
    case AMROASTING:
      tft.setTextColor(WHITE, RED); tft.println("*AmRoasting*");
      break;
    case AMSTOPPED:
      tft.println( "AmStopped        ");
      break;
    case AMAUTOCOOLING:
      tft.setTextColor(WHITE, BLUE); tft.println( "AmCooling        ");
      break;
    case AMOVERHEATEDCOIL:
      tft.println ("To Hot Coil            ");
      break;
    case AMOVERHEATEDFAN:
      tft.println ("To Hot Fan            ");
      break;
    case AMFANONLY:
      tft.println ("Fan only            ");
      break;
    default:
      tft.println ("unkState   ");
      break;
  }
}

#include <max6675.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "Average.h"
#include "MAX31856.h"
//#include "PID_v1.h"
#include "Chrono.h"




// ===========
//  PINS 
// ===========


//graphic
// For the Adafruit shield, these are the default.

#define TFT_DCp 48
#define TFT_CSp 53
#define TFT_MOSIp 51
#define TFT_MISOp 50
#define TFT_CLKp 52
#define TFT_RSTp 49

//THERMOCOUPLES
#define SCKp    12
#define SDIp    11
#define SDOp    6
#define CS3p    10
#define CS2p    9
#define CS1p    8
#define CS0p    7


#define PIDSSDp  2
#define CPLEDp   4
#define MaxVoltp  5
#define CURFANp   7
#define CURHEATp  6

//capacitive buttons
#define CP1p    22
#define CP2p    24
#define CP3p    26
#define CP4p    28
#define CP5p    30
#define CP6p    32
#define CP7p    34
#define CP8p    36
  
#define PIDRELAYp    37
#define HTRRELAYp   35
#define FANRELAYp    33
#define VIBRELAYp    31

#define TEMPEND_EP 0
#define TIMEEND_EP 1
#define GAIN_EP 2
#define TEMPLASTENDOFROAST_EP 3
//TempLastEndOfRoast
//Constants
#define OVERHEAT        800
#define BAUD            9600
#define AmRoasting      1
#define AmStopped       2
#define AmCooling       3
#define AmOverheated    4
#define AmReadyToStart  5




// ===========
// DEFINITIONS
// ===========


// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CSp, TFT_DCp, TFT_MOSIp, TFT_CLKp, TFT_RSTp, TFT_MISOp);

// MAX31856 Initial settings (see MAX31856.h and the MAX31856 datasheet)
// The default noise filter is 60Hz, suitable for the USA
#define CR0_INIT  (CR0_AUTOMATIC_CONVERSION + CR0_OPEN_CIRCUIT_FAULT_TYPE_K /* + CR0_NOISE_FILTER_50HZ */)
#define CR1_INIT  (CR1_AVERAGE_2_SAMPLES + CR1_THERMOCOUPLE_TYPE_K)
#define MASK_INIT (~(MASK_VOLTAGE_UNDER_OVER_FAULT + MASK_THERMOCOUPLE_OPEN_FAULT))


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

// ===============================================================================================================================================================================
// GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            GLOBALS            
// ======================================================================================================================================================================================

int State;

MAX31856 *temperature0; 
MAX31856 *temperature1; 
MAX31856 *temperature2; 
//MAX31856 *temperature3;
MAX6675 thermocouple3(SCKp, CS3p, SDOp);
//MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
//#define SCKp    12
//#define SDIp    11
//#define SDOp    6


//global variablers for temp control
//double Setpoint, Input, Output, 
int Gain = 10;  
int PIDWindowSize = 2000; 
unsigned long PIDWindowStartTime;
boolean PIDNewWindow;
int MyFiveSetpoints[] = {120,330,370,390,400,420,480};
int TempEnd = 450; //set by eprom
int TimeTarget = 15; //set by eprom
int TempCool = 150;
int TimeEnd = 25;
int TempLastEndOfRoast;
double TimeLastEndOfRoast;

//will turn on when delta T x 100 2000 
//PID myPID(&Input, &Output, &Setpoint, 100, 0, 0, DIRECT);

Chrono RoastTime(Chrono::SECONDS);
Chrono LcdUdateTime(Chrono::MILLIS);
Chrono KeyPressTime(Chrono::MILLIS);

     
 



 
 int CapButActive = 0;
 int CapButIncrease = true;
 


Average<float> AvgFan(10);
Average<float> AvgHeat(25);
Average<int> TBeanAvgRoll(5);


double PixelsPerF; 
int OffsetF;
int PixelsPerSpan; 
int PixelsPerMin;
int LastX;
int LastY;
int myLastGraph[320];    
//PROGRAM

// =====================================================================================================================================================================
// SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            SETUP            
// =====================================================================================================================================================================
void setup() {

  // Pin Configuration
  pinMode(HTRRELAYp, OUTPUT);pinMode(FANRELAYp, OUTPUT);pinMode(VIBRELAYp, OUTPUT); pinMode(PIDRELAYp, OUTPUT);

  //pinMode( CP1p, INPUT);  pinMode (CP2p, INPUT); pinMode (CP3p, INPUT);  pinMode (CP4p, INPUT);
  pinMode(CPLEDp, OUTPUT);

  

  pinMode(PIDSSDp, OUTPUT); 
  
  //set relays to high - cause that means off
  digitalWrite(FANRELAYp, HIGH); digitalWrite(VIBRELAYp, HIGH); digitalWrite(PIDRELAYp,HIGH); digitalWrite(HTRRELAYp, HIGH);

  //digitalWrite(TFT_RSTp, LOW);

  // PID Setup
//  PIDWindowStartTime = millis();  myPID.SetOutputLimits(0, PIDWindowSize); 
//  Setpoint = 200; myPID.SetMode(AUTOMATIC); Gain = 100; //gain
  
  // Display temperatures using the serial port
  Serial.begin(BAUD); delay(3000);

  // Power up MAX31856
//  digitalWrite(TPWRp, HIGH);
  // Wait for MAX31856 to initialize
  delay(200);  
  // Define the pins used to communicate with the MAX31856
  temperature0 = new MAX31856(SDIp, SDOp, CS0p, SCKp); 
  temperature1 = new MAX31856(SDIp, SDOp, CS1p, SCKp); 
  temperature2 = new MAX31856(SDIp, SDOp, CS2p, SCKp);
  //temperature3 = new MAX31856(SDIp, SDOp, CS3p, SCKp);
  // Initializing the MAX31856's registers
  temperature0->writeRegister(REGISTER_CR0, CR0_INIT); temperature0->writeRegister(REGISTER_CR1, CR1_INIT); temperature0->writeRegister(REGISTER_MASK, MASK_INIT);
  temperature1->writeRegister(REGISTER_CR0, CR0_INIT); temperature1->writeRegister(REGISTER_CR1, CR1_INIT); temperature1->writeRegister(REGISTER_MASK, MASK_INIT);
  temperature2->writeRegister(REGISTER_CR0, CR0_INIT); temperature2->writeRegister(REGISTER_CR1, CR1_INIT); temperature2->writeRegister(REGISTER_MASK, MASK_INIT);
//  temperature3->writeRegister(REGISTER_CR0, CR0_INIT); temperature3->writeRegister(REGISTER_CR1, CR1_INIT); temperature3->writeRegister(REGISTER_MASK, MASK_INIT);

  
  // Wait for the first sample to be taken
  delay(200);
 RoastTime.stop();
 //set up LCD
 lcd.begin (20,4); lcd.home ();lcd.print("Starting");  LcdUdateTime.restart(0);

//read dynamic data
int t;


//#define TEMPEND_EP 0;
//#define TIMEEND_EP 1;
//#define GAIN_EP 2;
//#define ROASTENDLAST_EP 3;


t = EEPROM.read(TEMPEND_EP);
if (t > 0){ TempEnd = t + 254;
}  else { TempEnd = 254 ;}

t = EEPROM.read(TIMEEND_EP);
if (t > 0){TimeTarget = t ;
}  else { TimeTarget = 12 ;}


t = EEPROM.read(GAIN_EP);
if (t > 0 && t < 101){Gain = t ;
}  else { Gain  = 10 ;}

t = EEPROM.read(TEMPLASTENDOFROAST_EP);
if (t > 0 && t < 250){TimeLastEndOfRoast = t + 254 ;
}  else { TimeLastEndOfRoast  = 0 ;}


   tft.begin();



  graphProfile(); 
State = AmReadyToStart;
}

// **************************************************************************************************************************************************************
// LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP
// **************************************************************************************************************************************************************
void loop () {

//per loop variables
int newState = 0;
float tHeat0, tBean1, tBean2, tFan;
double maxVoltage, currentFan, currentHeat;
int minuteToStart = 0;
boolean switchVal;


//**********************************************************************************************************************************
//read temperatures and amps             read temperatures and amps                read temperatures and amps              read temperatures and amps
//********************************************************************************************************************************
maxVoltage = (((double)analogRead(MaxVoltp))/1024) * 5;
currentFan = (((double)(analogRead(CURFANp) - 512) * maxVoltage))/185;
currentHeat = (((double)(analogRead(CURHEATp) - 512) * maxVoltage))/100;
AvgFan.push(currentFan);
AvgHeat.push(currentHeat * currentHeat);

currentFan = AvgFan.mean(); 
currentHeat = sqrt( AvgHeat.mean()); 

  tHeat0 = getTemperature(temperature0->readThermocouple(FAHRENHEIT));
  tBean1 = getTemperature(temperature1->readThermocouple(FAHRENHEIT));
  tBean2 = getTemperature(temperature2->readThermocouple(FAHRENHEIT));
  tFan =  thermocouple3.readFahrenheit(); // getTemperature(temperature3->readThermocouple(FAHRENHEIT));
  int tBeanAvg = getBeanAvgTemp(tBean1, tBean2);  
  TBeanAvgRoll.push(tBeanAvg);
  double roastMinutes = ((double)RoastTime.elapsed())/60;


//*******************************************************************************************************************************************************
//Input buttons                Input buttons                Input buttons                Input buttons                Input buttons                
//*******************************************************************************************************************************************************

//find and active button
int capButGoneHigh = 0;
if (CapButActive == 0) {
    if    (digitalRead(CP1p) == HIGH) {
            capButGoneHigh = CP1p;
            IncreaseDecrease(CapButIncrease, TimeTarget, 8, 30);
            EEPROM.write(TIMEEND_EP, TimeTarget);
          }
    else if ( digitalRead(CP2p) == HIGH){
          capButGoneHigh = CP2p;
          IncreaseDecrease(CapButIncrease, TempEnd, 430, 470);
          EEPROM.write(TEMPEND_EP, TempEnd - 254);
    }
    else if ( digitalRead(CP3p) == HIGH)
          {capButGoneHigh = CP3p;
          IncreaseDecrease(CapButIncrease, Gain, 0, 100);
          EEPROM.write(GAIN_EP, Gain);
          }
   else if ( digitalRead(CP4p) == HIGH)
          {capButGoneHigh = CP4p;
            CapButIncrease = !CapButIncrease;
          }
  else if ( digitalRead(CP5p) == HIGH)
          {capButGoneHigh = CP5p;
           if (State == AmReadyToStart or State == AmStopped){
                      newState = AmRoasting;  
                      Serial.println("Start Detected!");
           }
          
          }
  else if ( digitalRead(CP6p) == HIGH)
          {capButGoneHigh = CP6p;
          Serial.println("here");
          if (State == AmRoasting ){
                Serial.println("Stop detected!");
                if (tBeanAvg < TempCool){ 
                newState = AmStopped;
              }
          else{
                Serial.println("Still warm.  Going to cooling!");
                newState = AmCooling;     
            }  
          }
         else if (State != AmCooling){
             State = AmReadyToStart; 
        
         }
          
        }
  else if ( digitalRead(CP7p) == HIGH)
          {capButGoneHigh = CP7p;
          
          }
  else if ( digitalRead(CP8p) == HIGH)
          {capButGoneHigh = CP8p;
            
           digitalWrite(CPLEDp,HIGH);
           
            Serial.println ("start reset");
           
            digitalWrite(TFT_RSTp, LOW);
            tft.begin();
            //digitalWrite(TFT_RSTp, HIGH);
            Serial.println ("end reset start profile");
            graphProfile(); 
            Serial.println ("end profile");

          
          }
 
   else
   {

      
    }
   if (capButGoneHigh > 0)
   {
           digitalWrite(CPLEDp,HIGH);
           KeyPressTime.restart(0);
           Serial.print ("Button pressed:" );
           Serial.println (capButGoneHigh);
           CapButActive =  capButGoneHigh;
   }
}
else
{
    if    (digitalRead( CapButActive) == LOW) {
        digitalWrite(CPLEDp,LOW);
        Serial.print ("Button releasewd:" );
        Serial.println (CapButActive);
        CapButActive = 0;  
        KeyPressTime.stop();
       
    }

}





  


//**************************************************************************************************
 //DETERIM NEW STATE       DETERIM NEW STATE       DETERIM NEW STATE       DETERIM NEW STATE       DETERIM NEW STATE       
 //*****************************************************************************************************
 if (tHeat0 > OVERHEAT){
     newState = AmOverheated;
     Serial.println("Overheat Detected!");
 }
 else if (TBeanAvgRoll.mean() > TempEnd && State == AmRoasting){
    newState = AmCooling;
    Serial.print("Roast Temp Reached. Cooling starting");
    Serial.println();
 }
 else if (TBeanAvgRoll.mean() < TempCool && State == AmCooling){
    newState = AmStopped;
    Serial.print("Cooling Complete ");
    Serial.println();
 }
 else if (roastMinutes > TimeEnd && State == AmRoasting){
    newState = AmCooling;
    Serial.println("Max time reached. Cooling starting");
  }
 


// **************************************************************************************************************************************************************
 //APPLY NEW STATE           APPLY NEW STATE           APPLY NEW STATE           APPLY NEW STATE           APPLY NEW STATE           APPLY NEW STATE           APPLY NEW STATE
 // **************************************************************************************************************************************************************
if (newState == AmStopped){
     if (State != AmCooling){ 
         Serial.print("Stopping heat - and waiting 10 seconds ");
     }
     Serial.println();
     State = AmStopped;
    lcd.home();
    lcd.print("WaitToStop");
    digitalWrite(HTRRELAYp, HIGH); digitalWrite(PIDRELAYp, HIGH); 
    delay(5000);
     digitalWrite(FANRELAYp, HIGH); digitalWrite(VIBRELAYp, HIGH);
     newState = -2;
 }
   else if (newState == AmRoasting){ 
    State = AmRoasting;
    Serial.println("Starting Fans and Vibration - and waiting 5 seconds");
    digitalWrite(FANRELAYp, LOW); digitalWrite(VIBRELAYp, LOW);
    lcd.home();
    lcd.print("1 ClearGraph");
    //this clears any historic profile
    StartAddedLineW(0, 0);
    
    graphProfile();
    lcd.home();
    lcd.print("2 Wait..");
    delay(1000); 
    Serial.println("2 Starting Heaters ");
    RoastTime.restart(minuteToStart * 60);
    roastMinutes = ((double)RoastTime.elapsed())/60;
    
    
    digitalWrite(HTRRELAYp, LOW);  digitalWrite(PIDRELAYp, LOW);
    newState = -2;
  }
 else if (newState == AmCooling){ 
    State = AmCooling;
    TempLastEndOfRoast = TBeanAvgRoll.mean();
    EEPROM.write(TEMPLASTENDOFROAST_EP, TempLastEndOfRoast - 254);
    
    RoastTime.stop(); 
    digitalWrite(HTRRELAYp, HIGH);    digitalWrite(PIDRELAYp, HIGH);
    delay(1000);
    newState = -2;
  }
 else if (newState == AmOverheated){ 
    State = AmOverheated; 
    digitalWrite(HTRRELAYp, HIGH);digitalWrite(PIDRELAYp, HIGH); 
    delay(1000);
    newState = -2;
  }
else if (State == AmOverheated ){ 
    State = AmRoasting;
    newState = -2;
    digitalWrite(HTRRELAYp, LOW); digitalWrite(PIDRELAYp, LOW);
 }


//***************************************************************************************************************************************************************
 // CALCULATING PID VALUES         CALCULATING PID VALUES         CALCULATING PID VALUES         CALCULATING PID VALUES         CALCULATING PID VALUES        
 //*************************************************************************************************************************************************************
double err = 0 ;
double duty = 0 ; 
double setpoint ;
if (State == AmRoasting){
    setpoint =  calcSetpoint(roastMinutes); 
    err = setpoint - TBeanAvgRoll.mean();  //negative if temp is over setpoint
    duty =  (err / Gain); //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
                          //100% means never set low.  0% means set low right away
    long now = millis();   

        
    if (now - PIDWindowStartTime > PIDWindowSize){ 
       Serial.println("windowrest");
       PIDWindowStartTime = now;
       PIDNewWindow = true;
    }
    
    if(now - PIDWindowStartTime >  duty * PIDWindowSize) {
           Serial.println("set off");
           digitalWrite(PIDSSDp, LOW);          
    } else { //we allow it to go high once per window 
         if (PIDNewWindow == true){ 
              Serial.println("set on");
              digitalWrite(PIDSSDp, HIGH); 
              PIDNewWindow = false;
         }
         else 
         {
            Serial.println("is on");
         }
    }
}
else{
      digitalWrite(PIDSSDp, LOW);
}




//********************************************************************************************************************************
//LCD AND GRAPH DISPLAY         LCD AND GRAPH DISPLAY         LCD AND GRAPH DISPLAY         LCD AND GRAPH DISPLAY         LCD AND GRAPH DISPLAY         
//*******************************************************************************************************************************

 
if (newState = -2){
     displayLCDandGraphState(State);  newState =0;
}



displayLCDDouble(11,0,roastMinutes,5,2,"/"); 

displayLCDDouble(17,0,TimeTarget,2,0);
if (CapButIncrease) {displayLCDText(19,0,"U");}
else {displayLCDText(19,0,"D");}
 


if (LcdUdateTime.elapsed() > 3000)
 { 
    UpdateGraphA (roastMinutes,TBeanAvgRoll.mean(),duty,TempEnd);
  
    //Graph              Graph              Graph              Graph              Graph              Graph              Graph                          
    if (State == AmRoasting)
    {
        AddLineW(roastMinutes, tBeanAvg);  
    }
      
  
    //second row            second row            second row            second row            second row            second row            
    displayLCDDouble(0,1,TBeanAvgRoll.mean(),3,0,"F 2-");  displayLCDDouble(7,1,tBean2,3,0," 3-"); 
    displayLCDDouble(13,1,tBean1,3,0);     displayLCDDouble(17,1,TempEnd,3,0);

     
 
    //third row            third row            third row            third row            third row            third row            third row            third row            
    displayLCDText(0,2,"ht"); displayLCDDouble(2,2,currentHeat,4,1,"a");   
    displayLCDDouble(9,2,tHeat0,3,0,"F"); displayLCDDouble(16,2,(double)duty,3,0,"%");  
    
    //fourth row       fourth row       fourth row       fourth row       fourth row       fourth row       fourth row       fourth row       fourth row       
    displayLCDText(0,3,"fn"); displayLCDDouble(2,3,currentFan,4,1,"a"); 
    displayLCDDouble(9,3,tFan,3,0, "F");displayLCDDouble(15,3,maxVoltage,3,2, "V");
    LcdUdateTime.restart(0);
 }

   
}

//----------------------------------------------------------------------------------------------------------------------------------------------------

// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS            

//----------------------------------------------------------------------------------------------------------------------------------------------------

// Print the temperature, or the type of fault
void PrT(double temperature) {
  switch ((int) temperature) {
    case FAULT_OPEN:
      //Serial.print("OPEN");
      break;
    case FAULT_VOLTAGE:
      //Serial.print("NC");
      break;
    case NO_MAX31856:
      //Serial.print("FAULT");
      break;
    default:
      Serial.print(temperature);
      Serial.write(176);
      Serial.print("C");
      break;
  }
}

// Get the temperature
double getTemperature(double temperature) {
  switch ((int) temperature) {
    case FAULT_OPEN:
      // Serial.println("faultopn");
      return -1;
      break;
    case FAULT_VOLTAGE:
      //Serial.println("faultvoltage");
      return -1;
      break;
    case NO_MAX31856:
      //Serial.println("faultno_max31865");
      return -1;
      break;
    default:
      return temperature;
      break;
  }
}

double getBeanAvgTemp(double t1, double t2) {

  if (getTemperature(t1) != -1 && getTemperature(t2) != -1){
      return (t1 + t2) / 2;}
  else {
    if (t1 != -1) {
        return t1;}
    else {  
       return t2;  
    }
  }
  
}


double calcSetpoint(double roastminutes) {
     double setpoint = 0;
     double MinutesPerSpan = ((double)TimeTarget)/6;
     int spIndex = roastminutes/MinutesPerSpan;
     double r = roastminutes/MinutesPerSpan - spIndex;
        
     if (spIndex < 6)
     {
        setpoint = MyFiveSetpoints[spIndex] + ((MyFiveSetpoints[spIndex + 1] - MyFiveSetpoints[spIndex]) * r);
     }
     else 
     {
         setpoint = MyFiveSetpoints[5];
      
     }
     return setpoint;
}




 void IncreaseDecrease(boolean increase, int& val, int min, int max){

        Serial.println("start and I or D...");
        if (increase){
            if (val < max) {
                Serial.println("increasing...");
                val = val + 1;  
            } else {
            val = max;  
          }
         }
         else
         {
          if (val > min) {
              Serial.println("decreasing...");
              val = val - 1;  
           } else {
            val = min;  
          }
         
         }

 
 }

//-------------------------------------------------------------------------------------------------------------------------------
//GRAPHING             GRAPHING             GRAPHING             GRAPHING             GRAPHING             GRAPHING             
//-------------------------------------------------------------------------------------------------------------------------------


 void graphProfile() {
     // display is 240 x 320
     //15 pixels per minute
     // F / 2 is height

     
     
     //tft.begin();
     tft.setRotation(1);
     tft.fillScreen(ILI9341_BLACK);
     // {120,290,330,350,390,450,480}; 
     PixelsPerF = 1.8; 
     OffsetF = -40;
     double  timePerSpan = (double)TimeTarget/6;
     PixelsPerSpan = (double)320 / 5.5; 
     PixelsPerMin = (PixelsPerSpan * 5)/TimeTarget;
     int color = tft.color565(125, 125, 125);
     StartAddedLine (0, MyFiveSetpoints[0]);
     AddLine(1 * timePerSpan, MyFiveSetpoints[1], ILI9341_WHITE);
     AddLine(2 * timePerSpan, MyFiveSetpoints[2], color);
     AddLine(3 * timePerSpan, MyFiveSetpoints[3],  ILI9341_WHITE);
     AddLine(4 * timePerSpan, MyFiveSetpoints[4], color);
     AddLine(5 * timePerSpan, MyFiveSetpoints[5],  ILI9341_WHITE);
     AddLine(6 * timePerSpan, MyFiveSetpoints[6], color);
    LastX = 0;
    LastY = 0;    
      
     
     tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
     tft.setCursor(1 * PixelsPerMin, 230); tft.println("1");
     tft.setCursor(3 * PixelsPerMin, 230); tft.println("3");  
     tft.drawFastVLine(3 * PixelsPerMin ,0,tft.height(),color);
     
     tft.setCursor(6 * PixelsPerMin, 230); tft.println("6");  
     tft.drawFastVLine(6 * PixelsPerMin ,0,tft.height(),color);

     tft.setCursor(9 * PixelsPerMin, 230); tft.println("9");
     tft.drawFastVLine(9 * PixelsPerMin ,0,tft.height(),color);

     tft.setCursor(12 * PixelsPerMin, 230); tft.println("12");
     tft.drawFastVLine(12 * PixelsPerMin ,0,tft.height(),color);

     tft.setCursor(15 * PixelsPerMin, 230); tft.println("15");  
     tft.drawFastVLine(15 * PixelsPerMin ,0,tft.height(),color);

     tft.setCursor(18 * PixelsPerMin, 230); tft.println("18");  
     tft.drawFastVLine(18 * PixelsPerMin ,0,tft.height(),color);

     tft.drawFastVLine(PixelsPerMin * TimeTarget,0,tft.height(), ILI9341_BLUE);
     
     DrawTempTargetLine();
     
    tft.setCursor(0 , 240 - OffsetF - 200/PixelsPerF); tft.println("200");
    tft.setCursor(0 , 240 - OffsetF - 250/PixelsPerF); tft.println("250");
    tft.setCursor(0 , 240 - OffsetF - 300/PixelsPerF); tft.println("300");
    tft.setCursor(0 , 240 - OffsetF - 350/PixelsPerF); tft.println("350");
    tft.setCursor(0 , 240 - OffsetF - 400/PixelsPerF); tft.println("400"); 
    tft.setCursor(0 , 240 - OffsetF - 450/PixelsPerF); tft.println("450"); 
    delay(1000);
    DrawWFromArray();
}


void DrawTempTargetLine()
{
   tft.drawFastHLine(0, 240 - OffsetF - TempEnd/PixelsPerF,tft.width(), ILI9341_BLUE);
   UpdateGraphEndTemp();
}

  

void UpdateGraphEndTemp()
{
 tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
 tft.setCursor(40 , 10); tft.println("F:");
 tft.setCursor(50 , 10); tft.println(TempEnd);
  
}

void UpdateGraphA(double TimeMin, int Temp, double duty, int TempEnd)
{
 //tft.fillRect(40,50,40,30,ILI9341_BLACK);
 tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);  tft.setTextSize(1);

//second row
 tft.setCursor(40 , 10); tft.println("F:");
 tft.setCursor(50 , 10); tft.println(TempEnd);
 tft.setCursor(100 , 10); tft.println("Last:");
 tft.setCursor(130 , 10); tft.println(TempLastEndOfRoast);
 
//apx 5 row   
 tft.setCursor(40 , 40); tft.println("F:");
 tft.setCursor(50 , 40); tft.println(Temp);

 tft.setCursor(40 , 50); tft.println("M:");   
 tft.setCursor(50 , 50); tft.println(TimeMin);  

 tft.setCursor(40 , 60); tft.println("%:");
 tft.setCursor(50 , 60); tft.println(duty);   
 tft.setCursor(40 , 70); tft.println("G:");
 tft.setCursor(50 , 70); tft.println(Gain);   



}
void UpdateGraphB(double ampHeater, double ampFan, int tempHeater, int tempFan)
{
 //tft.fillRect(40,50,40,30,ILI9341_BLACK);
 tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);  tft.setTextSize(1);

 //tft.setCursor(40 , 50); tft.println("F:");
 //tft.setCursor(50 , 50); tft.println(Temp);

 //tft.setCursor(40 , 60); tft.println("M:");   
// tft.setCursor(50 , 60); tft.println(TimeMin);  

// tft.setCursor(40 , 70); tft.println("%:");
// tft.setCursor(50 , 70); tft.println(duty);   

}





void StartAddedLine(double timemins, int temp)
{

     LastX = (PixelsPerMin * timemins);
    if (temp > 0)
    {LastY = 240 - OffsetF - temp/PixelsPerF;}
    else
    {LastY = 240;} 
}

void StartAddedLineW(double timemins, int temp)
{
     for (int X = 0; X < 320; X++) {
        myLastGraph[X] = -1;
     }

     LastX = (PixelsPerMin * timemins);
    if (temp > 0)
    {LastY = 240 - OffsetF - temp/PixelsPerF;}
    else
    {LastY = 240;} 
}

void DrawLineW(int lastX, int lastY, int newX, int newY, int color)
{

   tft.drawLine(lastX, lastY, newX, newY, color);
   tft.drawLine(lastX + 1, lastY , newX + 1, newY , color);
   if (LastX > 0) 
      {tft.drawLine(lastX -1, lastY , newX - 1, newY , color);}
   tft.drawLine(lastX , lastY - 1 , newX , newY - 1 , color);
   if (lastY < 240) 
     { tft.drawLine(lastX , lastY + 1 , newX , newY + 1 , color);}
   if (lastY < 240) 
     { tft.drawLine(lastX , lastY + 1 , newX , newY + 1 , color);}

}

void AddLineW(double timemins, int temp)
{
  
  int newX = (PixelsPerMin * timemins) ;
  int newY = 239 - OffsetF - temp/PixelsPerF;

  if (newX == LastX & newY == LastY) {return;}
  if (newX < LastX) 
  {LastX = 0;
  LastY = 240;}
   
   DrawLineW(LastX, LastY, newX, newY, ILI9341_YELLOW);
   
   //Serial.print("AddLineW Lx:");
   //Serial.print (LastX);
   //Serial.print(" Ly:");
   //Serial.print(LastY);
   //Serial.print (" nX:");
   //Serial.print(newX);
   //Serial.print(" nY:");
   //Serial.print(newY);
   //Serial.println();


   LastX = newX;
   LastY = newY;
   myLastGraph[LastX] = LastY;


}
void DrawWFromArray() {
 
  LastX =0;
  LastY = 240;
   for (int X = 0; X < 320; X++) {
     if (myLastGraph[X] > 0 ){
            DrawLineW(LastX, LastY , X, myLastGraph[X] , ILI9341_ORANGE); 
            LastX = X;
            LastY = myLastGraph[X]; 
     }
     
  }
}

void AddLine(double timemins, int temp, int color)
{

  int newX = (PixelsPerMin * timemins);
  int newY = 240 - OffsetF - temp/PixelsPerF;
  tft.drawLine(LastX, LastY, newX, newY, color); 
  LastX = newX;
  LastY = newY;


}




//-------------------------------------------------------------------------------------------------------------------------------

//LCD Display                 LCD Display                 LCD Display                 LCD Display                 LCD Display                 
//-------------------------------------------------------------------------------------------------------------------------------

void displayLCDText(int col, int row, String text)
{
 lcd.setCursor(col,row);
 lcd.print(text);
  
}
void displayLCDDouble(int col, int row, double number, int len, int dec)
{
  displayLCDDouble(col, row, number, len, dec, "");
}

void displayLCDDouble(int col, int row, double number, int len, int dec, String label)
{
 String s = String(number,dec);

  while(s.length() < len){
   s = " " + s;
   }

  lcd.setCursor(col,row);

  lcd.print(s + label);

  
}

 void  displayLCDandGraphState(int state) {
  lcd.home();
  tft.setCursor(0, 0);
 
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);  tft.setTextSize(1);
  
  switch (state) {
    case AmRoasting:
      lcd.print("AmRoasting ");
      tft.println("AmRoasting        ");
  
      break;
    case AmStopped:
      lcd.print( "AmStopped  ");
      tft.println( "AmStopped        ");

      break;
    case AmCooling:
      lcd.print ("AmCooling  ");
      tft.println( "AmCooling        ");

      break;
    case AmOverheated:
      lcd.print ("To Hot     ");
      tft.println ("To Hot            ");
      break;
    case AmReadyToStart:
      lcd.print ("AmReadyTo ");
      tft.println ("AmReadyToRoast    ");
      break;  
    default:
      lcd.print   ("unknown   ");
      tft.println ("unknown   ");
      break;
  }
}




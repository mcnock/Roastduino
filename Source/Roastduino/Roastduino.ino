

// Roastduino v1.0
// 2016.09.20

// 
// 2016.09.20        Initial Version

// =========
// LIBRARIES
// =========
#include "MAX31856.h"
#include "PID_v1.h"
#include "Chrono.h"

// ===============
// PIN DEFINITIONS
// ===============

#define SCK    12
#define SDI    11
#define CS3    10
#define CS2    9
#define CS1    8
#define CS0    7
#define SDO    6
#define TPWR   5
#define HTRPID  3
#define INT_LED    13

//capacitive buttons
#define CP1    22
#define CP2    24
#define CP3    26
#define CP4    28


#define PWRPID A4
#define HTR1 A3
#define FAN A2
#define VIB A1
#define STARTME A0
  
#define AmRoasting      1
#define AmStopped       2
#define AmCooling       3
#define AmOverheated    4
#define AmReadyToStart  5

#define OVERHEAT 800
#define BAUD 57600

// ===========
// DEFINITIONS
// ===========
// MAX31856 Initial settings (see MAX31856.h and the MAX31856 datasheet)
// The default noise filter is 60Hz, suitable for the USA
#define CR0_INIT  (CR0_AUTOMATIC_CONVERSION + CR0_OPEN_CIRCUIT_FAULT_TYPE_K /* + CR0_NOISE_FILTER_50HZ */)
#define CR1_INIT  (CR1_AVERAGE_2_SAMPLES + CR1_THERMOCOUPLE_TYPE_K)
#define MASK_INIT (~(MASK_VOLTAGE_UNDER_OVER_FAULT + MASK_THERMOCOUPLE_OPEN_FAULT))

// =======
// GLOBALS
// =======
boolean ;
int State;
boolean StartmeSwitch;
MAX31856 *temperature0;
MAX31856 *temperature1;
MAX31856 *temperature2;
MAX31856 *temperature3;
double Setpoint, Input, Output;
int WindowSize = 1000;
unsigned long WindowStartTime;

Chrono RoastTime(Chrono::SECONDS);

PID myPID(&Input, &Output, &Setpoint, 70, 0, 0, DIRECT);

//                          000,001,002,003,004,005,006,007,008,009,010,011,012,013,014,015,016,017,018,019,020
// int MyMinuteSetpoints[] = {100,205,215,220,230,240,250,272,275,280,292,300,310,320,340,380,430,460,460,460,460};
 //                       200,200,300,400,500     
 int MyFiveSetpoints[] = {70,200,333,380,430,470};
 int TempEnd = 460;
 int TimeTarget = 12;
 int TimeMax = 16;
// =====
// SETUP
// =====
void setup() {
  // Pin Configuration
  pinMode(HTR1, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(VIB, OUTPUT);
  pinMode(PWRPID, OUTPUT);

  //capacitive buttons   
  //pinMode( CP1, INPUT); 
  //pinMode (CP2, INPUT); 
  //pinMode (CP3, INPUT);    
  //pinMode (CP4, INPUT);
  
  pinMode(STARTME, INPUT_PULLUP);
  pinMode(TPWR, OUTPUT);
  pinMode(INT_LED, OUTPUT);
  pinMode(HTRPID, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(FAN, HIGH);
  digitalWrite(VIB, HIGH);
  digitalWrite(PWRPID,HIGH);
  digitalWrite(HTR1, HIGH);

  // PID Setup
  WindowStartTime = millis();
  myPID.SetOutputLimits(0, WindowSize);
  Setpoint = 200;
  
  myPID.SetMode(AUTOMATIC);
  
  // Display temperatures using the serial port
  Serial.begin(BAUD);
  delay(3000);
  Serial.println("Roastduino Console");
  Serial.println("v1.0 2016.09.20");
    
  // Power up MAX31856
  digitalWrite(TPWR, HIGH);
  
  // Wait for MAX31856 to initialize
  delay(200);
  RoastTime.stop();
  // Define the pins used to communicate with the MAX31856
  temperature0 = new MAX31856(SDI, SDO, CS0, SCK);
  temperature1 = new MAX31856(SDI, SDO, CS1, SCK);
  temperature2 = new MAX31856(SDI, SDO, CS2, SCK);
  temperature3 = new MAX31856(SDI, SDO, CS3, SCK);
  
  // Initializing the MAX31856's registers
  temperature0->writeRegister(REGISTER_CR0, CR0_INIT);
  temperature0->writeRegister(REGISTER_CR1, CR1_INIT);
  temperature0->writeRegister(REGISTER_MASK, MASK_INIT);
  temperature1->writeRegister(REGISTER_CR0, CR0_INIT);
  temperature1->writeRegister(REGISTER_CR1, CR1_INIT);
  temperature1->writeRegister(REGISTER_MASK, MASK_INIT);
  temperature2->writeRegister(REGISTER_CR0, CR0_INIT);
  temperature2->writeRegister(REGISTER_CR1, CR1_INIT);
  temperature2->writeRegister(REGISTER_MASK, MASK_INIT);
  temperature3->writeRegister(REGISTER_CR0, CR0_INIT);
  temperature3->writeRegister(REGISTER_CR1, CR1_INIT);
  temperature3->writeRegister(REGISTER_MASK, MASK_INIT);
  
  // Wait for the first sample to be taken
  delay(200);

  //RoastTime.restart(0);
  //State = AmReadyToStart;
}

// ====
// LOOP
// ====
void loop () {

int newState;
newState =0;
float tHeat0, tBean1, tBean2, tFan;

int minuteToStart = 0;
   // Display the heat temperature
 
  tHeat0 = getTemperature(temperature0->readThermocouple(FAHRENHEIT));
  tBean1 = temperature1->readThermocouple(FAHRENHEIT);
  tBean2 = temperature2->readThermocouple(FAHRENHEIT);
  tFan = temperature3->readThermocouple(FAHRENHEIT);
  double tBeanAvg = getBeanAvgTemp(tBean1, tBean2);  

  double roastMinutes = ((double)RoastTime.elapsed())/60;
     
  boolean val = digitalRead(STARTME); 
  if (val == LOW)
  {
    digitalWrite(INT_LED, HIGH);
    if (State == AmReadyToStart)
    {
       newState = AmRoasting;  
       Serial.println("Start Detected!");
    }
  }
  else
  {
    digitalWrite(INT_LED, LOW);
    if (State != AmStopped && State != AmReadyToStart)
    {
       newState = AmStopped;
       Serial.println("Stop detected!");
    }
    else
    {
       State = AmReadyToStart;
    
    }
    
  }

  
 if (tHeat0 > OVERHEAT)
 {
     newState = AmOverheated;
     Serial.println("Overheat Detected!");
 }
 else if (tBeanAvg > TempEnd && State == AmRoasting)
 {
    newState = AmCooling;
    Serial.print("Roast Temp Reached. Cooling starting");
    Serial.println();
    
 }
 else if (tBeanAvg < 200 && State == AmCooling)
 {
    newState = AmStopped;
    Serial.print("Cooling Complete ");
    Serial.println();
    
 }
 else if (roastMinutes > TimeMax)
 {
    newState = AmCooling;
    Serial.print("Max time reached. Cooling starting");
 
    Serial.println();
    
 }
 
 
 if (newState == AmStopped)
 {
  //   RoastTime.stop(); 
     State = AmStopped; 
     digitalWrite(HTR1, HIGH);
     digitalWrite(PWRPID, HIGH); 
     digitalWrite(FAN, HIGH);
     digitalWrite(VIB, HIGH);
     delay(1000);
     newState = 0;
 }
 else if (newState == AmRoasting)
 { 
    State = AmRoasting;
    Serial.print("Starting Fans and Vibration - and waiting 5 seconds");
   
    Serial.println();
    digitalWrite(FAN, LOW);
    digitalWrite(VIB, LOW);
    
    delay(5000);
    Serial.print("Starting Heaters ");
    Serial.println();
    
    RoastTime.restart(minuteToStart * 60); 

    digitalWrite(HTR1, LOW);
    digitalWrite(PWRPID, LOW);
    newState = 0;
  }
 else if (newState == AmCooling)
 { 
    State = AmCooling;
    RoastTime.stop(); 
    digitalWrite(HTR1, HIGH);
    digitalWrite(PWRPID, HIGH);
    delay(1000);
    newState = 0;
  }
 else if (newState == AmOverheated)
 { 
    State = AmOverheated; 
    digitalWrite(HTR1, HIGH);
    digitalWrite(PWRPID, HIGH); 
    delay(1000);
    newState = 0;
  }
else if (State == AmOverheated )
 { 
    State = AmRoasting;
    digitalWrite(HTR1, LOW);
    digitalWrite(PWRPID, LOW);
   
 }


  
if (State == AmRoasting)

{

  Input = tBeanAvg;
  Setpoint =  calcSetpoint(roastMinutes);

  
  
  myPID.Compute();
  
    /***********************************************
  * turn the output pin on/off based on pid output
  ************************************************/
  
  unsigned long now = millis();
  if(now - WindowStartTime > WindowSize)
  { 
    //time to shift the Relay Window
    WindowStartTime += WindowSize;
  }
  
  if(Output > now - WindowStartTime) 
  {
    if (State == AmRoasting) 
    {
        digitalWrite(HTRPID, HIGH);
        
    }
    else
    {
       digitalWrite(HTRPID, LOW);
    }
    
  }
  else 
  { 
       digitalWrite(HTRPID, LOW);
       
  }
}
else
{
      digitalWrite(HTRPID, LOW);

}

   int duty = Output / WindowSize * 100;
   double err = Setpoint - tBeanAvg;
     
   
   printState(State);  
   Serial.print("Time: "); 
   if (roastMinutes < 10) Serial.print("0");
   Serial.print(roastMinutes);
   Serial.print (" (");
   Serial.print (TimeTarget);
   Serial.print(") Target: ");
   if (Setpoint < 100) Serial.print(" ");
   if (Setpoint < 10) Serial.print(" ");
   Serial.print(Setpoint, 0); 
   
   Serial.print("F beanT: ");
   if (tBeanAvg < 100) Serial.print(" ");
   if (tBeanAvg < 10) Serial.print(" ");
   Serial.print(tBeanAvg, 0);
   Serial.print (" (");
   Serial.print (TempEnd);
   Serial.print(") F Err: "); 
   if (err < 100 &&  err > -100) Serial.print(" ");
   if (err < 10 && err > -10) Serial.print(" ");
   if (err > 0) Serial.print (" ");
   Serial.print(err, 1);

   Serial.print("F (");
   if (tBean1 < 100) Serial.print(" ");
   if (tBean1 < 10) Serial.print(" ");
   Serial.print(tBean1, 0);
   Serial.print("F, "); 
   if (tBean2 < 100) Serial.print(" ");
   if (tBean2 < 10) Serial.print(" ");
   Serial.print(tBean2, 0);
   Serial.print("F)");  
   
   Serial.print(" HeatT: "); 
   if (tHeat0 < 100) Serial.print(" ");
   if (tHeat0 < 10) Serial.print(" ");
   Serial.print(tHeat0, 0);
   
   
   Serial.print(" FanT: "); 
   if (tFan < 100) Serial.print(" ");
   if (tFan < 10) Serial.print(" ");
   Serial.print(tFan, 0);

    
   Serial.print("F Duty: "); 
   if (duty < 100) Serial.print(" ");
   if (duty < 10) Serial.print(" ");
   Serial.print(duty);
   Serial.print("%"); 
   Serial.println();
}


// Print the temperature, or the type of fault
void PrT(double temperature) {
  switch ((int) temperature) {
    case FAULT_OPEN:
      Serial.print("OPEN");
      break;
    case FAULT_VOLTAGE:
      Serial.print("NC");
      break;
    case NO_MAX31856:
      Serial.print("FAULT");
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
      return -1;
      break;
    case FAULT_VOLTAGE:
      return -1;
      break;
    case NO_MAX31856:
      return -1;
      break;
    default:
      return temperature;
      break;
  }
}

double getBeanAvgTemp(double t1, double t2) {
  if (getTemperature(t1) != -1 && getTemperature(t2) != -1)
  return (t1 + t2) / 2;
  return -1;
}


double calcSetpoint(double roastminutes) {
// int MyFiveSetpoints[] = {100,300,350,430,460};
// int TempEnd = 440;
// int TimeEnd = 10;
     double setpoint = 0;
     double MinutesPerSpan = ((double)TimeTarget)/6;
     int spIndex = roastminutes/MinutesPerSpan;
     double r = roastminutes/MinutesPerSpan - spIndex;
    // Serial.print("MinutesPerSpan:");
   //  Serial.print(MinutesPerSpan);
   //  Serial.print (" spIndex:");
   //  Serial.print (spIndex);
     
  //   Serial.print (" r:");
  //   Serial.print (r);
        
     if (spIndex < 5)
     {
    //    Serial.print ("  ");
     //   Serial.print (MyFiveSetpoints[spIndex]);
     //   Serial.print ("+");
    //    Serial.print (((MyFiveSetpoints[spIndex + 1] - MyFiveSetpoints[spIndex]) * r));
    //    Serial.print ("  ");
    //    Serial.print (MyFiveSetpoints[spIndex + 1]);
    //    Serial.print ("  ");
        setpoint = MyFiveSetpoints[spIndex] + ((MyFiveSetpoints[spIndex + 1] - MyFiveSetpoints[spIndex]) * r);
     }
     else 
     {
         setpoint = MyFiveSetpoints[5];
      
     }
     return setpoint;
}


 void printState(int state) {
  switch (state) {
    case AmRoasting:
      Serial.print ("AmRoasting ");
      break;
    case AmStopped:
      Serial.print( "AmStopped  ");
      break;
    case AmCooling:
      Serial.print ("AmCooling  ");
      break;
    case AmOverheated:
      Serial.print ("To Hot     ");
      break;
    case AmReadyToStart:
      Serial.print ("AmReadyTo ");
      break;  
    default:
      Serial.print ("unknown   ");
      break;
  }
}


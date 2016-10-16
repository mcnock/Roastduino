
// Roastduino v1.0
// 2016.09.20

// This is sample Arduino code for the Maxim MAX31856 thermocouple IC
// http://datasheets.maximintegrated.com/en/ds/MAX31856.pdf
//
// Library Implementation Details
// ==============================
// DRDY and FAULT lines are not used in this driver. DRDY is useful for low-power mode so samples are only taken when
// needed; this driver assumes power isn't an issue.  The FAULT line can be used to generate an interrupt in the host
// processor when a fault occurs.  This library reads the fault register every time a reading is taken, and will
// return a fault error if there is one.  The MAX31856 has sophisticated usage scenarios involving FAULT.  For
// example, low and high temperature limits can be set, and the FAULT line triggers when these temperatures are
// breached. This is beyond the scope of this sample library.  The assumption is that most applications will be
// polling for temperature readings - but it is good to know these features are supported by the hardware.
//
// The MAX31856 differs from earlier thermocouple IC's in that it has registers that must be configured before
// readings can be taken.  This makes it very flexible and powerful, but one concern is power loss to the IC.  The IC
// should be as close to the cold junction as possible, which might mean there is a cable connecting the breakout
// board to the host processor.  If this cable is disconnected and reconnected (MAX31856 loses power) then the
// registers must be reinitialized.  This library detects this condition and will automatically reconfigure the
// registers.  This simplifies the software running on the host.
//
// A lot of configuration options appear in the .H file.  Of particular note is the line frequency filtering, which
// defaults to 60Hz (USA and others).  If your line voltage is 50Hz you should set CR0_NOISE_FILTER_50HZ.
//
// This library handles the full range of temperatures, including negative temperatures.
//
// When connecting the thermocouple, remember the 2 wires are polarized.  If temperatures go up when you expect
// them to go down just reverse the wires.  No damage will be done to the MAX31856.
//
// Change History:
// 2016.09.20        Initial Version

// =========
// LIBRARIES
// =========
#include "MAX31856.h"
#include "PID_v1.h"
#include <Chrono.h>

// ===============
// PIN DEFINITIONS
// ===============
// The power requirement for the board is less than 2mA.  Most microcontrollers can source or sink a lot more
// than that one each I/O pin.  For example, the ATmega328 supports up to 20mA.  So it is possible to power the
// board using I/O pins for power - so you can turn the board on and off (if you want to).
// FAULT and DRDY are not used by the library (see above)

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
boolean toggle;
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

PID myPID(&Input, &Output, &Setpoint, 50, 0, 0, DIRECT);

//                          000,001,002,003,004,005,006,007,008,009,010,011,012,013,014,015,016,017,018,019,020
 int MyMinuteSetpoints[] = {100,205,215,220,230,240,250,272,275,280,292,300,310,320,340,380,430,460,460,460,460};
 int TempEnd = 440;
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
  digitalWrite(FAN, LOW);
  digitalWrite(VIB, LOW);
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

  double totalseconds = RoastTime.elapsed();
  int minutes =  int( totalseconds / 60);
  int seconds  = (totalseconds - (minutes * 60));
     
  boolean val = digitalRead(STARTME); 
  if (val == LOW)
  {
    if (State != AmRoasting )
    {
       newState = AmRoasting;  
       Serial.println("Start Detected!");
    }
  }
  else
  {
    if (State != AmStopped)
    {
       newState = AmStopped;
       Serial.println("Stop detected!");
    }
  }

  
 if (tHeat0 > OVERHEAT)
 {
     newState = AmOverheated;
     Serial.println("Overheat Detected!");
 }
 else if (tBeanAvg > TempEnd)
 {
    newState = AmCooling;
    Serial.print("Roast done. Cooling starting");
    Serial.println();
 }
 else if (tBeanAvg < 200 && State == AmCooling)
 {
    newState = AmStopped;
    Serial.print("Cooling Complete ");
    Serial.println();
 }
 

 
 if (newState == AmStopped)
 {
     RoastTime.stop(); 
     State = AmStopped; 
     digitalWrite(HTR1, HIGH);
     digitalWrite(PWRPID, HIGH); 
     newState = 0;
 }
 else if (newState == AmRoasting)
 { 
    State = AmRoasting;
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
    newState = 0;
  }
 else if (newState == AmOverheated)
 { 
    State = AmOverheated; 
    digitalWrite(HTR1, HIGH);
    digitalWrite(PWRPID, HIGH); 
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
  myPID.Compute();
  Setpoint = MyMinuteSetpoints[minutes];
  
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

   Serial.print (getState(State));  
   Serial.print("Time: "); 
   if (minutes < 10) Serial.print("0");
   Serial.print(minutes);
   Serial.print(":"); 
   if (seconds < 10) Serial.print("0");
   Serial.print(seconds);

   Serial.print(" Target: ");
   if (Setpoint < 100) Serial.print(" ");
   if (Setpoint < 10) Serial.print(" ");
   Serial.print(Setpoint, 0); 
   
   Serial.print("F beanT: ");
   if (tBeanAvg < 100) Serial.print(" ");
   if (tBeanAvg < 10) Serial.print(" ");
   Serial.print(tBeanAvg, 0);
   
   Serial.print("F Err: "); 
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


char getState(int state) {
  switch (state) {
    case AmRoasting:
      return "Roasting ";
      break;
    case AmStopped:
      return "Stopped  ";
      break;
    case AmCooling:
      return "Cooling  ";
      break;
    case AmOverheated:
      return "To Hot   ";
      break;
    default:
      return " not ";
      break;
  }
}


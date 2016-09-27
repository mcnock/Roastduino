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
#include <MAX31856.h>
#include <PID_v1.h>

// ===============
// PIN DEFINITIONS
// ===============
// The power requirement for the board is less than 2mA.  Most microcontrollers can source or sink a lot more
// than that one each I/O pin.  For example, the ATmega328 supports up to 20mA.  So it is possible to power the
// board using I/O pins for power - so you can turn the board on and off (if you want to).
// FAULT and DRDY are not used by the library (see above)
#define TPWR   5
#define SCK    12
#define SDI    11
#define CS3    10
#define CS2    9
#define CS1    8
#define CS0    7
#define SDO    6

#define LED    13
#define HTR    3

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
MAX31856 *temperature0;
MAX31856 *temperature1;
MAX31856 *temperature2;
MAX31856 *temperature3;
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, 60, 3, 2, DIRECT);
int WindowSize = 1000;
unsigned long windowStartTime;

// =====
// SETUP
// =====
void setup() {
  // PID Setup
  windowStartTime = millis();
  myPID.SetOutputLimits(0, WindowSize);
  Setpoint = 200;
  myPID.SetMode(AUTOMATIC);
  
  // Display temperatures using the serial port
  Serial.begin(9600);
  delay(3000);
  Serial.println("Roastduino Console");
  Serial.println("v1.0 2016.09.20");

  // Pin Configuration
  pinMode(TPWR, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(HTR, OUTPUT);

  // Power up MAX31856
  digitalWrite(TPWR, HIGH);

  // Wait for MAX31856 to initialize
  delay(200);
  
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
  float t0, t1, t2, t3, tj;
  
  // Display the thermocouple temperature
  t0 = temperature0->readThermocouple(CELSIUS);
  Serial.print("T0: ");
  printTemperature(t0);
  Serial.print(", ");
  
  // Display the thermocouple temperature
  t1 = temperature1->readThermocouple(CELSIUS);
  Serial.print("T1: ");
  printTemperature(t1);
  Serial.print(", ");
  
  // Display the thermocouple temperature
  t2 = temperature2->readThermocouple(CELSIUS);
  Serial.print("T2: ");
  printTemperature(t2);
  Serial.print(", ");
  
//  // Display the thermocouple temperature
//  t3 = temperature3->readThermocouple(CELSIUS);
//  Serial.print("T3: ");
//  printTemperature(t3);
//  Serial.print(" ");
//  
//  // Display the junction (IC) temperature
//  tj = temperature0->readJunction(CELSIUS);
//  Serial.print("(jT: ");
//  printTemperature(tj);
//  Serial.print("), ");

  Input = t0;
  myPID.Compute();

  double beanTemperature = getBeanTemperature(t1, t2);  
  
  // Display the bean temperature
  Serial.print("Beans: ");
  Serial.print(beanTemperature);      
  Serial.write(176);
  Serial.print("C");
  Serial.print(", ");

  if (beanTemperature > 231 && Setpoint > 0)
    Setpoint = 0;
  else if (beanTemperature > 200 && Setpoint > 0)
    Setpoint = 340;
  else if (beanTemperature > 180 && Setpoint > 0)
    Setpoint = 300;
  else if (beanTemperature > 150 && Setpoint > 0)
    Setpoint = 280;
  
  Serial.print("Setpoint: ");
  Serial.print(Setpoint);
  Serial.write(176);
  Serial.print("C (");
  Serial.print(Output / WindowSize * 100);
  Serial.print("% duty) ");

  /************************************************
  * turn the output pin on/off based on pid output
  ************************************************/
  unsigned long now = millis();
  if(now - windowStartTime > WindowSize)
  { 
    //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  
  if(Output > now - windowStartTime) 
  {
    digitalWrite(LED, HIGH);
    digitalWrite(HTR, HIGH);
    Serial.print("ON");
  }
  else 
  { 
    digitalWrite(LED, LOW);
    digitalWrite(HTR, LOW);
    Serial.print("OFF");
  }
  
  Serial.println();
}

// Print the temperature, or the type of fault
void printTemperature(double temperature) {
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

double getBeanTemperature(double t1, double t2) {
  if (getTemperature(t1) != -1 && getTemperature(t2) != -1)
    return (t1 + t2) / 2;
  return -1;
}





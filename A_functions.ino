

//----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------


float FlowErrorLast = 0;

unsigned long FlowErrorLastTime = 0;

int GetHistoryIDfromLineID(int lineID) {
  for (int i = 0; i < GraphHistorySize; i++) {
    if (GraphHistory[i].LineID == lineID) {
      return i;
    }
  }
  return -1;
}

int round5(int n) {
  return ((n + 4) / 5) * 5;
}

float CalcflowsetpointForATime(double minutes) {
  //SPDEBUG("Starting CalcFanPWM");
  double calcedflowsetpoint;
  float ratio;
  if (minutes <= FlowSetPoints[1].Minutes) {
    ratio = (float)(minutes) / (float)(FlowSetPoints[1].Minutes);
    calcedflowsetpoint = FlowSetPoints[0].flow + (ratio * (FlowSetPoints[1].flow - FlowSetPoints[0].flow));
    //SPDEBUG("CaclPMW < sp1 mins: " + String(minutes) + " ratio:" + String(ratio) +  " pwm:" + String(calcedFanSpeed));
  } else if (minutes < FlowSetPoints[2].Minutes) {
    ratio = (float)(minutes - FlowSetPoints[1].Minutes) / (float)(FlowSetPoints[2].Minutes - FlowSetPoints[1].Minutes);
    calcedflowsetpoint = FlowSetPoints[1].flow + (ratio * (FlowSetPoints[2].flow - FlowSetPoints[1].flow));
    //SPDEBUG("CaclPMW < sp2 mins: " + String(minutes) + " ratio:" + String(ratio) +  " pwm:" + String(calcedFanSpeed));
  } else if (minutes <= FlowSetPoints[3].Minutes)  //this should roast end
  {
    ratio = (float)(minutes - FlowSetPoints[2].Minutes) / (float)(FlowSetPoints[3].Minutes - FlowSetPoints[2].Minutes);
    calcedflowsetpoint = FlowSetPoints[2].flow + (ratio * (FlowSetPoints[3].flow - FlowSetPoints[2].flow));
    //SPDEBUG("CaclPMW < sp3  mins: " + String(minutes) + " ratio:" + String(ratio) +  " pwm:" + String(calcedFanSpeed));
  } else {
    //this should be paste roaste - ie cooling
    if (State = STATECOOLING) {
      calcedflowsetpoint = FlowSetPoints[3].flow * FanCoolingBoostPercent;
    } else {
      calcedflowsetpoint = FlowSetPoints[3].flow;
    }
    //SPDEBUG("CaclPMW > 3   mins: " + String(minutes) +  " pwm:" + String(calcedFanSpeed));
  }
  return calcedflowsetpoint;
}

uint16_t XforATime(float time) {

  uint16_t result = 0;
  if (time <= timeSplitLow) {
    result = time * PixelsPerMinL;
    return result;
  } else {
    result = (timeSplitLow * PixelsPerMinL) + ((time - timeSplitLow) * PixelsPerMinM);
  }
  return result;
}

void SetFanOff() {
  DutyFan = 0;
  sendFan_D_Wire();
}

void CalcStartingIntegralSumFlow() {

  //double BeanYflowsetpoint = CalcflowsetpointForATime(0);

  IntegralSumFlow = _DutyStartingFlow * GainFlow / IntegralFlow;


  //DutyFan = (errorcombined / (double)GainFlow);
  //errorcombined = ErrFlow + ErrIFlow
  //ErrFlow = BeanYflowsetpoint - Lastflowvalueforpid;
}
void SetFanFromOpticalSensorPID() {
  Lastflowvalueforpid = sq(BeanYflow_avg.mean());
  BeanYflowsetpoint = CalcflowsetpointForATime(RoastMinutes);
  ErrFlow = BeanYflowsetpoint - Lastflowvalueforpid;
  if (PIDIntegralUdateTimeFlow.elapsed() > PIDIntegralUdateTimeValueFlow) {  //every x seconds we add the err to be a sum of err scaled by integral flow factor                                                                          //if (ErrIFlow < GainFlow) {
    IntegralSumFlow = IntegralSumFlow + ErrFlow;
    if (IntegralSumFlow < 0) {
      //IntegralSumFlow = 0;
    }
    ErrIFlow = (IntegralSumFlow * IntegralFlow);  //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
    PIDIntegralUdateTimeFlow.restart(0);
  }
  double errorcombined = ErrFlow + ErrIFlow;
  double elapsedmillseconds = millis() - FlowErrorLastTime;
  float changes = errorcombined - FlowErrorLast;
  float changesperseconds = changes * (float)(1000 / elapsedmillseconds);
  float percentchange = changesperseconds / BeanYflowsetpoint;
  float requestedchanges = changes;
  if (MaxPercentChangePerSecondFlow > 0)
    if (FlowErrorLastTime > 0) {
      if (requestedchanges > MaxPercentChangePerSecondFlow) {
        changes = MaxPercentChangePerSecondFlow;
        errorcombined = FlowErrorLast + MaxPercentChangePerSecondFlow;
      } else if (requestedchanges < -MaxPercentChangePerSecondFlow) {
        changes = -MaxPercentChangePerSecondFlow;
        errorcombined = FlowErrorLast - MaxPercentChangePerSecondFlow;
      }
    }
  FlowErrorLast = errorcombined;
  FlowErrorLastTime = millis();
  DutyFanCalced = (errorcombined / (double)GainFlow);
  if (DutyFanCalced > 1.0) {
    DutyFanCalced = 1.0;
  }
  if (FanManual == false) {
    DutyFan = DutyFanCalced;
  }

  if (Debugbyte == FLOWPIDINFO_10) {
    SERIALPRINT_DB("FlowSetpoint:");
    SERIALPRINT_DB(BeanYflowsetpoint);
    SERIALPRINT_DB(",AvgSq:");
    SERIALPRINT_DB(Lastflowvalueforpid);
    SERIALPRINT_DB(",Err:");
    SERIALPRINT_DB(ErrFlow);
    SERIALPRINT_DB(",ErrI:");
    SERIALPRINT_DB(ErrIFlow);
    SERIALPRINT_DB(",changes:");
    SERIALPRINT_DB(changes);
    SERIALPRINT_DB(",ISF:");
    SERIALPRINT_DB(IntegralSumFlow);
    SERIALPRINT_DB(",DutyFanCalced:");
    SERIALPRINT_DB(DutyFanCalced);
    SERIALPRINT_DB(",DutyFan:");
    SERIALPRINTLN_DB(DutyFan);
  }

  sendFan_D_Wire();
}

void sendFan_D_Wire() {
  float DACMax = 4092;
  int valuefordac = DutyFan * DACMax;
  double start = millis();
  Wire.beginTransmission(MCP4725_ADDR);
  Wire.write(64);                       // cmd to update the DAC
  Wire.write(valuefordac >> 4);         // the 8 most significant bits...
  Wire.write((valuefordac & 15) << 4);  // the 4 least significant bits...
  Wire.endTransmission();

  //SPDEBUG("updated fanDAC.  Elapsed:" + String(millis() - start) + " value:" + String(valuefordac));
}

uint16_t YforATemp(double temp) {
  uint16_t result = 0;
  if (temp < 0.0) {
    temp = 0.0;
  }
  if (temp <= TempSplitLow) {
    result = (myGLCD.getDisplayYSize() - ((double)temp / TempPerPixL));
    //Serial.print("low t:");
  } else if (temp <= TempSplitHigh) {
    //Serial.print("mid t:");
    result = double(myGLCD.getDisplayYSize() - PixelYSplitLow) - ((double)(temp - TempSplitLow) / (double)TempPerPixM);
  } else {
    //Serial.print("high t:");
    result = double(myGLCD.getDisplayYSize() - PixelY_SplitHigh) + ((double)(TempSplitHigh - temp) / (double)TempPerPixH);
  }
  //Serial.print (temp);
  //Serial.print ( "y:");
  //Serial.println(result);
  if (result < 0) {
    return 1;
  } else {
    return result;
  }
}

int getCleanTemp(int myID) {

  byte tries = 0;
  do {
    int temperature = thermocouples[myID].sensor.readFahrenheit();
    if (isnan(temperature)) {
      thermocouples[myID].Readingskipped++;
      //Serial.print (myID);Serial.print ("nan temp:");Serial.println(temperature);
    } else if (temperature > 1000) {
      thermocouples[myID].Readingskipped++;
      //Serial.print (myID);Serial.print ("too high temp:");Serial.println(temperature);
    } else if (temperature < 40) {
      thermocouples[myID].Readingskipped++;
      //Serial.print (myID);Serial.print ("too low temp:");Serial.println(temperature);
    } else {
      //SPDEBUG(String(myID) + " clean temp returned:" + String(temperature));
      return temperature;
    }
    tries++;
  } while (tries < 1);

  return -1;
}

float getCleanOpticaFlow(int mySensorID) {
  int16_t r;
  double root;
  byte tries = 0;
  //do {
  tries++;
  if (BeanOpticalFlowSensors[mySensorID].sensor.Initialized == false) {
    SERIALPRINT_DB(F("During run:Initializing flow sensor id:"));
    SERIALPRINTLN_DB(mySensorID);
    if (!BeanOpticalFlowSensors[mySensorID].sensor.Initialize()) {
      SERIALPRINT_DB(F("Initialization of the flow sensor failed :"));
      SERIALPRINTLN_DB(mySensorID);

    } else {
      SERIALPRINTLN_DB(F(" Success"));
    }
  }
  BeanOpticalFlowSensors[mySensorID].sensor.readMotionCountY(&r);
  if (r < 1000) {
    if (r > 0) {
      root = sqrt(float(r));
    } else {
      root = -sqrt(float(abs(r)));
    }
  } else {
    root = -999;
    BeanOpticalFlowSensors[mySensorID].YflowReadingskipped++;
  }
  if (Debugbyte == FLOWSENSORDATARAW_11) {
    SERIALPRINT_DB(F(",F"));
    SERIALPRINT_DB(mySensorID);
    SERIALPRINT_DB(F("raw:"));
    SERIALPRINT_DB(r);
    SERIALPRINT_DB(F(",F"));
    SERIALPRINT_DB(mySensorID);
    SERIALPRINT_DB(F("sqrt:"));
    SERIALPRINT_DB(root);
    if (root < UpFlowThreshold) {
      SERIALPRINT_DB(F("upflow:0"));
    } else {
      SERIALPRINT_DB(F("upflow:1"));
    }

  }  // note the println comes in AA_LOOP
  return root;
}

double getBeanAvgTemp(double t1, double t2) {
  if (t1 != -1 && t2 != -1) {
    return (t1 + t2) / 2;
  } else {
    if (t1 != -1) {
      return t1;
    } else {
      return t2;
    }
  }
}

double SetpointforATime(double roastminutes) {
  int setpoint;
  double r = (roastminutes - (int)roastminutes);
  setpoint = MyMinuteTemperature[(int)roastminutes] + ((double)(MyMinuteTemperature[int(roastminutes) + 1] - MyMinuteTemperature[(int)roastminutes]) * r);
  if (setpoint > 0) {
    return setpoint;
  }

  else {
    return 0;
  }
}

int RangeAint(int Value, int H, int L) {
  if (Value > H) {
    return H;
  } else if (Value < L) {
    return L;
  } else {

    return Value;
  }
}

int RangeAint_L_H(int Value, int L, int H) {
  if (Value > H) {
    return H;
  } else if (Value < L) {
    return L;
  } else {

    return Value;
  }
}

double RangeAdouble(double Value, double L, double H) {
  if (Value > H) {
    return H;
  } else if (Value < L) {
    return L;
  } else {
    return Value;
  }
}

void analogWriteIfDif(){

  
}

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else   // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

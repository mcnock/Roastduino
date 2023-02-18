

//----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------


float FlowErrorCombinedLast = 0;

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
  int flowspan = -1;
  if (minutes <= FlowSetPoints[1].Minutes) {
    flowspan = 0;
    FlowSetPointSpandProgressRatio = (float)(minutes) / (float)(FlowSetPoints[1].Minutes);
    calcedflowsetpoint = FlowSetPoints[0].flow + (FlowSetPointSpandProgressRatio * (FlowSetPoints[1].flow - FlowSetPoints[0].flow));
    //SPDEBUG("CaclPMW < sp1 mins: " + String(minutes) + " ratio:" + String(ratio) +  " pwm:" + String(calcedFanSpeed));
  } else if (minutes < FlowSetPoints[2].Minutes) {
    flowspan = 1;
    FlowSetPointSpandProgressRatio = (float)(minutes - FlowSetPoints[1].Minutes) / (float)(FlowSetPoints[2].Minutes - FlowSetPoints[1].Minutes);
    calcedflowsetpoint = FlowSetPoints[1].flow + (FlowSetPointSpandProgressRatio * (FlowSetPoints[2].flow - FlowSetPoints[1].flow));
    //SPDEBUG("CaclPMW < sp2 mins: " + String(minutes) + " ratio:" + String(ratio) +  " pwm:" + String(calcedFanSpeed));
  } else if (minutes <= FlowSetPoints[3].Minutes)  //this should roast end
  {
    flowspan = 2;
    FlowSetPointSpandProgressRatio = (float)(minutes - FlowSetPoints[2].Minutes) / (float)(FlowSetPoints[3].Minutes - FlowSetPoints[2].Minutes);
    calcedflowsetpoint = FlowSetPoints[2].flow + (FlowSetPointSpandProgressRatio * (FlowSetPoints[3].flow - FlowSetPoints[2].flow));
    //SPDEBUG("CaclPMW < sp3  mins: " + String(minutes) + " ratio:" + String(ratio) +  " pwm:" + String(calcedFanSpeed));
  } else {
    //this should be paste roaste - ie cooling
    if (State == STATECOOLING) {
      calcedflowsetpoint = FlowSetPoints[3].flow * FanCoolingBoostPercent;



    } else {
      flowspan = 3;
      calcedflowsetpoint = FlowSetPoints[3].flow;
    }
    //SPDEBUG("CaclPMW > 3   mins: " + String(minutes) +  " pwm:" + String(calcedFanSpeed));
  }
  //if (State == STATEROASTING) { //we initialize the span section while roasting to allow changing setpoint
  if (FlowCurrentSetpointSpan != flowspan) {
    FlowCurrentSetpointSpan = flowspan;
    IntegralSumFlowBySpan[FlowCurrentSetpointSpan] = IntegralSumFlow;
  }
  //}
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
  float errflowraw = BeanYflowsetpoint - Lastflowvalueforpid;
  float errflowadjusted;
  if (FlowSensorErrorUseSqrt == 1) { //take the sqrt of the error to minimize the impacts of large deviations
    errflowadjusted = sqrt(errflowraw);
  } else {
    errflowadjusted = errflowraw;
  }
  boolean updatingintegralsums = false;
  if (PIDIntegralUdateTimeFlow.elapsed() > PIDIntegralUdateTimeValueFlow) {  //every x seconds we add the err to be a sum of err scaled by integral flow factor                                                                          //if (ErrIFlow < GainFlow) {
    updatingintegralsums = true;
  }
  if (MaxPercentChangePerSecondFlow > 0 && FlowErrorLastTime > 0) { //we do this to limit the change that can occure per second
    float integralsumflow; 
    double elapsedmillseconds = millis() - FlowErrorLastTime;
    float changesallowed = (elapsedmillseconds / 1000.00) * (float)MaxPercentChangePerSecondFlow * (float)BeanYflowsetpoint;
    float errflow_maxallowed = 0;
    if (updatingintegralsums == true) {  //every x seconds we add the err to be a sum of err scaled by integral flow factor                                                                          //if (ErrIFlow < GainFlow) {
      integralsumflow = IntegralSumFlow + errflowadjusted;
      errflow_maxallowed = (changesallowed - (integralsumflow * IntegralFlow)) / (IntegralFlow + 1);
    } else {
      errflow_maxallowed = changesallowed - (IntegralSumFlow * IntegralFlow);
    }

    //Complex
    //changes =  ((IntegralSumFlow + errflow) * IntegralFlow) + errflow
    //        =  IntegralFlow * IntegralSumFlow + (IntegralFlow * errflow) + errflow
    // C = AB + Ax + x ==>   C = AB + x (A + 1)  ==>  x(A + 1) = C - AB ==>  x = (C-AB)/(A + 1)
    //errflow = (changes - (IntegralFlow * IntegralSumFlow))/(IntegralFlow + 1)
    //maxerroflow = (changesallowed - (IntegralSumFlow * IntegralFlow )) / (IntegralFlow + 1)


    //simple
    //changes = (IntegralSumFlow * IntegralFlow) + errflow
    //errflow = change - (IntegralSumFlow * IntegralFlow)
    //max errflow = (changesallowed - (IntegralSumFlow * IntegralFlow))

    if (errflowadjusted > errflow_maxallowed) {
      errflowadjusted = errflow_maxallowed;
    } else if (errflowadjusted < -errflow_maxallowed) {
      errflowadjusted = -errflow_maxallowed;
    }
    ErrFlow = errflowadjusted;
  } else {
    ErrFlow = errflowadjusted;
  }
  
  if (updatingintegralsums == true) {  //every x seconds we add the err to be a sum of err scaled by integral flow factor                                                                          //if (ErrIFlow < GainFlow) {
    IntegralSumFlow = IntegralSumFlow + ErrFlow;
    IntegralSumFlowBySpan[FlowCurrentSetpointSpan] = IntegralSumFlowBySpan[FlowCurrentSetpointSpan] + ErrFlow;
    ErrIFlow = (IntegralSumFlow * IntegralFlow);  //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
    PIDIntegralUdateTimeFlow.restart(0);
  }
  
  double errorcombined = ErrFlow + ErrIFlow;
  float changes = errorcombined - FlowErrorCombinedLast;
  FlowErrorCombinedLast = errorcombined;
  FlowErrorLastTime = millis();
  DutyFanCalced = (errorcombined / (double)GainFlow);
  if (DutyFanCalced > 1.0) {
    DutyFanCalced = 1.0;
  }
  if (FanManual == false) {
    DutyFan = DutyFanCalced;
  }

  if (Debugbyte == FLOWPIDINFO_10) {
    SERIALPRINT_DB("FlowSetpoint:");  //1
    SERIALPRINT_DB(BeanYflowsetpoint);
    SERIALPRINT_DB(",flowavg:");  //2
    SERIALPRINT_DB(Lastflowvalueforpid);
    SERIALPRINT_DB(",errflowraw:");  //3
    SERIALPRINT_DB(errflowraw);
    SERIALPRINT_DB(",Err:");  //4
    SERIALPRINT_DB(ErrFlow);
    SERIALPRINT_DB(",ErrI:");  //5
    SERIALPRINT_DB(ErrIFlow);
    SERIALPRINT_DB(",changes:");  //6
    SERIALPRINT_DB(changes);
    SERIALPRINT_DB(",ISF:");  //7
    SERIALPRINT_DB(IntegralSumFlow);
    SERIALPRINT_DB(",DutyFan:");  //8
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
  int16_t r2;
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
  r2 = r;
  if (r != -999) {
    BeanOpticalFlowSensors[mySensorID].error = 0;

    if (r > 0) {
      if (r > _MixMax) {
        r = _MixMax;
        BeanOpticalFlowSensors[mySensorID].YflowReadingskipped++;
      }
      root = sqrt(float(r));
    } else {
      r = -r;
      if (r > _MixMax) {
        r = _MixMax;
        BeanOpticalFlowSensors[mySensorID].YflowReadingskipped++;
      }
      root = -sqrt(float(r));
    }
  } else {
    BeanOpticalFlowSensors[mySensorID].error = r;
    BeanOpticalFlowSensors[mySensorID].YflowReadingskipped++;
  }
  if (root != FLOWREADINGERROR) {
    BeanYflowX_avg[mySensorID].push(root);
  }
  if (Debugbyte == FLOWSENSORDATARAW_11) {
    SERIALPRINT_DB(F(",F"));
    SERIALPRINT_DB(mySensorID);
    SERIALPRINT_DB(F("raw:"));
    SERIALPRINT_DB(r2);  //1 & 2
    SERIALPRINT_DB(F(",F"));
    SERIALPRINT_DB(mySensorID);
    SERIALPRINT_DB(F("sqrt:"));
    SERIALPRINT_DB(root);  //3 & 4
    SERIALPRINT_DB(F(",F"));
    SERIALPRINT_DB(mySensorID);
    SERIALPRINT_DB(F("avgsqrt:"));  //5 & 6
    SERIALPRINT_DB(BeanYflowX_avg[mySensorID].mean());
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

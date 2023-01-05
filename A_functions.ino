

//----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------


float DutyFanLast = 0;

int GetHistoryIDfromLineID(int lineID) {
  for (int i = 0; i < 2; i++) {
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
  int calcedflowsetpoint;
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

int XforAFanMin(double minute) {
  return (FanGraphXStart + (PixelsPerFlowRate * minute));
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

int YforADuty(int fanpwm) {
  //Serial.print("starting YforAFlow");Serial.println(fanpwm);
  if (fanpwm <= FlowSetPointMin) {
    //Serial.print("return min:"); Serial.println(FanGraphYBottom);
    return FlowSetPointGraphBottompx;
  } else {
    double ratio = (fanpwm - FlowSetPointMin) / float(FlowSetPointMax - FlowSetPointMin);
    //Serial.print("ratio:");Serial.println(ratio);
    if (ratio > 1) { ratio = 1; }
    return (FlowSetPointGraphBottompx - (ratio * FlowSetPointGraphHeightpx));
  }
}

int YforAFlow(int flowrate) {
  //Serial.print("starting YforAFlow");Serial.println(fanpwm);
  if (flowrate <= FlowSetPointMin) {
    //Serial.print("return min:"); Serial.println(FanGraphYBottom);
    return FlowSetPointGraphBottompx;
  } else {
    double ratio = (flowrate - FlowSetPointMin) / float(FlowSetPointMax - FlowSetPointMin);
    //Serial.print("ratio:");Serial.println(ratio);
    if (ratio > 1) { ratio = 1; }
    return (FlowSetPointGraphBottompx - (ratio * FlowSetPointGraphHeightpx));
  }
}

void SetFanFromOpticalSensorPID() {

  if (FanManual == false) {
    float value;
    value = BeanYflow_avg.mean();
    BeanYflowsetpoint = CalcflowsetpointForATime(RoastMinutes);
    ErrFlow = BeanYflowsetpoint - value;

    if (PIDIntegralUdateTimeFlow.elapsed() > PIDIntegralUdateTimeValueFlow) {  //every 5 seconds we add the err to be a sum of err
                                                                               //if (ErrIFlow < GainFlow) {
      IntegralSumFlow = IntegralSumFlow + ErrFlow;
      if (IntegralSumFlow < 0) {
        IntegralSumFlow = 0;
      }

      ErrIFlow = (IntegralSumFlow * IntegralFlow / 10);  //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
      //Serial.println("Isum:");Serial.println(IntegralSum);Serial.println("ErrI:");Serial.println(ErrI);
      PIDIntegralUdateTimeFlow.restart(0);
      //}
      //IntegralSumFlow = 0;
    }
    float DutyFanTemp = ((ErrFlow + ErrIFlow) / (double)GainFlow);
    float PercentChange = 0;
    if (DutyFanLast > 0) {
      PercentChange = (DutyFanTemp - DutyFanLast) / DutyFanLast;
    }
    if (abs(PercentChange) > (MaxPercentChangeFlow / 10)) {

      if (DutyFanTemp > DutyFanLast) {
        DutyFan = DutyFanLast * (1 + MaxPercentChangeFlow / 10);
      } else {
        DutyFan = DutyFanLast * (1 - MaxPercentChangeFlow / 10);
      }
    } else {
      DutyFan = DutyFanTemp;
    }
    if (DutyFan > 1.0) {
      DutyFan = 1.0;
    }
    DutyFanLast = DutyFan;
    SPDEBUG1("DeltaY:" + String(BeanYflow) + ",mean:" + String(value) + ",Err:" + String(ErrFlow) + ", %:" + String(PercentChange) + ",Duty:" + String(DutyFan) + ",intSum:" + String(IntegralSumFlow) + ",setpoint:" + String(BeanYflowsetpoint) + ",Gain:" + String(GainFlow) + ",integral:" + String(IntegralFlow));
    SPDEBUGXCLOSE;
    SPDEBUG2("DeltaY:" + String(BeanYflow) + ",mean:" + String(value) + ",Err:" + String(ErrFlow) + ", %:" + String(PercentChange) + ",Duty:" + String(DutyFan) + ",intSum:" + String(IntegralSumFlow) + ",setpoint:" + String(BeanYflowsetpoint) + ",Gain:" + String(GainFlow) + ",integral:" + String(IntegralFlow));
    SPDEBUGXCLOSE;
  }
  sendFan_D_Wire();
}

void SetAndSendFanPWMForATime(double minutes) {
  //Serial.println("Call From SetFan");
  //Serial.println(minutes);
  //FanSpeed254PWM = CalcFan254PWMForATime(minutes);
  //sendFan_Wire();
}

void StopAndSendFanPWM() {
  FanSpeed254PWM = 0;
  DutyFan = 0;
  sendFan_Wire();
}

void sendFan_Wire() {
  float DACMax = 4092;
  int valuefordac;
  float x = FanSpeed254PWM / 254;
  valuefordac = DACMax * x;
  double start = millis();
  Wire.beginTransmission(MCP4725_ADDR);
  Wire.write(64);                       // cmd to update the DAC
  Wire.write(valuefordac >> 4);         // the 8 most significant bits...
  Wire.write((valuefordac & 15) << 4);  // the 4 least significant bits...
  Wire.endTransmission();
  //SPDEBUG("updated fanDAC.  Elapsed:" + String(millis() - start) + " value:" + String(valuefordac));
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

void ReturnSetPoints(Stream& port) {
  for (int x = 0; x < SetPointCount; x++) {
    if (x > 0) {
      port.print("!");
    }
    port.print(x);
    port.print(":");
    port.print(MySetPoints[x].Minutes);
    port.print(":");
    port.print(MySetPoints[x].Temperature);
  }
  port.println("");
}

void Serial_println_rect(struct rect& rect) {
  Serial.print("x:");
  Serial.print(rect.x);
  Serial.print(" y:");
  Serial.print(rect.y);
  Serial.print(" xmax:");
  Serial.print(rect.xmax);
  Serial.print(" ymax:");
  Serial.println(rect.ymax);
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

int getCleanOpticaFlow(int myID) {
  int16_t r;
  byte tries = 0;
  //do {
  tries++;
  if (BeanOpticalFlowSensors[myID].sensor.Initialized == false) {
    Serial.println("Initializing flow sensor");
    if (!BeanOpticalFlowSensors[myID].sensor.Initialize()) {
      Serial.println("Initialization of the flow sensor failed");
    } else {
      Serial.println(" Success");
    }
  }
  BeanOpticalFlowSensors[myID].sensor.readMotionCountY(&r);
  if (r < 300) {
    if (r > -1) {
      return r;
    }
  } else {
    BeanOpticalFlowSensors[myID].YflowReadingskipped++;
  }
  return -1;
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

void saveChangedSetpoints() {

  for (int xSetPoint = 0; xSetPoint < 6; xSetPoint++) {
    if (MySetPoints[xSetPoint].TemperatureNew != 0) {
      MySetPoints[xSetPoint].Temperature = MySetPoints[xSetPoint].TemperatureNew;
      MySetPoints[xSetPoint].TemperatureNew = 0;

      EEPROM.put(SetpointTemp_EP[xSetPoint], MySetPoints[xSetPoint].Temperature);
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

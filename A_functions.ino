

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

int CalcFan254PWMForATime(double minutes) {
  //SpDebug("Starting CalcFanPWM");
  int calcedFanSpeed;
  int newFanSpeed;
  float ratio;
  if (minutes <= FanSetPoints[1].Minutes) {
    ratio = (float)(minutes) / (float)(FanSetPoints[1].Minutes);
    calcedFanSpeed = FanSetPoints[0].PWM + (ratio * (FanSetPoints[1].PWM - FanSetPoints[0].PWM));
    //SpDebug("CaclPMW < sp1 mins: " + String(minutes) + " ratio:" + String(ratio) +  " pwm:" + String(calcedFanSpeed));
  }

  else if (minutes < FanSetPoints[2].Minutes) {
    ratio = (float)(minutes - FanSetPoints[1].Minutes) / (float)(FanSetPoints[2].Minutes - FanSetPoints[1].Minutes);
    calcedFanSpeed = FanSetPoints[1].PWM + (ratio * (FanSetPoints[2].PWM - FanSetPoints[1].PWM));
    //SpDebug("CaclPMW < sp2 mins: " + String(minutes) + " ratio:" + String(ratio) +  " pwm:" + String(calcedFanSpeed));

  } else if (minutes <= FanSetPoints[3].Minutes)  //this should roast end
  {
    ratio = (float)(minutes - FanSetPoints[2].Minutes) / (float)(FanSetPoints[3].Minutes - FanSetPoints[2].Minutes);
    calcedFanSpeed = FanSetPoints[2].PWM + (ratio * (FanSetPoints[3].PWM - FanSetPoints[2].PWM));
    //SpDebug("CaclPMW < sp3  mins: " + String(minutes) + " ratio:" + String(ratio) +  " pwm:" + String(calcedFanSpeed));

  } else {  //this should be paste roaste - ie cooling
    calcedFanSpeed = FanSetPoints[3].PWM * 1.2;
    //SpDebug("CaclPMW > 3   mins: " + String(minutes) +  " pwm:" + String(calcedFanSpeed));
  }
  newFanSpeed = calcedFanSpeed + FanDeviation;
  if (FanSetPoints[0].PWM > 254) {
    if (FanDeviation > 0) {
      FanDeviation = 254 - calcedFanSpeed;
      if (FanDeviation > 0) { FanDeviation = 0; }
    }
  }

  if (FanSetPoints[0].PWM < 10) {
    if (FanDeviation < 0) {
      FanDeviation = calcedFanSpeed - 10;
      if (FanDeviation < 0) { FanDeviation = 0; }
    }

    FanSetPoints[0].PWM = 10;
  }

  //Serial.print("return:");Serial.println(newFanSpeed);
  return newFanSpeed;
}

int XforAFanMin(double minute) {
  return (FanGraphXStart + (PixelsPerMinFan * minute));
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
  //Serial.print("starting YforAFan");Serial.println(fanpwm);
  if (fanpwm <= FanGraphMinPWM) {
    //Serial.print("return min:"); Serial.println(FanGraphYBottom);
    return FanGraphBottom;
  } else {
    double ratio = (fanpwm - FanGraphMinPWM) / float(FanGraphMaxPWM - FanGraphMinPWM);
    //Serial.print("ratio:");Serial.println(ratio);
    if (ratio > 1) { ratio = 1; }
    return (FanGraphBottom - (ratio * FanGraphHeight));
  }
}

int YforAFan(int fanpwm) {
  //Serial.print("starting YforAFan");Serial.println(fanpwm);

  if (fanpwm <= FanGraphMinPWM) {
    //Serial.print("return min:"); Serial.println(FanGraphYBottom);
    return FanGraphBottom;
  } else {
    double ratio = (fanpwm - FanGraphMinPWM) / float(FanGraphMaxPWM - FanGraphMinPWM);
    //Serial.print("ratio:");Serial.println(ratio);
    if (ratio > 1) { ratio = 1; }
    return (FanGraphBottom - (ratio * FanGraphHeight));
  }
}

void SetFanFromOpticalSensorPID() {
  
  float value;
  
  switch(flowAveraging)
  {
      case 20:
      {
          value = deltaYflow_avg20.mean();
          break;
      }
      case 50:
      {
          value = deltaYflow_avg50.mean();
          break;
      }
      case 100:
      {
          value = deltaYflow_avg100.mean();
          break;
      }
      case 200:
      {
          value = deltaYflow_avg200.mean();
          break;
      }
 
      default:
      {
          value = deltaYflow_avg20.mean();
          break;
      }

  }
  ErrFlow = setpointflow - value;                           //negative if temp is over setpoint. Positive it temp is under setupt
                                                                             //spDebug("Call SetFanFromOpticalSensorPID  errflow:" + String(ErrFlow));
                                                                             //if (abs(ErrFlow) < GainFlow) {
  if (PIDIntegralUdateTimeFlow.elapsed() > PIDIntegralUdateTimeValueFlow) {  //every 5 seconds we add the err to be a sum of err
                                                                             //if (ErrIFlow < GainFlow) {
    IntegralSumFlow = IntegralSumFlow + ErrFlow;
    if (IntegralSumFlow < 0) {
      IntegralSumFlow = 0;
    }

    ErrIFlow = (IntegralSumFlow * IntegralFlow);  //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
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
  if (abs(PercentChange) > .01) {

    if (DutyFanTemp > DutyFanLast) {
      DutyFan = (DutyFanLast * 1.01);
    } else {
      DutyFan = (DutyFanLast * .99);
    }
  } else {
    DutyFan = DutyFanTemp;
  }

  if (DutyFan > 1.0) {
    DutyFan = 1.0;
  }

  DutyFanLast = DutyFan;
  //} else {  //clear out the integral before set point 1.
  //DutyFan = 1.0;
  //}

  spDebug("Avg" + String(flowAveraging) + ":" + String(value) + ",Err:" + String(ErrFlow) +  ", %:" + String(PercentChange) + ",Duty:" + String(DutyFan) + ",intSum:" + String(IntegralSumFlow) + ",setpoint:" + String(setpointflow) + ",Gain:" + String(GainFlow) + ",integral:" + String(IntegralFlow));

  sendFan_D_Wire();
}

void SetAndSendFanPWMForATime(double minutes) {
  //Serial.println("Call From SetFan");
  //Serial.println(minutes);
  FanSpeed254PWM = CalcFan254PWMForATime(minutes);
  sendFan_Wire();
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

  spDebug("updated fanDAC.  Elapsed:" + String(millis() - start) + " value:" + String(valuefordac));
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

  //spDebug("updated fanDAC.  Elapsed:" + String(millis() - start) + " value:" + String(valuefordac));
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

int getCleanTemp(double temperature, int myID) {
  if (isnan(temperature)) {
    TempReadingskipped[myID]++;
    //Serial.print (myID);Serial.print ("nan temp:");Serial.println(temperature);
    return -1;
  } else if (temperature > 1000) {
    TempReadingskipped[myID]++;
    //Serial.print (myID);Serial.print ("too high temp:");Serial.println(temperature);
    return -1;
  } else if (temperature < 40) {
    TempReadingskipped[myID]++;
    //Serial.print (myID);Serial.print ("too low temp:");Serial.println(temperature);
    return -1;
  } else {
    int r = temperature;
    //Serial.print (myID);Serial.print ("clean temp returned:");Serial.println(r);
    return r;
  }
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

      EEPROM.put(SETPOINTTEMP_EP[xSetPoint], MySetPoints[xSetPoint].Temperature);
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

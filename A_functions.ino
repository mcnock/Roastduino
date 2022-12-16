

//----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------

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

int CalcFanPWMForATime(double minutes) {
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


void SetFanFromOpticalSensor() {
  //Serial.println("Call From SetFan");
  //Serial.println(minutes);
  //read x y ticks and divide by time


  //get tick rate error from setpoint
  beanflow.readMotionCount(&deltaXflow, &deltaYflow);

  //calculated fan voltage error based on a gain
  //calc an integral-add to error corretiion
  //add to current voltage
  if (State == STATEROASTING) {

    ErrFlow = setpointflow - deltaYflow;  //negative if temp is over setpoint. Positive it temp is under setupt
    PIDIntegralUdateTimeValueFlow = 5000;
    if (abs(ErrFlow) < GainFlow) {
      if (PIDIntegralUdateTimeFlow.elapsed() > PIDIntegralUdateTimeValueFlow) {  //every 5 seconds we add the err to be a sum of err
        if (ErrIFlow < GainFlow) {
          IntegralSumFlow = IntegralSumFlow + ErrFlow;
          if (IntegralSumFlow < 0) {
            IntegralSumFlow = 0;
          }
          ErrIFlow = (IntegralSumFlow * IntegralFlow);  //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
          //Serial.println("Isum:");Serial.println(IntegralSum);Serial.println("ErrI:");Serial.println(ErrI);
          PIDIntegralUdateTimeFlow.restart(0);
        }
      }
      DutyFlow = ((ErrFlow + ErrIFlow) / (double)GainFlow);

      if (DutyFlow > 1.0) {
        DutyFlow = 1.0;
      }

    } else {  //clear out the integral before set point 1.
      DutyFlow = 1.0;
    }
  } else {
    ErrIFlow = 0.0;
    IntegralSumFlow = 0.0;
  }
}


void SetAndSendFanPWMForATime(double minutes) {
  //Serial.println("Call From SetFan");
  //Serial.println(minutes);

  FanSpeedPWM = CalcFanPWMForATime(minutes);
  sendFanPWM_Wire();
}

void StopAndSendFanPWM() {
  FanSpeedPWM = 0;
  sendFanPWM_Wire();
}

void sendFanPWM_Wire() {

  float q = 4092;
  float x = FanSpeedPWM;
  float j = q * x;
  int i = j / 254;

  //Serial.print("Setting Fan pwm:");Serial.println(FanSpeedPWM);
  //Serial.println(MCP4725_ADDR);
  //Serial.print("Setting Fan 10 bit :");Serial.println(i);
  //interrupts();
  Wire.beginTransmission(MCP4725_ADDR);
  Wire.write(64);             // cmd to update the DAC
  Wire.write(i >> 4);         // the 8 most significant bits...
  Wire.write((i & 15) << 4);  // the 4 least significant bits...
  Wire.endTransmission();
  // dac.setVoltage(i, false,100000);
  //Serial.print("Setting Fan pwm B:");Serial.println(FanSpeedPWM);
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
    result = double(myGLCD.getDisplayYSize() - PixelYSplit) - ((double)(temp - TempSplitLow) / (double)TempPerPixM);
  } else {
    //Serial.print("high t:");
    result = double(myGLCD.getDisplayYSize() - PixelYSplit2) + ((double)(TempSplitHigh - temp) / (double)TempPerPixH);
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

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

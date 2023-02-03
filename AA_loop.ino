//11000 stared
//8600 Fan
//6300 all disapbled
//5300 1 == 1
//5300 2 == 2
//4300 3 == 3
//4000 4 == 4
//3800 5 == 5
//3800 6 == 6
float TempErrorLast;
// **************************************************************************************************************************************************************
// LOOP A   LOOP A   LOOP A   LOOP  A  LOOP A   LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP
// **************************************************************************************************************************************************************
void theloop() {
  int TBeanAvgThisRun = 0;
  boolean bNewSecond = false;
  boolean bNewTempsAvailable = false;
  boolean bNewFlowAvailable = false;
  NewState = 0;
  boolean bUpdateFan = false;
  bool ReadCoilCurrentFlag = false;
  bool ReadBeanOpticalFlowRateFlag = false;
  bool ReadCurrents = false;
  
  if (RoastTime.isRunning()) {
    if (TimeManual == false) {
      RoastMinutes = ((double)RoastTime.elapsed()) / 60;
    }
  }
  if (SecondTimer.elapsed() > 1000) {
    bNewSecond = true;
    LoopsPerSecond = LoopsPerSecondCalcing;
    LoopsPerSecondCalcing = 0;
    BeanOpticalFlowReadsPerSecond = BeanOpticalFlowReadsPerSecondCalcing;
    BeanOpticalFlowReadsPerSecondCalcing = 0;
    TempSensorReadsPerSecond = TempSensorReadsPerSecondCalcing;
    TempSensorReadsPerSecondCalcing = 0;
    SecondTimer.restart(0);
  } else {
    bNewSecond = false;
    LoopsPerSecondCalcing++;
  }
  if (millis() >= TimeOpticalFlowNext) {
    ReadBeanOpticalFlowRateFlag = true;
    TimeOpticalFlowNext = millis() + TimeOpticalFlowDuration;
  }
  if (millis() >= TimeReadThermoNext) {
    ReadSensorInSequenceFlag = 0;
    TimeReadThermoNext = millis() + TimeReadThermoDuration;
  }
  if (ReadBeanOpticalFlowRateFlag == true) {
    BeanOpticalFlowReadsPerSecondCalcing++;
    ReadBeanOpticalFlowRateFlag = false;
    BeanYflowsqrt[0] = getCleanOpticaFlow(0);
    double upflowdetected0 = false;
    if (BeanYflowsqrt[0] > -999) {
      BeanYflowX_avg[0].push(BeanYflowsqrt[0]);
      if (BeanYflowsqrt[0] > UpFlowThreshold) {
        BeanYflow_avg.push(BeanYflowsqrt[0]);
        bNewFlowAvailable = true;
      } else {
        upflowdetected0 = true;
      }
    }
    BeanYflowsqrt[1] = getCleanOpticaFlow(1);
    if (BeanYflowsqrt[1] != -999) {
      BeanYflowX_avg[1].push(BeanYflowsqrt[1]);
      if (BeanYflowsqrt[1] > UpFlowThreshold) {
        BeanYflow_avg.push(BeanYflowsqrt[1]);
        bNewFlowAvailable = true;
      } else {
        if (upflowdetected0 == true) {  //this means upflow in both so don't ignore them
          BeanYflow_avg.push(BeanYflowsqrt[0]);
          BeanYflow_avg.push(BeanYflowsqrt[1]);
        }
      }
    }

    if (Debugbyte == FLOWSENSORDATARAW_11) {
      SERIALPRINT_DB(F(",AvgSqrt0:"));
      SERIALPRINT_DB(BeanYflowX_avg[0].mean());
      SERIALPRINT_DB(F(",AvgSqrt1:"));
      SERIALPRINT_DB(BeanYflowX_avg[1].mean());
      SERIALPRINT_DB(F(",AvgSqrt:"));
      SERIALPRINT_DB(BeanYflow_avg.mean());
      SERIALPRINT_DB(F(",AvgSqrt:"));
      SERIALPRINT_DB(BeanYflow_avg.mean());
      SERIALPRINT_DB(F(",Avg:"));
      SERIALPRINT_DB(sq(BeanYflow_avg.mean()));
      SERIALPRINT_DB(F(",AvgSz:"));
      SERIALPRINT_DB(BeanYflow_avg._size);
      SERIALPRINT_DB(F(",setpoint:"));
      SERIALPRINTLN_DB(BeanYflowsetpoint);
    }

  }
  ReadCurrents = true;
  if (ReadCurrents == true)
  {
    byte fc = (digitalRead(FANCURRENT_A0) * 50 * _CurrentFan_MVPerAmp)/1024.00;
    FanCurrentAvgRollx10.push(fc);
    byte cc = (digitalRead(COILCURRENT_A1) * 50 * _CurrentCoil_MVPerAmp)/1024.00;
    CoilCurrentAvgRollx10.push(cc);


  }
  if (ReadSensorInSequenceFlag > -1) {  //read thermocouples, 1 per loop
    //Serial.println("B1:");Serial.print(TBean1);Serial.println("B2:");Serial.print(TBean2);Serial.print("C:");Serial.println(TCoil);
    switch (ReadSensorInSequenceFlag) {
      case 0:  //coil temp
        MeasureTempTimer.restart();
        TCoil = getCleanTemp(ThermoCoil);
        //SPDEBUG("TCoil:" + String(TCoil));
        if (TCoil > -1) {
          TCoilAvgRoll.push(TCoil);
        }
        ReadSensorInSequenceFlag++;
        break;
      case 1:  //bean temp 1
        TBean1 = getCleanTemp(ThermoBean1);
        //SPDEBUG("TBean1:" + String(TBean1));
        ReadSensorInSequenceFlag++;
        break;
      case 2:  //beam temp 2
        TBean2 = getCleanTemp(ThermoBean2);
        //SPDEBUG("TBean2:" + String(TBean2) + " id:" + String(ThermoBean2));
        TBeanAvgThisRun = getBeanAvgTemp(TBean1, TBean2);
        if (TBeanAvgThisRun > -1) {
          TBeanAvgRoll.push(TBeanAvgThisRun);
        }
        ReadSensorInSequenceFlag = -1;
        MeasureTempTimer.stop();
        bNewTempsAvailable = true;
        TempSensorReadsPerSecondCalcing++;
        if (Debugbyte == TEMPDATARAW_31) {
          SERIALPRINT_DB(F("Bean1:"));
          SERIALPRINT_DB(TBean1);
          SERIALPRINT_DB(F(",Bean2:"));
          SERIALPRINT_DB(TBean2);
          SERIALPRINT_DB(F(",BeanAvg:"));
          SERIALPRINT_DB(TBeanAvgRoll.mean());
          SERIALPRINT_DB(F(",SetPoint:"));
          SERIALPRINT_DB(CurrentSetPointTemp);
          SERIALPRINT_DB(F(",Coil:"));
          SERIALPRINT_DB(TCoil);
          SERIALPRINT_DB(F(",CoilAvg"));
          SERIALPRINT_DB(TCoilAvgRoll.mean());
          SERIALPRINT_DB(F(",TooHottemp:"));
          SERIALPRINT_DB(TempCoilTooHot);
          SERIALPRINTLN_DB();
        }
        break;
    }
  }
  switch (State) {  //check if a newstate is needed based on temp or time
    case STATEROASTING:
      {
        if (1 == 1) {
          if (TBeanAvgRoll.mean() > (MySetPoints[SetPointCount - 1].Temperature) + 20) {
            TempReachedCount++;
            if (TempReachedCount > 20) {
              NewState = STATECOOLING;
              //Serial.println("Roast Temp Reached. Cooling starting End:");
            }
          } else {
            TempReachedCount = 0;
          }
          if (RoastMinutes > (double)RoastLength) {
            NewState = STATECOOLING;
            SERIALPRINTLN_OP(F("Max time reached. Cooling starting"));
          }
        }
        break;
      }
    case STATECOOLING:
      {
        if (TBeanAvgRoll.mean() < TempCoolingDone) {
          NewState = STATESTOPPED;
          SERIALPRINTLN_OP(F("Auto Cooling Complete "));
        } else if ((RoastMinutes < (double)RoastLength) & RoastRestartNeeded) {
          NewState = STATEROASTING;
          RoastRestartNeeded = false;
          SERIALPRINTLN_OP(F("RestartRoasting request detected"));
        }
        break;
      }
  }
  if (HasDisplay) {  //look for a new state because of touch inputs
    if (myTouch.dataAvailable()) {
      if (TouchDetected == false) {
        if (DetectTouch()) {
          //Serial.println("PRESS");
          TouchDetected = true;
          TouchTimer.restart(0);
        }
      } else {
        if (TouchStatus.objectpressID == PressMenu) {
          if (TouchTimer.elapsed() > 1500) {
            if (LongPressDetected == false) {
              LongPressDetected = true;
              //Serial.println("LONGPRESS");
              MenuTouchLongPressDetected();
            }
          }
        } else {
          //SPDEBUG("Hdere");
          DetectTouch();
        }
      }
    } else {
      //no data available ("aka UnPress")
      if (TouchDetected == true) {
        TouchDetected = false;
        switch (TouchStatus.objectpressID) {
          case PressMenu:
            {
              if (LongPressDetected == false) {
                if (TouchTimer.elapsed() > 100) {
                  MenuTouchClickDetected();
                  TouchStatus.objectpressID = PressNone;
                }
              } else {
                //long press is proccessed during when data available

                LongPressDetected = false;
              }
              //clear anyhightlighting that maybe left inplace
              OutlineClickedButton(BLACK);
              TouchStatus.objectpressID = PressNone;
              break;
            }
          case PressOpDetailBox:
          case PressConfigDisplayBox:
            {
              UpdateDisplayBoxLocation(TouchStatus.objectpressID);
              break;
            }
        }
      }
      TouchTimer.stop();
      TouchStatus.objectpressID = PressNone;
    }
    //else {
    //  SPDEBUG("No touch data available");
    //  if (TouchStatus.objectpressID == PressMenu) {
    //    SPDEBUG("ClearingC menu press");
    //    OutlineClickedButton(BLACK);
    //    TouchStatus.objectpressID = PressNone;
    //  }
    // }
  }
  if (Serial.available() > 0) {  //look for a new state because of serial inputs
    processSerial(Serial);
  }
  if (NewState != 0) {  //rules on how to switch state
    SERIALPRINT_DB(F("Moving  to new state:"));
    SERIALPRINTLN_DB(StateName[NewState]);
    switch (NewState) {
      case STATESTOPPED:  //newstate
        {
          analogWrite(SSR1_p7, 0);
          SSR1PWMLast = 0;
          analogWrite(SSR2_p6, 0);
          SSR2PWMLast = 0;
          if (TBeanAvgRoll.mean() < TempCoolingDone) {
            State = STATESTOPPED;
            SetFanOff();
            //digitalWrite(VIBRELAYp, RELAYOFF);
            RoastTime.stop();
            DutyFan = 0;
          } else {
            NewState = STATECOOLING;
            State = STATECOOLING;
          }
          break;
        }
      case STATERESTARTROASTING:
        {
          State = STATEROASTING;
          break;
        }

      case STATEROASTING:  //newstate
        {
          if (State == STATESTOPPED || State == STATEFANONLY) {
            //SetAndSendFanPWMForATime(0);
            TCoilAvgRoll.clear();
            TBeanAvgRoll.clear();
            thermocouples[0].Readingskipped = 0;
            thermocouples[1].Readingskipped = 0;
            thermocouples[2].Readingskipped = 0;
            //StartLinebyTemp(0, ROLLMAXLINEID);
            //StartLinebyTemp(0, ROLLMINLINEID);
            StartLinebyTemp(0, ROLLAVGLINEID);
            StartLinebyTemp(0, COILLINEID);
            //StartLinebyXAndY(FanGraphXStart, YforAFlow(FanSpeed254PWM), FANSPEEDLINEID);
            graphProfile();
            RoastTime.restart(0);
            RoastMinutes = 0;
            RoastRestartNeeded = false;
          } else if (State == STATECOOLING) {
            //nothing is needed
          }

          State = STATEROASTING;
          break;
        }
      case STATECOOLING:  //newstate
        {
          State = NewState;
          digitalWrite(SSR1_p7, LOW);
          digitalWrite(SSR2_p6, LOW);
          delay(1000);
          break;
        }
      case STATEFANONLY:  //newstate
        {
          State = NewState;
          //reset fan pid values to correct starting values
          ErrIFlow = 0.0;
          //IntegralSumFlow = _IntegralSumFlowStarting;
          CalcStartingIntegralSumFlow();

          BeanYflow_avg.clear();
          SetFanFromOpticalSensorPID();
          break;
        }
      case STATEOVERHEATED:  //newstate
        {
          State = NewState;
          digitalWrite(SSR1_p7, LOW);
          digitalWrite(SSR2_p6, LOW);
          delay(1000);
          break;
        }
      case STATENOFANCURRENT:  //newstate
        {
          State = NewState;
          digitalWrite(SSR1_p7, LOW);
          digitalWrite(SSR2_p6, LOW);
          break;
        }
      case DEBUGCOIL:  //newstate
        State = NewState;
      case DEBUGTOGGLE:  //newstate
        State = NewState;
        break;
      case DEBUGDUTY:  //newstate
        State = NewState;
        break;
    }
  }
  if (State == STATEROASTING) {  //calc error and duty for temperature PID
    if (bNewTempsAvailable == true) {
      double changes;
      CurrentSetPointTemp = SetpointforATime(RoastMinutes);
      ErrTemp = CurrentSetPointTemp - TBeanAvgRoll.mean();  //negative if temp is over setpoint. Positive it temp is under setupt
      if (abs(ErrTemp) < GainTemp) {
        if (PIDIntegralUdateTimeTemp.elapsed() > PIDIntegralUdateTimeValueTemp) {  //every 5 seconds we add the err to be a sum of err
          if (ErrITemp < GainTemp) {
            IntegralSumTemp = IntegralSumTemp + ErrTemp;
            if (IntegralSumTemp < 0) {
              IntegralSumTemp = 0;
            }
            ErrITemp = (IntegralSumTemp * IntegralTemp);  //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
            //Serial.println("Isum:");Serial.println(IntegralSum);Serial.println("ErrI:");Serial.println(ErrI);
            PIDIntegralUdateTimeTemp.restart(0);
          }
        }
        float tempErrorCombined = ErrTemp + ErrITemp;
        changes = TempErrorLast - tempErrorCombined;
        TempErrorLast = ErrTemp + ErrITemp;
        DutyTemp = (tempErrorCombined) / (double)GainTemp;
        if (DutyTemp > 1.0) {
          DutyTemp = 1.0;
        }
      } else {  //clear out the integral before set point 1.
        DutyTemp = 1.0;
      }
      if (Debugbyte == TEMPPIDINFO_30) {
        SERIALPRINT_DB("TempSetpoint:");
        SERIALPRINT_DB(CurrentSetPointTemp);
        SERIALPRINT_DB(F("Bean1:"));
        SERIALPRINT_DB(TBean1);
        SERIALPRINT_DB(F(",Bean2:"));
        SERIALPRINT_DB(TBean2);
        SERIALPRINT_DB(",AvgTemp:");
        SERIALPRINT_DB(TBeanAvgRoll.mean());
        SERIALPRINT_DB(",Err:");
        SERIALPRINT_DB(ErrTemp);
        SERIALPRINT_DB(",ErrI:");
        SERIALPRINT_DB(ErrITemp);
        SERIALPRINT_DB(",changes:");
        SERIALPRINT_DB(changes);
        SERIALPRINT_DB(",ISR:");
        SERIALPRINT_DB(IntegralSumTemp);
        SERIALPRINT_DB(",Duty:");
        SERIALPRINTLN_DB(DutyTemp);
      }
    }
  } else {
    ErrITemp = 0.0;
    IntegralSumTemp = 0.0;
  }
  
  if ((State == STATEROASTING || StateDebug == DEBUGDUTY)) {  //set pwm1 and 2 based on duty
    byte SSR1PWM;
    byte SSR2PWM;
    if (DutyTemp >= 1.0) {  // >1
      SSR1PWM = 255;
      SSR1Status = SSRFULL;
      SSR2PWM = 255;
      SSR2Status = SSRFULL;
    } else if (DutyTemp > 0.5) {  // .5 to 1
      SSR1Status = SSRFULL;
      SSR1PWM = 255;
      SSR2Status = SSRPWM;
      SSR2PWM = ((DutyTemp - .5) * 2) * 255;
    } else {  //< .50
      SSR1Status = SSRPWM;
      SSR2Status = SSROFF;
      SSR2PWM = 0;
      SSR1PWM = (DutyTemp * 2) * 254;
    }
    if (TCoil > TempCoilTooHot) {
      if (TempCoilTooHotCount > 10) {
        ErrorStatus.newerrmsg = true;
        ErrorStatus.error = ErrorCoilTooHot;
        UpdateErrorDisplayArea(VALUESONLY);
        //Serial.println("too hot");
      }
      TempCoilTooHotCount++;
      SSR1PWM = 0;
      SSR2PWM = 0;
    } else {
      if (TempCoilTooHotCount > 0) {
        if (ErrorStatus.error == ErrorCoilTooHot) {
          ErrorStatus.newerrmsg = true;
          ErrorStatus.lasterror = ErrorStatus.error;
          ErrorStatus.error = NoError;
          UpdateErrorDisplayArea(VALUESONLY);
        }
        TempCoilTooHotCount = 0;
      }
    }
  
    if (SSR1PWMLast != SSR1PWM) {
        analogWrite(SSR1_p7, SSR1PWM);
        SSR1PWMLast = SSR1PWM;
    }
    if (SSR2PWMLast != SSR2PWM) {
      analogWrite(SSR2_p6, SSR2PWM);
      SSR2PWMLast = SSR2PWM;
    }
  }
  
  if (not(State == STATEROASTING || StateDebug == DEBUGDUTY || StateDebug == DEBUGCOIL)) {  //make sure to turn SSD's and other off if not running
    
    if (SSR1PWMLast != 0) {
        analogWrite(SSR1_p7, 0);
        SSR1PWMLast = 0;
    }
    if (SSR2PWMLast != 0) {
      analogWrite(SSR2_p6, 0);
      SSR2PWMLast = 0;
    }
    SSR1Status = SSROFF;
    SSR2Status = SSROFF;
    DutyTemp = 0;
    ErrTemp = 0;
    CurrentSetPointTemp = 0;
    IntegralSumTemp = 0;
    IntegralLastTimeTemp = 0;
  }
  if (NewState > 0) {  //update gui on state change
    UpdateProgressDisplayArea(VALUESONLY);
    NewState = 0;  //this clears it for next run
  }
  //update graph
  if (bNewSecond) {  //output to UI each second
    UpdateProgressDisplayArea(VALUESONLY);
    UpdateOpDetailsDisplayArea(VALUESONLY);
    if ((State == STATEROASTING || State == DEBUGDUTY || State == STATECOOLING)) {
      if (1 == 2) {
        float dummymean = SetpointforATime(RoastMinutes) - 5;
        float dummycoil = 700 + RoastMinutes;
        RoastAcumHeat = RoastAcumHeat + dummymean;
        AddLinebyTimeAndTemp(RoastMinutes, dummymean, ROLLAVGLINEID);
        float coiloffsetted = (dummycoil + CoilTempOffSet);
        if (coiloffsetted < 0) { coiloffsetted = 0; }
        AddPointbyTimeAndTemp(RoastMinutes, coiloffsetted, COILLINEID, 2);
      } else {
        if (State == STATEROASTING) {
          RoastAcumHeat = RoastAcumHeat + TBeanAvgRoll.mean();
        }
        //compress time when cooling after ending at expected roast roast time
        if (RoastMinutes > RoastLength) {
          double RoastMinutesAdjustedDuringCooling = RoastMinutes + (RoastMinutes - (double)RoastLength) / _CoolingTimeCompression;
          AddLinebyTimeAndTemp(RoastMinutesAdjustedDuringCooling, TBeanAvgRoll.mean(), ROLLAVGLINEID);
        } else {
          AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.mean(), ROLLAVGLINEID);
        }
        int coiloffsetted = TCoilAvgRoll.mean() / 2;
        AddPointbyTimeAndTemp(RoastMinutes, coiloffsetted, COILLINEID, 2);
      }
    }
  }
  //set fan
  if ((State == STATEROASTING || State == DEBUGDUTY || State == STATECOOLING || State == STATEFANONLY)) {
    if (bNewSecond == true) {
      SetFanFromOpticalSensorPID();
    }
  }

}  //end of loop

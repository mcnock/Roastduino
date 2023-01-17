//11000 stared
//8600 Fan
//6300 all disapbled
//5300 1 == 1
//5300 2 == 2
//4300 3 == 3
//4000 4 == 4
//3800 5 == 5
//3800 6 == 6

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
    byte sensorindex = 0;
    BeanYflow = getCleanOpticaFlow(sensorindex);
    if (BeanYflow > 0) {
      //Serial.println(BeanYflow);
      BeanYflow_avg.push(BeanYflow);
      //float mytest =  BeanYflow_avg.mean();
      //Serial.println(mytest);

      //SPDEBUG2("deltaY:" + String(BeanYflow) + ",mean:" + String(BeanYflow_avg.mean()) + ",Y10:10,Y0:0,avgsize:" + String(BeanYflow_avg.getCount()) + ",skipped:" + String(BeanOpticalFlowSensors[sensorindex].YflowReadingskipped));
      //SPDEBUGXCLOSE
      bNewFlowAvailable = true;
    } else {
      BeanYflow_avg.push(BeanYflow);
      bNewFlowAvailable = true;
    }
    byte fc = digitalRead(FANCURRENT_A0);
    FanCurrentAvgRoll.push(fc);
    byte cc = digitalRead(COILCURRENT_A1);
    CoilCurrentAvgRoll.push(cc);
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
          if (RoastMinutes > MySetPoints[SetPointCount - 1].Minutes) {
            NewState = STATECOOLING;
            Serial.println(F("Max time reached. Cooling starting"));
          }
        }
        break;
      }
    case STATECOOLING:
      {
        if (TBeanAvgRoll.mean() < TempCoolingDone) {
          NewState = STATESTOPPED;
          Serial.println(F("Auto Cooling Complete "));
        } else if ((RoastMinutes < MySetPoints[SetPointCount - 1].Minutes) & RoastRestartNeeded) {
          NewState = STATEROASTING;
          RoastRestartNeeded = false;
          Serial.println(F("RestartRoasting request detected"));
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
    Serial.print("Moving  to new state:");
    Serial.println(StateName[NewState]);
    switch (NewState) {
      case STATESTOPPED:  //newstate
        {
          analogWrite(SSR1_p7, 0);
          analogWrite(SSR2_p6, 0);
          if (TBeanAvgRoll.mean() < TempCoolingDone) {
            State = STATESTOPPED;
            digitalWrite(SSR1_p7, LOW);
            digitalWrite(SSR2_p6, LOW);
            digitalWrite(FANRELAYp_2, RELAYOFF);
            //digitalWrite(VIBRELAYp, RELAYOFF);
            RoastTime.stop();
            FanSpeed254PWM = 0;
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
          digitalWrite(FANRELAYp_2, RELAYON);
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
          //reset fan pid values
          ErrIFlow = 0.0;
          IntegralSumFlow = _IntegralSumFlow;
          BeanYflow_avg.clear();
          //DutyFlow = .7;

          // FanSpeed254PWM = FlowSetPoints[0].PWM;
          //SetAndSendFanPWMForATime(0);
          digitalWrite(FANRELAYp_2, RELAYON);
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
  if (2 == 2 & (State == STATEROASTING)) {  //calc error and duty for temperature PID
    if (bNewTempsAvailable == true) {
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
        DutyTemp = ((ErrTemp + ErrITemp) / (double)GainTemp);

        if (DutyTemp > 1.0) {
          DutyTemp = 1.0;
        }

      } else {  //clear out the integral before set point 1.
        DutyTemp = 1.0;
      }
    }
  } else {
    ErrITemp = 0.0;
    IntegralSumTemp = 0.0;
  }
  if (3 == 3 & (State == STATEROASTING || StateDebug == DEBUGDUTY)) {  //set pwm1 and 2 based on duty
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
    SPDEBUG4("SSR1:" + String(SSR1PWM) + ",SSR2:" + String(SSRPWM));
    SPDEBUGXCLOSE
    analogWrite(SSR1_p7, SSR1PWM);
    analogWrite(SSR2_p6, SSR2PWM);
  }
  if (not(State == STATEROASTING || StateDebug == DEBUGDUTY || StateDebug == DEBUGCOIL)) {  //make sure to turn SSD's and other off if not running
    analogWrite(SSR1_p7, 0);
    analogWrite(SSR2_p6, 0);
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
  if (bNewSecond) {  //what to output to UI each second
    //Serial.println("update after reach new temp");
    UpdateProgressDisplayArea(VALUESONLY);
    UpdateOpDetailsDisplayArea(VALUESONLY);
    if (FanLegacy) {
      UpdateFanPWMValuesDisplay(VALUESONLY);
    }
    if (SerialOutPutTempsBySecond == true) {
      SerialOutputTempsForPlotting();
    }
    if (SerialOutPutStatusBySecond == true) {

      SerialOutputStatus();
    }
    if (FanLegacy == true) {
      if (State == STATEROASTING || State == DEBUGDUTY || State == STATECOOLING) {
        SetAndSendFanPWMForATime(RoastMinutes);
        AddLinebyTimeAndFan(RoastMinutes);
      }
    }
  }
  if (4 == 4 & (State == STATEROASTING || State == DEBUGDUTY || State == STATECOOLING || State == STATEFANONLY)) {
    //if (bNewFlowAvailable == true) {

    if (bNewSecond == true) {
      SetFanFromOpticalSensorPID();
    }
  }
  //}
  if (bNewSecond) {  //add a line to graph UI if we have new temp data
    if (5 == 5 & (State == STATEROASTING || State == DEBUGDUTY || State == STATECOOLING)) {
      //SpDebu)g("Adding maxium of:\t" + String(TBeanAvgRoll.maximum()));
      if (1 == 2) {
        float dummymean = SetpointforATime(RoastMinutes) - 5;
        float dummycoil = 700 + RoastMinutes;
        RoastAcumHeat = RoastAcumHeat + dummymean;
        AddLinebyTimeAndTemp(RoastMinutes, dummymean, ROLLAVGLINEID);
        float coiloffsetted = (dummycoil + CoilTempOffSet);
        if (coiloffsetted < 0) { coiloffsetted = 0; }
        AddPointbyTimeAndTemp(RoastMinutes, coiloffsetted, COILLINEID, 2);

      } else {
        RoastAcumHeat = RoastAcumHeat + TBeanAvgRoll.mean();
        AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.mean(), ROLLAVGLINEID);
        int coiloffsetted = (TCoilAvgRoll.mean() - CoilTempOffSet);
        if (coiloffsetted < 0) { coiloffsetted = 0; }
        AddPointbyTimeAndTemp(RoastMinutes, coiloffsetted, COILLINEID, 2);
      }
      //AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.maximum(), ROLLMAXLINEID);
      //AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.minimum(), ROLLMINLINEID);
    }
  }

  if (ThreeSecondTimer.elapsed() > 3000) {  //what to output to UI each 3 seconds
    if (SerialOutPutTempsBy3Seconds == true) {
      SerialOutputTempsForPlotting();
    }
    if (SerialOutPutStatusBy3Seconds == true) {
      SerialOutputStatus();
    }
    if (State == STATECOOLING) {
      SetAndSendFanPWMForATime(RoastMinutes);
    }
    if (6 == 6 & State == STATEROASTING) {
      //Serial.println('b');

      //SetAndSendFanPWMForATime(FanSpeedPWNDecreaseByMinutes);
    }
    UpdateProgressDisplayArea(VALUESONLY);
    ThreeSecondTimer.restart(0);
  }

}  //end of loop

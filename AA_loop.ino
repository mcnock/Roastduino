
//

// **************************************************************************************************************************************************************
// LOOP A   LOOP A   LOOP A   LOOP  A  LOOP A   LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP
// **************************************************************************************************************************************************************
int CoilTempOffSet;

void theloop() {
  int TBeanAvgThisRun = 0;
  boolean bNewSecond = false;
  boolean bNewTempsAvailable = false;
  newState = 0;
  boolean bUpdateFan = false;
  if (RoastTime.isRunning()) {
    RoastMinutes = ((double)RoastTime.elapsed()) / 60;
  }
  if (millis() >= TimeSubSecondNext) {
    ReadBeanOpticalFlowRateFlag = true;
    TimeSubSecondNext = millis() + TimeSubSecondDuration;
  } else {
    ReadBeanOpticalFlowRateFlag = false;
  }
  if (SecondTimer.elapsed() > 1000) {
    //Serial.println("New second");
    ReadTempFlag = 0;
    bNewSecond = true;
    LoopsPerSecond = LoopsPerSecondCalcing;
    LoopsPerSecondCalcing = 0;
    SecondTimer.restart(0);
  } else {
    bNewSecond = false;
    LoopsPerSecondCalcing++;
  }
  if (ReadCoilCurrentFlag == true) {
    SPI.begin();
    CoilAmps = 0;
    digitalWrite(COILCURRENT_SPI_SSp53, LOW);
    CoilAmps = SPI.transfer(0);
    digitalWrite(COILCURRENT_SPI_SSp53, HIGH);
    SPI.end();
    ReadCoilCurrentFlag = false;
  };
  if (ReadBeanOpticalFlowRateFlag == true) {
    //double start = millis();
    digitalWrite(BEAN_OPTICAL_FLOW_SPI_SSp48, LOW);
    SPI.begin();
    if (BeanOpticalFlowSensor.Initialized == false) {
      Serial.println("Initializing");
      if (!BeanOpticalFlowSensor.Initialize()) {
        Serial.println("Initialization of the flow sensor failed");
      }
    }
    BeanOpticalFlowSensor.readMotionCountY(&deltaYflow);
    if (deltaYflow < 100) {
      if (deltaYflow > -1) {
        switch (flowAveraging) {
          case 20:
            {
              deltaYflow_avg20.push(deltaYflow);
              break;
            }
          case 50:
            {
              deltaYflow_avg50.push(deltaYflow);
              break;
            }
          case 100:
            {
              deltaYflow_avg100.push(deltaYflow);
              break;
            }
          case 200:
            {
              deltaYflow_avg200.push(deltaYflow);
              break;
            }
  
        default:
            {
              deltaYflow_avg20.push(deltaYflow);
              break;
            }
        }
      }
    } else {
      spDebug("High bean flow value:" + String(deltaYflow));
    }
    digitalWrite(BEAN_OPTICAL_FLOW_SPI_SSp48, HIGH);
    SPI.end();
    //spDebug("time to read flow:" + String(millis() - start));
  }
  if (ReadTempFlag > -1) {  //read thermocouples, 1 per loop
    //Serial.println("B1:");Serial.print(TBean1);Serial.println("B2:");Serial.print(TBean2);Serial.print("C:");Serial.println(TCoil);
    switch (ReadTempFlag) {
      case 0:  //coil
        MeasureTempTimer.restart();
        TCoil = getCleanTemp(thermocouple3.readFahrenheit(), 0);
        //Serial.print("New Coil temp:");Serial.println(TCoil);
        if (TCoil > -1) {
          TCoilRoll.push(TCoil);
        }
        ReadTempFlag++;


        //Serial.print("New Coil temp time:");Serial.println(t);

        break;
      case 1:
        TBean1 = getCleanTemp(thermocouple1.readFahrenheit(), 1);
        //SpDebug("New bean 1 temp:" + String(TBean1));
        if (TBean1 == -1) {
          TBean1 = getCleanTemp(thermocouple1.readFahrenheit(), 1);
        }
        ReadTempFlag++;
        break;
      case 2:
        TBean2 = getCleanTemp(thermocouple2.readFahrenheit(), 2);
        //SpDebug("New bean 2 temp" + String(TBean2));
        if (TBean2 == -1) {
          TBean2 = getCleanTemp(thermocouple2.readFahrenheit(), 2);
        }
        TBeanAvgThisRun = getBeanAvgTemp(TBean1, TBean2);
        if (TBeanAvgThisRun > -1) {
          TBeanAvgRoll.push(TBeanAvgThisRun);
          //SpDebug("Added value:" + String(TBeanAvgThisRun) + " to TbeanAvgRoll with new max of:\t" + String(TBeanAvgRoll.maximum()) + "/t avg of:" + String(TBeanAvgRoll.mean())  );
        }
        ReadTempFlag = -1;
        //        long t = MeasureTempTimer.elapsed();

        MeasureTempTimer.stop();
        //Serial.print("New temps time:");Serial.println(t);
        //Serial.print(F("bean1:"));Serial.print(TBean1);Serial.print(F(",bean2:"));Serial.print(TBean2);
        bNewTempsAvailable = true;
        break;
    }
  }
  switch (State) {  //check if a newstate is needed based on temp or time
    case STATEROASTING:
      {
        if (TBeanAvgRoll.mean() > (MySetPoints[SetPointCount - 1].Temperature) + 20) {
          TempReachedCount++;
          if (TempReachedCount > 20) {
            newState = STATECOOLING;
            //Serial.println("Roast Temp Reached. Cooling starting End:");
          }
        } else {
          TempReachedCount = 0;
        }
        if (RoastMinutes > MySetPoints[SetPointCount - 1].Minutes) {
          newState = STATECOOLING;
          Serial.println(F("Max time reached. Cooling starting"));
        }
        break;
      }
    case STATECOOLING:
      {
        if (TBeanAvgRoll.mean() < TEMPCOOLINGDONE) {
          newState = STATESTOPPED;
          Serial.println(F("Auto Cooling Complete "));
        } else if ((RoastMinutes < MySetPoints[SetPointCount - 1].Minutes) & RoastRestartNeeded) {
          newState = STATEROASTING;
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
        if (TouchStatus.objectpress == PressMenu) {
          if (TouchTimer.elapsed() > 1500) {
            if (LongPressDetected == false) {
              LongPressDetected = true;
              //Serial.println("LONGPRESS");
              MenuTouchLongPressDetected();
            }
          }
        } else {
          DetectTouch();
        }
      }
    } else {
      //Serial.println("UnPress")
      if (TouchDetected == true) {
        TouchDetected = false;
        if (TouchStatus.objectpress == PressMenu) {
          if (LongPressDetected == false) {
            if (TouchTimer.elapsed() > 100) {
              MenuTouchClickDetected();
              TouchStatus.objectpress = 0;
            }

          } else {
            LongPressDetected = false;
          }
        }
        if (TouchStatus.objectpress == PressMenu) {
          //spDebug("ClearingC menu press");
          OutlineClickedButton(BLACK);
          TouchStatus.objectpress = 0;
        }
        if (TouchStatus.objectpress == PressOperDetailBox) {
          spDebug("ClearingD menu press");
          myGLCD.setColor(BLACK);
          myGLCD_fillRect(OpDetailDisplay);
          myGLCD.setColor(PALEYELLOW);
          OpDetailDisplay.x = OpDetailDisplay.x - TouchStatus.dragx;
          OpDetailDisplay.y = OpDetailDisplay.y - TouchStatus.dragy;
          OpDetailDisplay.xmax = OpDetailDisplay.xmax - TouchStatus.dragx;
          OpDetailDisplay.ymax = OpDetailDisplay.ymax - TouchStatus.dragy;
          UpdateOpDetailsDisplayArea(All);
          TouchStatus.objectpress = 0;
          EEPROM.put(OPERDETAILDISPLAY_X_EP, OpDetailDisplay.x);
          EEPROM.put(OPERDETAILDISPLAY_Y_EP, OpDetailDisplay.y);
        }
        if (TouchStatus.objectpress == PressCongurationBox) {
          spDebug("ClearingD menu press");
          myGLCD.setColor(BLACK);
          myGLCD_fillRect(ConfigDisplay);
          myGLCD.setColor(PALEYELLOW);
          ConfigDisplay.x = ConfigDisplay.x - TouchStatus.dragx;
          ConfigDisplay.y = ConfigDisplay.y - TouchStatus.dragy;
          ConfigDisplay.xmax = ConfigDisplay.xmax - TouchStatus.dragx;
          ConfigDisplay.ymax = ConfigDisplay.ymax - TouchStatus.dragy;
          UpdateConfigsDisplayArea(All);
          TouchStatus.objectpress = 0;
          EEPROM.put(CONFIGURATIONDISPLAY_X_EP, ConfigDisplay.x);
          EEPROM.put(CONFIGURATIONDISPLAY_Y_EP, ConfigDisplay.y);
        }

        TouchTimer.stop();
      }


      TouchTimer.stop();
    }
    //else {
    //  spDebug("No touch data available");
    //  if (TouchStatus.objectpress == PressMenu) {
    //    spDebug("ClearingC menu press");
    //    OutlineClickedButton(BLACK);
    //    TouchStatus.objectpress = 0;
    //  }
    // }
  }
  if (Serial.available() > 0) {  //look for a new state because of serial inputs
    processSerial(Serial);
  }
  if (newState != 0) {  //rules on how to switch state
    Serial.print("Moving  to new state:");
    Serial.println(StateName[newState]);
    switch (newState) {
      case STATESTOPPED:  //newstate
        {
          //Serial.print("D:");Serial.println(LOW);

          digitalWrite(SSR1_p7, LOW);
          digitalWrite(SSR2_p6, LOW);
          if (TBeanAvgRoll.mean() < TEMPCOOLINGDONE) {
            State = STATESTOPPED;
            digitalWrite(SSR1_p7, LOW);
            digitalWrite(SSR2_p6, LOW);
            digitalWrite(FANRELAYp_2, RELAYOFF);
            //digitalWrite(VIBRELAYp, RELAYOFF);
            RoastTime.stop();
            FanSpeed254PWM = 0;
            DutyFan = 0;
          } else {
            newState = STATECOOLING;
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
            SetAndSendFanPWMForATime(0);
            TCoilRoll.clear();
            TBeanAvgRoll.clear();
            TempReadingskipped[TCoilID] = 0;
            TempReadingskipped[TBean1ID] = 0;
            TempReadingskipped[TBean2ID] = 0;
            StartLinebyTemp(0, ROLLMAXLINEID);
            StartLinebyTemp(0, ROLLMINLINEID);
            StartLinebyTemp(0, ROLLAVGLINEID);
            StartLinebyTemp(0, COILLINEID);
            StartLinebyXAndY(FanGraphXStart, YforAFan(FanSpeed254PWM), FANSPEEDLINEID);
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
          State = newState;
          SetAndSendFanPWMForATime(RoastMinutes);
          //FanSpeed254PWMAutoMode = false;
          //Serial.print("F:");Serial.println(LOW);

          digitalWrite(SSR1_p7, LOW);
          digitalWrite(SSR2_p6, LOW);
          delay(1000);
          break;
        }
      case STATEFANONLY:  //newstate
        {
          State = newState;
          //reset fan pid values
          ErrIFlow = 0.0;
          IntegralSumFlow = 0.0;
          //DutyFlow = .7;
          SetFanFromOpticalSensorPID();
          // FanSpeed254PWM = FanSetPoints[0].PWM;
          //SetAndSendFanPWMForATime(0);
          digitalWrite(FANRELAYp_2, RELAYON);
          break;
        }
      case STATEOVERHEATED:  //newstate
        {
          State = newState;
          digitalWrite(SSR1_p7, LOW);
          digitalWrite(SSR2_p6, LOW);
          delay(1000);
          break;
        }
      case STATENOFANCURRENT:  //newstate
        {
          State = newState;
          digitalWrite(SSR1_p7, LOW);
          digitalWrite(SSR2_p6, LOW);
          break;
        }
      case DEBUGCOIL:  //newstate
        State = newState;
      case DEBUGTOGGLE:  //newstate
        State = newState;
        break;
      case DEBUGDUTY:  //newstate
        State = newState;
        break;
    }
  }
  if (State == STATEROASTING) {  //calc error and duty for temperature PID
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
  } else {
    ErrITemp = 0.0;
    IntegralSumTemp = 0.0;
  }
  if (State == STATEROASTING || StateDebug == DEBUGDUTY) {  //calc duty window and set SSR's based on duty
    int SSR1 = LOW;
    SSR1Status = SSROFF;
    int SSR2 = LOW;
    SSR2Status = SSROFF;
    PIDDutyWindowSizeTemp = 1000;
    unsigned long now = millis();
    boolean ExceedsWholePidWindow = (PIDWindowStartTimeTemp == 0) || (now - PIDWindowStartTimeTemp > PIDDutyWindowSizeTemp);
    if (ExceedsWholePidWindow) {  //keep checking if we need to start a new PID window
      PIDWindowStartTimeTemp = now;
      if (DutyTemp > 0.0) {
        SSR1 = HIGH;
        SSR1Status = SSRPWM;
      }
      if (DutyTemp > 0.5) {
        SSR2 = HIGH;
        SSR2Status = SSRFULL;
      }
    } else {

      SSR1Status = SSRPWM;

      if (DutyTemp <= 0.5 && ((now - PIDWindowStartTimeTemp) <= (DutyTemp * 2 * PIDDutyWindowSizeTemp))) {
        SSR1 = HIGH;
      }
      if (DutyTemp >= 1.0) {
        SSR1 = HIGH;
        SSR1Status = SSRFULL;
        SSR2 = HIGH;
        SSR2Status = SSRFULL;
      }
      if (DutyTemp > 0.5) {
        SSR1 = HIGH;
        SSR1Status = SSRFULL;
        SSR2Status = SSRPWM;
        if ((now - PIDWindowStartTimeTemp) <= ((DutyTemp - .5) * 2 * PIDDutyWindowSizeTemp)) {
          SSR2 = HIGH;
        }
      }
    }
    if (TCoil > TEMPCOILTOOHOT) {
      if (TEMPCOILTOOHOTCount > 10) {
        bNewSecond = true;  //force display immediately
        ErrorStatus.newerrmsg = true;
        ErrorStatus.error = ErrorCoilTooHot;
        UpdateErrorDisplayArea(ValuesOnly);
        //Serial.println("too hot");
      }
      TEMPCOILTOOHOTCount++;
      SSR1 = LOW;
      SSR2 = LOW;
    } else {
      if (TEMPCOILTOOHOTCount > 0) {
        if (ErrorStatus.error == ErrorCoilTooHot) {
          ErrorStatus.newerrmsg = true;
          ErrorStatus.lasterror = ErrorStatus.error;
          ErrorStatus.error = NoError;
          UpdateErrorDisplayArea(ValuesOnly);
        }

        TEMPCOILTOOHOTCount = 0;
      }
    }

    digitalWrite(SSR1_p7, SSR1);
    digitalWrite(SSR2_p6, SSR2);
  }
  if (not(State == STATEROASTING || StateDebug == DEBUGDUTY || StateDebug == DEBUGCOIL)) {  //make sure to turn SSD's and other off if not running
    digitalWrite(SSR1_p7, LOW);
    digitalWrite(SSR2_p6, LOW);
    SSR1Status = SSROFF;
    SSR2Status = SSROFF;
    DutyTemp = 0;
    ErrTemp = 0;
    CurrentSetPointTemp = 0;
    IntegralSumTemp = 0;
    IntegralLastTimeTemp = 0;
  }
  if (newState > 0) {  //update gui on state change
    UpdateProgressDisplayArea(ValuesOnly);
    newState = 0;  //this clears it for next run
  }
  if (bNewSecond) {  //what to output to UI each second
    //Serial.println("update after reach new temp");
    UpdateProgressDisplayArea(ValuesOnly);
    UpdateOpDetailsDisplayArea(ValuesOnly);
    UpdateFanPWMValuesDisplay(ValuesOnly);
    if (serialOutPutTempsBySecond == true) {
      SerialOutputTempsForPlotting();
    }
    if (serialOutPutStatusBySecond == true) {

      SerialOutputStatus();
    }
    if (State == STATEROASTING || State == DEBUGDUTY || State == STATECOOLING) {
      SetAndSendFanPWMForATime(RoastMinutes);
      AddLinebyTimeAndFan(RoastMinutes);
    }
    if (State == STATEFANONLY) {  //fot testing fan flow pid
      SetFanFromOpticalSensorPID();
    }
  }
  if (bNewTempsAvailable) {  //add a line to graph UI if we have new temp data
    if (State == STATEROASTING || State == DEBUGDUTY || State == STATECOOLING) {
      //SpDebug("Adding maxium of:\t" + String(TBeanAvgRoll.maximum()));
      RoastAcumHeat + TBeanAvgRoll.mean();
      AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.maximum(), ROLLMAXLINEID);
      AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.minimum(), ROLLMINLINEID);
      AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.mean(), ROLLAVGLINEID);
      int coiloffsetted = (TCoilRoll.mean() - CoilTempOffSet);
      if (coiloffsetted < 0) { coiloffsetted = 0; }
      AddPointbyTimeAndTempAndLineID(RoastMinutes, coiloffsetted, COILLINEID, 2);
    }
  }
  if (ThreeSecondTimer.elapsed() > 3000) {  //what to output to UI each 3 seconds
    if (serialOutPutTempsBy3Seconds == true) {
      SerialOutputTempsForPlotting();
    }
    if (serialOutPutStatusBy3Seconds == true) {
      SerialOutputStatus();
    }
    if (State == STATECOOLING) {
      SetAndSendFanPWMForATime(RoastMinutes);
    }
    if (State == STATEROASTING) {
      //Serial.println('b');

      //SetAndSendFanPWMForATime(FanSpeedPWNDecreaseByMinutes);
    }
    UpdateProgressDisplayArea(ValuesOnly);
    ThreeSecondTimer.restart(0);
  }

}  //end of loop

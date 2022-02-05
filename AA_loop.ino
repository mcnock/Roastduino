
//

// **************************************************************************************************************************************************************
// LOOP A   LOOP A   LOOP A   LOOP  A  LOOP A   LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP
// **************************************************************************************************************************************************************

void theloop() {
  int TBeanAvgThisRun = 0;
  boolean bNewSecond = false;
  boolean bNewTempsAvailable = false;
  newState = 0;

  //pre work for  loop

  if (SecondTimer.elapsed() > 1000) {
    //Serial.println("New second");
    ReadTempFlag = 0;
    bNewSecond = true;
    LoopsPerSecond = 0;
    SecondTimer.restart(0);
  } else {
    bNewSecond = false;
    LoopsPerSecond++;
  }

  //capture a fixed roast time that will not change during execution of the loop
  if (RoastTime.isRunning()) {
    RoastMinutes = ((double)RoastTime.elapsed()) / 60;
  }

  //update temps
  if (ReadTempFlag > -1) {
    //Serial.println("B1:");Serial.print(TBean1);Serial.println("B2:");Serial.print(TBean2);Serial.print("C:");Serial.println(TCoil);
    switch (ReadTempFlag) {
      case 0://coil
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
        if (TBean1 = -1) {
          TBean1 = getCleanTemp(thermocouple1.readFahrenheit(), 1);
        }
        ReadTempFlag++;
        break;
      case 2:
        TBean2 = getCleanTemp(thermocouple2.readFahrenheit(), 2);
        //SpDebug("New bean 2 temp" + String(TBean2));
        if (TBean2 = -1) {
          TBean2 = getCleanTemp(thermocouple2.readFahrenheit(), 2);
        }
        TBeanAvgThisRun = getBeanAvgTemp(TBean1, TBean2);
        if (TBeanAvgThisRun > -1) {
          TBeanAvgRoll.push(TBeanAvgThisRun);
        }
        ReadTempFlag = -1;
        long t = MeasureTempTimer.elapsed();

        MeasureTempTimer.stop();
        //Serial.print("New temps time:");Serial.println(t);
        //Serial.print(F("bean1:"));Serial.print(TBean1);Serial.print(F(",bean2:"));Serial.print(TBean2);
        bNewTempsAvailable = true;
        break;
    }
  }

  //DETERIM NEW STATE BASE ON STATE TEMPERATURE or TIME
  switch (State) {
    case STATEROASTING: {
        if (TBeanAvgRoll.mean() > (MySetPoints[EndingSetPoint].Temperature) + 20) {
          TempReachedCount++;
          if (TempReachedCount > 20) {
            newState = STATECOOLING;
            //Serial.println("Roast Temp Reached. Cooling starting End:");
          }
        } else {
          TempReachedCount = 0;
        }
        if (RoastMinutes > MySetPoints[EndingSetPoint].Minutes) {
          newState = STATECOOLING;
          Serial.println(F("Max time reached. Cooling starting"));
        }
        break;
      }
    case STATECOOLING: {
        if (TBeanAvgRoll.mean() < TEMPCOOLINGDONE) {
            newState = STATESTOPPED;
            Serial.println(F("Auto Cooling Complete "));
        }
        else if (RoastMinutes < MySetPoints[EndingSetPoint].Minutes & RoastRestartNeeded)
        {
          newState = STATEROASTING;
          RoastRestartNeeded =false;
          Serial.println(F("RestartRoasting request detected"));
       }
      break;
      }
  }
  //determin action based on input from touch or serial
  if (HasDisplay) {
    if (myTouch.dataAvailable()) {
      //Serial.println("touch");
      if (TouchDetected == false) {
        if (DetectTouch()) {
          //Serial.println("PRESS");
          TouchDetected = true;
          TouchTimer.restart(0);
        }
      } else {
        //Serial.println("B");

        if (TouchTimer.elapsed() > 1500) {
          if (LongPressDetected == false) {
            LongPressDetected = true;
            //Serial.println("LONGPRESS");

            TouchLongPress();
          }
        }
      }
    }

    else {
      if (TouchDetected == true) {
        TouchDetected = false;

        if (LongPressDetected == false) {


          if (TouchTimer.elapsed() > 100) {
            //Serial.println("CLICK!");

            TouchClick();
          }

        } else {
          LongPressDetected = false;
          //Serial.println("Long press release");
        }
        OutlineMenuButton(TouchButtonSet, TouchButton, BLACK);
        TouchTimer.stop();
      }
      TouchTimer.stop();
    }
  }
  if (Serial.available() > 0) {
    processSerial(Serial);
  }
  if (Serial2.available() > 0) {
    processSerial(Serial2);
  }

  //Action is state is changing
  if (newState != 0 )
  {
    Serial.print("Moving  to new state:");Serial.println(StateName[newState]);
  
  }
  switch (newState) {
    case STATESTOPPED: //newstate
      {
        //Serial.print("D:");Serial.println(LOW);

        digitalWrite(SSR1_p7, LOW);
        digitalWrite(SSR2_p6, LOW);
        if (TBeanAvgRoll.mean() < TEMPCOOLINGDONE) {
          State = STATESTOPPED;
          //Serial.print("E:");Serial.println(LOW);

          digitalWrite(SSR1_p7, LOW);
          digitalWrite(SSR2_p6, LOW);
          digitalWrite(FANRELAYp_2, RELAYOFF);
          //digitalWrite(VIBRELAYp, RELAYOFF);
          RoastTime.stop();
          //  FanSpeedPWMAutoMode = false;
          FanSpeedPWM = 0;
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
        //digitalWrite(VIBRELAYp, RELAYON);


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
          StartLinebyXAndY(FanGraphXStart, YforAFan(FanSpeedPWM), FANSPEEDLINEID);
          graphProfile();
          RoastTime.restart(0);
          RoastMinutes = 0;
          RoastRestartNeeded =false;
        } 
        else if (State == STATECOOLING) {
          //nothing is needed
        }
        State = STATEROASTING;
        break;
      }

    case STATECOOLING: //newstate
      {
        State = newState;
        SetAndSendFanPWMForATime(RoastMinutes);
        //FanSpeedPWMAutoMode = false;
        //Serial.print("F:");Serial.println(LOW);

        digitalWrite(SSR1_p7, LOW);
        digitalWrite(SSR2_p6, LOW);
        delay(1000);
        break;
      }
    case STATEFANONLY: //newstate
      {
        State = newState;
        FanSpeedPWM = FanSetPoints[0].PWM;
        SetAndSendFanPWMForATime(0);
        digitalWrite(FANRELAYp_2, RELAYON);
        break;
      }
    case STATEOVERHEATED: //newstate
      {
        State = newState;
        digitalWrite(SSR1_p7, LOW);
        digitalWrite(SSR2_p6, LOW);
        delay(1000);
        break;
      }
    case STATENOFANCURRENT: //newstate
      {
        State = newState;
        digitalWrite(SSR1_p7, LOW);
        digitalWrite(SSR2_p6, LOW);
        break;
      }
    case DEBUGCOIL: //newstate
      State = newState;
    case DEBUGTOGGLE: //newstate
      State = newState;
      break;
    case DEBUGDUTY: //newstate
      State = newState;
      break;
  }

  //Action base on state

  if (State == DEBUGTOGGLE || State == DEBUGCOIL) {
    SetAndSendFanPWMForATime(0);
  }

  //calculate new PID for coils
  if (State == STATEROASTING) {
    //if (false){
    //Serial.println("A");
    //Err, Duty, DutyRaw,.mean(), integralsum, integral,CurrentSetpointTemp all floats
    //only gain is an Int
    CurrentSetPointTemp = SetpointforATime(RoastMinutes);
    Err = CurrentSetPointTemp - TBeanAvgRoll.mean();  //negative if temp is over setpoint. Positive it temp is under setupt
    PIDIntegralUdateTimeValue = 5000;
    if (abs(Err) < Gain) {
      if (PIDIntegralUdateTime.elapsed() > PIDIntegralUdateTimeValue) {  //every 5 seconds we add the err to be a sum of err
        if (ErrI < Gain) {
          IntegralSum = IntegralSum + Err;
          if (IntegralSum < 0) {
            IntegralSum = 0;
          }
          ErrI = (IntegralSum * Integral);  //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
          //Serial.println("Isum:");Serial.println(IntegralSum);Serial.println("ErrI:");Serial.println(ErrI);
          PIDIntegralUdateTime.restart(0);
        }
      }
      Duty = ((Err + ErrI) / (double)Gain);

      if (Duty > 1.0) {
        Duty = 1.0;
      }

    } else {  //clear out the integral before set point 1.
      Duty = 1.0;
      //  ErrI = 0;
      //  IntegralSum= 0;
      //  PIDIntegralUdateTime.restart(0);
      //   IntegralLastTime = 0;
    }
  } else {
    ErrI = 0.0;
    IntegralSum = 0.0;
  }

  //set SSR of coils
  if (State == STATEROASTING || State == DEBUGDUTY) {
    int SSR1 = LOW;
    int SSR2 = LOW;
    //APPLY THE ERROR WITH THE PID WINDOW
    PIDWindowSize = 1000;
    unsigned long now = millis();
    boolean ExceedsWholePidWindow = (PIDWindowStartTime == 0) || (now - PIDWindowStartTime > PIDWindowSize);
    if (ExceedsWholePidWindow) {  //keep checking if we need to start a new PID window
      PIDWindowStartTime = now;
      if (Duty > 0.0) {
        SSR1 = HIGH;
      }
      if (Duty > 0.5) {
        SSR2 = HIGH;
      }
    } else {
      if (Duty <= 0.5 && ((now - PIDWindowStartTime) <= (Duty * 2 * PIDWindowSize))) {
        SSR1 = HIGH;
        //Serial.println("SSR1 is high");
      }
      if (Duty >= 1.0) {
        SSR1 = HIGH;
        //Serial.println("SSR1 is high");

        SSR2 = HIGH;
      }
      if (Duty > 0.5) {
        SSR1 = HIGH;
        if ((now - PIDWindowStartTime) <= ((Duty - .5) * 2 * PIDWindowSize)) {
          SSR2 = HIGH;
        }
      }
    }

    if (TCoil > TEMPCOILTOOHOT) {
      if (TEMPCOILTOOHOTCount > 10) {
        bNewSecond = true;  //force display immediately

        newerrmsg == true;
        errmsg = "HOT COIL CUTOUT";
        //Serial.println("too hot");
      }
      TEMPCOILTOOHOTCount++;
      SSR1 = LOW;
      SSR2 = LOW;
    } else {

      if (TEMPCOILTOOHOTCount > 0) {
        if (errmsg == "HOT COIL CUTOUT") {
          newerrmsg == true;
          errmsg = "";
        }

        TEMPCOILTOOHOTCount = 0;
      }
    }

    digitalWrite(SSR1_p7, SSR1);
    digitalWrite(SSR2_p6, SSR2);
  }

  //make sure to turn stuff off as default
  if (not(State == STATEROASTING || State == DEBUGDUTY || State == DEBUGTOGGLE || State == DEBUGCOIL)) {

    //Serial.println("not roastine is off");
    //Serial.println("B:LOW");

    digitalWrite(SSR1_p7, LOW);
    digitalWrite(SSR2_p6, LOW);
    Duty = 0;
    Err = 0;
    CurrentSetPointTemp = 0;
    IntegralSum = 0;
    IntegralLastTime = 0;
  }

  //What to output to UI real time
  if (newState > 0) {
    UpdateStateDisplayArea(ValuesOnly);
    newState = 0;  //this clears it for next run
  }

  //what to output to UI each second
  if (bNewSecond) {
    //Serial.println("update after reach new temp");
    UpdateStateDisplayArea(ValuesOnly);
    UpdateTempDisplayArea(ValuesOnly);
    UpdateFanPWMValuesDisplay();
    if (serialOutPutTempsBySecond == true) {
      SerialOutputTempsForPlotting();
    }
    if (serialOutPutStatusBySecond == true) {

      SerialOutputStatus();
    }
    if (State == STATEROASTING || State == DEBUGDUTY || State == STATECOOLING) {
      AddLinebyTimeAndFan(RoastMinutes);
    }
  }

  if (bNewTempsAvailable)
  {
    if (State == STATEROASTING || State == DEBUGDUTY || State == STATECOOLING) {
      AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.maximum(), ROLLMAXLINEID);
      AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.minimum(), ROLLMINLINEID);
      AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.mean(), ROLLAVGLINEID);
      AddPointbyTimeAndTempAndLineID(RoastMinutes, TCoilRoll.mean(), COILLINEID, 2);
    }
  }
  //what to output to UI each 3 seconds
  if (LcdUdateTime.elapsed() > 3000) {
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
      SetAndSendFanPWMForATime(RoastMinutes);
      //SetAndSendFanPWMForATime(FanSpeedPWNDecreaseByMinutes);
    }
    UpdateProgessDisplayArea(ValuesOnly);
    LcdUdateTime.restart(0);
  }

} //end of loop

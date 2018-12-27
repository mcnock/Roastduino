
// **************************************************************************************************************************************************************
// LOOP A   LOOP A   LOOP A   LOOP  A  LOOP A   LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP
// **************************************************************************************************************************************************************
void theloop () {


  ReadSerial(Serial,SerialInputTimer);//for debugging

  ReadSerial(Serial1,Serial1InputTimer);//for blue tooth
  //per loop variables

  int minuteToStart = 0;
  boolean bNewSecond = false;
  RoastMinutes = ((double)RoastTime.elapsed()) / 60;
  //Serial.println(roastMinutes);
  if (FlashTimer.elapsed() > 100) {
    if (Flasher == false) {
      digitalWrite(LEDp, HIGH);
      Flasher = true;
    }
    else
    { digitalWrite(LEDp, LOW);
      Flasher = false;
    }
    FlashTimer.restart(0);
  }

  if (SecondTimer.elapsed() > 1000) {
    if (bNewSecond == true) {
      //the program is not running correctly.  Shut the relays down
      digitalWrite(SSR1p, LOW);
      digitalWrite(SSR2p, LOW);
      digitalWrite(Buzzerp, LOW); digitalWrite(LEDp, HIGH); //error on
    }
    else {
      digitalWrite(Buzzerp, HIGH);  //error off
      digitalWrite(LEDp, LOW);
      bNewSecond = true;//Serial.print("LoopsPerSec:");Serial.println(LoopsPerSecond);
      LoopsPerSecond = 0;
      SecondTimer.restart(0);
    }
  }
  else {
    LoopsPerSecond ++;
  }
  TSPoint p = ts.getPoint();
  if (p.z > MINTOUCHPRESSURE && p.z < MAXTOUCHPRESSURE) {
    ProcessRawTouch (&p);
  }

  //**********************************************************************************************************************************
  //read temperatures and amps    B         read temperatures and amps   B             read temperatures and amps     B         read temperatures and amps
  //********************************************************************************************************************************
  float tempraw;

  //we read current each loop since it is fast
  int tempread;

  MaxVread = analogRead(VOLT5ap);
  MaxVoltage = (((double)MaxVread) / 1024) * 5;
  // the center of the max voltage is 0
  tempread = analogRead(CURFANap);
  //Serial.print("volt:");Serial.print(maxVread);Serial.print("fan:");Serial.println(tempread);
  //185 millamps per volt
  CurrentFan = (((double)(tempread - (MaxVread / 2) ) * 5)) / 125;
  if (CurrentFan < 0) {
    CurrentFan = 0;
  }
  AvgFanCurrent.push(CurrentFan); CurrentFan = AvgFanCurrent.mean();
  CurrentHeat1 = (((double)(analogRead(CURHEAT1ap) - (MaxVread / 2)) * 5)) / 100; AvgCoil1Amp.push(CurrentHeat1 * CurrentHeat1);
  CurrentHeat2 = (((double)(analogRead(CURHEAT2ap) - (MaxVread / 2)) * 5)) / 100; AvgCoil2Amp.push(CurrentHeat2 * CurrentHeat2);
  CurrentHeat1 = sqrt( AvgCoil1Amp.mean());
  CurrentHeat2 = sqrt( AvgCoil2Amp.mean());


  if (bNewSecond) { //we speed up loop per sec by reading temps once per second.  reading temps is very slow.
    TCoil =  getCleanTemp(thermocouple1.readFahrenheit(), 1);
    //Serial.print("Coil teamp:");Serial.println(TCoil);
    //TFan =   getCleanTemp(thermocouple4.readFahrenheit(), 4);
    TBean1 = getCleanTemp(thermocouple2.readFahrenheit(), 2);
    TBean2 = getCleanTemp(thermocouple3.readFahrenheit(), 3);
    TBeanAvg = getBeanAvgTemp(TBean1, TBean2);
    //Serial.print(TBean1);Serial.print (TBean2);Serial.print(TCoil);Serial.println(TFan);

    if (TCoil > 100) {
      TCoilRoll.push(TCoil);
    }






    double newtempratiotoaverage;
    if (TBeanAvgRoll.getCount() > 1) {
      newtempratiotoaverage = TBeanAvg / TBeanAvgRoll.mean();
    }
    else {
      newtempratiotoaverage = 1;
      Readingskipped++;
    }

    if    ( (newtempratiotoaverage > .7 &&  newtempratiotoaverage < 1.2)  || TBeanAvgRoll.mean() < 200  || RoastMinutes < 1 )  {
      TBeanAvgRoll.push(TBeanAvg);
    }
    else {
      Readingskipped++;
      //Serial.println("out of range:");
      //Serial.print("TBean2:");Serial.print(TBean2);Serial.print(" TBean1:");Serial.print(TBean1);
      //Serial.print(" avg:");Serial.print(TBeanAvgRoll.mean());Serial.print(" TCoil:");
      //Serial.print(TCoil);Serial.print(" TFan:");Serial.println(TFan);

    }
  }

  //**************************************************************************************************
  //DETERIM NEW STATE BASE ON TEMPERATURE or TIME
  //*****************************************************************************************************
  if (State == STATEROASTING) {
    if (TCoil > OVERHEATCOIL) {
      OVERHEATCOILCount++;
      if (OVERHEATCOILCount > 20) {
        newState = STATEOVERHEATED;
      }
    }
    else {
      OVERHEATCOILCount = 0;
    }
    if ( TBeanAvgRoll.mean() > MySetPoints[EndingSetPoint].Temperature) {
      TempReachedCount ++;
      if (TempReachedCount > 20) {
        newState = STATECOOLING;
        Serial.println("Roast Temp Reached. Cooling starting End:");
      }
    }
    else {
      TempReachedCount = 0;
    }

    if ( RoastMinutes > MySetPoints[EndingSetPoint].Minutes) {
      newState = STATECOOLING;
      Serial.println("Max time reached. Cooling starting");
    }
  }
  else if (State == STATECOOLING && TBeanAvgRoll.mean() < TEMPCOOLINGDONE ) {
    newState = STATESTOPPED;
    //Serial.println("Auto Cooling Complete ");
  }


  // **************************************************************************************************************************************************************
  //APPLY NEW STATE     E      APPLY NEW STATE      E     APPLY NEW STATE   E        APPLY NEW STATE      E     APPLY NEW STATE   E        APPLY NEW STATE           APPLY NEW STATE
  // **************************************************************************************************************************************************************
  if (newState == STATESTOPPED) {
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    if (TBeanAvgRoll.mean() < TEMPCOOLINGDONE ) {
      State = STATESTOPPED;
      digitalWrite(FANRELAYp, RELAYOFF); digitalWrite(VIBRELAYp, RELAYOFF);
      RoastTime.stop();
    }
    else {
      newState = STATECOOLING;
      State = STATECOOLING;
    }
  }
  else if (newState == STATEROASTING) {
    Serial.print("starting roasting with end temp of ");
    Serial.print(MySetPoints[EndingSetPoint].Temperature);
    Serial.print(" and end time of ");
    Serial.println(MySetPoints[EndingSetPoint].Minutes);
    
    digitalWrite(FANRELAYp, RELAYON); digitalWrite(VIBRELAYp, RELAYON);
    if (State == STATESTOPPED || State == STATEFANONLY) {
      TCoilRoll.clear();
      TBeanAvgRoll.clear();
      //Serial.println("Starting Fans and Vibration - and waiting 5 seconds");
      delay(2000);
      Readingskipped = 0;
      StartLinebyTimeAndTemp(0, 0, AVGLINEID , BLUE);
      StartLinebyTimeAndTemp(0, 0, ROLLAVGLINEID , YELLOW);
      StartLinebyTimeAndTemp(0, 0, COILLINEID , RED);
      graphProfile();
      delay(2000);
      //Serial.println("2 Starting Heaters ");
      RoastTime.restart(minuteToStart * 60);
      RoastMinutes = ((double)RoastTime.elapsed()) / 60;
    }
    State = STATEROASTING;
  }
  else if (newState == STATECOOLING) {
    State = STATECOOLING;
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    delay(1000);
  }
  else if (newState == STATEFANONLY) {
    State = STATEFANONLY;
    digitalWrite(FANRELAYp, RELAYON); digitalWrite(VIBRELAYp, RELAYON);
  }
  else if (newState == STATEOVERHEATED) {
    State = STATEOVERHEATED;
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    delay(1000);
  }
  else if (State == STATENOFANCURRENT) {
    State = STATENOFANCURRENT;
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
  }


  //***************************************************************************************************************************************************************
  // CALCULATING PID VALUES     F    CALCULATING PID VALUES     F    CALCULATING PID VALUES    F     CALCULATING PID VALUES    F     CALCULATING PID VALUES
  //*************************************************************************************************************************************************************
  //pid window size should vary based on duty cycle. 10 millsec and 50% would to 20 millisecond.  10% would be 100 milliseconds.  01 % would be 1 seconds
  //but we run 9 times per second, so shortest on off is 1 cycles or ~ 100 millseconds.   50% means .2 seconds, 10% means 1 seconds  .05% means 2 seconds

  if (State == STATEROASTING) {
    //CALC THE ERR AND INTEGRAL
    CurrentSetPointTemp =  SetpointforATime(RoastMinutes);
    Err = CurrentSetPointTemp - TBeanAvgRoll.mean();  //negative if temp is over setpoint
    //if (bNewSecond) {Serial.print(" new calc of err:");Serial.println(err);    };
    PIDIntegralUdateTimeValue = 5000;
    Dutyraw = ((double)(Err) / (double)Gain) ;
    if (RoastMinutes > MySetPoints[1].Minutes ) { //only calc intergral error if we are above the 1st setpoint
      if (PIDIntegralUdateTime.elapsed() > PIDIntegralUdateTimeValue) { //every 3 seconds we add the err to be a sum of err
        if (Dutyraw < 1 && abs(ErrI) < Gain) {
          //only add/remove from the integral if reasonable
          IntegralSum =  IntegralSum + double(Err);
          ErrI = (IntegralSum * Integral) ; //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
          //Serial.print("Isum:");Serial.print(IntegralSum);Serial.print("ErrI:");Serial.println(ErrI);

          
          
          PIDIntegralUdateTime.restart(0);
        }
      }
    }
    else { //clear out the integral before set point 1.
      PIDIntegralUdateTime.restart(0);
      IntegralSum = 0;
      IntegralLastTime = 0;
      ErrI = 0;
    }

    Duty = ((double)(Err + ErrI) / (double)Gain) ;
    //APPLY THE ERROR WITH THE PID WINDOW
    PIDWindowSize = 1000;
    unsigned long now = millis();
    int SSR1 = LOW;
    int SSR2 = LOW;
    boolean ExceedsWholePidWindow = (PIDWindowStartTime == 0) || (now - PIDWindowStartTime > PIDWindowSize);
    if (ExceedsWholePidWindow) { //keep checking if we need to start a new PID window
      PIDWindowStartTime = now;
      if (Duty > 0) {
        SSR1 = HIGH;
      }
      if (Duty > 0.5) {
        SSR2 = HIGH;
      }
    }
    else {
      if ( Duty <= 0.5 && ((now - PIDWindowStartTime) <=  (Duty  * 2 * PIDWindowSize))) {
        SSR1 = HIGH;
      }
      if ( Duty > 0.5 ) {
        SSR1 = HIGH;
        if ((now - PIDWindowStartTime) <=  ( (Duty - .5) * 2 *  PIDWindowSize)) {
          SSR2 = HIGH;
        }

      }
    }
    digitalWrite(SSR1p, SSR1);
    digitalWrite(SSR2p, SSR2);
  }

  else {
    //Serial.println("not roastine is off");
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    Duty = 0;
    Dutyraw = 0;
    Err = 0;
    CurrentSetPointTemp = 0;
    IntegralSum = 0;
    IntegralLastTime = 0;

  }

  //********************************************************************************************************************************
  //LCD AND GRAPH DISPLAY    G     LCD AND GRAPH DISPLAY   G      LCD AND GRAPH DISPLAY    G     LCD AND GRAPH DISPLAY    G     LCD AND GRAPH DISPLAY
  //*******************************************************************************************************************************
  //its slow to update th

  if (newState > 0) {
    //Serial.println("newstate detected  Will update tft immediately");
    displayState(State);  newState = 0;
  }

  //we update the area when we get new temps
  if    (bNewSecond) {
    //Serial.println("update after reach new temp");
    displayState(State);

    UpdateGraphA();

    AddPointbyTimeAndTempAndLineID(RoastMinutes, TBeanAvg, AVGLINEID, 1);

  }

  if (LcdUdateTime.elapsed() > 3000) {
    //Serial.print("slow update. once per:");Serial.println(3000);Serial.println(" millseconds");
    // void UpdateGraphB(int temp1, int temp2, int tempCoil, double ampHeater1, double ampHeater2, int tempFan, double ampFan, double volts)
    //Serial.print("TBean2:");Serial.print(TBean2);Serial.print("TBean1:");Serial.print (TBean1);Serial.print("TCoil:");Serial.print(TCoil);Serial.print("TFan:");Serial.println(TFan);
    AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.mean(), ROLLAVGLINEID);
    AddPointbyTimeAndTempAndLineID(RoastMinutes, TCoilRoll.mean(), COILLINEID, 2);

    //UpdateGraphB(TBean2, TBean1, TCoil, CurrentHeat1, CurrentHeat2, TFan, CurrentFan, MaxVoltage);

    UpdateGraphB();

    UpdateGraphC();
    LcdUdateTime.restart(0);
  }

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
//      END LOOP  H      END LOOP      H    END LOOP    H      END LOOP    H      END LOOP   H       END LOOP     H     END LOOP    H      END LOOP
//----------------------------------------------------------------------------------------------------------------------------------------------------

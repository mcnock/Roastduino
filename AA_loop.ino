


// **************************************************************************************************************************************************************
// LOOP A   LOOP A   LOOP A   LOOP  A  LOOP A   LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP
// **************************************************************************************************************************************************************

void theloop () {
  //Allways do this in a loop
   boolean bNewSecond = false;
   newState = 0;
   
  if (SecondTimer.elapsed() > 1000) {   
      digitalWrite(LEDp, LOW);
      bNewSecond = true;
      LoopsPerSecond = 0;
      SecondTimer.restart(0);
    }
  else {
    bNewSecond = false;
    LoopsPerSecond ++;
  }

//update roast time
 if (State == STATEROASTING || State == STATECOOLING)
 {
    RoastMinutes = ((double)RoastTime.elapsed()) / 60;
    //Serial.println(RoastMinutes);
 } 

  
  //**********************************************************************************************************************************
  //read temperatures and amps    B         read temperatures and amps   B             read temperatures and amps     B         read temperatures and amps
  //******************************************************************************************************************************** 
 //read pressure
  FanPressureRoll.push(((double)analogRead(fanPressurep)-538.00)/538*2.2);
   //we read current each loop since it is fast 
    //Serial.print("fan:");Serial.println(tempread);
  //185 millamps per volt
  //DC
  CurrentFan = ((((double)(analogRead(CURFANap) - (MaxVread / 2)) * 5)) / 125) ; 
  AvgFanCurrent.push(CurrentFan); 
  CurrentFan = AvgFanCurrent.mean() - CurrentFanOffset;
  if (CurrentFan < 0){
    CurrentFan =0;
  }
  CurrentHeat1 = (((double)(analogRead(CURHEAT1ap) - (MaxVread / 2)) * 5)) / 100; AvgCoil1Amp.push(CurrentHeat1 * CurrentHeat1);
  CurrentHeat2 = (((double)(analogRead(CURHEAT2ap) - (MaxVread / 2)) * 5)) / 100; AvgCoil2Amp.push(CurrentHeat2 * CurrentHeat2);
  CurrentHeat1 = sqrt( AvgCoil1Amp.mean()) - CurrentHeat1Offset;
  CurrentHeat2 = sqrt( AvgCoil2Amp.mean()) - CurrentHeat2Offset;
  if (bNewSecond) { //we speed up loop per sec by reading temps once per second.  reading temps is very slow.
    TCoil = getCleanTemp(thermocouple4.readFahrenheit(), 1);
    //Serial.println("Coil teamp:");Serial.println(TCoil);
    TCoilRoll.push(TCoil);
    

    
    TBean3 =   getCleanTemp(thermocouple4.readFahrenheit(), 4);
    TBean1 = getCleanTemp(thermocouple2.readFahrenheit(), 2);
    TBean2 = getCleanTemp(thermocouple3.readFahrenheit(), 3);
    TBeanAvg = getBeanAvgTemp(TBean1, TBean2);
    //Serial.println("B1:");Serial.print(TBean1);Serial.println("B2:");Serial.print(TBean2);Serial.print("C:");Serial.println(TCoil);
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
      //Serial.println("TBean2:");Serial.println(TBean2);Serial.println(" TBean1:");Serial.println(TBean1);
      //Serial.println(" avg:");Serial.println(TBeanAvgRoll.mean());Serial.println(" TCoil:");
      //Serial.println(TCoil);Serial.println(" TFan:");Serial.println(TFan);

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
  else if (State == STATECOOLING){
    if (TBeanAvgRoll.mean() < TEMPCOOLINGDONE ) {
      newState = STATESTOPPED;
    //Serial.println("Auto Cooling Complete ");
    
    }
  }

  
  
  //ProcessButton Clicks/find user new state requests
  if (myTouch.dataAvailable())
  {
      myTouch.read();
      int16_t x = myTouch.getX();
      int16_t y = myTouch.getY();
      ProcessTouch (x,y);
  }
  



  // **************************************************************************************************************************************************************
  //APPLY NEW STATE     E      APPLY NEW STATE      E     APPLY NEW STATE   E        APPLY NEW STATE      E     APPLY NEW STATE   E        APPLY NEW STATE           APPLY NEW STATE
  // **************************************************************************************************************************************************************

 switch (newState){
  case STATESTOPPED: {
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    if (TBeanAvgRoll.mean() < TEMPCOOLINGDONE ) {
      State = STATESTOPPED;
      digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
      digitalWrite(FANRELAYp, RELAYOFF); digitalWrite(VIBRELAYp, RELAYOFF);
      RoastTime.stop();
      FanSpeedPWMAutoMode = false;
      FanSpeedPWM = 0;
    }
    else {
      newState = STATECOOLING;
      State = STATECOOLING;
    }
    break;
   }
 case STATEROASTING: {
    //Serial.println("D");
    digitalWrite(FANRELAYp, RELAYON); digitalWrite(VIBRELAYp, RELAYON);
    if (State == STATESTOPPED || State == STATEFANONLY) {
      if (FanSpeedPWM > 0)
      {   
          FanSpeedPWMStart = FanSpeedPWM ;      
          EEPROM.write(FANSPEED_EP,FanSpeedPWMStart);
      }
      SetFanPWMForATime(0);
      FanSpeedPWMAutoMode = true;
      //CurrentHeat1Offset = CurrentHeat1Offset + CurrentHeat1;
      AvgCoil1Amp.clear();
      //]CurrentHeat2Offset = CurrentHeat2Offset + CurrentHeat2;
      AvgCoil2Amp.clear(); 
      TCoilRoll.clear();
      TBeanAvgRoll.clear();
      //delay(1000);
      Readingskipped = 0;
      StartLinebyTimeAndTemp(0, 0, AVGLINEID , BLUE);
      StartLinebyTimeAndTemp(0, 0, ROLLAVGLINEID , LGBLUE);
      StartLinebyTimeAndTemp(0, 0, COILLINEID , RED);
      graphProfile();
      //delay(1000);
      //Serial.println("2 Starting Heaters ");
      RoastTime.restart(0);
      RoastMinutes = 0;
      //RoastMinutes = ((double)RoastTime.elapsed()) / 60;
    }
    else if (State == STATECOOLING) {
      //nothing is needed
    }
    State = STATEROASTING;
    break;
  }
 
  case STATECOOLING: {
    State = newState;
    SetFanPWMForATime(FanSpeedPWNDecreaseByMinutes - 2);
    FanSpeedPWMAutoMode = false;

    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    delay(1000);
    break;
  }
  case STATEFANONLY: {
    State =  newState;
    FanSpeedPWM = FanSpeedPWMStart ;
    CurrentFanOffset = CurrentFanOffset + CurrentFan ;    
    AvgFanCurrent.clear();   
    SetFanPWMForATime(0);   
    //Serial.println("VIB on");
    digitalWrite(VIBRELAYp, RELAYON);
    //Serial.println("Fan on");
    digitalWrite(FANRELAYp, RELAYON); 
    break;
    }
  case STATEOVERHEATED: {
    State =  newState;
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    delay(1000);
        break;
  }
  case STATENOFANCURRENT: {
    State =  newState;
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
        break;
  }
  case DEBUGTOGGLE: 
      State = newState;
     break;
  case DEBUGDUTY:
     State = newState;
     break;
 }
 


 //DO STATE SPECIFIC ACTION
  //***************************************************************************************************************************************************************
  // CALCULATING PID VALUES     F    CALCULATING PID VALUES     F    CALCULATING PID VALUES    F     CALCULATING PID VALUES    F     CALCULATING PID VALUES
  //*************************************************************************************************************************************************************
  //pid window size should vary based on duty cycle. 10 millsec and 50% would to 20 millisecond.  10% would be 100 milliseconds.  01 % would be 1 seconds
  //but we run 9 times per second, so shortest on off is 1 cycles or ~ 100 millseconds.   50% means .2 seconds, 10% means 1 seconds  .05% means 2 seconds
 if (State == DEBUGTOGGLE){
    SetFanPWMForATime(0);   
  

 }
 
 
 if (State == STATEROASTING) {
    //CALC THE ERR AND INTEGRAL
    //Serial.println("A");
    CurrentSetPointTemp =  SetpointforATime(RoastMinutes);
    Err = CurrentSetPointTemp - TBeanAvgRoll.mean();  //negative if temp is over setpoint
    //if (bNewSecond) {Serial.println(" new calc of err:");Serial.println(err);    };
    PIDIntegralUdateTimeValue = 5000;
    Dutyraw = ((double)(Err) / (double)Gain) ;
    if (RoastMinutes > MySetPoints[1].Minutes ) { //only calc intergral error if we are above the 1st setpoint
      if (PIDIntegralUdateTime.elapsed() > PIDIntegralUdateTimeValue) { //every 3 seconds we add the err to be a sum of err
        if (Duty < 1 && ErrI < Gain ) {
          IntegralSum =  IntegralSum + double(Err);
          ErrI = (IntegralSum * Integral) ; //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
          //Serial.println("Isum:");Serial.println(IntegralSum);Serial.println("ErrI:");Serial.println(ErrI);          
          PIDIntegralUdateTime.restart(0);
        }
      }
      Duty = ((double)(Err + ErrI) / (double)Gain) ;
    }
    else { //clear out the integral before set point 1.
      Duty = Dutyraw;     
      ErrI = 0;
      IntegralSum= 0;
      PIDIntegralUdateTime.restart(0);
      IntegralLastTime = 0;
    }
  }
  
 if (State == STATEROASTING || State == DEBUGDUTY) {
  
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
 
 if (not (State == STATEROASTING || State == DEBUGDUTY || State == DEBUGTOGGLE)) {
  
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
    //Serial.print("newstate detected  Will update tft immediately");
 
    UpdateRealTime(ValuesOnly);  
    newState = 0; //this clears it for next run
  }

  //we update the area when we get new temps
  if    (bNewSecond) {
    //Serial.println("update after reach new temp");
    UpdateRealTime(ValuesOnly);
    UpdateEachSecond(ValuesOnly);
    if (State == STATEROASTING || State == STATECOOLING){
          AddPointbyTimeAndTempAndLineID(RoastMinutes, TBeanAvg, AVGLINEID, 2);
    }
  }

  if (LcdUdateTime.elapsed() > 3000) {
    if (State == STATEROASTING || State == STATECOOLING){
        SetFanPWMForATime(RoastMinutes);
        DrawFanGraph();
        AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.mean(), ROLLAVGLINEID);
        AddPointbyTimeAndTempAndLineID(RoastMinutes, TCoilRoll.mean(), COILLINEID, 2);
    }
    UpdateFanPWMValues();
    UpdateGraphA();
    LcdUdateTime.restart(0);
  }

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
//      END LOOP  H      END LOOP      H    END LOOP    H      END LOOP    H      END LOOP   H       END LOOP     H     END LOOP    H      END LOOP
//----------------------------------------------------------------------------------------------------------------------------------------------------


//

// **************************************************************************************************************************************************************
// LOOP A   LOOP A   LOOP A   LOOP  A  LOOP A   LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP
// **************************************************************************************************************************************************************

void theloop () {
  newState = 0;
   
 //Allways do this in a loop
   boolean bNewSecond = false;
   
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


 //Serial.println("here AA");  
  //**********************************************************************************************************************************
  //read temperatures and amps    B         read temperatures and amps   B             read temperatures and amps     B         read temperatures and amps
  //******************************************************************************************************************************** 
 //read pressure
 // FanPressureRoll.push(((double)analogRead(fanPressurep)-538.00)/538*2.2);
   //we read current each loop since it is fast 
  //Serial.print("fan:");Serial.println(tempread);
  //185 millamps per volt
  //DC
  //CurrentFan = ((((double)(analogRead(CURFAN_A6) - (MaxVread / 2)) * 5)) / 125) ; 
  //AvgFanCurrent.push(CurrentFan); 
  //CurrentFan = AvgFanCurrent.mean() - CurrentFanOffset;
  //if (CurrentFan < 0){
  //  CurrentFan =0;
 // }

  //  int i1 = analogRead(CURHEAT1_A4);
 //   AvgCoil1Amp.push(i1);
 //   int i2 = analogRead(CURHEAT2_A5);
//    AvgCoil2Amp.push(i2);
    
    
  int TBeanAvgThisRun =0;
  if (bNewSecond == true || badLastTempCount > 0 ) { //we speed up loop per sec by reading temps once per second.  reading temps is very slow.
    TCoil = getCleanTemp(thermocouple4.readFahrenheit(), 4);
    //Serial.println("Coil teamp:");Serial.println(TCoil);
    TCoilRoll.push(TCoil);
  
    TBean3 = getCleanTemp(thermocouple1.readFahrenheit(), 1);
    TBean1 = getCleanTemp(thermocouple2.readFahrenheit(), 2);
    TBean2 = getCleanTemp(thermocouple3.readFahrenheit(), 3);
    
    if (VerticalMenuShowing == VMenuAdj_1_5_10_V && myButtonVertMenus[VMenuAdj_1_5_10_V].inputbutton == 8){  
        if (manualtemp == -1){
            TBeanAvgRoll.clear();
            if (TBeanAvgThisRun > 0){
                manualtemp = TBeanAvgThisRun;
            }
            else
            {
                manualtemp = 300;  
            }
        }
        else
        {
            TBeanAvgThisRun = manualtemp ;
        }        
    }
    else
    {
        TBeanAvgThisRun = getBeanAvgTemp(TBean1, TBean2);
        manualtemp = -1;
    }
    
    //Serial.println("B1:");Serial.print(TBean1);Serial.println("B2:");Serial.print(TBean2);Serial.print("C:");Serial.println(TCoil);
    double newtempratiotoaverage;
    if (TBeanAvgRoll.getCount() > 1) {
       newtempratiotoaverage = TBeanAvgThisRun / TBeanAvgRoll.mean();
    }
    else {
      newtempratiotoaverage = 1;
      
      Readingskipped++;
    }

    if    ( (newtempratiotoaverage > .95 &&  newtempratiotoaverage < 1.05)  || TBeanAvgRoll.mean() < 200  || RoastMinutes < 1 )  {
      TBeanAvgRoll.push(TBeanAvgThisRun);
      badLastTempCount = 0;
    }
    else {
      badLastTempCount++;
      Readingskipped++;
      //Serial.println("out of range:");
      //Serial.println("TBean2:");Serial.println(TBean2);Serial.println(" TBean1:");Serial.println(TBean1);
      //Serial.println(" avg:");Serial.println(TBeanAvgRoll.mean());Serial.println(" TCoil:");
      //Serial.println(TCoil);Serial.println(" TFan:");Serial.println(TFan);

    }
  }

  //Serial.println("here A");
  //**************************************************************************************************
  //DETERIM NEW STATE BASE ON TEMPERATURE or TIME
  //*****************************************************************************************************
  if (State == STATEROASTING) {
  
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
     Serial.println("Auto Cooling Complete ");
    
    }
  }

  
  
  //ProcessButton Clicks/find user new state requests
 // if (myTouch.dataAvailable() )
 // {
 //     myTouch.read();
 //     int16_t x = myTouch.getX();
 //     int16_t y = myTouch.getY();
 //     ProcessTouch (x,y);
 // }
  
  if (Serial.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial.read();
    int incomingByte2 = 0;
    int incomingByte3 = 0;
    delay(100);
    if (Serial.available() > 0) {
      incomingByte2 = Serial.read();
 
    }
    delay(100);
    
     if (Serial.available() > 0) {
      incomingByte3 = Serial.read();
    }  
    // say what you got:
   switch (incomingByte){
    case 63: //?
    { Serial.println("F:Fan[S,i,I,d,D]:Start,Incr,Decr; R:Roast[S,A,R] Start,AddMinute,RemoveMinute E:End s:GetStatus t:temperatures c:GetConfiguration P[i,I,d,D]:ProfileIncrDec]");
      
      break;
    }
    case 70: //F
    { 
     switch (incomingByte2){
     case 83: //S
       {
          Serial.print ("Fan On ");  
          ProcessHorControlMenu(2);
          Serial.println (FanSpeedPWM);
          break;
       }
     case 68: //i
        { Serial.print("Fan Decrease 10 ");
          ProcessHorFanMenu(0);
          Serial.println (FanSpeedPWM);
          break;
        }
     case 100:
        { Serial.print("Fan Decrease 3 ");
          ProcessHorFanMenu(1);
          Serial.println (FanSpeedPWM);

          break;
        }
      case 105:
        { Serial.print("Fan Increase 3 ");
          ProcessHorFanMenu(3);
          Serial.println (FanSpeedPWM);

          break;
        }
      case 73:
        { Serial.print("Fan Increase 10 ");
          ProcessHorFanMenu(3);
          Serial.println (FanSpeedPWM);
          break;
        } 
       default:{ 
      }
     }
       
     break;
    } 
    case 80: //P
    { 
      Serial.print("Profile before:");
       for (int x = 0; x < SetPointCount; x++) {
        Serial.print(x);
        Serial.print("-");
        Serial.print( MySetPoints[x].Minutes);
        Serial.print("-");
        Serial.print (MySetPoints[x].Temperature);
        Serial.print(":");
       }
       Serial.println("");

     switch (incomingByte2){
     case 68:
        { Serial.println("Profile Decrease 5 ");
          moveamount = -5;
          MoveLast3Point();
          saveChangedSetpoints();
          break;
        }
        case 100:
        { Serial.println("Profile Decrease 3 ");
          moveamount = -3;
          MoveLast3Point();
          saveChangedSetpoints();
          
          break;
        }
      case 105:
        { Serial.println("Profile Increase 3 ");
          moveamount = 3;
          MoveLast3Point();
          saveChangedSetpoints();
          
          break;
        }
      case 73:
        { Serial.println("Profile Increase 5 ");
          moveamount = 5;
          MoveLast3Point();
          saveChangedSetpoints();
          
       
          break;
        }
     }
      Serial.print("Profile  after:");
       for (int x = 0; x < SetPointCount; x++) {
        Serial.print(x);
        Serial.print("-");
        Serial.print( MySetPoints[x].Minutes);
       Serial.print("-");
        Serial.print (MySetPoints[x].Temperature);
        Serial.print(":");
       }
       Serial.println("");

      break;  
      
     }
       
     
    case 82: //R
    { 
       switch (incomingByte2){
       case 83: //S
       {
          Serial.println("Start Roast");
          ProcessHorControlMenu(0);
          break;
  
       }
      case 65: //A
      { Serial.println("Add a minute");
        ProcessBaseVMenu(7);  
        break;
      }
      case 82: //R
      { Serial.println("Remove a minute");
        ProcessBaseVMenu(6);  
        break;
      }
      
      }
    }
    case 69: //E
    { Serial.println("End Roast");
      ProcessHorControlMenu(1);
      break;
    }
     case 116: //t
    {  
       Serial.print("BeanAvg:");   
       Serial.print(TBeanAvgRoll.mean());
       Serial.print(" Bean1:");   
       Serial.print(TBean1);   
       Serial.print(" Bean2:");   
       Serial.print(TBean2);
       Serial.print(" Bean3");   
       Serial.print(TBean3); 
       Serial.print(" Coil Mean");   
       Serial.print(TCoilRoll.mean());        
       Serial.print(" Coil:");   
       Serial.print(TCoil);
       Serial.println();
      
       break;
    
      
     
    }
 
    case 115: //s
    {  
       Serial.print("Time:");   
       Serial.print(RoastMinutes);
       Serial.print("/");   
       Serial.print(MySetPoints[SetPointCount-1].Minutes);   
       Serial.print(" Bean Temp:");   
       Serial.print(TBeanAvgRoll.mean());
       Serial.print("/");   
       Serial.print(CurrentSetPointTemp); 
       Serial.print("/");   
       Serial.print(MySetPoints[SetPointCount-1].Temperature);        
       Serial.print(" Duty:");   
       Serial.print(Duty);
       Serial.print(" Coil Temp:");   
       Serial.print(TCoilRoll.mean());
       Serial.print(" FanPWM:");   
       Serial.print(FanSpeedPWM);
       Serial.print("/254");

       Serial.println();
       break;
    
      
     
    }
    case 99: //c
    {  

        Serial.print("Profile");
        for (int x = 0; x < SetPointCount; x++) {
        Serial.print(x);
        Serial.print("-");
        Serial.print( MySetPoints[x].Minutes);
        Serial.print("-");
        Serial.print (MySetPoints[x].Temperature);
        Serial.print(":");
       }
       Serial.println("");

      Serial.print ("G:"); Serial.print(Gain); Serial.print ("I:"); Serial.println(Integral);
      Serial.print ("FanDecrease DelayMin:"); Serial.print(FanSpeedPWNDelayDecreaseByMinutes); Serial.print ("Decrease:"); Serial.print(FanSpeedPWMAutoDecrease);Serial.print ("DecreaseMinutes"); Serial.println(FanSpeedPWNDecreaseByMinutes);

   
      
      break;
     
    }
    
    case 65: //A
    { Serial.println("Add a minute");
      ProcessBaseVMenu(7);  
      break;
    }
     default:
    {
    
      Serial.print("I received : ");
      Serial.println(incomingByte, DEC);
   
    

    
    }

    
   }
  incomingByte = 0;
  }

 


  // **************************************************************************************************************************************************************
  //APPLY NEW STATE     E      APPLY NEW STATE      E     APPLY NEW STATE   E        APPLY NEW STATE      E     APPLY NEW STATE   E        APPLY NEW STATE           APPLY NEW STATE
  // **************************************************************************************************************************************************************

 switch (newState){
  case STATESTOPPED:
  {
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
 case STATEROASTING: //newstate
 {
    //Serial.println("D");
    digitalWrite(FANRELAYp, RELAYON); digitalWrite(VIBRELAYp, RELAYON);
    if (State == STATESTOPPED || State == STATEFANONLY) {
      
      if (FanSpeedPWM > 0 && FanSpeedPWMStart != FanSpeedPWM)
      {   
          FanSpeedPWMStart = FanSpeedPWM ;      
          EEPROM.write(FanSpeedPWMStart_EP,FanSpeedPWMStart);
      }
      FanSpeedPWMAutoDecrease = EEPROM.read(FanSpeedPWMAutoDecrease_EP);
      FanSpeedPWMAutoDecreaseStart = FanSpeedPWMAutoDecrease;
      DrawFanGraph_ex(true);
      SetAndSendFanPWMForATime(0);
      FanSpeedPWMAutoMode = true;
      //CurrentHeat1Offset = CurrentHeat1Offset + CurrentHeat1;
      AvgCoil1Amp.clear();
      //]CurrentHeat2Offset = CurrentHeat2Offset + CurrentHeat2;
      AvgCoil2Amp.clear(); 
      TCoilRoll.clear();
      TBeanAvgRoll.clear();
      //delay(1000);
      Readingskipped = 0;
      StartLinebyTimeAndTemp(0, 0, AVGLINEID , YELLOW);
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
    SetAndSendFanPWMForATime(FanSpeedPWNDecreaseByMinutes - 2);
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
    SetAndSendFanPWMForATime(0);   
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
 case DEBUGCOIL:
      State = newState;
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
 if (State == DEBUGTOGGLE || State == DEBUGCOIL){
    SetAndSendFanPWMForATime(0);   
  

 }
 
 
 if (State == STATEROASTING) {
    //CALC THE ERR AND INTEGRAL
    //Serial.println("A");
    CurrentSetPointTemp =  SetpointforATime(RoastMinutes);
    Err = CurrentSetPointTemp - TBeanAvgRoll.mean();  //negative if temp is over setpoint
    //if (bNewSecond) {Serial.println(" new calc of err:");Serial.println(err);    };
    PIDIntegralUdateTimeValue = 5000;
    Dutyraw = ((double)(Err) / (double)Gain) ;
    //if (RoastMinutes > MySetPoints[1].Minutes ) 
    //if (Duty < 1 ) 
    if (abs(Dutyraw) < 1 )
    { //only calc intergral error if we are above the 1st setpoint
      if (PIDIntegralUdateTime.elapsed() > PIDIntegralUdateTimeValue) { //every 3 seconds we add the err to be a sum of err
        //if (Duty < 1 && ErrI < Gain ) {
        if (ErrI < Gain ) {
          IntegralSum =  IntegralSum + double(Err);
          ErrI = (IntegralSum * Integral) ; //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
          //Serial.println("Isum:");Serial.println(IntegralSum);Serial.println("ErrI:");Serial.println(ErrI);          
          PIDIntegralUdateTime.restart(0);
        }
      }
      Duty = ((double)(Err + ErrI) / (double)Gain) ;
    }
    else { //clear out the integral before set point 1.
      Duty = 1;     
    //  ErrI = 0;
    //  IntegralSum= 0;
    //  PIDIntegralUdateTime.restart(0);
   //   IntegralLastTime = 0;
    }
  }
  else
  {
      ErrI = 0;
      IntegralSum= 0;
    
  
  }
  
 if (State == STATEROASTING || State == DEBUGDUTY) {
    int SSR1 = LOW;
    int SSR2 = LOW;
    
 
    //APPLY THE ERROR WITH THE PID WINDOW
    PIDWindowSize = 1000;
    unsigned long now = millis();
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
      if (Duty >= 1.0)
      {
        SSR1 = HIGH;
        SSR2 = HIGH;
      }
      if ( Duty > 0.5 ) {
        SSR1 = HIGH;
        if ((now - PIDWindowStartTime) <=  ( (Duty - .5) * 2 *  PIDWindowSize)) {
          SSR2 = HIGH;
        }

      }
    }
    
    if (TCoil > TEMPCOILTOOHOT) {
      if (TEMPCOILTOOHOTCount == 0){
          bNewSecond = true; //force display immediately
          newerrmsg == true;
          errmsg = "HOT COIL CUTOUT";
     }
     TEMPCOILTOOHOTCount++;
    //Serial.println("too hot");
      SSR1 = LOW;
      SSR2 = LOW;
    }
    else
    {

      if (TEMPCOILTOOHOTCount > 0)
      {
            if (errmsg == "HOT COIL CUTOUT"){
                  newerrmsg == true;
                  errmsg = "";

              
            }
            
            TEMPCOILTOOHOTCount = 0;
      }
    
    }
    
    digitalWrite(SSR1p, SSR1);
    digitalWrite(SSR2p, SSR2);
 }
 
 if (not (State == STATEROASTING || State == DEBUGDUTY || State == DEBUGTOGGLE|| State == DEBUGCOIL)) {
  
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
       AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgThisRun, AVGLINEID);
    }
  }

  if (LcdUdateTime.elapsed() > 3000) {
    if ( State == STATECOOLING){
      
        SetAndSendFanPWMForATime(FanSpeedPWNDecreaseByMinutes - 2);
        AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.mean(), ROLLAVGLINEID);
        AddPointbyTimeAndTempAndLineID(RoastMinutes, TCoilRoll.mean(), COILLINEID, 2);
    }
    if (State == STATEROASTING ){
        SetAndSendFanPWMForATime(RoastMinutes);
        AddLinebyTimeAndTemp(RoastMinutes, TBeanAvgRoll.mean(), ROLLAVGLINEID);
        AddPointbyTimeAndTempAndLineID(RoastMinutes, TCoilRoll.mean(), COILLINEID, 2);
    }
    DrawFanGraph();
    UpdateDisplayDetailA(true);
    LcdUdateTime.restart(0);
  }

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
//      END LOOP  H      END LOOP      H    END LOOP    H      END LOOP    H      END LOOP   H       END LOOP     H     END LOOP    H      END LOOP
//----------------------------------------------------------------------------------------------------------------------------------------------------

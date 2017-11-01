#include "libraries\TFTLCD-Library-master\Adafruit_TFTLCD.h"
// **************************************************************************************************************************************************************
// LOOP A   LOOP A   LOOP A   LOOP  A  LOOP A   LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP    LOOP
// **************************************************************************************************************************************************************
void theloop () {

  //per loop variables
  int newState = 0;
  int minuteToStart = 0;
  boolean bNewSecond = false;
  double roastMinutes = ((double)RoastTime.elapsed()) / 60;
  //Serial.println(roastMinutes);
  if (FlashTimer.elapsed() > 100) {
      if (Flasher == false){
        digitalWrite(LEDp, HIGH);
        Flasher = true;
      }
      else
      {digitalWrite(LEDp, LOW);
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
  
      
      bNewSecond = true;// Serial.print("LoopsPerSec:");Serial.println(LoopsPerSecond);
      LoopsPerSecond = 0;
      SecondTimer.restart(0);
    
     }
  }
  else {
      LoopsPerSecond ++;
      
  }
       

  TSPoint p = ts.getPoint();
  if (p.z > MINTOUCHPRESSURE && p.z < MAXTOUCHPRESSURE)
  {
    //Serial.print("touch x:");Serial.print (p.x);Serial.println(" y:");Serial.println(p.y);
    int x = 0; int y = 0;
    //        A        B
    x = map(p.y, 80, 880, 0, tft.width());
    //lower A to increase report X pixes at left side of screen 3 ~ 3px
    //lower D to increase report X pixel at right side of scrreen
    //        C       D
    y = map(p.x, 140, 890, 0, tft.height());
    //lower D to increase Reporter Y Pixels at top of screen 5 ~ 1px
    //lower C to increase reportd Y pixels at bottom of screen 2 ~ 1px
    //Serial.print ("x:");Serial.print (x);Serial.print (" y:");Serial.println (y);
    ProcessTouch(x, y);
  }

  //**********************************************************************************************************************************
  //read temperatures and amps    B         read temperatures and amps   B             read temperatures and amps     B         read temperatures and amps
  //********************************************************************************************************************************
  float tempraw;
 
//we read current all the time since
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

 
  if (bNewSecond) { //we speed up by reading temps once per second.  they are very slow.
    TCoil =  getCleanTemp(thermocouple1.readFahrenheit(), 1);
    //Serial.print("Coil teamp:");Serial.println(TCoil);
    TFan =   getCleanTemp(thermocouple4.readFahrenheit(), 4);
    TBean1 = getCleanTemp(thermocouple2.readFahrenheit(), 2);
    TBean2 = getCleanTemp(thermocouple3.readFahrenheit(), 3);
    TBeanAvg = getBeanAvgTemp(TBean1, TBean2);

    //Serial.print(TBean1);Serial.print (TBean2); Serial.print(TCoil); Serial.println(TFan);
    double newtempratiotoaverage;
    if (TBeanAvgRoll.getCount() > 1) {
      newtempratiotoaverage = TBeanAvg / TBeanAvgRoll.mean();
    }
    else {
      newtempratiotoaverage = 1;
      Readingskipped++;
    }

    if    ( (newtempratiotoaverage > .7 &&  newtempratiotoaverage < 1.2)  || TBeanAvgRoll.mean() < 200  || roastMinutes < 1 )  {
      TBeanAvgRoll.push(TBeanAvg);
    }
    else {
      Readingskipped++;
      Serial.println("out of range:");
      Serial.print("TBean2:"); Serial.print(TBean2); Serial.print(" TBean1:"); Serial.print(TBean1);
      Serial.print(" avg:"); Serial.print(TBeanAvgRoll.mean()); Serial.print(" TCoil:");
      Serial.print(TCoil); Serial.print(" TFan:"); Serial.println(TFan);

    }
  }
 

  //*******************************************************************************************************************************************************
  //Input buttons      C          Input buttons        C        Input buttons       C         Input buttons      C          Input buttons
  //*******************************************************************************************************************************************************
  //look for pressed buttons being released
  if (CapButActive > 0 ) {
    if (digitalRead(CapButActive) == LOW) {
      Serial.print ("Cap Button released:" ); Serial.println (CapButActive);
      CapButActive = 0;
    }
  }
  //find newly pressed  button
  //int capButGoneHigh = 0;

  boolean capbuttonpressed = false;
  if (CapButActive == 0 ) {
    if (digitalRead(CP1p) == HIGH) {
      CapButActive = CP1p;
    }
    else if ( digitalRead(CP2p) == HIGH) {
      CapButActive = CP2p;
    }
    else if ( digitalRead(CP3p) == HIGH) {
      CapButActive = CP3p;
    }
    else if ( digitalRead(CP4p) == HIGH) {
      CapButActive = CP4p;
    }

    if (CapButActive > 0) {
      capbuttonpressed = true;
      Serial.print ("Cap Button pressed:" ); Serial.println (CapButActive);
    }
  }


  //**************************************************************************************************
  //DETERIM NEW STATE   D    DETERIM NEW STATE   D    DETERIM NEW STATE  D     DETERIM NEW STATE  D     DETERIM NEW STATE
  //*****************************************************************************************************


  //look at button inputs
  if (capbuttonpressed) {
    Serial.print ("Doing button work:" ); Serial.println (CapButActive);
    switch (CapButActive) {
      case CP1p:
        if (State == AMSTOPPED || State == AMFANONLY  ) {
          newState = AMROASTING;
          Serial.println("Start Detected!");
        }
        else  if (State == AMOVERHEATEDCOIL || State == AMOVERHEATEDFAN || State == AMAUTOCOOLING ) {
          newState = AMROASTING;
          Serial.println("ReStart Detected!");
        }
        else {
          newState = AMSTOPPED;
        }
        break;
      case CP2p:
        break;
      case CP3p:
        if ( State == AMSTOPPED) {
          Serial.println("Fan On Detected!");
          newState = AMFANONLY;
        }
        else if (State == AMFANONLY) {
          Serial.println("Fan Off Detected!");
          newState = AMSTOPPED;
        }
        else {
          Serial.println("Fan on off detected by state not right to turn on or off");
        }
        break;
      case CP4p:
        if (State == AMOVERHEATEDCOIL || State == AMOVERHEATEDFAN) {
          Serial.println("Manually Resetting overheat");
          newState = AMROASTING;
        }
        graphProfile();
        Serial.println("cp4");
        break;
      default:
        break;
    }
  }

  //look at temp inputs
  if (State == AMROASTING && TCoil > OVERHEATCOIL) {
    OVERHEATCOILCount++;
    Serial.print("Overheat coil Detected! count:"); Serial.println(OVERHEATCOILCount);
    if (OVERHEATCOILCount > 20) {
      newState = AMOVERHEATEDCOIL;
    }
  }
  else if (State == AMROASTING && TFan > OVERHEATFAN) {
    OVERHEATFANCount ++;
    Serial.print("Overheat fan Detected! count:"); Serial.println(OVERHEATFANCount);
    if (OVERHEATFANCount > 20) {
      newState = AMOVERHEATEDFAN;
    }
  }
  else {
    OVERHEATFANCount = 0;  //reset counts whenever we see neither overheat condition
    OVERHEATCOILCount = 0;
    if (State == AMROASTING && TBeanAvgRoll.mean() > TempEnd ) {
      TempReachedCount ++;
      if (TempReachedCount > 20) {
        newState = AMAUTOCOOLING;
        Serial.print("Roast Temp Reached. Cooling starting End:"); Serial.print(TempEnd); Serial.print("  Tempavg"); Serial.println(TBeanAvgRoll.mean());
      }
    }
    else if (State == AMAUTOCOOLING && TBeanAvgRoll.mean() < TEMPCOOLINGDONE ) {
      newState = AMSTOPPED;
      Serial.println("Auto Cooling Complete ");
    }
    else if (State == AMROASTING && roastMinutes > TimeScreenLeft ) {
      newState = AMAUTOCOOLING;
      Serial.println("Max time reached. Cooling starting");
    }
    else {
      TempReachedCount = 0;
    }
  }


  // **************************************************************************************************************************************************************
  //APPLY NEW STATE     E      APPLY NEW STATE      E     APPLY NEW STATE   E        APPLY NEW STATE      E     APPLY NEW STATE   E        APPLY NEW STATE           APPLY NEW STATE
  // **************************************************************************************************************************************************************
  if (newState == AMSTOPPED) {
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    if (TBeanAvgRoll.mean() < TEMPCOOLINGDONE  && TFan < TEMPCOOLINGDONE ) {
      State = AMSTOPPED;
      digitalWrite(FANRELAYp, RELAYOFF); digitalWrite(VIBRELAYp, RELAYOFF);
      RoastTime.stop();
    }
    else {
      newState = AMAUTOCOOLING;
      State = AMAUTOCOOLING;
    }
  }
  else if (newState == AMROASTING) {
    digitalWrite(FANRELAYp, RELAYON); digitalWrite(VIBRELAYp, RELAYON);
    if (State == AMSTOPPED || State == AMFANONLY) {

      Serial.println("Starting Fans and Vibration - and waiting 5 seconds");
      delay(2000);
      Readingskipped = 0;
      StartLinebyTimeAndTemp(0, 0, AVGLINEID , BLUE);
      StartLinebyTimeAndTemp(0, 0, ROLLAVGLINEID , YELLOW);
      graphProfile();
      delay(2000);
      Serial.println("2 Starting Heaters ");
      RoastTime.restart(minuteToStart * 60);
      roastMinutes = ((double)RoastTime.elapsed()) / 60;
    }
    State = AMROASTING;
  }
  else if (newState == AMAUTOCOOLING) {
    State = AMAUTOCOOLING;
    TempLastEndOfRoast = TBeanAvgRoll.mean();
    TimeLastEndOfRoast = roastMinutes;
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    LoadORSaveToHistory(false);
    delay(1000);
  }
  else if (newState == AMFANONLY) {
    State = AMFANONLY;
    digitalWrite(FANRELAYp, RELAYON); digitalWrite(VIBRELAYp, RELAYON);
  }
  else if (newState == AMOVERHEATEDCOIL) {
    State = AMOVERHEATEDCOIL;
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    delay(1000);
  }
  else if (State == AMOVERHEATEDFAN) {
    State = AMOVERHEATEDFAN;
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
  }


  //***************************************************************************************************************************************************************
  // CALCULATING PID VALUES     F    CALCULATING PID VALUES     F    CALCULATING PID VALUES    F     CALCULATING PID VALUES    F     CALCULATING PID VALUES
  //*************************************************************************************************************************************************************
  int err = 0;
  //pid window size should vary based on duty cycle. 10 millsec and 50% would to 20 millisecond.  10% would be 100 milliseconds.  01 % would be 1 seconds
  //but we run 9 times per second, so shortest on off is 1 cycles or ~ 100 millseconds.   50% means .2 seconds, 10% means 1 seconds  .05% means 2 seconds
  Duty = 0;
  Setpoint = 0;

  if (State == AMROASTING) {
    //CALC THE ERR AND INTEGRAL
    Setpoint =  calcSetpoint(roastMinutes);
    err = Setpoint - TBeanAvgRoll.mean();  //negative if temp is over setpoint
    //if (bNewSecond) { Serial.print(" new calc of err:");Serial.println(err);    };     
    PIDIntegralUdateTimeValue = 5000;
    if (roastMinutes > MySpanAccumulatedMinutes[2] ) { //only calc intergral error if we are above the 1st setup
      if (PIDIntegralUdateTime.elapsed() > PIDIntegralUdateTimeValue) { //every 3 seconds we add the err to be a sum of err     
        IntegralSum =  IntegralSum + double(err);
        ErrI = (IntegralSum * Integral) ; //duty is proportion of PIDWindow pid heater should be high before we turn it off.  If duty drops during window - we kill it.  If duty raise during window we may miss the turn on.
        Serial.print("Isum:");Serial.print(IntegralSum);Serial.print("ErrI:");    Serial.println(ErrI);
        PIDIntegralUdateTime.restart(0);
      }
    }
    else { //clear out the integral before set point 1.
      PIDIntegralUdateTime.restart(0);
      IntegralSum = 0;
      IntegralLastTime = 0;
      ErrI = 0;
    }

    
    Duty = ((double)(err + ErrI) / (double)Gain) ;   
    //APPLY THE ERROR WITH THE PID WINDOW
    PIDWindowSize = 1000;
    unsigned long now = millis();
    int SSR1 = LOW;
    int SSR2 = LOW;
    ManagingSSR = 1;
    boolean ExceedsWholePidWindow = (PIDWindowStartTime == 0) || (now - PIDWindowStartTime > PIDWindowSize);
    if (ExceedsWholePidWindow) { //keep checking if we need to start a new PID window
      PIDWindowStartTime = now;
      if (Duty > 0) {SSR1 = HIGH;}
      if (Duty > 0.5) {SSR2 = HIGH; ManagingSSR = 2;}
      }
    else{
        if ( Duty <= 0.5 && ((now - PIDWindowStartTime) <=  (Duty  * 2 * PIDWindowSize))) {
             SSR1 = HIGH;
        }  
        if ( Duty > 0.5 ){
              SSR1 = HIGH;
             if ((now - PIDWindowStartTime) <=  ( (Duty - .5) * 2 *  PIDWindowSize)) {
              SSR2 = HIGH;
              }
             ManagingSSR = 2;
        }
    }
    digitalWrite(SSR1p, SSR1);
    digitalWrite(SSR2p, SSR2);
    
  }
  
  else {
    //Serial.println("not roastine is off");
    digitalWrite(SSR1p, LOW); digitalWrite(SSR2p, LOW);
    
  }

  //********************************************************************************************************************************
  //LCD AND GRAPH DISPLAY    G     LCD AND GRAPH DISPLAY   G      LCD AND GRAPH DISPLAY    G     LCD AND GRAPH DISPLAY    G     LCD AND GRAPH DISPLAY
  //*******************************************************************************************************************************
  //its slow to update th

  if (newState > 0) {
    Serial.println("newstate detected  Will update tft immediately");
    displayState(State);  newState = 0;
  }
  
  //we update the area when we get new temps
  if    (bNewSecond) {
  //  Serial.println("update after reach new temp");
    displayState(State); 
    UpdateGraphA(roastMinutes, Duty, Setpoint, err, ErrI);
    AddLinebyTimeAndTemp(roastMinutes, TBeanAvg, AVGLINEID);
    AddLinebyTimeAndTemp(roastMinutes, TBeanAvgRoll.mean(), ROLLAVGLINEID);
  }
  if (LcdUdateTime.elapsed() > 3000) {
    //Serial.print("slow update. once per:");Serial.println(3000);Serial.println(" millseconds");
    // void UpdateGraphB(int temp1, int temp2, int tempCoil, double ampHeater1, double ampHeater2, int tempFan, double ampFan, double volts)
    //   Serial.print("TBean2:");Serial.print(TBean2);Serial.print("TBean1:");Serial.print (TBean1);Serial.print("TCoil:"); Serial.print(TCoil); Serial.print("TFan:");Serial.println(TFan);
    UpdateGraphB(TBean2, TBean1, TCoil, CurrentHeat1, CurrentHeat2, TFan, CurrentFan, MaxVoltage);
    LcdUdateTime.restart(0);
  }

}

//----------------------------------------------------------------------------------------------------------------------------------------------------
//      END LOOP  H      END LOOP      H    END LOOP    H      END LOOP    H      END LOOP   H       END LOOP     H     END LOOP    H      END LOOP
//----------------------------------------------------------------------------------------------------------------------------------------------------


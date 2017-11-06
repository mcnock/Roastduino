
//----------------------------------------------------------------------------------------------------------------------------------------------------
// TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH
//----------------------------------------------------------------------------------------------------------------------------------------------------

void ProcessTouch(int Xtouch, int Ytouch)
{

  Serial.print ("x:"); Serial.print (Xtouch); Serial.print (" y:"); Serial.println (Ytouch);


  //looking to see if should show adjust graph boxes
  for (int X = 1; X < SetPointCount; X++) {
    if (abs(Xtouch - MySetpointsX[X]) < 20 && abs(Ytouch - MySetpointsY[X]) < 20 ) {
      DrawAdjustMentBoxes(X);
      return;
    }
  }
  
if (AdjustmentSetpoint > 0) {
    int Ytouch2 = 240 - Ytouch;
    int beforeTemp = MyBaseSetpoints[AdjustmentSetpoint];
    int beforeTime = MySpanAccumulatedMinutes[AdjustmentSetpoint];
    if (Xtouch > 180 && Xtouch < 200 && Ytouch2 > AdjustmentDisplayTop && Ytouch2 < AdjustmentDisplayTop + 20) {
      MyBaseSetpoints[AdjustmentSetpoint] = MyBaseSetpoints[AdjustmentSetpoint] + 1;
      //Serial.println ("up");
      SaveTempEprom(MySetpointsEprom[AdjustmentSetpoint] , MyBaseSetpoints[AdjustmentSetpoint]);
    }
    if (Xtouch > 200 && Xtouch < 220 && Ytouch2 > AdjustmentDisplayTop + 20 && Ytouch2 < AdjustmentDisplayTop + 40) {
      if (AdjustmentSetpoint > 0) {
        MySpanMinutes[AdjustmentSetpoint] = MySpanMinutes[AdjustmentSetpoint] + 1;
        MySpanMinutes[AdjustmentSetpoint + 1] = MySpanMinutes[AdjustmentSetpoint + 1] - 1;
        MySpanAccumulatedMinutes[AdjustmentSetpoint] =  MySpanAccumulatedMinutes[AdjustmentSetpoint] + 1;
        EEPROM.update( MySpanMinutesEprom[AdjustmentSetpoint] ,  MySpanMinutes[AdjustmentSetpoint] );
        EEPROM.update(MySpanMinutesEprom[AdjustmentSetpoint + 1] , MySpanMinutes[AdjustmentSetpoint + 1]);
        //Serial.println ("right");
      }
    }
    if (Xtouch > 180 && Xtouch < 200 && Ytouch2 > AdjustmentDisplayTop + 40 && Ytouch2 < AdjustmentDisplayTop + 60) {
      MyBaseSetpoints[AdjustmentSetpoint] = MyBaseSetpoints[AdjustmentSetpoint] - 1;
      SaveTempEprom(MySetpointsEprom[AdjustmentSetpoint] , MyBaseSetpoints[AdjustmentSetpoint]);
      Serial.println ("botton");
    }
    if (Xtouch > 160 && Xtouch < 180 && Ytouch2 > AdjustmentDisplayTop + 20 && Ytouch2 < AdjustmentDisplayTop + 40) {
      if (AdjustmentSetpoint > 0) {

        MySpanMinutes[AdjustmentSetpoint] = MySpanMinutes[AdjustmentSetpoint] - 1;
        MySpanMinutes[AdjustmentSetpoint + 1] = MySpanMinutes[AdjustmentSetpoint + 1] + 1;
        MySpanAccumulatedMinutes[AdjustmentSetpoint] =  MySpanAccumulatedMinutes[AdjustmentSetpoint] - 1;
        EEPROM.update(MySpanMinutesEprom[AdjustmentSetpoint] , MySpanMinutes[AdjustmentSetpoint]);
        EEPROM.update(MySpanMinutesEprom[AdjustmentSetpoint + 1] , MySpanMinutes[AdjustmentSetpoint + 1]);
        Serial.println ("left");
      }
    }
    if (Xtouch > 275 && Xtouch < 305 && Ytouch2 > AdjustmentDisplayTop - 15  && Ytouch2 < AdjustmentDisplayTop + 15) {
      HideAdjustMentBoxes();
      Serial.println ("close");
      return;
    }
    else {
      DrawLinebyTimeAndTemp(MySpanAccumulatedMinutes[AdjustmentSetpoint - 1], MyBaseSetpoints[AdjustmentSetpoint - 1], beforeTime, beforeTemp, BLUE);
      DrawLinebyTimeAndTemp(beforeTime, beforeTemp, MySpanAccumulatedMinutes[AdjustmentSetpoint + 1], MyBaseSetpoints[AdjustmentSetpoint + 1], BLUE);
      DrawLinebyTimeAndTemp(MySpanAccumulatedMinutes[AdjustmentSetpoint - 1], MyBaseSetpoints[AdjustmentSetpoint - 1], MySpanAccumulatedMinutes[AdjustmentSetpoint], MyBaseSetpoints[AdjustmentSetpoint ], WHITE);
      DrawLinebyTimeAndTemp(MySpanAccumulatedMinutes[AdjustmentSetpoint], MyBaseSetpoints[AdjustmentSetpoint ], MySpanAccumulatedMinutes[AdjustmentSetpoint + 1], MyBaseSetpoints[AdjustmentSetpoint + 1], WHITE);
      UpdateAdjustMentText();
    }
    return;
  }
 
  //looking to set gain
  if (1 == 1) {
    if (abs(Xtouch - GainUX) < 5 && abs(Ytouch - GainUY) < 5 ) {
      Gain = Gain + 1;
      if (Gain > 50) {
        Gain = 50;
      }
      delay(500);
      tft.setTextSize(1);
      tft.setCursor(GainX , GainY); tft.println(Gain);
      Serial.print("Gain U"); Serial.println(Gain);
      EEPROM.update(GAIN_EP, Gain);
      return;
    }
    if (abs(Xtouch - GainDX) < 5 && abs(Ytouch - GainDY) < 5 ) {
      Gain = Gain - 1;
      delay(500);
      tft.setTextSize(1);
      tft.setCursor(GainX , GainY); tft.println(Gain);
      Serial.print("Gain D"); Serial.println(Gain);
      EEPROM.update(GAIN_EP, Gain);
      return;
    }
    if (abs(Xtouch - IntegralUX) < 5 && abs(Ytouch - IntegralUY) < 5 ) {
      Integral = Integral + 0.1;
      delay(500);
      tft.setTextSize(1);
      tft.setCursor(IntegralX , IntegralY); tft.println(Integral);
      Serial.print("Integral U"); Serial.println(Integral);
      EEPROM.update(INTEGRAL_EP , (int)(Integral * 10));
      return;
    }
    if (abs(Xtouch - IntegralDX) < 5 && abs(Ytouch - IntegralDY) < 5 ) {
      Integral = Integral - 0.1;
      delay(500);
      if (Integral < 0) Integral = 0;
      tft.setTextSize(1);
      tft.setCursor(IntegralX , IntegralY); tft.println(Integral);
      Serial.print("Integral D"); Serial.println(Integral);
      EEPROM.update(INTEGRAL_EP , (int)(Integral * 10));
      return;
    }
  }
  
   
   //looking for right sidemenus
    if (MenuShowing > 0)  
    {
        int Ytouchinv = 240 - Ytouch;
        Serial.print ("y invert:");Serial.println(Ytouchinv);
                     
        if (Xtouch > MenuShowingXmin and Ytouchinv > MenuShowingYmin)
        {
            if (Xtouch < MenuShowingXmax and Ytouchinv < MenuShowingYmax){
                  if (MenuShowing == 1)
                  {
                      
                      Serial.println ("menu 1 found");
                       for (int i= 0 ; i < 2; i++){
                            if (Ytouchinv > myButtonMenu1[i].y and Ytouchinv < (myButtonMenu1[i].y +  myButtonMenu1[i].h))
                            {
                               switch (i){
                                case 0:
                                        DrawMenu2();
                                break;
                                case 1:
                                        DrawMenu2();
                                break;
                                
                               }
                                                        
                            }
                       }
 

                  }
                  else if (MenuShowing == 2)
                  {
                       Serial.println ("menu 2 found");



                  }
                    
              
            }
        
          
        }
        
    }
  

}


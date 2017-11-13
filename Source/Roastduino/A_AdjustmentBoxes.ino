//----------------------------------------------------------------------------------------------------------------------------------------------------
// ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT
//----------------------------------------------------------------------------------------------------------------------------------------------------

const int AjTop = 140;
const int AjLeft = 150;
const int AjSize = 20;
const int AjTotalWidth = 150;
const int AjTotalHeight = 80;
const rect  mytop = {0,0,0,0};

void UpdateAdjustMentText(){
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.setCursor(225, AjTop + 25);  tft.println("New:"); tft.setCursor(260, AjTop + 25) ;   tft.println( MyBaseSetpoints[SetpointbeingAdjusted]);
  tft.setCursor(225, AjTop + 55 ); tft.println("New:"); tft.setCursor(260, AjTop + 55);  tft.println( MySpanAccumulatedMinutes[SetpointbeingAdjusted]);
}

void HideAdjustMentBoxes(){
  SetpointbeingAdjusted = 0;
  tft.fillRect(150, AjTop - 10, 150, 80, BLACK);
}

void DrawAdjustMentBoxes(int setpoint){
  // make the color selection boxes
  SetpointbeingAdjusted = setpoint;
   
  //block out box 
  tft.fillRect(AjLeft, AjTop - 10, AjTotalWidth, AjTotalHeight, BLACK);
  tft.drawRect(AjLeft, AjTop - 10, AjTotalWidth, AjTotalHeight, BLUE);

   //
  //tft.fillRect(180, AjTop      , 20,  20, GREEN); //TOP
  //tft.fillRect(200, AjTop + 20 , 20, 20, GREEN); //RIGHT
  //tft.fillRect(180, AjTop + 40 , 20, 20, GREEN); //BOTTOM
  //tft.fillRect(160, AjTop + 20 , 20, 20, GREEN); //LEFT
  tft.fillRect(AjLeft + 10 + 20, AjTop +  0 , 20, 20, GREEN); //TOP
  tft.fillRect(AjLeft + 10     , AjTop + 20 , 20, 20, GREEN); //LEFT
  tft.fillRect(AjLeft + 10 + 40, AjTop + 20 , 20, 20, GREEN); //RIGHT
  tft.fillRect(AjLeft + 10 + 20, AjTop + 40 , 20, 20, GREEN); //BOTTOM
  

  //tft.fillRect(280, AjTop - 10 , 20, 20,  BLUE); //LEFT
  //tft.drawLine(280, AjTop + 10 , 300 ,  AjTop - 10, BLACK);
  //tft.drawLine(280, AjTop - 10 , 300 ,  AjTop + 10, BLACK);

  tft.fillRect(AjLeft + 130, AjTop - 10 , 20, 20,  BLUE); //LEFT
  tft.drawLine(AjLeft + 130, AjTop + 10 , AjLeft + 150 ,  AjTop - 10, BLACK);
  tft.drawLine(AjLeft + 130, AjTop - 10 , AjLeft + 150 ,  AjTop + 10, BLACK);

  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(2);
  //tft.setCursor(184, AjTop + 23 ); tft.println(setpoint);
  tft.setCursor(AjLeft + 34, AjTop + 23 ); tft.println(setpoint);

  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  //tft.setCursor(225, AjTop + 15);  tft.println("Temp:"); tft.setCursor(260, AjTop + 15) ;   tft.println( MyBaseSetpoints[SetpointbeingAdjusted]);
  //tft.setCursor(225, AjTop + 40 ); tft.println("Mins:"); tft.setCursor(260, AjTop + 40);  tft.println( MySpanAccumulatedMinutes[SetpointbeingAdjusted]);
  tft.setCursor(AjLeft + 75, AjTop + 15);  tft.println("Temp:"); tft.setCursor(AjLeft + 110, AjTop + 15) ; tft.println( MyBaseSetpoints[SetpointbeingAdjusted]);
  tft.setCursor(AjLeft + 75, AjTop + 40 ); tft.println("Mins:"); tft.setCursor(AjLeft + 110, AjTop + 40);  tft.println( MySpanAccumulatedMinutes[SetpointbeingAdjusted]);
}

void ProcessAdjustmentBoxes(int Xtouch, int Ytouchinv){
    int beforeTemp = MyBaseSetpoints[SetpointbeingAdjusted];
    int beforeTime = MySpanAccumulatedMinutes[SetpointbeingAdjusted];
    //look for click on top
    Serial.print("process adjustment boxes for x:");Serial.print(Xtouch);Serial.print("yinv:");Serial.println(Ytouchinv);
    if (Xtouch > 180 && Xtouch < 200 && Ytouchinv > AjTop && Ytouchinv < AjTop + 20) {
      MyBaseSetpoints[SetpointbeingAdjusted] = MyBaseSetpoints[SetpointbeingAdjusted] + 1;
      //Serial.println ("up");
      SaveTempEprom(MySetpointsEprom[SetpointbeingAdjusted] , MyBaseSetpoints[SetpointbeingAdjusted]);
    }
    //look for click on right box
    if (Xtouch > 200 && Xtouch < 220 && Ytouchinv > AjTop + 20 && Ytouchinv < AjTop + 40) {
      if (SetpointbeingAdjusted > 0) {
        MySpanMinutes[SetpointbeingAdjusted] = MySpanMinutes[SetpointbeingAdjusted] + 1;
        MySpanMinutes[SetpointbeingAdjusted + 1] = MySpanMinutes[SetpointbeingAdjusted + 1] - 1;
        MySpanAccumulatedMinutes[SetpointbeingAdjusted] =  MySpanAccumulatedMinutes[SetpointbeingAdjusted] + 1;
        EEPROM.update( MySpanMinutesEprom[SetpointbeingAdjusted] ,  MySpanMinutes[SetpointbeingAdjusted] );
        EEPROM.update(MySpanMinutesEprom[SetpointbeingAdjusted + 1] , MySpanMinutes[SetpointbeingAdjusted + 1]);
        //Serial.println ("right");
      }
    }
    //look for click on lower box
    if (Xtouch > 180 && Xtouch < 200 && Ytouchinv > AjTop + 40 && Ytouchinv < AjTop + 60) {
      MyBaseSetpoints[SetpointbeingAdjusted] = MyBaseSetpoints[SetpointbeingAdjusted] - 1;
      SaveTempEprom(MySetpointsEprom[SetpointbeingAdjusted] , MyBaseSetpoints[SetpointbeingAdjusted]);
      //Serial.println ("botton");
    }
    //look for click on left box
    if (Xtouch > 160 && Xtouch < 180 && Ytouchinv > AjTop + 20 && Ytouchinv < AjTop + 40) {
      if (SetpointbeingAdjusted > 0) {

        MySpanMinutes[SetpointbeingAdjusted] = MySpanMinutes[SetpointbeingAdjusted] - 1;
        MySpanMinutes[SetpointbeingAdjusted + 1] = MySpanMinutes[SetpointbeingAdjusted + 1] + 1;
        MySpanAccumulatedMinutes[SetpointbeingAdjusted] =  MySpanAccumulatedMinutes[SetpointbeingAdjusted] - 1;
        EEPROM.update(MySpanMinutesEprom[SetpointbeingAdjusted] , MySpanMinutes[SetpointbeingAdjusted]);
        EEPROM.update(MySpanMinutesEprom[SetpointbeingAdjusted + 1] , MySpanMinutes[SetpointbeingAdjusted + 1]);
        //Serial.println ("left");
      }
    }

    //look for click on close box
    if (Xtouch > 275 && Xtouch < 305 && Ytouchinv > AjTop - 15  && Ytouchinv < AjTop + 15) {
      HideAdjustMentBoxes();
      //Serial.println ("close");
      return;
    }
    else {
      //draw new line
      boolean log = true;
      Serial.println ("Drawing adjustment line...");
      DrawLinebyTimeAndTemp(log, MySpanAccumulatedMinutes[SetpointbeingAdjusted - 1], MyBaseSetpoints[SetpointbeingAdjusted - 1], beforeTime, beforeTemp, BLUE);
      DrawLinebyTimeAndTemp(log, beforeTime, beforeTemp, MySpanAccumulatedMinutes[SetpointbeingAdjusted + 1], MyBaseSetpoints[SetpointbeingAdjusted + 1], BLUE);
      DrawLinebyTimeAndTemp(log, MySpanAccumulatedMinutes[SetpointbeingAdjusted - 1], MyBaseSetpoints[SetpointbeingAdjusted - 1], MySpanAccumulatedMinutes[SetpointbeingAdjusted], MyBaseSetpoints[SetpointbeingAdjusted ], WHITE);
      DrawLinebyTimeAndTemp(log, MySpanAccumulatedMinutes[SetpointbeingAdjusted], MyBaseSetpoints[SetpointbeingAdjusted ], MySpanAccumulatedMinutes[SetpointbeingAdjusted + 1], MyBaseSetpoints[SetpointbeingAdjusted + 1], WHITE);
      UpdateAdjustMentText();
    }
}


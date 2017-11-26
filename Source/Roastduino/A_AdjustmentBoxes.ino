//----------------------------------------------------------------------------------------------------------------------------------------------------
// ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT          ADJUSTMENT
//----------------------------------------------------------------------------------------------------------------------------------------------------

const int AjTop = 140;
const int AjLeft = 150;
const int AjSize = 20;
const int AjTotalWidth = 150;
const int AjTotalHeight = 80;
const rect  mytop = {0, 0, 0, 0};

void UpdateAdjustMentText() {
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.setCursor(225, AjTop + 25);  tft.println("New:"); tft.setCursor(260, AjTop + 25) ;   tft.println( MySetpointTempuratures[SetpointbeingAdjusted]);
  tft.setCursor(225, AjTop + 55 ); tft.println("New:"); tft.setCursor(260, AjTop + 55);  tft.println( MySetpointTempuratures[SetpointbeingAdjusted]);
}

void HideAdjustMentBoxes() {
  SetpointbeingAdjusted = 0;
  tft.fillRect(150, AjTop - 10, 150, 80, BLACK);
}

void DrawAdjustMentBoxes(int setpoint) {
  // make the color selection boxes
  SetpointbeingAdjusted = setpoint;
  BeforeTemp = MySetpointTempuratures[SetpointbeingAdjusted];
  BeforeTime = MySetpointAccumlulativeMinutes[SetpointbeingAdjusted];

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
  //tft.setCursor(225, AjTop + 15);  tft.println("Temp:"); tft.setCursor(260, AjTop + 15) ;   tft.println( MySetpointTempuratures[SetpointbeingAdjusted]);
  //tft.setCursor(225, AjTop + 40 ); tft.println("Mins:"); tft.setCursor(260, AjTop + 40);  tft.println( BySpanTempuratureSetpoints[SetpointbeingAdjusted]);
  tft.setCursor(AjLeft + 75, AjTop + 15);  tft.println("Temp:"); tft.setCursor(AjLeft + 110, AjTop + 15) ; tft.println( MySetpointTempuratures[SetpointbeingAdjusted]);
  tft.setCursor(AjLeft + 75, AjTop + 40 ); tft.println("Mins:"); tft.setCursor(AjLeft + 110, AjTop + 40);  tft.println( MySetpointTempuratures[SetpointbeingAdjusted]);
}

void ProcessAdjustmentBoxes(int Xtouch, int Ytouchinv) {
  int beforeTemp = MySetpointTempuratures[SetpointbeingAdjusted];
  int beforeTime = MySetpointAccumlulativeMinutes[SetpointbeingAdjusted];
  //look for click on top
  Serial.print("process adjustment boxes for x:"); Serial.print(Xtouch); Serial.print("yinv:"); Serial.println(Ytouchinv);
  if (Xtouch > 180 && Xtouch < 200 && Ytouchinv > AjTop && Ytouchinv < AjTop + 20) {
    MySetpointTempuratures[SetpointbeingAdjusted] = MySetpointTempuratures[SetpointbeingAdjusted] + 1;
    //Serial.println ("up");
    SaveTempEprom(MySetpointsEprom[SetpointbeingAdjusted] , MySetpointTempuratures[SetpointbeingAdjusted]);
  }
  //look for click on right box
  if (Xtouch > 200 && Xtouch < 220 && Ytouchinv > AjTop + 20 && Ytouchinv < AjTop + 40) {
    if (SetpointbeingAdjusted > 0) {

      //we can only move to right if the next segment is greater that 1 in length
      if (SpanMinutesLength[SetpointbeingAdjusted + 1] > 2) {
        SpanMinutesLength[SetpointbeingAdjusted] = SpanMinutesLength[SetpointbeingAdjusted] + 1;
        SpanMinutesLength[SetpointbeingAdjusted + 1] = SpanMinutesLength[SetpointbeingAdjusted + 1] - 1;
        MySetpointTempuratures[SetpointbeingAdjusted] =  MySetpointTempuratures[SetpointbeingAdjusted] + 1;
        EEPROM.update( MySpanMinutesEprom[SetpointbeingAdjusted] ,  SpanMinutesLength[SetpointbeingAdjusted] );
        EEPROM.update( MySpanMinutesEprom[SetpointbeingAdjusted + 1] , SpanMinutesLength[SetpointbeingAdjusted + 1]);
      }
      //Serial.println ("right");
    }
  }
  //look for click on lower box
  if (Xtouch > 180 && Xtouch < 200 && Ytouchinv > AjTop + 40 && Ytouchinv < AjTop + 60) {
    MySetpointTempuratures[SetpointbeingAdjusted] = MySetpointTempuratures[SetpointbeingAdjusted] - 1;
    SaveTempEprom(MySetpointsEprom[SetpointbeingAdjusted] , MySetpointTempuratures[SetpointbeingAdjusted]);
    //Serial.println ("botton");
  }
  //look for click on left box
  if (Xtouch > 160 && Xtouch < 180 && Ytouchinv > AjTop + 20 && Ytouchinv < AjTop + 40) {
    if (SetpointbeingAdjusted > 0) {
      //we can only move to left if the next segment is less that 1 in length
      if (SpanMinutesLength[SetpointbeingAdjusted - 1] > 2) {
        SpanMinutesLength[SetpointbeingAdjusted] = SpanMinutesLength[SetpointbeingAdjusted] - 1;
        SpanMinutesLength[SetpointbeingAdjusted + 1] = SpanMinutesLength[SetpointbeingAdjusted + 1] + 1;
        MySetpointTempuratures[SetpointbeingAdjusted] =  MySetpointTempuratures[SetpointbeingAdjusted] - 1;
        EEPROM.update(MySpanMinutesEprom[SetpointbeingAdjusted] , SpanMinutesLength[SetpointbeingAdjusted]);
        EEPROM.update(MySpanMinutesEprom[SetpointbeingAdjusted + 1] , SpanMinutesLength[SetpointbeingAdjusted + 1]);
        //Serial.println ("left");
      }
    }
  }

  //look for click on close box
  if (Xtouch > 275 && Xtouch < 305 && Ytouchinv > AjTop - 15  && Ytouchinv < AjTop + 15) {
    HideAdjustMentBoxes();
    //Serial.println ("close");
    return;
  }
  else {
    //draw new line graphic
    boolean log = true;
    Serial.println ("Drawing adjustment line...");
    //blackout prior line
    DrawLinebyTimeAndTemp(log, MySetpointAccumlulativeMinutes[SetpointbeingAdjusted - 1], MySetpointTempuratures[SetpointbeingAdjusted - 1], beforeTime, beforeTemp, BLACK);
    //draw new line
    DrawLinebyTimeAndTemp(log, MySetpointAccumlulativeMinutes[SetpointbeingAdjusted - 1], MySetpointTempuratures[SetpointbeingAdjusted - 1], MySetpointAccumlulativeMinutes[SetpointbeingAdjusted], MySetpointTempuratures[SetpointbeingAdjusted ], YELLOW);
    if (SetpointbeingAdjusted < 5)
    { DrawLinebyTimeAndTemp(log, beforeTime, beforeTemp, MySetpointAccumlulativeMinutes[SetpointbeingAdjusted + 1], MySetpointTempuratures[SetpointbeingAdjusted + 1], BLACK);
      DrawLinebyTimeAndTemp(log, MySetpointAccumlulativeMinutes[SetpointbeingAdjusted], MySetpointTempuratures[SetpointbeingAdjusted ], MySetpointAccumlulativeMinutes[SetpointbeingAdjusted + 1], MySetpointTempuratures[SetpointbeingAdjusted + 1], YELLOW);
    }

    AddPointbyTimeandTemp(MySetpointAccumlulativeMinutes[SetpointbeingAdjusted ], MySetpointTempuratures[SetpointbeingAdjusted], YELLOW, 4);

    AddPointbyTimeAndTempAndLineID(BeforeTime, BeforeTemp, SETPOINTLINEID, 5);

    UpdateAdjustMentText();
  }
}


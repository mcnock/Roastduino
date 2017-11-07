
//----------------------------------------------------------------------------------------------------------------------------------------------------
// TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH
//----------------------------------------------------------------------------------------------------------------------------------------------------


void ProcessRawTouch(TSPoint p)
{

  int mappedx = 0; int mappedy = 0;

  if (p.y > TouchExtent.xmax) {
    TouchExtent.xmax = p.y;
  }
  if (p.y < TouchExtent.xmin) {
    TouchExtent.xmin = p.y;
  }
  if (p.x > TouchExtent.ymax) {
    TouchExtent.ymax = p.x;
  }
  if (p.x < TouchExtent.ymin) {
    TouchExtent.ymin = p.x;
  }

  //Serial.print("touch x:");Serial.print (p.x);Serial.println(" y:");Serial.println(p.y);
//Serial.println(" ");
//Serial.println("Start touch");
//Serial.print("TouchMin X:");Serial.print (TouchExtent.xmin);Serial.print(" Y:");Serial.println(TouchExtent.ymin);
//Serial.print("TouchMax X:");Serial.print (TouchExtent.xmax);Serial.print(" Y:");Serial.println(TouchExtent.ymax);

  Ts_minx = 74;
  Ts_maxx = 923;

  Ts_miny = 162;
  Ts_maxy = 918 ;

//Serial.print("RangeDefX min:");Serial.print (Ts_minx);Serial.print("-");Serial.print(Ts_maxx);Serial.print(" max:");Serial.print (0);Serial.print("-");Serial.println(tft.width());
//Serial.print("RangeDefY min:");Serial.print (Ts_miny);Serial.print("-");Serial.print(Ts_maxy);Serial.print(" max:");Serial.print (0);Serial.print("-");Serial.println(tft.height());

  // 240 / 320
  //        A        B
  mappedx = map(p.y, Ts_minx, Ts_maxx, 0, tft.width());
  //lower A to increase report X pixes at left side of screen 3 ~ 3px
  //lower D to increase report X pixel at right side of scrreen
  //        C       D
  mappedy = map(p.x, Ts_miny, Ts_maxy, 0, tft.height());
  //lower D to increase Reporter Y Pixels at top of screen 5 ~ 1px
  //lower C to increase reportd Y pixels at bottom of screen 2 ~ 1px
  //Serial.print ("x:");Serial.print (x);Serial.print (" y:");Serial.println (y);
  //map(value,fromlow,fromhigh,tolow,tohight)
//Serial.print("touch  x:");Serial.print (p.y);Serial.print (" y:");Serial.println(p.x);
//Serial.print("mapped x:");Serial.print (mappedx);Serial.print (" y:");Serial.println(mappedy);
  //
  if (mappedx > MappedExtent.xmax) {
    MappedExtent.xmax = mappedx;
  }
  if (mappedx < MappedExtent.xmin) {
    MappedExtent.xmin = mappedx;
  }
  if (mappedy > MappedExtent.ymax) {
    MappedExtent.ymax = mappedy;
  }
  if (mappedy < MappedExtent.ymin) {
    MappedExtent.ymin = mappedy;
  }

//Serial.print("MappedMin X:");Serial.print (MappedExtent.xmin);Serial.print(" Y:");Serial.println(MappedExtent.ymin);
//Serial.print("MappedMax X:");Serial.print (MappedExtent.xmax);Serial.print(" Y:");Serial.println(MappedExtent.ymax);


  ProcessTouch(mappedx, mappedy);
}
void ProcessTouch(int Xtouch, int Ytouch)
{
  int Ytouchinv = 240 - Ytouch;

//Serial.println ("looking for menu2");
  if (Menu2Showing == true && InRect(Xtouch, Ytouchinv, myButtonMenu2rect) == true) {
  //Serial.println ("found menu2"); 
    int i = WhatMenu(Xtouch, Ytouchinv, myButtonMenu2,myButtonMenu2Count);
  //Serial.print ("menu2 found index");Serial.println(i);
    ProcessMenu2(i);
    return;
  }

  //looking to see if should show adjust graph boxes
  for (int X = 1; X < SetPointCount; X++) {
    if (abs(Xtouch - MySetpointsX[X]) < 20 && abs(Ytouch - MySetpointsY[X]) < 20 ) {
      DrawAdjustMentBoxes(X);
      return;
    }
  }

  if (AdjustmentSetpoint > 0) {
    int beforeTemp = MyBaseSetpoints[AdjustmentSetpoint];
    int beforeTime = MySpanAccumulatedMinutes[AdjustmentSetpoint];
    if (Xtouch > 180 && Xtouch < 200 && Ytouchinv > AdjustmentDisplayTop && Ytouchinv < AdjustmentDisplayTop + 20) {
      MyBaseSetpoints[AdjustmentSetpoint] = MyBaseSetpoints[AdjustmentSetpoint] + 1;
      //Serial.println ("up");
      SaveTempEprom(MySetpointsEprom[AdjustmentSetpoint] , MyBaseSetpoints[AdjustmentSetpoint]);
    }
    if (Xtouch > 200 && Xtouch < 220 && Ytouchinv > AdjustmentDisplayTop + 20 && Ytouchinv < AdjustmentDisplayTop + 40) {
      if (AdjustmentSetpoint > 0) {
        MySpanMinutes[AdjustmentSetpoint] = MySpanMinutes[AdjustmentSetpoint] + 1;
        MySpanMinutes[AdjustmentSetpoint + 1] = MySpanMinutes[AdjustmentSetpoint + 1] - 1;
        MySpanAccumulatedMinutes[AdjustmentSetpoint] =  MySpanAccumulatedMinutes[AdjustmentSetpoint] + 1;
        EEPROM.update( MySpanMinutesEprom[AdjustmentSetpoint] ,  MySpanMinutes[AdjustmentSetpoint] );
        EEPROM.update(MySpanMinutesEprom[AdjustmentSetpoint + 1] , MySpanMinutes[AdjustmentSetpoint + 1]);
        //Serial.println ("right");
      }
    }
    if (Xtouch > 180 && Xtouch < 200 && Ytouchinv > AdjustmentDisplayTop + 40 && Ytouchinv < AdjustmentDisplayTop + 60) {
      MyBaseSetpoints[AdjustmentSetpoint] = MyBaseSetpoints[AdjustmentSetpoint] - 1;
      SaveTempEprom(MySetpointsEprom[AdjustmentSetpoint] , MyBaseSetpoints[AdjustmentSetpoint]);
      //Serial.println ("botton");
    }
    if (Xtouch > 160 && Xtouch < 180 && Ytouchinv > AdjustmentDisplayTop + 20 && Ytouchinv < AdjustmentDisplayTop + 40) {
      if (AdjustmentSetpoint > 0) {

        MySpanMinutes[AdjustmentSetpoint] = MySpanMinutes[AdjustmentSetpoint] - 1;
        MySpanMinutes[AdjustmentSetpoint + 1] = MySpanMinutes[AdjustmentSetpoint + 1] + 1;
        MySpanAccumulatedMinutes[AdjustmentSetpoint] =  MySpanAccumulatedMinutes[AdjustmentSetpoint] - 1;
        EEPROM.update(MySpanMinutesEprom[AdjustmentSetpoint] , MySpanMinutes[AdjustmentSetpoint]);
        EEPROM.update(MySpanMinutesEprom[AdjustmentSetpoint + 1] , MySpanMinutes[AdjustmentSetpoint + 1]);
        //Serial.println ("left");
      }
    }
    if (Xtouch > 275 && Xtouch < 305 && Ytouchinv > AdjustmentDisplayTop - 15  && Ytouchinv < AdjustmentDisplayTop + 15) {
      HideAdjustMentBoxes();
      //Serial.println ("close");
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
    if (abs(Xtouch - TGainIncreaseX) < TGainSize && abs(Ytouch - TGainIncreaseY) < TGainSize ) {
      Gain = Gain + 1;
      if (Gain > 50) {
        Gain = 50;
      }
      delay(500);
      tft.setTextSize(1);
      tft.setCursor(TGainCurrentX , TGainCurrentY); tft.println(Gain);
      //Serial.print("Gain U");Serial.println(Gain);
      EEPROM.update(GAIN_EP, Gain);
      return;
    }
    if (abs(Xtouch - TGainDecreaseX) < TGainSize && abs(Ytouch - TGainDecreaseY) < TGainSize ) {
      Gain = Gain - 1;
      delay(500);
      tft.setTextSize(1);
      tft.setCursor(TGainCurrentX , TGainCurrentY); tft.println(Gain);
      //Serial.print("Gain D");Serial.println(Gain);
      EEPROM.update(GAIN_EP, Gain);
      return;
    }
    if (abs(Xtouch - TIntegralIncreaseX) < TGainSize && abs(Ytouch - TIntegralIncreaseY) < TGainSize ) {
      Integral = Integral + 0.1;
      delay(500);
      tft.setTextSize(1);
      tft.setCursor(TIntegralCurrentX , TIntegralCurrentY); tft.println(Integral);
      //Serial.print("Integral U");Serial.println(Integral);
      EEPROM.update(INTEGRAL_EP , (int)(Integral * 10));
      return;
    }
    if (abs(Xtouch - TIntegralDecreaseX) < TGainSize && abs(Ytouch - TIntegralDecreaseY) < TGainSize ) {
      Integral = Integral - 0.1;
      delay(500);
      if (Integral < 0) Integral = 0;
      tft.setTextSize(1);
      tft.setCursor(TIntegralCurrentX , TIntegralCurrentY); tft.println(Integral);
      //Serial.print("Integral D");Serial.println(Integral);
      EEPROM.update(INTEGRAL_EP , (int)(Integral * 10));
      return;
    }
  }

//Serial.println ("looking for buttons"); 
  if (Menu2Showing == false && InRect(Xtouch, Ytouchinv, myButtonControlrect)) {
   //Serial.println ("found buttons"); 
    int i = WhatMenu(Xtouch, Ytouchinv, myButtonControl,myButtonControlCount);
  //Serial.print ("menu control found: ");Serial.println(i);
    ProcessControl(i);
  }
  
//Serial.println ("looking for menu1"); 
  if (Menu2Showing == false && InRect(Xtouch, Ytouchinv, myButtonMenu1rect)) {
   //Serial.print ("found menu1 count:");Serial.println (myButtonMenu1Count); 
    int i = WhatMenu(Xtouch, Ytouchinv, myButtonMenu1, myButtonMenu1Count);
   //Serial.print ("menu1 found: ");Serial.println(i);
    ProcessMenu1(i);
  }


}






boolean InRect(int &x, int &y, rect &Rect)
{
 //Serial.print ("checking point x:");Serial.print (x);Serial.print (" y:");Serial.println (y);

//Serial.print ("in rect ");Serial.print (" Rect.xmin");Serial.print (Rect.xmin);Serial.print (" Rect.xmax");Serial.print (Rect.xmax);
//Serial.print (" Rect.ymin");Serial.print (Rect.ymin);Serial.print (" Rect.ymax");Serial.println (Rect.ymax);
  if (x > Rect.xmin && x < Rect.xmax) {
  //Serial.println("Found X");
    if (y > Rect.ymin && y < Rect.ymax) {
    //Serial.println("Found Y");
      return true;
    }
    else
    {
      return false;
    }
  }
  else {
    return false;
  }
}


int WhatMenu(int &x, int &y, buttondef butdefs[],int count)
{
  
//Serial.print ("what menu count:");Serial.println( count);
  for (int i = 0 ; i < count; i++) {
  //Serial.print ("looking at index:");Serial.println(i);
    if (InRect(x, y, butdefs[i].Rect) == true) {

      return i;
    }
  }
  return -1;
}


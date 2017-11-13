
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


  TsSetting.xmin = 78;
  TsSetting.xmax = 920;

  TsSetting.ymin = 170;
  TsSetting.ymax = 912 ;

  //Serial.print("touch x:");Serial.print (p.x);Serial.println(" y:");Serial.println(p.y);
  //Serial.println(" ");
  Serial.println("Start touch");
  Serial.print("TouchMin X:"); Serial.print (TouchExtent.xmin); Serial.print(" Y:"); Serial.println(TouchExtent.ymin);
  Serial.print("TouchSet X:"); Serial.print (TsSetting.xmin); Serial.print(" Y:"); Serial.println(TsSetting.ymin);
  Serial.print("TouchMax X:"); Serial.print (TouchExtent.xmax); Serial.print(" Y:"); Serial.println(TouchExtent.ymax);
  Serial.print("TouchSet X:"); Serial.print (TsSetting.xmax); Serial.print(" Y:"); Serial.println(TsSetting.ymax);


  //Serial.print("RangeDefX min:");Serial.print (TsSetting.xmin);Serial.print("-");Serial.print(TsSetting.xmax);Serial.print(" max:");Serial.print (0);Serial.print("-");Serial.println(tft.width());
  //Serial.print("RangeDefY min:");Serial.print (TsSetting.ymin);Serial.print("-");Serial.print(TsSetting.ymax);Serial.print(" max:");Serial.print (0);Serial.print("-");Serial.println(tft.height());

  // 240 / 320
  //        A        B
  mappedx = map(p.y, TsSetting.xmin, TsSetting.xmax, 0, tft.width());
  //lower A to increase report X pixes at left side of screen 3 ~ 3px
  //lower D to increase report X pixel at right side of scrreen
  //        C       D
  mappedy = map(p.x, TsSetting.ymin, TsSetting.ymax, 0, tft.height());
  //lower D to increase Reporter Y Pixels at top of screen 5 ~ 1px
  //lower C to increase reportd Y pixels at bottom of screen 2 ~ 1px
  //Serial.print ("x:");Serial.print (x);Serial.print (" y:");Serial.println (y);
  //map(value,fromlow,fromhigh,tolow,tohight)
  //Serial.print("touch  x:");Serial.print (p.y);Serial.print (" y:");Serial.println(p.x);

  if (ShowTouch) tft.drawPixel(mappedx, 240 - mappedy, YELLOW);
  Serial.print("   raw x:"); Serial.print (p.y); Serial.print (" y:"); Serial.println(p.x);
  Serial.print("mapped x:"); Serial.print (mappedx); Serial.print (" y:"); Serial.print(mappedy); Serial.print (" invy:"); Serial.println(240 - mappedy);
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
  if (Menu2Showing == true && InRect(Xtouch, Ytouchinv, myButtonMenu2.bounding) == true) {
    //Serial.println ("found menu2");
    int i = WhatMenu(Xtouch, Ytouchinv, myButtonMenu2);
    //Serial.print ("menu2 found index");Serial.println(i);
    ProcessMenu2(i);
    return;
  }

  //looking to see if should show adjust graph boxes
  //is the click within 20 pixels of the setpoint?
  for (int X = 1; X < SetPointCount; X++) {
    if (abs(Xtouch - MySetpoints[X].x) < 5 && abs(Ytouch - MySetpoints[X].y) < 5 ) {
      DrawAdjustMentBoxes(X);
      return;
    }
  }


  if (SetpointbeingAdjusted > 0) {
    ProcessAdjustmentBoxes(Xtouch, Ytouchinv);
    return;
  }

  //looking to increase or decrease gain and integral
  if (1 == 1) {
    //all the points are where the text start.  We increase them by half a font to get to center 
    //Serial.println("looking for gain U and D");
    if (NearChar(Xtouch, Ytouchinv, TGainIncrease) ) {
      Gain = Gain + 1;
      if (Gain > 50) {
        Gain = 50;
      }
      delay(500);
      tft.setTextSize(1);
      tft.setCursor(TGainValue.x , TGainValue.y); tft.println(Gain);
      //Serial.print("Gain U");Serial.println(Gain);
      EEPROM.update(GAIN_EP, Gain);
      return;
    }
    if (NearChar(Xtouch, Ytouchinv, TGainDecrease )) {
      Gain = Gain - 1;
      delay(500);
      tft.setTextSize(1);
      tft.setCursor(TGainValue.x , TGainValue.y); tft.println(Gain);
      //Serial.print("Gain D");Serial.println(Gain);
      EEPROM.update(GAIN_EP, Gain);
      return;
    }
    Serial.println("looking for int I");
    if (NearChar(Xtouch, Ytouchinv, TIntegralIncrease )) {
      Integral = Integral + 0.02;
      delay(500);
      tft.setTextSize(1);
      tft.setCursor(TIntegralValue.x , TIntegralValue.y); tft.println(Integral);
      //Serial.print("Integral U");Serial.println(Integral);
      EEPROM.update(INTEGRAL_EP , (int)(Integral * 100));
      return;
    }
    Serial.println("looking for int D");
    if  (NearChar(Xtouch, Ytouchinv, TIntegralDecrease)) {
      Integral = Integral - 0.02;
      delay(500);
      if (Integral < 0) Integral = 0;
      tft.setTextSize(1);
      tft.setCursor(TIntegralValue.x , TIntegralValue.y); tft.println(Integral);
      //Serial.print("Integral D");Serial.println(Integral);
      EEPROM.update(INTEGRAL_EP , (int)(Integral * 100));
      return;
    }
  }

  //Serial.println ("looking for buttons");
  if (Menu2Showing == false && InRect(Xtouch, Ytouchinv, myButtonControl.bounding)) {
    //Serial.println ("found buttons");
    int i = WhatMenu(Xtouch, Ytouchinv, myButtonControl);
    //Serial.print ("menu control found: ");Serial.println(i);
    ProcessControl(i);
  }

  Serial.println ("looking for menu1");
  //  SerialprintRect(myButtonMenu1.bounding);
  if (Menu2Showing == false && InRect(Xtouch, Ytouchinv, myButtonMenu1.bounding)) {
    Serial.print ("found menu1 count:"); Serial.println (myButtonMenu1.Count);
    int i = WhatMenu(Xtouch, Ytouchinv, myButtonMenu1);
    Serial.print ("menu1 found index: "); Serial.println(i);
    ProcessMenu1(i);
  }
}




boolean NearChar(int Xtouch, int Ytouchinv , point &textdrawpoint) {

  Serial.print("Nearchar for x:");Serial.print(Xtouch);Serial.print(" y:");Serial.println(Ytouchinv);
  Serial.print("     drawpoint ");SerialprintPoint(textdrawpoint);
  
  return (NearPoint(Xtouch, Ytouchinv, textdrawpoint, TTextClickBuffer, TTextClickBufferOffset));

}


boolean NearPoint(int Xtouch, int Ytouchinv , point &targetpoint, int Buffer, point BufferOffset) {
  
  Serial.print("NearPoint for x:");Serial.print(Xtouch);Serial.print(" y:");Serial.println(Ytouchinv);
  Serial.print("    drawpoint ");SerialprintPoint(targetpoint);
  Serial.print("       Offset ");SerialprintPoint(BufferOffset);
  Serial.print("       Adjust x:");Serial.print(Xtouch + BufferOffset.x);Serial.print("y:");Serial.println(Ytouchinv + BufferOffset.y);
  Serial.print("       abs    x:");Serial.print(abs((Xtouch + BufferOffset.x) -  targetpoint.x));Serial.print("y:");Serial.println(abs((Ytouchinv + BufferOffset.y) - targetpoint.y));
  
  Serial.print("Buffer:");Serial.println(Buffer);
  
  return (abs((Xtouch + BufferOffset.x) -  targetpoint.x) < Buffer && abs((Ytouchinv + BufferOffset.y) - targetpoint.y) < Buffer);

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


int WhatMenu(int &x, int &y, buttonsetdef &butsetdef)
{

  Serial.print ("what menu count:"); Serial.println( butsetdef.Count);
  for (int i = 0 ; i < butsetdef.Count; i++) {
    Serial.print ("looking at index:"); Serial.println(i);
    SerialprintRect(butsetdef.buttondefs[i].Rect);
    if (InRect(x, y, butsetdef.buttondefs[i].Rect) == true) {

      return i;
    }
  }
  return -1;
}


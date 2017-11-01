#include "libraries\TFTLCD-Library-master\Adafruit_TFTLCD.h"
//----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------
// Get the temperature
int getCleanTemp(double temperature, int myID) {
  if (isnan(temperature)) {
    Readingskipped++;
    //Serial.print (myID); Serial.print ("nan temp:"); Serial.println(temperature);
    return -1;
  }
  else if (temperature > 1000) {
    Readingskipped++;
    Serial.print (myID); Serial.print ("too high temp:"); Serial.println(temperature);
    return -1;
  }
  else if (temperature < 40) {
    Readingskipped++;
    Serial.print (myID); Serial.print ("too low temp:"); Serial.println(temperature);
    return -1;
  }
  else {
    int r = temperature;
    //  Serial.print (myID);Serial.print ("clean temp returned:");Serial.println(r);
    return r;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ReadIntEprom(int loc, int min, int max, int def) {
  int t;
  t = EEPROM.read(loc);
  if (t >= 0 && t < 100) {
    return t ;
  }  else {
    return def ;
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ReadTempEprom(int loc, int def) {
  int t;
  t = EEPROM.read(loc) + 254;
  if (t > 254 && t < 495) {
    return ( t);
  }  else {
    return def ;
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SaveTempEprom(int loc, int temp) {
  EEPROM.update(loc, (temp - 254));
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double getBeanAvgTemp(double t1, double t2) {
  if (t1 != -1 && t2 != -1) {
    return (t1 + t2) / 2;
  }
  else {
    if     (t1 != -1) {
      return t1;
    }
    else {
      return t2;
    }
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double calcSetpoint(double roastminutes) {
  int setpoint;
  double r = (roastminutes - (int)roastminutes);
  setpoint = MyMinuteSetpoints[(int)roastminutes] + ((MyMinuteSetpoints[int(roastminutes) + 1] - MyMinuteSetpoints[(int)roastminutes]) * r);
  return setpoint;
}

//-------------------------------------------------------------------------------------------------------------------------------
//GRAPHING             GRAPHING             GRAPHING             GRAPHING             GRAPHING             GRAPHING
//-------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void graphProfile() {
  tft.setRotation(3);
  tft.fillScreen(BLACK);


  IYscale = ((TempScreenTop) * (TempScreenTop) * (TempScreenTop)) / 240;
  //IYscale = IYscale * 1000;
  //Serial.print("IYscale:");Serial.print(TempScreenTop);Serial.print("  ");Serial.println(IYscale);
  //draw grid
  int color = tft.color565(125, 125, 125);
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.setCursor(1 * PixelsPerMin, 230); tft.println("1");
  tft.drawFastVLine(3 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor(3 * PixelsPerMin, 230); tft.println("3");
  tft.drawFastVLine(6 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor(6 * PixelsPerMin, 230); tft.println("6");
  tft.drawFastVLine(9 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor(9 * PixelsPerMin, 230); tft.println("9");
  tft.drawFastVLine(12 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor(12 * PixelsPerMin, 230); tft.println("12");
  tft.drawFastVLine(12 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor(15 * PixelsPerMin, 230); tft.println("15");


  tft.setCursor(0 , Y(200)); tft.println("200");
  tft.drawFastHLine(0,  Y(300), tft.width(), color);
  tft.setCursor(0 , Y(300)); tft.println("300");
  tft.drawFastHLine(0,  Y(375), tft.width(), color);
  tft.setCursor(0 , Y(375)); tft.println("375");
  tft.drawFastHLine(0,  Y(400), tft.width(), color);
  tft.setCursor(0 , Y(400)); tft.println("400");
  tft.drawFastHLine(0,  Y(425), tft.width(), color);
  tft.setCursor(0 , Y(425)); tft.println("425");
  tft.drawFastHLine(0,  Y(440), tft.width(), color);
  tft.setCursor(0 , Y(440)); tft.println("440");
  tft.drawFastHLine(0,  Y(460), tft.width(), color);
  tft.setCursor(0 , Y(460)); tft.println("460");
  tft.drawFastHLine(0,  Y(475), tft.width(), color);
  tft.setCursor(0 , Y(475)); tft.println("475");
  tft.setCursor(0 , Y(490)); tft.println("490");

  //draw steamp
  tft.setCursor(200, 0); tft.print("TS:");
  tft.setCursor(220, 0); tft.print(day(MyProfileTimeStamp)); tft.print("/"); tft.print(month(MyProfileTimeStamp)); tft.print(" ");
  tft.print(hour(MyProfileTimeStamp)); tft.print(":"); tft.println(minute(MyProfileTimeStamp));



  //the first setpoint is the left y axis
  //the last setpoint is the right y axis
  //so we have setpoint - 1 spans on the graph

  StartLinebyTimeAndTemp (0, MyBaseSetpoints[0], SETPOINTLINEID , WHITE);
  MySpanAccumulatedMinutes[0] = 0;


  for (int X = 1; X < SetPointCount; X++) {
    MySpanAccumulatedMinutes[X] = MySpanAccumulatedMinutes[X - 1] + MySpanMinutes[X];

    for (int Y = MySpanAccumulatedMinutes[X - 1] ; Y < MySpanAccumulatedMinutes[X]; Y++) {
      MyMinuteSetpoints[Y] =  MyBaseSetpoints[X - 1] + ((MyBaseSetpoints[X] - MyBaseSetpoints[X - 1]) * (double)(Y - MySpanAccumulatedMinutes[X - 1]) / (double)MySpanMinutes[X]);
      //Serial.print("minutesp:");Serial.print(Y);Serial.print("  ");Serial.println(MyMinuteSetpoints[Y]);
      //Serial.print (MyBaseSetpoints[X]);Serial.print (" - ");Serial.print (MyBaseSetpoints[X-1]);Serial.print (" - ");Serial.print ( Y );Serial.print (" - ");Serial.println ( MyMinuteSetpoints[Y]);
    }
    MySetpointsX[X] = MySpanAccumulatedMinutes[X] * PixelsPerMin;
    MySetpointsY[X] = 240 - Y(MyBaseSetpoints[X]);

    AddLinebyTimeAndTemp(MySpanAccumulatedMinutes[X], MyBaseSetpoints[      X], SETPOINTLINEID);

    tft.drawFastVLine(MySpanAccumulatedMinutes[X] * PixelsPerMin, Y(MyBaseSetpoints[X] + 10), 20, WHITE);
  }

  //create setpoint array for last segment
  for (int Y = MySpanAccumulatedMinutes[SetPointCount - 1] ; Y <=     TimeScreenLeft; Y++) {
    MyMinuteSetpoints[Y] = MyBaseSetpoints[SetPointCount - 1] ;
    //Serial.print("minutesp:");Serial.print(Y);Serial.print("  ");Serial.println(MyMinuteSetpoints[Y]);
  }


  TempEnd = MyBaseSetpoints[SetPointCount - 2];
  //Serial.print ("TempEnd:");Serial.println (TempEnd);
  tft.setCursor(tft.width() / 2, Y(TempEnd) - 10);  tft.println(TempEnd);
  tft.drawFastHLine(((tft.width() / 2) + 20)    ,  Y(TempEnd), tft.width(), BLUE);
  tft.drawFastVLine(PixelsPerMin * MySpanAccumulatedMinutes[SetPointCount - 2], 0, tft.height() / 2, BLUE);


  //tft.fillRect(0,0,5,5, BLUE);
  //tft.fillRect(0,235,5,5, RED);
  //tft.fillRect(315,0,5,5,YELLOW);
  //tft.fillRect(315,235,5,5,WHITE);

  tft.drawFastVLine(PixelsPerMin * MySpanAccumulatedMinutes[SetPointCount - 2], 0, tft.height() / 2, BLUE);
  //delay(1000);

  ReDrawROLLAVGLINEFromArray(ORANGE);
  if (AdjustmentSetpoint > 0) DrawAdjustMentBoxes(AdjustmentSetpoint);
}


void UpdateAdjustMentText()
{
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.setCursor(225, AdjustmentDisplayTop + 25);  tft.println("New:"); tft.setCursor(260, AdjustmentDisplayTop + 25) ;   tft.println( MyBaseSetpoints[AdjustmentSetpoint]);
  tft.setCursor(225, AdjustmentDisplayTop + 55 ); tft.println("New:"); tft.setCursor(260, AdjustmentDisplayTop + 55);  tft.println( MySpanAccumulatedMinutes[AdjustmentSetpoint]);
}
void HideAdjustMentBoxes()
{
  AdjustmentSetpoint = 0;
  tft.fillRect(150, AdjustmentDisplayTop - 10, 150, 80, BLACK);
}

void DrawAdjustMentBoxes(int setpoint)
{
  // make the color selection boxes
  AdjustmentSetpoint = setpoint;

  tft.fillRect(150, AdjustmentDisplayTop - 10, 150, 80, BLACK);
  tft.drawRect(150, AdjustmentDisplayTop - 10, 150, 80, BLUE);

  tft.fillRect(180, AdjustmentDisplayTop      , 20,  20, GREEN); //TOP
  tft.fillRect(200, AdjustmentDisplayTop + 20 , 20, 20, GREEN); //RIGHT
  tft.fillRect(180, AdjustmentDisplayTop + 40 , 20, 20, GREEN); //BOTTOM
  tft.fillRect(160, AdjustmentDisplayTop + 20 , 20, 20, GREEN); //LEFT

  tft.fillRect(280, AdjustmentDisplayTop - 10 , 20, 20,  BLUE); //LEFT
  tft.drawLine(280, AdjustmentDisplayTop + 10 , 300 ,  AdjustmentDisplayTop - 10, BLACK);
  tft.drawLine(280, AdjustmentDisplayTop - 10 , 300 ,  AdjustmentDisplayTop + 10, BLACK);

  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(2);
  tft.setCursor(184, AdjustmentDisplayTop + 23 ); tft.println(setpoint);

  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.setCursor(225, AdjustmentDisplayTop + 15);  tft.println("Temp:"); tft.setCursor(260, AdjustmentDisplayTop + 15) ;   tft.println( MyBaseSetpoints[AdjustmentSetpoint]);
  tft.setCursor(225, AdjustmentDisplayTop + 40 ); tft.println("Mins:"); tft.setCursor(260, AdjustmentDisplayTop + 40);  tft.println( MySpanAccumulatedMinutes[AdjustmentSetpoint]);

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ProcessTouch(int Xtouch, int Ytouch)
{

  Serial.print ("x:"); Serial.print (Xtouch); Serial.print (" y:"); Serial.println (Ytouch);
  for (int X = 1; X < SetPointCount; X++) {
    if (abs(Xtouch - MySetpointsX[X]) < 20 && abs(Ytouch - MySetpointsY[X]) < 20 ) {
      DrawAdjustMentBoxes(X);
      return;
    }
  }

  if (abs(Xtouch - GainUX) < 5 && abs(Ytouch - GainUY) < 5 ) {
    Gain = Gain + 1;
    if (Gain > 50) {Gain = 50;}
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
  }

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------


int Y(int temp)
{

  //460 x 460 x 460 /240  = 379.687
  return (240 -  ((double)temp * (double)temp * double(temp)) / IYscale);
}

void tftPrintDouble(double num)
{
  //tft.println("  ");
  tft.println(num, 1);
}
void tftPrintIntTo5Char(int num)
{
  //  Serial.print("print:");Serial.println(num);
  sprintf(Buf5, "% 5d", num);
  tft.println(Buf5);
  // Serial.print("printbuf:");Serial.println(num);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void UpdateGraphA(double roastMinutes, double duty, int setpoint, double err, double errI) {
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  int row = 0;
  // tft.setCursor(40 ,row); tft.println("F:"); tft.setCursor(50 , row); tft.println(TempEnd);
  // tft.setCursor(100 , row); tft.println("Last:"); tft.setCursor(130 , row); tft.println(TempLastEndOfRoast);
  row = 20;
  tft.setCursor(40 , row); tft.print("sp:"); tft.setCursor(75 , row); tft.println(setpoint); tft.setCursor(100 , row); tft.print("SSD:"); 
  tft.setCursor(130 , row); tft.println(ManagingSSR);
  
  row = 35;
  tft.setCursor(40 , row); tft.print("Time:");   tft.setCursor(75 , row); tft.println(roastMinutes);
  row = 50;
  if (duty > 1) {
    tft.setCursor(40 , row);tft.println("Duty:");tft.setCursor(75 , row); tft.println("1.00");
  }
  else {
    tft.setCursor(40 , row); tft.println("Duty:"); tft.setCursor(75 , row); tft.println(duty);
  }
  row = 65;
  tft.setCursor(40 , row); tft.print("Err:"); tft.setCursor(75 , row); tft.println("      "); tft.println(-err);
  row = 80;
  tft.setCursor(40 , row); tft.print("IEr:"); tft.setCursor(75 , row);  tft.println("      ");tft.println(-errI);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void UpdateGraphB(int temp2, int temp1, int tempCoil, double ampHeater1, double ampHeater2, int tempFan     , double ampFan, double volts)
{
  //Serial.print("TBean2:");Serial.print(temp2);Serial.print("TBean1:");Serial.print (temp1);Serial.print("TCoil:"); Serial.print(tempCoil); Serial.print("TFan:");Serial.println(tempFan);
  if (AdjustmentSetpoint > 0 ) return;


  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  int row = 135;
  tft.setCursor(150 , row); tft.println("Tvg:"); tft.setCursor(190 , row); tftPrintIntTo5Char(TBeanAvgRoll.mean());
  tft.setCursor(240 , row); tft.println("Skip"); tft.setCursor(270 , row); tftPrintIntTo5Char(Readingskipped);

  row = 150;
  tft.setCursor(150 , row); tft.println("T1:"); tft.setCursor(190 , row);  tft.println("   "); tftPrintIntTo5Char(temp1);
  tft.setCursor(240 , row); tft.println("T2"); tft.setCursor(270 , row);   tft.println("   "); tftPrintIntTo5Char(temp2);
  row = 165;
  tft.setCursor(150 , row); tft.println("Fan  T:"); tft.setCursor(190 , row); tftPrintIntTo5Char(tempFan);
  tft.setCursor(240 , row); tft.println("Amp:"); tft.setCursor(270 , row); tftPrintDouble(ampFan);
  row = 180;
  tft.setCursor(050 , row); tft.println("G:"); tft.setCursor(60 , row); tft.println(Gain); tft.setCursor(100 , row); tft.println("U D");
  GainX =  60; GainY = 180;
  GainUX = 102; GainUY = 65;
  GainDX = 114; GainDY = 65;
  tft.setCursor(150 , row); tft.println("Heat T:"); tft.setCursor(190 , row); tftPrintIntTo5Char(tempCoil);
  tft.setCursor(240 , row); tft.println("Amp1:"); tft.setCursor(270 , row); tftPrintDouble(ampHeater1);
  row = 195;
  tft.setCursor(050 , row); tft.println("I:"); tft.setCursor(60 , row);
  tft.println(Integral) ; tft.setCursor(100 , row); tft.println("U D");
  IntegralX  = 60; IntegralY  = 195;
  IntegralUX = 102; IntegralUY = 55;
  IntegralDX = 114; IntegralDY = 55;
  tft.setCursor(150 , row); tft.println("Volts:"); tft.setCursor(190 , row); tft.println(volts);
  tft.setCursor(240 , row); tft.println("Amp2:"); tft.setCursor(270 , row); tftPrintDouble(ampHeater2);
  row = 210;
  tft.setCursor(050 , row); tft.println("ps:"); tft.setCursor(60 , row); tftPrintIntTo5Char(LoopsPerSecond) ;

  // tft.setCursor(240 , row); tft.println("Amp:"); tft.setCursor(270 , row); tft.println(ampHeater);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void StartLinebyTimeAndTemp(double timemins, int temp, int lineID, uint16_t color)
{
  Serial.print ("StartLineTandT id:"); Serial.print (lineID); Serial.print(" color:"); Serial.println(color);
  LastXforLineID[lineID] = (PixelsPerMin * timemins);
  if (temp > 0) {
    LastYforLineID[lineID] = Y(temp);
  }
  else {
    LastYforLineID[lineID] = 240;
  }
  LineColorforLineID[lineID] = color;
  if (lineID == ROLLAVGLINEID) {
    for (int X = 0; X < 320; X++) {
      myLastGraph[X] = -1;
    }
  }
}


//void DrawPointByTimeAndTemp(double timemins, int temp, int lineID)
//{
//  int newX = (PixelsPerMin * timemins);
//  int newY = Y(temp);
//  //Serial.print ("AddLineTandT i:");Serial.print (i);Serial.print(" color:");Serial.println(LineColorforLineID[i]);
//  //Serial.print ("AddLineTandT i:");Serial.print (i);Serial.print(" time:");Serial.print(timemins);Serial.print(" temp:");Serial.println(temp);
//
//  tft.drawPixel(newX, newY, LineColorforLineID[lineID]);
//}
void AddLinebyTimeAndTemp(double timemins, int temp, int lineID)
{
  uint16_t newX = (uint16_t)(PixelsPerMin * timemins);
  int newY = Y(temp);
  //Serial.print ("AddLineTandT line iD:");Serial.print (lineID);Serial.print(" time:");Serial.print(timemins);Serial.print("temp:");Serial.print(temp);Serial.print(" color:");Serial.println(LineColorforLineID[lineID]);
  //Serial.print ("newX:");Serial.print (newX); Serial.print ("pixelspermin:");Serial.print (PixelsPerMin); Serial.print(" newY:");Serial.println(newY);
  tft.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY, LineColorforLineID[lineID]);
  LastXforLineID[lineID] = newX;
  LastYforLineID[lineID] = newY;
  if (lineID == ROLLAVGLINEID) {
    BoldLine(LastXforLineID[lineID], LastYforLineID[lineID] + 1, newX, newY + 1, LineColorforLineID[lineID]);
    myLastGraph[newX] = newY;
  }
}

void LoadORSaveToHistory(boolean Load) {
  int number = 1;
  //300 to 306
  int address = number * 100;
  if (!Load) {
    unsigned long timestamp = millis();
    EEPROM.put(address, timestamp);
    address = address + 4;
    EEPROM.write(address, TempLastEndOfRoast);
    address++;
    EEPROM.write(address, (TimeLastEndOfRoast * 10));
  }
  //312 - 318 & 319 to 325
  address = (number * 100) + 12;
  if (!Load) {
    for (int X = 0; X < SetPointCount; X++) {
      address++;
      EEPROM.write(address, MySpanMinutes[X]);
      SaveTempEprom(address + SetPointCount, MyBaseSetpoints[X]);
    }
  }
  else {
    for (int X = 0; X < SetPointCount; X++) {
      address++;
      MySpanMinutes[X] = EEPROM.read(address);
      MyBaseSetpoints[X] = ReadTempEprom(address + SetPointCount , 0);
    }
  }
  //330 to 394 (320 pixes wide/5 = 64)
  address = (number * 100) + 30;
  int avgwidth = 5;
  if (!Load) {
    int total; int count;
    for (int X = 0; X < 320; X++) {
      total = total + myLastGraph[X];
      count++;
      if (count >= avgwidth) {
        address++;
        int avg = total / avgwidth;
        EEPROM.write(address, avg);
      }
    }
  }
  else {
    int Xlast = 0;
    int Ylast = EEPROM.read(address);
    address++;
    for (int X = 1; X < 64; X++) {
      int Ynext =  EEPROM.read(address);
      double Ydelta = (double)(Ynext - Ylast) / (double)avgwidth;
      for (int k = 1;  k <= avgwidth; X++) {
        myLastGraph[(X - 1) + k] = Ylast + (Ydelta * k) ;
      }
      address++;
    }
  }
  if (Load) {

  }
}
void ReDrawROLLAVGLINEFromArray(int color) {
  LastXforLineID[1] = 0;
  LastYforLineID[1] = 240;
  for (int X = 0; X < 320; X++) {
    if (myLastGraph[X] > 0 ) {
      //  Serial.print ("DrawRealTime ");Serial.print(" color:");Serial.println(color);
      tft.drawLine(LastXforLineID[1], LastYforLineID[1] , X, myLastGraph[X], color);
      BoldLine(LastXforLineID[1], LastYforLineID[1] , X, myLastGraph[X], color);
      LastXforLineID[1] = X;
      LastYforLineID[1] = myLastGraph[X];
    }
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BoldLine(int x, int y, int newX, int newY, int color) {
  tft.drawLine(x, y + 1, newX, newY + 1, color);
  tft.drawLine(x + 1, y + 1, newX + 1, newY + 1, color);
  tft.drawLine(x + 1, y, newX + 1, newY, color);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawLinebyTimeAndTemp(double timemins1, int temp1, double timemins2, int temp2, int color) {
  tft.drawLine(PixelsPerMin * timemins1, Y(temp1), PixelsPerMin * timemins2, Y(temp2), color);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  displayState(int state) {
  tft.setCursor(40, 0);
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  switch (state) {
    case AMROASTING:
      tft.setTextColor(WHITE, RED); tft.println("*AmRoasting*");
      break;
    case AMSTOPPED:
      tft.println( "AmStopped        ");
      break;
    case AMAUTOCOOLING:
      tft.setTextColor(WHITE, BLUE); tft.println( "AmCooling        ");
      break;
    case AMOVERHEATEDCOIL:
      tft.println ("To Hot Coil            ");
      break;
    case AMOVERHEATEDFAN:
      tft.println ("To Hot Fan            ");
      break;
    case AMFANONLY:
      tft.println ("Fan only            ");
      break;
    default:
      tft.println ("unkState   ");
      break;
  }
}

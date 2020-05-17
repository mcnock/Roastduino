
//----------------------------------------------------------------------------------------------------------------------------------------------------
// GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN
//----------------------------------------------------------------------------------------------------------------------------------------------------


void graphProfile() {

  tft.setRotation(3);

  TempYMax = 700; 
  TempYSplit2 = 440;
  TempYSplit2 = MySetPoints[5].Temperature ;
  PixelYSplit2 = 180;
  TempYSplit = (MySetPoints[1].Temperature) ;
  PixelYSplit = 90;

//we have 240 units...
//240-120 is for < 400 >> 400/120  3.333 degrees per pixel
//120 - 0 is for < 400 - 600 >> 200/120 1.66 degrees per pixel
  TempPerPixL = TempYSplit / PixelYSplit;
  TempPerPixM = (TempYSplit2 - TempYSplit) / (PixelYSplit2 - PixelYSplit);
  TempPerPixH = (TempYMax - TempYSplit2) / (240.00 - PixelYSplit2);

  TimeScreenLeft = MySetPoints[5].Minutes + 1;

  PixelsPerMin =  (int)(320 / TimeScreenLeft);

  tft.fillScreen(BLACK);

  //draw x scale
  int color = tft.color565(125, 125, 125);
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.drawFastVLine(3 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor((3 * PixelsPerMin) - 12, 230); tft.println("3min");
  tft.drawFastVLine(6 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor((6 * PixelsPerMin) - 12, 230); tft.println("6min");
  tft.drawFastVLine(9 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor((9 * PixelsPerMin) - 12, 230); tft.println("9min");
  tft.drawFastVLine(12 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor((12 * PixelsPerMin) - 12, 230); tft.println("12min");
  tft.drawFastVLine(12 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor((15 * PixelsPerMin) - 12, 230); tft.println("15");

  //draw y scale

  Serial.println(TempYSplit);
  for (int t = 100; t < (TempYSplit - 20); t = t + 100) {
    Serial.println(t);
    tft.drawFastHLine(0,  YforATemp(t), tft.width(), color);
    tft.setCursor(2 , YforATemp(t) - 4); tft.println(t);
  }


  for (int t = TempYSplit; t < (TempYSplit2); t = t + 20) {
    tft.drawFastHLine(0,  YforATemp(t), tft.width(), color);
    tft.setCursor(2 , YforATemp(t) - 4); tft.println(t);
  }


  for (int t = TempYSplit2 +10; t < TempYMax ; t = t + 100) {
    tft.drawFastHLine(0,  YforATemp(t), tft.width(), color);
    tft.setCursor(2 , YforATemp(t) - 4); tft.println(t);
  }

  StartLinebyTimeAndTemp (0, MySetPoints[0].Temperature, SETPOINTLINEID , WHITE);
  //set the by minute temp profile for 5 spans
  MyMinuteSetpoints[0] = 0;
  int accumulatedMinutes = 0;
  for (int x = 1; x < SetPointCount; x++) {
    double TempPerMinuteinSpan = ((double)(MySetPoints[x].Temperature - MySetPoints[x - 1].Temperature)) / MySetPoints[x].SpanMinutes ;
    for (int xSpanMinute = 1 ; xSpanMinute <= MySetPoints[x].SpanMinutes  ;  xSpanMinute++)
    {
      accumulatedMinutes = accumulatedMinutes + 1;
      MyMinuteSetpoints[accumulatedMinutes] =  MySetPoints[x - 1].Temperature + ( TempPerMinuteinSpan * xSpanMinute);
      AddLinebyTimeAndTemp(accumulatedMinutes, MyMinuteSetpoints[accumulatedMinutes], SETPOINTLINEID);
    }
    AddPointbyTimeAndTempAndLineID(accumulatedMinutes, MySetPoints[x].Temperature, SETPOINTLINEID, 5);
  }

  //create temp setpoint array between the last setpoint and the left of screen
  // for (int Y = MySetPoints.Temperature[SetPointCount - 1] ; Y <= TimeScreenLeft; Y++) {
  //   MyMinuteSetpoints[Y] = MySetPoints[SetPointCount - 1].Temperature ;
  //Serial.print("minutesp:");Serial.print(Y);Serial.print("  ");Serial.println(MyMinuteSetpoints[Y]);
  //  }

  //draw endpoint highlights
  tft.setCursor((tft.width() / 2), YforATemp(MySetPoints[EndingSetPoint].Temperature) - 10);
  tft.println("END:"); tft.setCursor((tft.width() / 2) + 24, YforATemp(MySetPoints[EndingSetPoint].Temperature) - 10);
  tft.println(MySetPoints[EndingSetPoint].Temperature);
  tft.drawFastHLine(((tft.width() / 2) + 20)    ,  YforATemp(MySetPoints[EndingSetPoint].Temperature), tft.width(), BLUE);
  tft.drawFastVLine(PixelsPerMin * MySetPoints[EndingSetPoint].Temperature, 0, tft.height() / 2, BLUE);
  tft.drawFastVLine(PixelsPerMin * MySetPoints[EndingSetPoint].Minutes, 0, tft.height() / 2, BLUE);
  //delay(1000);

  ReDrawROLLAVGLINEFromArray(ORANGE);

  DrawControlButtons();
  DrawVertMenu1();

}
void drawprofileline() {
  StartLinebyTimeAndTemp (0, MySetPoints[0].Temperature, SETPOINTLINEID , WHITE);
  //set the by minute temp profile for 5 spans
  MyMinuteSetpoints[0] = 0;
  int accumulatedMinutes = 0;
  for (int x = 1; x < SetPointCount; x++) {
    double TempPerMinuteinSpan = ((double)(MySetPoints[x].Temperature - MySetPoints[x - 1].Temperature)) / MySetPoints[x].SpanMinutes ;
    for (int xSpanMinute = 1 ; xSpanMinute <= MySetPoints[x].SpanMinutes  ;  xSpanMinute++)
    {
      accumulatedMinutes = accumulatedMinutes + 1;
      MyMinuteSetpoints[accumulatedMinutes] =  MySetPoints[x - 1].Temperature + ( TempPerMinuteinSpan * xSpanMinute);
      AddLinebyTimeAndTemp(accumulatedMinutes, MyMinuteSetpoints[accumulatedMinutes], SETPOINTLINEID);
    }
    AddPointbyTimeAndTempAndLineID(accumulatedMinutes, MySetPoints[x].Temperature, SETPOINTLINEID, 5);
  }




}
void tftPrintDouble5b(double num ) {

  //+11.1
  //12345
  if (num < 0)
  {
    sprintf(Buf5, "%d.%1d", (int)num, abs((int)(num * 100) % 100));

  } else
  {
    sprintf(Buf5, " %d.%1d", (int)num, (int)(num * 100) % 100);
  }
  tft.println(Buf5);
}
void tftPrintDouble5(double num ) {

  //+1.11
  //12345
  if (num < 0)
  {
    sprintf(Buf5, "%d.%02d", (int)num, abs((int)(num * 100) % 100));

  } else
  {
    sprintf(Buf5, " %d.%02d", (int)num, (int)(num * 100) % 100);
  }
  tft.println(Buf5);
}
void tftPrintDouble6(double num ) {

  //+11.11
  //123456
  if (num < 0)
  {
    sprintf(Buf6, "%d.%02d", (int)num, abs((int)(num * 100) % 100));

  } else
  {
    sprintf(Buf6, " %d.%02d", (int)num, (int)(num * 100) % 100);
  }
  tft.println(Buf6);
}
void tftPrintDouble7(double num ) {
  //+111.11
  //1234567

  if (num < 0)
  {
    sprintf(Buf7, "%d.%02d", (int)num, abs((int)(num * 100) % 100));

  } else
  {
    sprintf(Buf7, " %d.%02d", (int)num, (int)(num * 100) % 100);
  }
  tft.println(Buf7);
}
void tftPrintDouble(double num ) {

  tft.println(num, 1);

}
void tftPrintIntTo5Char(int num) {
  //  Serial.print("print:");Serial.println(num);
  sprintf(Buf5, "% 5d", num);
  tft.println(Buf5);
  // Serial.print("printbuf:");Serial.println(num);
}

void UpdateGraphA() {
  //this is top row
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  int rowheight = 12;
  int row = 25;
  int col = 40;
  tft.setCursor(col , row); tft.println("Time:");   tft.setCursor(col + 45 , row); tft.println(RoastMinutes);
  tft.setCursor(col + 90 , row); tft.print(" sp:"); tft.setCursor(col + 130 , row);   tftPrintDouble7(CurrentSetPointTemp);
  row = row + rowheight;
  if (Duty > 1) {
    tft.setCursor(col , row); tft.println("Duty:"); tft.setCursor(col + 40 , row); tftPrintDouble7(1.00);
  }
  else {
    tft.setCursor(col , row); tft.println("Duty:"); tft.setCursor(col + 40 , row); tftPrintDouble7(Duty);
  }
  row = row + rowheight;
  tft.setCursor(col + 10 , row); tft.print("Err:"); tft.setCursor(col + 40 , row);   tftPrintDouble7(-Err);

  row = row + rowheight;
  tft.setCursor(col + 10 , row); tft.print("IEr:"); tft.setCursor(col + 40 , row);   tftPrintDouble7(-ErrI);
  row = row + rowheight;

}
void UpdateGraphB() {

  int rowheight = 11;

  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  int row = 160 ;
  int col = 135 ;

  tft.setCursor(col , row); tft.println("Tvg:"); tft.setCursor(col + 40 , row); tftPrintIntTo5Char(TBeanAvgRoll.mean());

  row = row + rowheight; ;
  tft.setCursor(col , row); tft.println("T1:"); tft.setCursor(col + 40 , row);  tft.println("   "); tft.setCursor(col + 40 , row); tftPrintIntTo5Char(TBean1);
  tft.setCursor(col + 90 , row); tft.println("T2"); tft.setCursor(col + 120 , row);   tft.println("   "); tft.setCursor(col + 120 , row); tftPrintIntTo5Char(TBean2);

  row = row + rowheight;
  tft.setCursor(col , row); tft.println("Fan  T:"); tft.setCursor(col + 40 , row); tftPrintIntTo5Char(TFan);
  tft.setCursor(col + 90 , row); tft.println("Amp:"); tft.setCursor(col + 120 , row); tftPrintDouble5b(CurrentFan);

  row = row + rowheight;

  tft.setCursor(col , row); tft.println("Heat T:"); tft.setCursor(col + 40 , row); tftPrintIntTo5Char(TCoil); tft.setCursor(col + 90 , row); tft.println("Amp1:"); tft.setCursor(col + 120 , row); tftPrintDouble5b(CurrentHeat1);

  row = row + rowheight;;

  tft.setCursor(col , row); tft.println("Volts:"); tft.setCursor(col + 40 , row); tft.println(MaxVoltage);
  tft.setCursor(col + 90 , row); tft.println("Amp2:"); tft.setCursor(col + 120 , row); tftPrintDouble5b(CurrentHeat2);


}
void UpdateGraphC() {
  int rowheight = 11;
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  int row = 180 ;
  int colr = 40 ;


  TGainValue.x =  colr + 10; TGainValue.y = row;// TGainIncrease.x = colr + 35;
  //TGainIncrease.y = row; TGainDecrease.x = colr + 55; TGainDecrease.y = row;

  tft.setCursor(colr , row); tft.println("G:"); tft.setCursor(TGainValue.x , TGainValue.y); tft.println(Gain);
  //tft.setCursor(TGainIncrease.x, TGainIncrease.y); tft.print("U"); tft.setCursor(TGainDecrease.x   , TGainDecrease.y); tft.println("D");


  row = row + rowheight;;
  TIntegralValue.x  = colr + 5; TIntegralValue.y  = row;//  TIntegralIncrease.x = colr + 35 ;
 // TIntegralIncrease.y = row; TIntegralDecrease.x = colr + 55; TIntegralDecrease.y = row;
  TIntegralReset.y = row; TIntegralReset.x = colr + 75; //TIntegralReset.y = row;

  tft.setCursor(colr - 5 , row); tft.println("I:"); tft.setCursor(TIntegralValue.x , TIntegralValue.y); tft.println(Integral) ;
 // tft.setCursor(TIntegralIncrease.x  , TIntegralIncrease.y); tft.println("U"); tft.setCursor(TIntegralDecrease.x , TIntegralDecrease.y); tft.print("D");
  tft.setCursor(TIntegralReset.x , TIntegralReset.y); tft.print("R");

  row = row + rowheight;;
  tft.setCursor(colr - 10 , row); tft.println("ps:"); tft.setCursor(colr  , row); tftPrintIntTo5Char(LoopsPerSecond) ;
  tft.setCursor(colr +  35 , row); tft.print("Skp"); tft.setCursor(colr + 60 , row); tft.println(Readingskipped);

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void StartLinebyTimeAndTemp(double timemins, int temp, int lineID, uint16_t color) {
  //Serial.print ("StartLineTandT id:");Serial.print (lineID);Serial.print(" color:");Serial.println(color);
  LastXforLineID[lineID] = (PixelsPerMin * timemins);
  if (temp > 0) {
    LastYforLineID[lineID] = YforATemp(temp);
  }
  else {
    LastYforLineID[lineID] = 240;
  }
  LineColorforLineID[lineID] = color;
  if (lineID == ROLLAVGLINEID) {
    for (int X = 0; X < 320; X++) {
      myLastGraphYPixels[X] = -1;
      myLastGraphTemps[X] = -1;
    }
  }
}


void AddLinebyTimeAndTemp(double timemins, int temp, int lineID) {
  uint16_t newX = (uint16_t)(PixelsPerMin * timemins);
  int newY = YforATemp(temp);
  //Serial.print ("AddLineTandT line iD:");Serial.print (lineID);Serial.print(" time:");Serial.print(timemins);Serial.print("temp:");Serial.print(temp);Serial.print(" color:");Serial.println(LineColorforLineID[lineID]);
  //Serial.print ("newX:");Serial.print (newX);Serial.print ("pixelspermin:");Serial.print (PixelsPerMin);Serial.print(" newY:");Serial.println(newY);
  tft.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY, LineColorforLineID[lineID]);
  LastXforLineID[lineID] = newX;
  LastYforLineID[lineID] = newY;
  if (lineID == ROLLAVGLINEID) {
    BoldLine(LastXforLineID[lineID], LastYforLineID[lineID] + 1, newX, newY + 1, LineColorforLineID[lineID]);
    myLastGraphYPixels[newX] = newY;
    myLastGraphTemps[newX] = temp;
  }
}
void AddPointbyTimeAndTempAndLineID(double timemins, int temp, int lineID, int radius) {
  uint16_t newX = (uint16_t)(PixelsPerMin * timemins);
  int newY = YforATemp(temp);
  tft.fillCircle(newX, newY, radius, LineColorforLineID[lineID]);

  //Serial.print ("AddLineTandT line iD:");Serial.print (lineID);Serial.print(" time:");Serial.print(timemins);Serial.print("temp:");Serial.print(temp);Serial.print(" color:");Serial.println(LineColorforLineID[lineID]);
  //Serial.print ("newX:");Serial.print (newX);Serial.print ("pixelspermin:");Serial.print (PixelsPerMin);Serial.print(" newY:");Serial.println(newY);
  //tft.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY, LineColorforLineID[lineID]);
  LastXforLineID[lineID] = newX;
  LastYforLineID[lineID] = newY;

}

void AddPointbyTimeandTemp(double timemins, int temp, int color, int radius) {
  uint16_t newX = (uint16_t)(PixelsPerMin * timemins);
  int newY = YforATemp(temp);
  tft.fillCircle(newX, newY, radius, color);

  //Serial.print ("AddLineTandT line iD:");Serial.print (lineID);Serial.print(" time:");Serial.print(timemins);Serial.print("temp:");Serial.print(temp);Serial.print(" color:");Serial.println(LineColorforLineID[lineID]);
  //Serial.print ("newX:");Serial.print (newX);Serial.print ("pixelspermin:");Serial.print (PixelsPerMin);Serial.print(" newY:");Serial.println(newY);
  //tft.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY, LineColorforLineID[lineID]);

}

void ReDrawROLLAVGLINEFromArray(int color) {
  LastXforLineID[1] = 0;
  LastYforLineID[1] = 240;
  for (int X = 0; X < 320; X++) {
    if (myLastGraphYPixels[X] > 0 ) {
      //  Serial.print ("DrawRealTime ");Serial.print(" color:");Serial.println(color);
      tft.drawLine(LastXforLineID[1], LastYforLineID[1] , X, myLastGraphYPixels[X], color);
      BoldLine(LastXforLineID[1], LastYforLineID[1] , X, myLastGraphYPixels[X], color);
      LastXforLineID[1] = X;
      LastYforLineID[1] = myLastGraphYPixels[X];
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
void DrawLinebyTimeAndTemp(boolean log, double timemins1, int temp1, double timemins2, int temp2, int color) {
  if (log == true)
  {
    //Serial.print ("timemins1:");
    //Serial.print (timemins1);
    //Serial.print(" temp1:");
    //Serial.println(temp1);
    //Serial.print(" timemins2:");
    //Serial.print (timemins2);
    //Serial.print (" color:");
    //Serial.println(color);
  }
  tft.drawLine(PixelsPerMin * timemins1, YforATemp(temp1), PixelsPerMin * timemins2, YforATemp(temp2), color);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void  displayState(int state) {
  tft.setCursor(30, 0);
  tft.setTextSize(2);
  tft.setTextColor(WHITE, BLACK);
  switch (state) {
    case STATEROASTING:
      tft.setTextColor(WHITE, RED);
      strncpy(StateName, "Roasting", 8);
      tft.println("Roasting");
      break;
    case STATESTOPPED:
      strncpy(StateName, "Stopped ", 8);
      tft.println("Stopped ");
      break;
    case STATECOOLING:
      tft.setTextColor(WHITE, BLUE);
      strncpy(StateName, "Cooling ", 8);
      tft.println( "Cooling ");
      break;
    case STATEOVERHEATED:
      strncpy(StateName, "HotCoil  ", 9);
      tft.println ("HotCoil ");
      break;
    case STATENOFANCURRENT:
      strncpy(StateName, "NoFanCur   ", 8);
      tft.println ("HotFan ");
      break;
    case STATEFANONLY:
      strncpy(StateName, "FanOnly ", 8);
      tft.println ("FanOnly ");
      break;
    default:
      strncpy(StateName, "unk   ", 8);
      tft.println ("unk     ");
      break;
  }
}

int UpdateMySetPointSpan(struct setpoint *setpoint)
{
}
int YforATemp(int temp) {
  int result = 0;
  if ( temp < 0 ) {
    temp = 0;
  }  
  if (temp <= TempYSplit) {
    result = (240 - ((double)temp / TempPerPixL));
  }
  else if (temp <= TempYSplit2) {
    result = ((240 - PixelYSplit) - ((double)(temp - TempYSplit) / TempPerPixM));
    //460 x 460 x 460 /240  = 379.687
    //return (240 -  ((double)temp * (double)temp * double(temp)) / IYscale);
  } else {
    result = ((240 -  PixelYSplit2) - ((double)(temp - TempYSplit2) / TempPerPixH));
  }
  if (result < 0) {
    return 1;
  }
  else
  { return result;
  }


}

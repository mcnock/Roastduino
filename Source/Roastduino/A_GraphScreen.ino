
//----------------------------------------------------------------------------------------------------------------------------------------------------
// GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN
//----------------------------------------------------------------------------------------------------------------------------------------------------


void graphProfile() {
  
  tft.setRotation(3);
  
  tft.fillScreen(BLACK);


  int color = tft.color565(125, 125, 125);
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  tft.drawFastVLine(3 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor((3 * PixelsPerMin)-12, 230); tft.println("3min");
  tft.drawFastVLine(6 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor((6 * PixelsPerMin)-12, 230); tft.println("6min");
  tft.drawFastVLine(9 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor((9 * PixelsPerMin)-12, 230); tft.println("9min");
  tft.drawFastVLine(12 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor((12 * PixelsPerMin)-12, 230); tft.println("12min");
  tft.drawFastVLine(12 * PixelsPerMin , 0, tft.height(), color);
  tft.setCursor((15 * PixelsPerMin)-12, 230); tft.println("15");


  tft.drawFastHLine(0,  Y(100), tft.width(), color);
  tft.setCursor(2 , Y(100) - 4); tft.println("100");
  tft.drawFastHLine(0,  Y(200), tft.width(), color);
  tft.setCursor(2 , Y(200) - 4); tft.println("200");
  tft.drawFastHLine(0,  Y(300), tft.width(), color);
  tft.setCursor(2 , Y(300)-4); tft.println("300");
  tft.drawFastHLine(0,  Y(350), tft.width(), color);
  tft.setCursor(2 , Y(350)-4); tft.println("350");
  tft.drawFastHLine(0,  Y(400), tft.width(), color);
  tft.setCursor(2 , Y(400)-4); tft.println("400");
  tft.drawFastHLine(0,  Y(410), tft.width(), color);
  tft.setCursor(2 , Y(410)-4); tft.println("410");
  tft.drawFastHLine(0,  Y(420), tft.width(), color);
  tft.setCursor(2 , Y(420)-4); tft.println("420");
  tft.drawFastHLine(0,  Y(430), tft.width(), color);
  tft.setCursor(2 , Y(430)-4); tft.println("430");
  tft.drawFastHLine(0,  Y(440), tft.width(), color);
  tft.setCursor(2 , Y(440)-4); tft.println("440");
  tft.drawFastHLine(0,  Y(500), tft.width(), color);
  tft.setCursor(2 , Y(500)-4); tft.println("500");
  tft.drawFastHLine(0,  Y(600), tft.width(), color);
  tft.setCursor(2 , Y(600)-4); tft.println("600");

  //draw steamp
 // tft.setCursor(200, 0); tft.print("TS:");
 // tft.setCursor(220, 0); tft.print(day(MyProfileTimeStamp)); tft.print("/"); tft.print(month(MyProfileTimeStamp)); tft.print(" ");
 // tft.print(hour(MyProfileTimeStamp)); tft.print(":"); tft.println(minute(MyProfileTimeStamp));



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


  TempRoastDone = MyBaseSetpoints[SetPointCount - 2];
  Serial.print ("TempRoastDone:");Serial.println (TempRoastDone);
  tft.setCursor((tft.width() / 2),Y(TempRoastDone) - 10);tft.println("SP:"); tft.setCursor((tft.width() / 2) + 24,Y(TempRoastDone) - 10); tft.println(TempRoastDone);
  tft.drawFastHLine(((tft.width() / 2) + 20)    ,  Y(TempRoastDone), tft.width(), BLUE);
  tft.drawFastVLine(PixelsPerMin * MySpanAccumulatedMinutes[SetPointCount - 2], 0, tft.height() / 2, BLUE);


  //tft.fillRect(0,0,5,5, BLUE);
  //tft.fillRect(0,235,5,5, RED);
  //tft.fillRect(315,0,5,5,YELLOW);
  //tft.fillRect(315,235,5,5,WHITE);

  tft.drawFastVLine(PixelsPerMin * MySpanAccumulatedMinutes[SetPointCount - 2], 0, tft.height() / 2, BLUE);
  //delay(1000);

  ReDrawROLLAVGLINEFromArray(ORANGE);
  
  if (AdjustmentSetpoint > 0){
    DrawAdjustMentBoxes(AdjustmentSetpoint);}

  DrawMenu1();
}


int Y(int temp)
{
  if (temp <= TempYSplit) {  
     return (240 - ((double)temp/TempPerPixL));
  }
  else if (temp <= TempYSplit2){
      return ((240 - PixelYSplit) - ((double)(temp-TempYSplit)/TempPerPixM));
      //460 x 460 x 460 /240  = 379.687
      //return (240 -  ((double)temp * (double)temp * double(temp)) / IYscale);    
  } else {
    
      return ((240 -  PixelYSplit2) - ((double)(temp-TempYSplit2)/TempPerPixH));
  }
  
}

void tftPrintDouble5b(double num )
{
  
    //+11.1
    //12345
    if (num < 0)
    {
      sprintf(Buf5, "%d.%1d", (int)num,abs((int)(num*100)%100));
      
    }else
    {
      sprintf(Buf5, " %d.%1d", (int)num,(int)(num*100)%100);
    }
    tft.println(Buf5); 
}
void tftPrintDouble5(double num )
{
  
    //+1.11
    //12345
    if (num < 0)
    {
      sprintf(Buf5, "%d.%02d", (int)num,abs((int)(num*100)%100));
      
    }else
    {
      sprintf(Buf5, " %d.%02d", (int)num,(int)(num*100)%100);
    }
    tft.println(Buf5); 
}
void tftPrintDouble6(double num )
{
  
    //+11.11
    //123456
    if (num < 0)
    {
      sprintf(Buf6, "%d.%02d", (int)num,abs((int)(num*100)%100));
      
    }else
    {
      sprintf(Buf6, " %d.%02d", (int)num,(int)(num*100)%100);
    }
    tft.println(Buf6); 
}
void tftPrintDouble7(double num )
{
    //+111.11
    //1234567
    
    if (num < 0)
    {
      sprintf(Buf7, "%d.%02d", (int)num,abs((int)(num*100)%100));
      
    }else
    {
      sprintf(Buf7, " %d.%02d", (int)num,(int)(num*100)%100);
    }
    tft.println(Buf7);
}
void tftPrintDouble(double num )
{
    
    tft.println(num,1);
    
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
  int rowheight = 11;
  int row = 10;
  // tft.setCursor(40 ,row); tft.println("F:"); tft.setCursor(50 , row); tft.println(TempEnd);
  // tft.setCursor(100 , row); tft.println("Last:"); tft.setCursor(130 , row); tft.println(TempLastEndOfRoast);
  row = row + rowheight;
  tft.setCursor(40 , row); tft.print("sp:"); tft.setCursor(75 , row); tft.println(setpoint); tft.setCursor(100 , row); tft.print("SSD:");
  tft.setCursor(130 , row); tft.println(ManagingSSR);

  row = row + rowheight;
  tft.setCursor(40 , row); tft.print("Time:");   tft.setCursor(75 , row); tft.println(roastMinutes);
  row = row + rowheight;
  if (duty > 1) {
    tft.setCursor(40 , row); tft.println("Duty:"); tft.setCursor(70 , row); tftPrintDouble7(1.00);
  }
  else {
    tft.setCursor(40 , row); tft.println("Duty:"); tft.setCursor(70 , row); tftPrintDouble7(duty);
  }
  row = row + rowheight;
  tft.setCursor(40 , row); tft.print("Err:"); tft.setCursor(70 , row);   tftPrintDouble7(-err);
  row = row + rowheight;
  tft.setCursor(40 , row); tft.print("IEr:"); tft.setCursor(70 , row);   tftPrintDouble7(-errI);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void UpdateGraphB(int temp2, int temp1, int tempCoil, double ampHeater1, double ampHeater2, int tempFan     , double ampFan, double volts)
{

  if (MenuShowing == 2) return;
  //Serial.print("TBean2:");Serial.print(temp2);Serial.print("TBean1:");Serial.print (temp1);Serial.print("TCoil:"); Serial.print(tempCoil); Serial.print("TFan:");Serial.println(tempFan);
  if (AdjustmentSetpoint > 0 ) return;
  
  int rowheight = 11;

  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(1);
  int row = 150 ;
  tft.setCursor(150 , row); tft.println("Tvg:"); tft.setCursor(190 , row); tftPrintIntTo5Char(TBeanAvgRoll.mean());
  tft.setCursor(240 , row); tft.println("Skip"); tft.setCursor(270 , row); tftPrintIntTo5Char(Readingskipped);

  row = row + rowheight; ;
  tft.setCursor(150 , row); tft.println("T1:"); tft.setCursor(190 , row);  tft.println("   "); tft.setCursor(190 , row); tftPrintIntTo5Char(temp1);
  tft.setCursor(240 , row); tft.println("T2"); tft.setCursor(270 , row);   tft.println("   "); tft.setCursor(270 , row); tftPrintIntTo5Char(temp2);
  row = row + rowheight;
  tft.setCursor(150 , row); tft.println("Fan  T:"); tft.setCursor(190 , row); tftPrintIntTo5Char(tempFan);
  tft.setCursor(240 , row); tft.println("Amp:"); tft.setCursor(270 , row); tftPrintDouble5b(ampFan);
  row = row + rowheight;
  tft.setCursor(050 , row); tft.println("G:"); tft.setCursor(60 , row); tft.println(Gain); tft.setCursor(100 , row); tft.println("U D");
  GainX =  60; GainY = 180;
  GainUX = 102; GainUY = 65;
  GainDX = 114; GainDY = 65;
  tft.setCursor(150 , row); tft.println("Heat T:"); tft.setCursor(190 , row); tftPrintIntTo5Char(tempCoil);
  tft.setCursor(240 , row); tft.println("Amp1:"); tft.setCursor(270 , row); tftPrintDouble5b(ampHeater1);
  row = row + rowheight;;
  tft.setCursor(050 , row); tft.println("I:"); tft.setCursor(60 , row);
  tft.println(Integral) ; tft.setCursor(100 , row); tft.println("U D");
  IntegralX  = 60; IntegralY  = 195;
  IntegralUX = 102; IntegralUY = 55;
  IntegralDX = 114; IntegralDY = 55;
  tft.setCursor(150 , row); tft.println("Volts:"); tft.setCursor(190 , row); tft.println(volts);
  tft.setCursor(240 , row); tft.println("Amp2:"); tft.setCursor(270 , row); tftPrintDouble5b(ampHeater2);
  row = row + rowheight;;
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

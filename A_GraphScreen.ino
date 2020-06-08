
//----------------------------------------------------------------------------------------------------------------------------------------------------
// GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN
//----------------------------------------------------------------------------------------------------------------------------------------------------


void graphProfile() {
  
    int accumulated = 0;
    for (int X = 0; X < SetPointCount; X++) {
         if (X > 0) {
            EEPROM.get(MySetpointsEprom[X],MySetPoints[X].Temperature);
            //MySetPoints[X].Temperature = MySetPoints[X].TemperatureDefault;
            MySetPoints[X].SpanMinutes = MySetPoints[X].Minutes - MySetPoints[X - 1].Minutes;
            accumulated = accumulated + MySetPoints[X].SpanMinutes;
            MySetPoints[X].Minutes = accumulated;
        }
        else
        {
            MySetPoints[X].SpanMinutes = 0;
            MySetPoints[X].Minutes = 0;
            MySetPoints[X].Temperature = 0;
        }
    }

  TempYMax = 800; 
  TempYSplit2 = 440;
  TempYSplit2 = MySetPoints[5].Temperature ;
  PixelYSplit2 = 360;//180;
  TempYSplit = (MySetPoints[1].Temperature) ;
  PixelYSplit = 150;//90;

  TempPerPixL = TempYSplit / PixelYSplit;
  TempPerPixM = (TempYSplit2 - TempYSplit) / (PixelYSplit2 - PixelYSplit);
  TempPerPixH = (TempYMax - TempYSplit2) / (480 - PixelYSplit2);
  PixelsPerMin =  (int)(800 / TimeScreenLeft);
  myGLCD.setBackColor(BLACK);
  myGLCD.fillRect(0, 0, 800, 480);
  myGLCD.clrScr();
  myGLCD.setColor(100,100,100);  
  myGLCD.setFont(BigFont);
  myGLCD.drawLine(3 * PixelsPerMin ,0,  3 * PixelsPerMin,myGLCD.getDisplayYSize()-30 );
  myGLCD.print("3min",(3 * PixelsPerMin) - 30, 460);
  myGLCD.drawLine(6 * PixelsPerMin , 0, 6 * PixelsPerMin , myGLCD.getDisplayYSize()-30);
  myGLCD.print("6min",(6 * PixelsPerMin) - 30, 460);
  myGLCD.drawLine(9 * PixelsPerMin , 0, 9 * PixelsPerMin, myGLCD.getDisplayYSize()-30);
  myGLCD.print("9min",(9 * PixelsPerMin) - 30, 460);
  myGLCD.drawLine(12 * PixelsPerMin ,0,12 * PixelsPerMin,  myGLCD.getDisplayYSize()-30 );
  myGLCD.print("12min",(12 * PixelsPerMin) - 30, 460);
 
  //draw y scale for 3 ranges
  
  myGLCD.setFont(SmallFont);
  int yaxislable = 30;
  for (int t = 100; t < (TempYSplit - 20); t = t + 100) {
    //Serial.println(t);
    myGLCD.drawLine(yaxislable,  YforATemp(t),myGLCD.getDisplayXSize(),YforATemp(t) );
    myGLCD.printNumI(t,2 , YforATemp(t) - 5);
  }
  for (int t = TempYSplit; t < (TempYSplit2); t = t + 10) {
    myGLCD.drawLine(yaxislable,  YforATemp(t),myGLCD.getDisplayXSize(),  YforATemp(t));
    myGLCD.printNumI(t,2, YforATemp(t) - 5);
  }  
  for (int t = TempYSplit2 +10; t < TempYMax ; t = t + 100) {
    myGLCD.drawLine(yaxislable,  YforATemp(t),myGLCD.getDisplayXSize(),  YforATemp(t));
    myGLCD.printNumI(t,2 , YforATemp(t) - 5);
  }

  StartLinebyTimeAndTemp (0, MySetPoints[0].Temperature, SETPOINTLINEID , BLUE);
  //set the by minute temp profile for 5 spans
  Serial.println(MySetPoints[0].Temperature);
  
  MyMinuteTemperature[0] = 0;
  int accumulatedMinutes = 0;
  for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++) {
    double TempPerMinuteinSpan = ((double)(MySetPoints[xSetPoint].Temperature - MySetPoints[xSetPoint - 1].Temperature)) / MySetPoints[xSetPoint].SpanMinutes ;
    for (int xSpanMinute = 1 ; xSpanMinute <= MySetPoints[xSetPoint].SpanMinutes  ;  xSpanMinute++)
    {
      accumulatedMinutes = accumulatedMinutes + 1;
      MyMinuteTemperature[accumulatedMinutes] = MySetPoints[xSetPoint - 1].Temperature + (TempPerMinuteinSpan * xSpanMinute);
      AddLinebyTimeAndTemp(accumulatedMinutes, MyMinuteTemperature[accumulatedMinutes], SETPOINTLINEID);
    }
    DrawSetPoint(xSetPoint, LineColorforLineID[SETPOINTLINEID]); 
  }


  //draw endpoint highlights
  myGLCD.setFont(BigFont);
  myGLCD.setColor(BLUE);
  int PixYEndPoint = YforATemp(MySetPoints[EndingSetPoint].Temperature);
  myGLCD.drawLine(400, PixYEndPoint, myGLCD.getDisplayXSize()-50, PixYEndPoint);
  myGLCD.print("SP:",500, PixYEndPoint-20);myGLCD.printNumI(MySetPoints[EndingSetPoint].Temperature, 550, PixYEndPoint-20);

  ReDrawROLLAVGLINEFromArray(ORANGE);

  DrawHorMenu1();
  
  if (spSelected > 0) {
      DrawVertMenu(1);
  }
  else
  {
      DrawVertMenu(0);
  }
}
void UpdateGraphA() {
  //this is top row
  myGLCD.setBackColor(BLACK);
 
  myGLCD.setColor(VGA_WHITE);  myGLCD.setFont(BigFont);
  int rowheight = 30;
  int row = 70;
  int col = 40;
  int col2 = 120;
  int col3 = 180;
  int col4 = 240;

   myGLCD.print("Time:",col , row);   myGLCD.print("***", col2 , row);
   row = row + rowheight;
   if (Duty > 1) {
      myGLCD.print("Duty:",col , row); myGLCD.printNumF(1.0,2,col2,row);
   }
  else {
 //   myGLCD.setCursor(col , row); myGLCD.print("Duty:"); myGLCD.setCursor(col + 40 , row); tftPrintDouble7(Duty);
       myGLCD.print("Duty:", col, row); myGLCD.printNumF(Duty,2, col2, row, '.');

   }
  row = row + rowheight;
   myGLCD.print("Err:",col , row);   myGLCD.printNumF(-Err,2,col2,row);

   // row = row + rowheight;
  // myGLCD.print("IEr:",col + 10 , row);// myGLCD.setCursor(col + 40 , row);   tftPrintDouble7(-ErrI);
  //row = row + rowheight;

}
void UpdateGraphB() {

  int rowheight = 30;
  myGLCD.setBackColor(BLACK);
  myGLCD.setColor(VGA_WHITE);  
  myGLCD.setFont(BigFont);
  int row = 320 ;
  int col =  300 ;
  int col2 = 370;
  int col3 = 500;
  int col4 = 600;

  row = 360;
   myGLCD.print("Tvg:",col , row);          myGLCD.printNumI(TBeanAvgRoll.mean(),col2,row,4,'0');
   row = row + rowheight; 
   myGLCD.print("T1:",col , row);           myGLCD.printNumI(TBean1, col2, row, 4, ' ');
   row = row + rowheight;
   myGLCD.print("T2:", col, row);           myGLCD.printNumI(TBean2, col2, row, 4, '0');
   row = row + rowheight;
   row = 330;
   myGLCD.print("Fan:", col3 , row);    myGLCD.print("***",col4 , row);
   row = row + rowheight;
   myGLCD.print("Coil1:",col3  , row);  myGLCD.printNumI(CurrentHeat1,col4 ,row, 4, '0');
   row = row + rowheight;
   myGLCD.print("Coil2:",col3 , row);   myGLCD.printNumI(CurrentHeat2,col4 , row, 4, '0');
   row = row + rowheight;
   myGLCD.print("Volts:", col3, row);   myGLCD.printNumI(MaxVoltage, col4, row, 4, '0');

}
void UpdateGraphC() {
  int rowheight = 20;
  int row = 180 ;
  int col = 40 ;
  int col2 = 80;


   myGLCD.setColor(VGA_WHITE);  myGLCD.setFont(SmallFont);
   myGLCD.print("Gain:",col , row); myGLCD.printNumI(Gain,col2 , row);
   row = row + rowheight;
   myGLCD.print("Int:", col, row);  myGLCD.printNumF(Integral,2,col2,row);
   row = row + rowheight;
   myGLCD.print("l/s:",col , row);  myGLCD.printNumI(LoopsPerSecond,col2 , row) ;
   row = row + rowheight;
   myGLCD.print("Skp", col, row);  myGLCD.printNumI(Readingskipped,col2 , row);

}
void  UpdateState(int state) {
  myGLCD.setFont(BigFont);
  myGLCD.setColor(VGA_WHITE);
  switch (state) {
    case STATEROASTING:
      myGLCD.setColor(RED);
      strncpy(StateName, "Roasting", 8);
      myGLCD.print("Roasting",50, 0);
      break;
    case STATESTOPPED:
      strncpy(StateName, "Stopped ", 8);
      myGLCD.print("Stopped ",50, 0);
      break;
    case STATECOOLING:
      myGLCD.setColor(VGA_WHITE);
      strncpy(StateName, "Cooling ", 8);
      myGLCD.print( "Cooling ",50, 0);
      break;
    case STATEOVERHEATED:
      strncpy(StateName, "HotCoil  ", 9);
      myGLCD.print ("HotCoil ",50, 0);
      break;
    case STATENOFANCURRENT:
      strncpy(StateName, "NoFanCur   ", 8);
      myGLCD.print ("HotFan ",50, 0);
      break;
    case STATEFANONLY:
      strncpy(StateName, "FanOnly ", 8);
      myGLCD.print ("FanOnly ",50, 0);
      break;
    default:
      strncpy(StateName, "unk   ", 8);
      myGLCD.print ("unk     ",50, 0);
      break;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void StartLinebyTimeAndTemp(double timemins, int temp, int lineID, uint16_t color) {
  //Serial.println ("StartLineTandT id:");Serial.println (lineID);Serial.println(" color:");Serial.println(color);
  LastXforLineID[lineID] = (PixelsPerMin * timemins);
  if (temp > 0) {
    LastYforLineID[lineID] = YforATemp(temp);
  }
  else {
    LastYforLineID[lineID] = 480;
  }
  LineColorforLineID[lineID] = color;
  if (lineID == ROLLAVGLINEID) {
    for (int X = 0; X < 800; X++) {
      myLastGraphYPixels[X] = -1;
      myLastGraphTemps[X] = -1;
    }
  }
}


void AddLinebyTimeAndTemp(double timemins, int temp, int lineID) {
  uint16_t newX = (uint16_t)(PixelsPerMin * timemins);
  int newY = YforATemp(temp);
  //Serial.println ("AddLineTandT line iD:");Serial.println (lineID);Serial.println(" time:");Serial.println(timemins);Serial.println("temp:");Serial.println(temp);Serial.println(" color:");Serial.println(LineColorforLineID[lineID]);
  //Serial.println ("newX:");Serial.println (newX);Serial.println ("pixelspermin:");Serial.println (PixelsPerMin);Serial.println(" newY:");Serial.println(newY);
  myGLCD.setColor(LineColorforLineID[lineID]);//LineColorforLineID[lineID]);
  myGLCD.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY );
  LastXforLineID[lineID] = newX;
  LastYforLineID[lineID] = newY;
  if (lineID == ROLLAVGLINEID) {
    BoldLine(LastXforLineID[lineID], LastYforLineID[lineID] + 1, newX, newY + 1, LineColorforLineID[lineID]);
    myLastGraphYPixels[newX] = newY;
    myLastGraphTemps[newX] = temp;
  }
}
void DrawSetPoint(int setpoint, uint16_t color)
{
    int Y = YforATemp(MySetPoints[setpoint].Temperature);
    int X = MySetPoints[setpoint].Minutes * PixelsPerMin;
    myGLCD.setColor(color);
    myGLCD.fillCircle(X, Y, 7);
    myGLCD.printNumI(MySetPoints[setpoint].Temperature, X - 15, Y - 20);


}

void DrawMovedSetPoint(int setpoint)
{
    int Y = YforATemp(MySetPoints[setpoint].TemperatureNew);
    int X = MySetPoints[setpoint].Minutes * PixelsPerMin;
    myGLCD.setColor(YELLOW);
    myGLCD.fillCircle(X, Y, 7);
    myGLCD.setColor(WHITE);
    myGLCD.drawCircle(X, Y, 7);

    int Ytext = YforATemp(MySetPoints[setpoint].Temperature);
    myGLCD.setBackColor(BLACK);
    myGLCD.setColor(YELLOW);
    myGLCD.setFont(BigFont);
 
    myGLCD.print("N:", X + 8, Ytext - 15);
    myGLCD.printNumI(MySetPoints[setpoint].TemperatureNew, X + 30, Ytext - 15);
    myGLCD.print("D:", X + 8, Ytext + 25);
    myGLCD.printNumI(MySetPoints[setpoint].TemperatureNew - MySetPoints[setpoint].Temperature, X + 30, Ytext + 25);

    myGLCD.setColor(BLUE);

    myGLCD.printNumI(MySetPoints[setpoint].Temperature, X + 30, Ytext + 5);

}
void AddPointbyTimeAndTempAndLineID(double timemins, int temp, int lineID, int radius) {
  uint16_t newX = (uint16_t)(PixelsPerMin * timemins);
  int newY = YforATemp(temp);
  myGLCD.setColor(LineColorforLineID[lineID]);
  //myGLCD.setColor(0,150,0);
  //Serial.print('a');Serial.print(newX);Serial.print( "   ");;Serial.print(timemins);Serial.print( "   ");Serial.println(temp);
  myGLCD.fillCircle(newX, newY, radius );

  //Serial.println ("AddLineTandT line iD:");Serial.println (lineID);Serial.println(" time:");Serial.println(timemins);Serial.println("temp:");Serial.println(temp);Serial.println(" color:");Serial.println(LineColorforLineID[lineID]);
  //Serial.println ("newX:");Serial.println (newX);Serial.println ("pixelspermin:");Serial.println (PixelsPerMin);Serial.println(" newY:");Serial.println(newY);
  //myGLCD.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY, LineColorforLineID[lineID]);
  LastXforLineID[lineID] = newX;
  LastYforLineID[lineID] = newY;

}

void AddPointbyTimeandTemp(double timemins, int temp, int color, int radius) {
  uint16_t newX = (uint16_t)(PixelsPerMin * timemins);
  int newY = YforATemp(temp);
  myGLCD.setColor(color);
  myGLCD.fillCircle(newX, newY, radius);

  //Serial.println ("AddLineTandT line iD:");Serial.println (lineID);Serial.println(" time:");Serial.println(timemins);Serial.println("temp:");Serial.println(temp);Serial.println(" color:");Serial.println(LineColorforLineID[lineID]);
  //Serial.println ("newX:");Serial.println (newX);Serial.println ("pixelspermin:");Serial.println (PixelsPerMin);Serial.println(" newY:");Serial.println(newY);
  //myGLCD.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY, LineColorforLineID[lineID]);

}

void ReDrawROLLAVGLINEFromArray(int color) {
  LastXforLineID[1] = 0;
  LastYforLineID[1] = 240;
  for (int X = 0; X < 320; X++) {
    if (myLastGraphYPixels[X] > 0 ) {
      //  Serial.println ("DrawRealTime ");Serial.println(" color:");Serial.println(color);
      myGLCD.setColor(color);
      myGLCD.drawLine(LastXforLineID[1], LastYforLineID[1] , X, myLastGraphYPixels[X]);
      BoldLine(LastXforLineID[1], LastYforLineID[1] , X, myLastGraphYPixels[X], color);
      LastXforLineID[1] = X;
      LastYforLineID[1] = myLastGraphYPixels[X];
    }
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void BoldLine(int x, int y, int newX, int newY, int color) {
  myGLCD.setColor(color);
  myGLCD.drawLine(x, y + 1, newX, newY + 1);
  
  myGLCD.drawLine(x + 1, y + 1, newX + 1, newY + 1);
  myGLCD.drawLine(x + 1, y, newX + 1, newY);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void DrawLinebyTimeAndTemp(boolean log, double timemins1, int temp1, double timemins2, int temp2, int color) {
  if (log == true)
  {
    //Serial.println ("timemins1:");
    //Serial.println (timemins1);
    //Serial.println(" temp1:");
    //Serial.println(temp1);
    //Serial.println(" timemins2:");
    //Serial.println (timemins2);
    //Serial.println (" color:");
    //Serial.println(color);
  }
  myGLCD.setColor(color);
  myGLCD.drawLine(PixelsPerMin * timemins1, YforATemp(temp1), PixelsPerMin * timemins2, YforATemp(temp2));
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------


int UpdateMySetPointSpan(struct setpoint *setpoint)
{
}
int YforATemp(int temp) {
  int result = 0;
  if ( temp < 0 ) {
    temp = 0;
  }  
  if (temp <= TempYSplit) {
    result = (myGLCD.getDisplayYSize() - ((double)temp / TempPerPixL));
  }
  else if (temp <= TempYSplit2) {
    result = ((myGLCD.getDisplayYSize() - PixelYSplit) - ((double)(temp - TempYSplit) / TempPerPixM));
    //460 x 460 x 460 /240  = 379.687
    //return (myGLCD.getDisplayYSize() -  ((double)temp * (double)temp * double(temp)) / IYscale);
  } else {
    result = ((myGLCD.getDisplayYSize() -  PixelYSplit2) + ((double)(TempYSplit2 - temp  ) / TempPerPixH));
  }
  if (result < 0) {
    return 1;
  }
  else
  { return result;
  }


}

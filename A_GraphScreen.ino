
//----------------------------------------------------------------------------------------------------------------------------------------------------
// GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN
//----------------------------------------------------------------------------------------------------------------------------------------------------


void graphProfile() {
    
    //draw scales
    myGLCD.setFont(SmallFont);
    //Calculate  setpoint accumlated minutes basedon current minute settings    
    int accumulated = 0;
    for (int X = 0; X < SetPointCount; X++) {
         if (X > 0) {
            EEPROM.get(SETPOINTTEMP_EP[X],MySetPoints[X].Temperature);
            MySetPoints[X].SpanMinutes = MySetPoints[X].Minutes - MySetPoints[X - 1].Minutes;
            accumulated = accumulated + MySetPoints[X].SpanMinutes;
            MySetPoints[X].Minutes = accumulated;
        }
        else
        {
            //EEPROM.get(SETPOINTTEMP_EP[X],MySetPoints[X].Temperature);
            MySetPoints[X].Temperature = 390;
            MySetPoints[X].SpanMinutes = 0;
            MySetPoints[X].Minutes = 0;
                    
        }
    }

 //perform some necesary calculations to size our scales to the display 
 //yscale is done in 3 ranges
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

  //draw our x time axis
  myGLCD.drawLine(3 * PixelsPerMin ,0,  3 * PixelsPerMin,myGLCD.getDisplayYSize()-30 );
  myGLCD.print("3min",(3 * PixelsPerMin) - 30, 380);
  myGLCD.drawLine(6 * PixelsPerMin , 0, 6 * PixelsPerMin , myGLCD.getDisplayYSize()-30);
  myGLCD.print("6min",(6 * PixelsPerMin) - 30, 460);
  myGLCD.drawLine(9 * PixelsPerMin , 0, 9 * PixelsPerMin, myGLCD.getDisplayYSize()-30);
  myGLCD.print("9min",(9 * PixelsPerMin) - 30, 460);
  myGLCD.drawLine(12 * PixelsPerMin ,0,12 * PixelsPerMin,  myGLCD.getDisplayYSize()-30 );
  myGLCD.print("12min",(12 * PixelsPerMin) - 30, 460);
 
  //draw y scale for 3 ranges
  int yaxislable = 30;
  int lastt =0;
  for (int t = 100; t < (TempYSplit - 20); t = t + 100) {
    //Serial.println(t);
    myGLCD.drawLine(yaxislable,  YforATemp(t),myGLCD.getDisplayXSize(),YforATemp(t) );
    myGLCD.printNumI(t,2 , YforATemp(t) - 5);
    lastt = t;
  }
  myGLCD.setColor(WHITE);  

  myGLCD.drawLine(yaxislable,  YforATemp(lastt),myGLCD.getDisplayXSize(),YforATemp(lastt) );
  myGLCD.setColor(100,100,100);  

  for (int t = TempYSplit; t < (TempYSplit2); t = t + 10) {
    myGLCD.drawLine(yaxislable,  YforATemp(t),myGLCD.getDisplayXSize(),  YforATemp(t));
    myGLCD.printNumI(t,2, YforATemp(t) - 5);
      lastt = t;
  }  
  myGLCD.setColor(WHITE);  

  myGLCD.drawLine(yaxislable,  YforATemp(lastt),myGLCD.getDisplayXSize(),YforATemp(lastt) );
  myGLCD.setColor(100,100,100);  

  for (int t = TempYSplit2 +10; t < TempYMax ; t = t + 100) {
    myGLCD.drawLine(yaxislable,  YforATemp(t),myGLCD.getDisplayXSize(),  YforATemp(t));
    myGLCD.printNumI(t,2 , YforATemp(t) - 5);
  }
  //Serial.println(FanPWMForATime(0));




  //draw the by minute temp profile for 5 spans 
  StartLinebyTimeAndTemp (0, MySetPoints[0].Temperature, SETPOINTLINEID , WHITE);
  MyMinuteTemperature[0] = MySetPoints[0].Temperature;
  int accumulatedMinutes = 0;
  for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++) {
    //Serial.println(xSetPoint);
    double TempPerMinuteinSpan = ((double)(MySetPoints[xSetPoint].Temperature - MySetPoints[xSetPoint - 1].Temperature)) / MySetPoints[xSetPoint].SpanMinutes ;
    for (int xSpanMinute = 1 ; xSpanMinute <= MySetPoints[xSetPoint].SpanMinutes  ;  xSpanMinute++)
    {
      accumulatedMinutes = accumulatedMinutes + 1;
      MyMinuteTemperature[accumulatedMinutes] = MySetPoints[xSetPoint - 1].Temperature + (TempPerMinuteinSpan * xSpanMinute);
      AddLinebyTimeAndTemp(accumulatedMinutes, MyMinuteTemperature[accumulatedMinutes], SETPOINTLINEID);
      //Serial.println(FanPWMForATime(accumulatedMinutes));
  
      //AddLinebyTimeAndY(accumulatedMinutes, FanPWMForATime(accumulatedMinutes), FANSPEEDLINEID);
    }
    DrawSetPoint(xSetPoint, LineColorforLineID[SETPOINTLINEID]); 
  }

  



  //draw endpoint highlights
  myGLCD.setFont(BigFont);
  myGLCD.setColor(BLUE);
  int PixYEndPoint = YforATemp(MySetPoints[EndingSetPoint].Temperature);
  myGLCD.drawLine(400, PixYEndPoint, myGLCD.getDisplayXSize()-50, PixYEndPoint);
  myGLCD.print("SP:",500, PixYEndPoint-20);myGLCD.printNumI(MySetPoints[EndingSetPoint].Temperature, 550, PixYEndPoint-20);

  //drawing cooling line
  int PixYCoolingPoint = YforATemp(TEMPCOOLINGDONE);
  myGLCD.drawLine(500, PixYCoolingPoint, myGLCD.getDisplayXSize()-50, PixYCoolingPoint);
  myGLCD.print("Co:",500, PixYCoolingPoint+7);myGLCD.printNumI(TEMPCOOLINGDONE, 550, PixYCoolingPoint+7);



  ReDrawROLLAVGLINEFromArray(ORANGE);
  DrawControlMenu();
  DrawFanMenu();

 //if sp is  selected, assume user is adjusting setpoint redraw the menu
  if (spSelected >= 0) {
      DrawVMenu(1);
  }
  else
  {
      DrawVMenu(0);
  }
  DrawFanDecrease(AQUA);
}

void UpdateGraphA() {
  //this is top row
  myGLCD.setFont(BigFont);
  
  myGLCD.setBackColor(BLACK);
  myGLCD.setColor(WHITE);  
  int rowheight = 30;
  int row = 70;
  int col = 40;
  int col2 = 120;
  int col3 = 180;
  int col4 = 240;

   myGLCD.print("Time:",col , row);   myGLCD.printNumF(RoastMinutes,2,col2 , row);
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

  row = row + rowheight;
  myGLCD.print("IEr:",col  , row); myGLCD.printNumF(-ErrI, 2,col2,row);
  //row = row + rowheight;

}
void UpdateGraphB() {
  
  myGLCD.setFont(BigFont);
  int rowheight = 30;
  int row = 270 ;
  int col =  350 ;
  int col2 = 420;
  int col3 = 550;
  int col4 = 650;

   myGLCD.setBackColor(BLACK);
   myGLCD.setColor(VGA_WHITE);  

   myGLCD.print("Tvg:",col , row);          myGLCD.printNumI(TBeanAvgRoll.mean(),col2,row,4,' ');
   row = row + rowheight; 
   myGLCD.print("T1:",col , row);           myGLCD.printNumI(TBean1, col2, row, 4, ' ');
   row = row + rowheight;
   myGLCD.print("T2:", col, row);           myGLCD.printNumI(TBean2, col2, row, 4, ' ');
   row = row + rowheight;
   myGLCD.print("Coil:", col, row);         myGLCD.printNumI(TCoilRoll.mean(), col2, row, 4, ' ');
   
   row = 270;
   myGLCD.print("Fan A:", col3 , row);    myGLCD.printNumF(CurrentFan,0,col4 , row,'.',1,' ');
   row = row + rowheight;
   myGLCD.print("Coil1:",col3  , row);    myGLCD.printNumF(CurrentHeat1,0,col4 ,row, '.',1,' ');
   row = row + rowheight;
   myGLCD.print("Coil2:",col3 , row);     myGLCD.printNumF(CurrentHeat2,0,col4 , row, '.',1,' ');
   row = row + rowheight;
   myGLCD.print("Fan P:", col3, row);     myGLCD.printNumF((double)FanPressureRoll.mean(),2, col4, row,'.',2,' ');

}
void UpdateGraphC() {
  int rowheight = 20;
  int row = 180 ;
  int col = 40 ;
  int col2 = 80;
  myGLCD.setFont(SmallFont);

   myGLCD.setColor(VGA_WHITE);  
   myGLCD.print("Gain:",col , row); myGLCD.printNumI(Gain,col2 , row);
   row = row + rowheight;
   myGLCD.print("Int:", col, row);  myGLCD.printNumF(Integral,2,col2,row);
   row = row + rowheight;
   myGLCD.print("l/s:",col , row);  myGLCD.printNumI(LoopsPerSecond,col2 , row) ;
   row = row + rowheight;
   myGLCD.print("Skp", col, row);  myGLCD.printNumI(Readingskipped,col2 , row);
   row = row + rowheight;
   myGLCD.print("FanD", col, row);  myGLCD.printNumI(FanSpeedPWMAutoDecrease,col2 , row);

}
void UpdateFanPWMBut() {
  myGLCD.setFont(BigFont);
  int rowheight = 20;
  int row = myFanButtonControl.bounding.ymax + 8;
  int col = myFanButtonControl.bounding.xmin + 15;
  int col1 = col + (myFanButtonControl.bounding.xmax - myFanButtonControl.bounding.xmin)/3  ;
  int col2 = col1 + (myFanButtonControl.bounding.xmax - myFanButtonControl.bounding.xmin)/3  ;

   if (FanSpeedPWMAutoMode == true)
   {
      myGLCD.setColor(LGBLUE) ;
   }
   else
   {
      myGLCD.setColor(AQUA);
   
   }
   myGLCD.printNumI(FanSpeedPWMStart,col , row);
   myGLCD.printNumI(FanSpeedPWM,col1 , row);
   myGLCD.printNumI((FanSpeedPWMStart-FanSpeedPWMAutoDecrease),col2 , row);
   

    
}

void UpdateState(int state) {
  myGLCD.setFont(Grotesk24x48);
  int row = 50;
  int col = 0;
  switch (state) {
    case STATEROASTING:
      myGLCD.setColor(GREEN);
      strncpy(StateName, "Roasting", 8);
      myGLCD.print("Roasting",row,col);
      break;
    case STATESTOPPED:
      myGLCD.setColor(RED);
 
      strncpy(StateName, "Stopped ", 8);
      myGLCD.print("Stopped ",row,col);
      break;
    case STATECOOLING:
      myGLCD.setColor(BLUE);
      strncpy(StateName, "Cooling ", 8);
      myGLCD.print( "Cooling ",row,col);
      break;
    case STATEOVERHEATED:
      myGLCD.setColor(RED);
 
      strncpy(StateName, "HotCoil  ", 9);
      myGLCD.print ("HotCoil ",row,col);
      break;
    case STATENOFANCURRENT:
      myGLCD.setColor(RED);
 
      strncpy(StateName, "NoFanCur   ", 8);
      myGLCD.print ("HotFan ",row,col);
      break;
    case STATEFANONLY:
      myGLCD.setColor(BLUE);
  
      strncpy(StateName, "FanOnly ", 8);
      myGLCD.print ("FanOnly ",row,col);
      break;
    default:
      strncpy(StateName, "unk   ", 8);
      myGLCD.print ("unk     ",row,col);
      break;
  }
}


void DrawFanDecrease(uint16_t color){
  myGLCD.setColor(color);
  int Col1 = (PixelsPerMin * 6);
  int Y1 = YforATemp(CalcFanPWMForATime(0));
  int X1 = (PixelsPerMin * 6);
  int Y2 = YforATemp(CalcFanPWMForATime(FanSpeedPWNDelayDecreaseByMinutes));
  int X2 =  Col1 + ((PixelsPerMin * FanSpeedPWNDelayDecreaseByMinutes)/4 );
  //Serial.println(X2);
  myGLCD.drawLine(X1, Y1, X2, Y2 );
  X1 = X2; Y1 = Y2;
  Y2 = YforATemp(CalcFanPWMForATime(FanSpeedPWNDecreaseByMinutes));
  X2 =  Col1 + ((PixelsPerMin * FanSpeedPWNDecreaseByMinutes)/4 );
  //Serial.println(FanSpeedPWNDecreaseByMinutes);
  //Serial.println(X2);
  myGLCD.drawLine(X1, Y1, X2, Y2 );
  X1 = X2; Y1 = Y2;
  Y2 = YforATemp(CalcFanPWMForATime(MySetPoints[EndingSetPoint].Minutes));
  X2 =  Col1 + ((PixelsPerMin * MySetPoints[EndingSetPoint].Minutes)/4 );
  //Serial.println(MySetPoints[6].Minutes);
  //Serial.println(X2);
  myGLCD.drawLine(X1, Y1, X2, Y2 );
  
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void StartLinebyTimeAndY(double timemins, int Y, int lineID, uint16_t color) {
  //Serial.println ("StartLineTandT id:");
  
 // Serial.println (temp);
 // Serial.println (lineID);
 // Serial.println(" color:");
  //Serial.println(color);
 // Serial.println("X");
  
  LastXforLineID[lineID] = (PixelsPerMin * timemins);
  LastYforLineID[lineID] = Y;
  
  LineColorforLineID[lineID] = color;
  
  
}

void StartLinebyTimeAndTemp(double timemins, int temp, int lineID, uint16_t color) {

  int Y = 0;
  if (temp > 0) {
    Y = YforATemp(temp);
  }
  else {
    Y = 480;
  }
  StartLinebyTimeAndY(timemins, Y, lineID, color);
  if (lineID == ROLLAVGLINEID) {
    for (int X = 0; X < 800; X++) {
      myLastGraphYPixels[X] = -1;
      myLastGraphTemps[X] = -1;
    }
  }
}

void AddLinebyTimeAndTemp(double timemins, int temp, int lineID) {
  int newY = YforATemp(temp);
  uint16_t newX = (uint16_t)(PixelsPerMin * timemins);
  //Serial.print("AddLineByTimeAndTemp X:");
  //Serial.print(LastXforLineID[lineID]); Serial.print(" ");Serial.print(newX); Serial.print(" Y:");
  //Serial.print(LastYforLineID[lineID]); Serial.print(" ");Serial.print(newY); Serial.println(" ");
  myGLCD.setColor(LineColorforLineID[lineID]);//LineColorforLineID[lineID]);
  myGLCD.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY );
  LastXforLineID[lineID] = newX;
  LastYforLineID[lineID] = newY;

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
    int spYnew = YforATemp(MySetPoints[setpoint].TemperatureNew);
    int spX = MySetPoints[setpoint].Minutes * PixelsPerMin;
    myGLCD.setColor(YELLOW);
    myGLCD.fillCircle(spX, spYnew, 7);
    myGLCD.setColor(WHITE);
    myGLCD.drawCircle(spX, spYnew, 7);

    int spYold = YforATemp(MySetPoints[setpoint].Temperature);
    myGLCD.setBackColor(BLACK);
    myGLCD.setFont(BigFont);

    int Row1Y = spYold - 15;
    int Row2Y = spYold + 5;
    int Row3Y = spYold + 25;
    int col1  = spX + 8;
    int col2  = col1 + (myGLCD.getFontXsize() * 2);
    int col3  =  col2 + (myGLCD.getFontXsize() * 1);

    
    //myGLCD.setColor(WHITE);
    //myGLCD.printNumI(MySetPoints[setpoint].Temperature, col2, Row1Y);
    myGLCD.setColor(YELLOW);
    myGLCD.print("N:", col1, Row1Y); myGLCD.printNumI(MySetPoints[setpoint].TemperatureNew, col2,Row1Y );

 
    int delta = MySetPoints[setpoint].TemperatureNew - MySetPoints[setpoint].Temperature;
    
    myGLCD.print("D:",col1, Row2Y);
    if (delta > 0){
      myGLCD.print("+", col2, Row2Y);
    }
    if (delta < 0){
      myGLCD.print("-", col2, Row2Y);
    }
    myGLCD.printNumI(abs(delta), col3, Row2Y);

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
  LastYforLineID[1] = 480;
  for (int X = 0; X < 800; X++) {
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

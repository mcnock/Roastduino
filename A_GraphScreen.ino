
//----------------------------------------------------------------------------------------------------------------------------------------------------
// GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN
//----------------------------------------------------------------------------------------------------------------------------------------------------


void graphProfile() {

  
  TempYMax = 800; 
  TempYSplit2 = 440;
  TempYSplit2 = MySetPoints[5].Temperature ;
  PixelYSplit2 = 360;//180;
  TempYSplit = (MySetPoints[1].Temperature) ;
  PixelYSplit = 150;//90;

  TempPerPixL = TempYSplit / PixelYSplit;
  TempPerPixM = (TempYSplit2 - TempYSplit) / (PixelYSplit2 - PixelYSplit);
  TempPerPixH = (TempYMax - TempYSplit2) / (480 - PixelYSplit2);

  //TimeScreenLeft = MySetPoints[5].Minutes + 1;

  PixelsPerMin =  (int)(800 / (TimeScreenLeft + 1));
  
  myGLCD.setBackColor(BLACK);
 
  myGLCD.fillRect(0, 0, 800, 480);

  myGLCD.clrScr();
  
  myGLCD.setColor(50,50,50);  
  myGLCD.setFont(BigFont);
  myGLCD.drawLine(3 * PixelsPerMin ,0,  3 * PixelsPerMin,myGLCD.getDisplayYSize()-30 );
  myGLCD.print("3min",(3 * PixelsPerMin) - 30, 460);
  myGLCD.drawLine(6 * PixelsPerMin , 0, 6 * PixelsPerMin , myGLCD.getDisplayYSize()-30);
  myGLCD.print("6min",(6 * PixelsPerMin) - 30, 460);

  myGLCD.drawLine(9 * PixelsPerMin , 0, 9 * PixelsPerMin, myGLCD.getDisplayYSize()-30);
  myGLCD.print("9min",(9 * PixelsPerMin) - 30, 460);
  myGLCD.drawLine(12 * PixelsPerMin ,0,12 * PixelsPerMin,  myGLCD.getDisplayYSize()-30 );
  myGLCD.print("12min",(12 * PixelsPerMin) - 30, 460);
  myGLCD.drawLine(15 * PixelsPerMin ,0,15 * PixelsPerMin, myGLCD.getDisplayYSize()-30);
  myGLCD.print("15min",(15 * PixelsPerMin) - 30, 460);

  //draw y scale for 3 ranges
  

  
    //draw y scale
  myGLCD.setFont(SmallFont);
  int yaxislable = 30;
  for (int t = 100; t < (TempYSplit - 20); t = t + 100) {
    Serial.println(t);
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
  StartLinebyTimeAndTemp (0, MySetPoints[0].Temperature, SETPOINTLINEID , WHITE);
  //set the by minute temp profile for 5 spans
  
  MyMinuteTemperature[0] = 0;
  int accumulatedMinutes = 0;
  for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++) {
    double TempPerMinuteinSpan = ((double)(MySetPoints[xSetPoint].Temperature - MySetPoints[xSetPoint - 1].Temperature)) / MySetPoints[xSetPoint].SpanMinutes ;
    for (int xSpanMinute = 1 ; xSpanMinute <= MySetPoints[xSetPoint].SpanMinutes  ;  xSpanMinute++)
    {
      accumulatedMinutes = accumulatedMinutes + 1;
     // MyMinuteTemperature[accumulatedMinutes] =  MySetPoints[MySetPoints[xSetPoint].SpanMinutes - 1].Temperature + ( TempPerMinuteinSpan * xSpanMinute);
      
      MyMinuteTemperature[accumulatedMinutes] = MySetPoints[xSetPoint - 1].Temperature + (TempPerMinuteinSpan * xSpanMinute);

      AddLinebyTimeAndTemp(accumulatedMinutes, MyMinuteTemperature[accumulatedMinutes], SETPOINTLINEID);
    }
    AddPointbyTimeAndTempAndLineID(accumulatedMinutes, MySetPoints[xSetPoint].Temperature, SETPOINTLINEID, 5);
  }


  //create temp setpoint array between the last setpoint and the left of screen
  // for (int Y = MySetPoints.Temperature[SetPointCount - 1] ; Y <= TimeScreenLeft; Y++) {
  //   MyMinuteTemperature[Y] = MySetPoints[SetPointCount - 1].Temperature ;
  //Serial.println("minutesp:");Serial.println(Y);Serial.println("  ");Serial.println(MyMinuteTemperature[Y]);
  //  }

  //draw endpoint highlights
  myGLCD.setColor(BLUE);
  
  //myGLCD.setCursor((myGLCD.width() / 2), YforATemp(MySetPoints[EndingSetPoint].Temperature) - 10);
  //myGLCD.print("END:"); myGLCD.setCursor((myGLCD.width() / 2) + 24, YforATemp(MySetPoints[EndingSetPoint].Temperature) - 10);
  //myGLCD.print(MySetPoints[EndingSetPoint].Temperature);
  //myGLCD.drawLine(((myGLCD.width() / 2) + 20)    ,  YforATemp(MySetPoints[EndingSetPoint].Temperature), myGLCD.width());
  //myGLCD.drawLine(0,PixelsPerMin * MySetPoints[EndingSetPoint].Temperature, 0, myGLCD.print / 2);
  //myGLCD.drawLine(0,PixelsPerMin * MySetPoints[EndingSetPoint].Minutes, 0, myGLCD.print / 2);
  //delay(1000);

  ReDrawROLLAVGLINEFromArray(ORANGE);

  DrawHorMenu1();
  
  DrawVertMenu1();

}
void drawprofileline() {
  StartLinebyTimeAndTemp (0, MySetPoints[0].Temperature, SETPOINTLINEID , WHITE);
  //set the by minute temp profile for 5 spans
  MyMinuteTemperature[0] = 0;
  int accumulatedMinutes = 0;
  for (int x = 1; x < SetPointCount; x++) {
    double TempPerMinuteinSpan = ((double)(MySetPoints[x].Temperature - MySetPoints[x - 1].Temperature)) / MySetPoints[x].SpanMinutes ;
    for (int xSpanMinute = 1 ; xSpanMinute <= MySetPoints[x].SpanMinutes  ;  xSpanMinute++)
    {
      accumulatedMinutes = accumulatedMinutes + 1;
      MyMinuteTemperature[accumulatedMinutes] =  MySetPoints[x - 1].Temperature + ( TempPerMinuteinSpan * xSpanMinute);
      AddLinebyTimeAndTemp(accumulatedMinutes, MyMinuteTemperature[accumulatedMinutes], SETPOINTLINEID);
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
//  myGLCD.print(Buf5);
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
  //myGLCD.print(Buf5);
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
//  myGLCD.print(Buf6);
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
//  myGLCD.print(Buf7);
}
void tftPrintDouble(double num ) {

//  myGLCD.print(num, 1);

}
void tftPrintIntTo5Char(int num) {
  //  Serial.println("print:");Serial.println(num);
  sprintf(Buf5, "% 5d", num);
  //myGLCD.print(Buf5);
  // Serial.println("printbuf:");Serial.println(num);
}

void UpdateGraphA() {
  //this is top row
  myGLCD.setColor(VGA_WHITE);  myGLCD.setFont(BigFont);
  int rowheight = 12;
  int row = 25;
  int col = 40;
 myGLCD.print("Time:",col , row);   myGLCD.print("***", col + 45 , row);
//  myGLCD.setCursor(col + 90 , row); myGLCD.print(" sp:"); myGLCD.setCursor(col + 130 , row);   tftPrintDouble7(CurrentSetPointTemp);
  row = row + rowheight;
  if (Duty > 1) {
//     myGLCD.print("Duty:",col , row);// myGLCD.setCursor(col + 40 , row); tftPrintDouble7(1.00);
  }
  else {
 //   myGLCD.setCursor(col , row); myGLCD.print("Duty:"); myGLCD.setCursor(col + 40 , row); tftPrintDouble7(Duty);
  }
  row = row + rowheight;
   myGLCD.print("Err:",col + 10 , row);// myGLCD.setCursor(col + 40 , row);   tftPrintDouble7(-Err);

  row = row + rowheight;
   myGLCD.print("IEr:",col + 10 , row);// myGLCD.setCursor(col + 40 , row);   tftPrintDouble7(-ErrI);
  row = row + rowheight;

}
void UpdateGraphB() {

  int rowheight = 11;

  myGLCD.setColor(VGA_WHITE);  myGLCD.setFont(BigFont);
  int row = 160 ;
  int col = 135 ;

   myGLCD.print("Tvg:",col , row); //myGLCD.setCursor(col + 40 , row,col + 40 , row); //tftPrintIntTo5Char(TBeanAvgRoll.mean());

  row = row + rowheight; ;
   myGLCD.print("T1:",col , row);   myGLCD.print("   ",col + 40 , row); // tftPrintIntTo5Char(TBean1,col + 40 , row);
   myGLCD.print("T2",col + 90 , row);    myGLCD.print("   ",col + 120 , row); // tftPrintIntTo5Char(TBean2,col + 120 , row);

  row = row + rowheight;
  myGLCD.print("Fan  T:",col , row); myGLCD.print(TFan,col + 40 , row);
  myGLCD.print("Amp:", col + 90 , row);  myGLCD.print("***",col + 120 , row);

  row = row + rowheight;

 myGLCD.print("Heat T:",col , row); myGLCD.print("Amp1:",col + 90 , row);  myGLCD.printNumI(CurrentHeat1,col + 120 , row);

  row = row + rowheight;;

 myGLCD.print("Volts:",col , row);  myGLCD.printNumI(MaxVoltage,col + 40 , row);
  myGLCD.print("Amp2:",col + 90 , row); myGLCD.printNumI(CurrentHeat2,col + 120 , row);


}
void UpdateGraphC() {
  int rowheight = 11;
  myGLCD.setColor(VGA_WHITE);  myGLCD.setFont(SmallFont);
  int row = 180 ;
  int colr = 40 ;


  TGainValue.x =  colr + 10; TGainValue.y = row;// TGainIncrease.x = colr + 35;
  //TGainIncrease.y = row; TGainDecrease.x = colr + 55; TGainDecrease.y = row;

   myGLCD.print("G:",colr , row); myGLCD.print(Gain,TGainValue.x , TGainValue.y);
  //myGLCD.setCursor(TGainIncrease.x, TGainIncrease.y); myGLCD.print("U"); myGLCD.setCursor(TGainDecrease.x   , TGainDecrease.y); myGLCD.print("D");


  row = row + rowheight;;
  TIntegralValue.x  = colr + 5; TIntegralValue.y  = row;//  TIntegralIncrease.x = colr + 35 ;
 // TIntegralIncrease.y = row; TIntegralDecrease.x = colr + 55; TIntegralDecrease.y = row;
  TIntegralReset.y = row; TIntegralReset.x = colr + 75; //TIntegralReset.y = row;

   myGLCD.print("I:",colr - 5 , row);  myGLCD.printNumI(Integral , TIntegralValue.x , TIntegralValue.y) ;
 // myGLCD.setCursor(TIntegralIncrease.x  , TIntegralIncrease.y); myGLCD.print("U"); myGLCD.setCursor(TIntegralDecrease.x , TIntegralDecrease.y); myGLCD.print("D");
   myGLCD.print("R",TIntegralReset.x , TIntegralReset.y);

  row = row + rowheight;;
   myGLCD.print("ps:",colr - 10 , row); // tftPrintIntTo5Char(LoopsPerSecond,colr  , row) ;
   myGLCD.print("Skp",colr +  35 , row);  myGLCD.print(Readingskipped,colr + 60 , row);

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void StartLinebyTimeAndTemp(double timemins, int temp, int lineID, uint16_t color) {
  //Serial.println ("StartLineTandT id:");Serial.println (lineID);Serial.println(" color:");Serial.println(color);
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
  //Serial.println ("AddLineTandT line iD:");Serial.println (lineID);Serial.println(" time:");Serial.println(timemins);Serial.println("temp:");Serial.println(temp);Serial.println(" color:");Serial.println(LineColorforLineID[lineID]);
  //Serial.println ("newX:");Serial.println (newX);Serial.println ("pixelspermin:");Serial.println (PixelsPerMin);Serial.println(" newY:");Serial.println(newY);
  myGLCD.setColor(0,0,255  );//LineColorforLineID[lineID]);
  myGLCD.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY );
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
  myGLCD.setColor(LineColorforLineID[lineID]);
  //myGLCD.setColor(0,150,0);
  Serial.print('a');Serial.print(newX);Serial.print( "   ");;Serial.print(timemins);Serial.print( "   ");Serial.println(temp);
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

void  displayState(int state) {
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(VGA_WHITE);
  switch (state) {
    case STATEROASTING:
      myGLCD.setColor(WHITE);
      strncpy(StateName, "Roasting", 8);
      myGLCD.print("Roasting",30, 0);
      break;
    case STATESTOPPED:
      strncpy(StateName, "Stopped ", 8);
      myGLCD.print("Stopped ",30, 0);
      break;
    case STATECOOLING:
      myGLCD.setColor(VGA_WHITE);
      strncpy(StateName, "Cooling ", 8);
      myGLCD.print( "Cooling ",30, 0);
      break;
    case STATEOVERHEATED:
      strncpy(StateName, "HotCoil  ", 9);
      myGLCD.print ("HotCoil ",30, 0);
      break;
    case STATENOFANCURRENT:
      strncpy(StateName, "NoFanCur   ", 8);
      myGLCD.print ("HotFan ",30, 0);
      break;
    case STATEFANONLY:
      strncpy(StateName, "FanOnly ", 8);
      myGLCD.print ("FanOnly ",30, 0);
      break;
    default:
      strncpy(StateName, "unk   ", 8);
      myGLCD.print ("unk     ",30, 0);
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

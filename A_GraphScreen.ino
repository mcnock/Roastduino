
//----------------------------------------------------------------------------------------------------------------------------------------------------
// GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN
//----------------------------------------------------------------------------------------------------------------------------------------------------


void graphProfile() {
    
    //draw scales
    myGLCD.setFont(SmallFont);
    //Calculate  setpoint accumlated minutes basedon current minute settings    
   
   if (setpointschanged == true){
        //read from memory
        int accumulatedbySetPoint = 0;
        int accumulatedMinutes = 0;
        
        for (int xSetPoint = 0; xSetPoint < SetPointCount; xSetPoint++) {
             if (xSetPoint == 0) {
                //EEPROM.get(SETPOINTTEMP_EP[X],MySetPoints[X].Temperature);
       
                MySetPoints[0].Temperature = 390;
                MySetPoints[0].SpanMinutes = 0;
                MySetPoints[0].Minutes = 0;
                MyMinuteTemperature[0] = MySetPoints[0].Temperature;
            }
            else
            {
                EEPROM.get(SETPOINTTEMP_EP[xSetPoint],MySetPoints[xSetPoint].Temperature);
                MySetPoints[xSetPoint].SpanMinutes = MySetPoints[xSetPoint].Minutes - MySetPoints[xSetPoint - 1].Minutes;
                accumulatedbySetPoint = accumulatedbySetPoint + MySetPoints[xSetPoint].SpanMinutes;
                MySetPoints[xSetPoint].Minutes = accumulatedbySetPoint;
         
                //now calc the minute setpoint for the span leading to this setpoint
                double TempPerMinuteinSpan = ((double)(MySetPoints[xSetPoint].Temperature - MySetPoints[xSetPoint - 1].Temperature)) / MySetPoints[xSetPoint].SpanMinutes ;
                for (int xSpanMinute = 1 ; xSpanMinute <= MySetPoints[xSetPoint].SpanMinutes  ;  xSpanMinute++)
                {
                    accumulatedMinutes = accumulatedMinutes + 1;
                    MyMinuteTemperature[accumulatedMinutes] = MySetPoints[xSetPoint - 1].Temperature + (TempPerMinuteinSpan * xSpanMinute);
                }
                        
            }
        }
    
             
     Serial.println("AAA");
     
     
     //perform some necesary calculations to size our scales to the display 
     //yscale is done in 3 ranges
      TimeScreenLeft =  MySetPoints[EndingSetPoint].Minutes + 5;
      TempYMax = 800; 
      TempSplitHigh = 440;
      TempSplitHigh = MySetPoints[EndingSetPoint].Temperature ;
      PixelYSplit2 = 400;//180;
      TempSplitLow = (MySetPoints[2].Temperature - Gain) ;
      PixelYSplit = 150;//90;
    
      TempPerPixL = TempSplitLow / PixelYSplit;
      TempPerPixM = (TempSplitHigh - TempSplitLow) / (PixelYSplit2 - PixelYSplit);
      TempPerPixH = (TempYMax - TempSplitHigh) / (480 - PixelYSplit2);
      PixelsPerMin =  (int)(800 / TimeScreenLeft);
  
  
  }
  
  myGLCD.setColor(BLACK);
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
  if (TimeScreenLeft > 15){
     myGLCD.drawLine(15 * PixelsPerMin ,0,15 * PixelsPerMin,  myGLCD.getDisplayYSize()-30 );
     myGLCD.print("15min",(15 * PixelsPerMin) - 30, 460);
  }
  if (TimeScreenLeft > 18){
      myGLCD.drawLine(18 * PixelsPerMin ,0,18 * PixelsPerMin,  myGLCD.getDisplayYSize()-30 );
      myGLCD.print("18min",(18 * PixelsPerMin) - 30, 460);
  }
 
  //draw y scale for 3 ranges
  int yaxislable = 30;
  int lastt =0;
  //Low range A
  myGLCD.setColor(100,100,100);  

     Serial.println("AAB");

  for (int t = 50; t < (TempSplitLow - 50); t = t + 50) {
    //Serial.println(t);
    myGLCD.drawLine(yaxislable,  YforATemp(t),myGLCD.getDisplayXSize(),YforATemp(t) );
    myGLCD.printNumI(t,2 , YforATemp(t) - 5);
    lastt = t;
  }

  myGLCD.setColor(100,100,100);  

 
  //med range 
//  myGLCD.setColor(WHITE);
  for (int t = TempSplitLow; t < (TempSplitHigh + 5); t = t + 5) {
    myGLCD.drawLine(yaxislable,  YforATemp(t),myGLCD.getDisplayXSize(),  YforATemp(t));
    myGLCD.printNumI(t,2, YforATemp(t) - 5);
    lastt = t;
  }  

  
 
  //high range 
  int tstart = 500;
  if (TempSplitHigh < 380) {tstart = 400;}
  else if (TempSplitHigh < 420) {tstart = 450;}
    
  myGLCD.setColor(100,100,100);  
  for (int t = tstart; t < TempYMax ; t = t + 100) {
    myGLCD.drawLine(yaxislable,  YforATemp(t),myGLCD.getDisplayXSize(),  YforATemp(t));
    myGLCD.printNumI(t,2 , YforATemp(t) - 5);
  }

  
  
  //draw the profile by and setpoint 
  StartLinebyTimeAndTemp (0, MySetPoints[0].Temperature, PROFILELINEID , WHITE);

  for (int Minutes = 1; Minutes <= MySetPoints[EndingSetPoint].Minutes; Minutes++) {
      AddLinebyTimeAndTemp(Minutes, MyMinuteTemperature[Minutes], PROFILELINEID);
  }
  
  for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++) {
    DrawSetPoint(xSetPoint, LineColorforLineID[PROFILELINEID]); 
  }

  
  

//draw endpoint highlights
//  myGLCD.setFont(BigFont);
//  myGLCD.setColor(BLUE);
//  int PixYEndPoint = YforATemp(MySetPoints[EndingSetPoint].Temperature);
//  myGLCD.drawLine(400, PixYEndPoint, myGLCD.getDisplayXSize()-50, PixYEndPoint);
//  myGLCD.print("SP:",500, PixYEndPoint-20);myGLCD.printNumI(MySetPoints[EndingSetPoint].Temperature, 550, PixYEndPoint-20);

  //drawing cooling line
 // int PixYCoolingPoint = YforATemp(TEMPCOOLINGDONE);
 // myGLCD.drawLine(500, PixYCoolingPoint, myGLCD.getDisplayXSize()-50, PixYCoolingPoint);
 // myGLCD.print("Co:",500, PixYCoolingPoint+7);myGLCD.printNumI(TEMPCOOLINGDONE, 550, PixYCoolingPoint+7);
  ReDrawROLLAVGLINEFromArray(ORANGE);
     Serial.println("AAC");

  DrawHorControlMenu();

  DrawHorFanMenu();

 //if sp is  selected, assume user is adjusting setpoint redraw the menu
  if (spSelected >= 0) {
      DrawVMenu(1,-1);
  }
  else
  {
      DrawVMenu(0,-1);
  }
       Serial.println("AAD");

  DrawFanGraph();
  UpdateEachSecond(All);
  UpdateRealTime(All);
  setpointschanged = false;
       Serial.println("AAE");

  UpdateDisplayDetailA(false);
}

void UpdateDisplayDetailA(boolean bValuesOnly) {
  //this is top row
 
  myGLCD.setBackColor(BLACK);
  
  int rowstart = 70;
  
  int rowheight = 20;
  int row = rowstart;
  int col = 40;
  int col2 = 120;
  int col3 = 220;
  int col4 = 260;
  if (bValuesOnly == false){
      myGLCD.setColor(BLACK);
      myGLCD.fillRect(col - 5,row, col4 + (myGLCD.getFontYsize() * 5)  , row + (rowheight * 5));
  }
  //Serial.println("AAF");
  myGLCD.setColor(WHITE);  

  myGLCD.setFont(BigFont);
 
  if (bValuesOnly == false){
       myGLCD.print("Time:", col , row);   
       myGLCD_printNumF(RoastMinutes,col2 , row,5,2);
       row = row + rowheight;
       //Serial.println("AAF2");
       myGLCD.print("Set :",col , row);    
       myGLCD.printNumI(CurrentSetPointTemp,col2 , row,5);
       row = row + rowheight;
       myGLCD.print("Duty:", col, row); 
       myGLCD_printNumF(Duty, col2, row, 5,2);
       row = row + rowheight;
       myGLCD.print("Err :",col , row);  
       myGLCD.printNumI(Err,col2,row,5, ' ');
       row = row + rowheight;
       myGLCD.print("IEr :",col  , row);  
       myGLCD.printNumI(ErrI,col2,row, 5, ' ');
       myGLCD.setFont(SmallFont);
       row = rowstart;
       myGLCD.print("Gain:",col3 , row);      
       myGLCD.printNumI(Gain,col4 , row, 6, ' ');
       row = row + rowheight;
       myGLCD.print("Int :", col3, row);   
       myGLCD_printNumF(Integral,col4, row, 6, 2);  
       row = row + rowheight;
       myGLCD.print("l/s :",col3, row); 
       myGLCD.printNumI(LoopsPerSecond,col4 , row, 6,' ') ;
       row = row + rowheight;
       myGLCD.print("Skp : ", col3, row);  
       myGLCD.printNumI(Readingskipped,col4 , row, 6,' ');
       row = row + rowheight;
       myGLCD.print("FanD:", col3, row);  
       myGLCD.printNumI(FanSpeedPWMAutoDecrease,col4 , row,6,' ');
      }
      else{
       //myGLCD.print("Time:", col , row);   
       myGLCD_printNumF(RoastMinutes,col2 , row,5,2);
       row = row + rowheight;
       //Serial.println("AAF2");
       //myGLCD.print("Set :",col , row);    
       myGLCD.printNumI(CurrentSetPointTemp,col2 , row,5);
       row = row + rowheight;
       //myGLCD.print("Duty:", col, row); 
       myGLCD_printNumF(Duty, col2, row, 5,2);
       row = row + rowheight;
       //myGLCD.print("Err :",col , row);  
       myGLCD.printNumI(Err,col2,row,5, ' ');
       row = row + rowheight;
       //myGLCD.print("IEr :",col  , row);  
       myGLCD.printNumI(ErrI,col2,row, 5, ' ');
       myGLCD.setFont(SmallFont);
       row = rowstart;
       //myGLCD.print("Gain:",col3 , row);      
       myGLCD.printNumI(Gain,col4 , row, 6, ' ');
       row = row + rowheight;
       //myGLCD.print("Int :", col3, row);   
       myGLCD_printNumF(Integral,col4, row, 6, 2);  
       row = row + rowheight;
       //myGLCD.print("l/s :",col3, row); 
       myGLCD.printNumI(LoopsPerSecond,col4 , row, 6,' ') ;
       row = row + rowheight;
       //myGLCD.print("Skp : ", col3, row);  
       myGLCD.printNumI(Readingskipped,col4 , row, 6,' ');
       row = row + rowheight;
       //myGLCD.print("FanD:", col3, row);  
       myGLCD.printNumI(FanSpeedPWMAutoDecrease,col4 , row,6,' ');
    
      }
  //Serial.println("AAG");
}
void UpdateRealTime(boolean OnlyChanges) {
  
  if (lastStateUpdated != State || OnlyChanges == false){
    lastStateUpdated = State;
    //Serial.print("Updated to new state");Serial.println(State);
   myGLCD.setFont(Grotesk24x48);
  int row = 50 -myGLCD.getFontYsize();
  int col = 40;
  switch (State) {
    case STATEROASTING:
      myGLCD.setColor(GREEN);
      //strncpy(StateName, "Roasting", 8);
      myGLCD.print("Roasting",col,row);
      break;
    case STATESTOPPED:
      myGLCD.setColor(RED); 
      //strncpy(StateName, "Stopped ", 8);
      myGLCD.print("Stopped ",col,row);
      break;
    case STATECOOLING:
      myGLCD.setColor(BLUE);
      //strncpy(StateName, "Cooling ", 8);
      myGLCD.print( "Cooling ",col,row);
      break;
    case STATEOVERHEATED:
      myGLCD.setColor(RED);
      //strncpy(StateName, "HotCoil  ", 8);
      myGLCD.print ("HotCoil ",col,row);
      break;
    case STATENOFANCURRENT:
      myGLCD.setColor(RED);
      //strncpy(StateName, "NoFanCur   ", 8);
      myGLCD.print ("HotFan ",col,row);
      break;
    case DEBUGTOGGLE:
      myGLCD.setColor(BLUE);
      //strncpy(StateName, "DebugTog", 8);
      myGLCD.print ("Dbg-Tog ",col,row);
      break;
    case DEBUGDUTY:
      myGLCD.setColor(BLUE);
      //strncpy(StateName, "DebugDut", 8);
      myGLCD.print ("Dbg-Duty",col,row);
      break;
    case STATEFANONLY:
      myGLCD.setColor(BLUE);
      //strncpy(StateName, "FanOnly ", 8);
      myGLCD.print ("FanOnly ",col,row);
      break;

    default:
      strncpy(StateName, "unk   ", 8);
      myGLCD.print ("unk     ",col,row);
      break;
  }
  }
  else{
 //Serial.println("no updated becaue states is same as last update");
  }
  if (newerrmsg == true || OnlyChanges == false){
     myGLCD.setFont(BigFont);
     int row = 65-myGLCD.getFontYsize() ;
     int col = 40;
     newerrmsg =false;
     if (lenlasterrmsg > 0){
         myGLCD.setColor(BLACK);  
         myGLCD.fillRect(col,row, myGLCD.getFontXsize() * (lenlasterrmsg + 1), row + myGLCD.getFontYsize());
         lenlasterrmsg = 0;
     }
    if (errmsg.length() > 0){
        myGLCD.setColor(ORANGE);
        myGLCD.print(errmsg,col,row);
        lenlasterrmsg = errmsg.length();
 
    }
  }  
}
void UpdateEachSecond(boolean bValuesOnly) { 
  myGLCD.setFont(BigFont);
  int rowheight = 20;
  int row =  220 ;
  int col =  350 ;
  int col2 = 420;
  int col3 = 550;
  int col4 = 640;
  int col5 = 710;
  col= col3;
  col2 = col4;
  myGLCD.setBackColor(BLACK);
  myGLCD.setColor(VGA_WHITE);        
   if (bValuesOnly == false){
       row = row + rowheight;
         myGLCD.print("Tvg :",col , row);  myGLCD.print("F",col5 , row);
         myGLCD.printNumI(TBeanAvgRoll.mean(),col2,row,4,' ');
       row = row + rowheight; 
         myGLCD.print("T1  :",col , row);   myGLCD.print("F",col5 , row);       
         myGLCD.printNumI(TBean1, col2, row, 4, ' ');
       row = row + rowheight;
         myGLCD.print("T2  :", col, row);        myGLCD.print("F",col5 , row);   
         myGLCD.printNumI(TBean2, col2, row, 4, ' ');
       row = row + rowheight;
         myGLCD.print("T3  :", col, row);myGLCD.print("F",col5 , row);          
         myGLCD.printNumI(TBean3, col2, row, 4, ' ');
       row = row + rowheight;
          myGLCD.print("Coil:", col, row); myGLCD.print("F",col5 , row);       
          myGLCD.printNumI(TCoilRoll.mean(), col2, row, 4, ' ');
       row = row + rowheight;
          myGLCD.print("Fan A:", col3 , row); myGLCD.print("A",col5 , row);   
          myGLCD.printNumI(CurrentFan,col4 , row, 4,' ');         
       row = row + rowheight;
          myGLCD.print("Coil1:",col3  , row);myGLCD.print("A",col5 , row);    
          myGLCD.printNumI(CurrentHeat1,col4 ,row, 4,' ');
       row = row + rowheight;
          myGLCD.print("Coil2:",col3 , row); myGLCD.print("A",col5 , row);    
          myGLCD.printNumI(CurrentHeat2,col4 , row, 4,' ');
       row = row + rowheight;
          myGLCD.print("Fan P:", col3, row);myGLCD.print("P",col5 , row);     
          myGLCD.printNumI(FanPressureRoll.mean() * 100,col4, row,4,' '); 
   
   }
   else
   {
       row = row + rowheight;
         //myGLCD.print("Tvg :",col , row);  myGLCD.print("F",col5 , row);
         myGLCD.printNumI(TBeanAvgRoll.mean(),col2,row,4,' ');
       row = row + rowheight; 
         //myGLCD.print("T1  :",col , row);   myGLCD.print("F",col5 , row);       
         myGLCD.printNumI(TBean1, col2, row, 4, ' ');
       row = row + rowheight;
         //myGLCD.print("T2  :", col, row);        myGLCD.print("F",col5 , row);   
         myGLCD.printNumI(TBean2, col2, row, 4, ' ');
       row = row + rowheight;
         //myGLCD.print("T3  :", col, row);myGLCD.print("F",col5 , row);          
         myGLCD.printNumI(TBean3, col2, row, 4, ' ');
       row = row + rowheight;
          //myGLCD.print("Coil:", col, row); myGLCD.print("F",col5 , row);       
          myGLCD.printNumI(TCoilRoll.mean(), col2, row, 4, ' ');
       row = row + rowheight;
          //myGLCD.print("Fan A:", col3 , row); myGLCD.print("A",col5 , row);   
          myGLCD.printNumI(CurrentFan,col4 , row, 4,' ');         
       row = row + rowheight;
          //myGLCD.print("Coil1:",col3  , row);myGLCD.print("A",col5 , row);    
          myGLCD.printNumI(CurrentHeat1,col4 ,row, 4,' ');
       row = row + rowheight;
          //myGLCD.print("Coil2:",col3 , row); myGLCD.print("A",col5 , row);    
          myGLCD.printNumI(CurrentHeat2,col4 , row, 4,' ');
       row = row + rowheight;
          //myGLCD.print("Fan P:", col3, row);     
          myGLCD.printNumI(FanPressureRoll.mean() * 100,col4, row,4,' '); 
      
   }
   
}
void DrawFanGraph(){
    myGLCD.setColor(AQUA);
   int Col1 = 320;
   int Col2 = Col1 + ((PixelsPerMin * MySetPoints[EndingSetPoint].Minutes)/4 );
  int Y1 = YforATemp(CalcFanPWMForATime(0)) - 10 ;
  int yshiftdown = YforATemp(220) - Y1 ;
  
  Y1 = YforATemp(CalcFanPWMForATime(0)) - 10 + yshiftdown;
  int X1 = Col1 - 10;
  int Y2 = YforATemp(CalcFanPWMForATime(MySetPoints[EndingSetPoint].Minutes))+ 10  + yshiftdown ;
  int X2 =  Col2 + 10;
  
  myGLCD.setColor(BLACK);
  
  myGLCD.fillRect(X1, Y1, X2, Y2);
  myGLCD.setColor(AQUA);
  myGLCD.drawRect(X1 - 1, Y1 -1 , X2 + 1, Y2 + 1);
   
   Y1 = YforATemp(CalcFanPWMForATime(0))  + yshiftdown;
   X1 = Col1;
   Y2 = YforATemp(CalcFanPWMForATime(FanSpeedPWNDelayDecreaseByMinutes))  + yshiftdown;
   X2 =  Col1 + ((PixelsPerMin * FanSpeedPWNDelayDecreaseByMinutes)/4 );  
   myGLCD.setFont(SmallFont);
   myGLCD.printNumI(FanSpeedPWMStart,Col1-35 , Y1 - (myGLCD.getFontYsize()/2)  );
   
  
  myGLCD.drawLine(X1, Y1, X2, Y2 );
  X1 = X2; Y1 = Y2;
  Y2 = YforATemp(CalcFanPWMForATime(FanSpeedPWNDecreaseByMinutes))  + yshiftdown;
  X2 =  Col1 + ((PixelsPerMin * FanSpeedPWNDecreaseByMinutes)/4 )  ;
  myGLCD.drawLine(X1, Y1, X2, Y2 );
  X1 = X2; Y1 = Y2;
  Y2 = YforATemp(CalcFanPWMForATime(MySetPoints[EndingSetPoint].Minutes)) + yshiftdown;
  X2 =  Col2 ;
  myGLCD.drawLine(X1, Y1, X2, Y2 );  
 
  myGLCD.printNumI(FanSpeedPWMStart -FanSpeedPWMAutoDecrease ,Col2 + 15 , Y2 - (myGLCD.getFontYsize()/2)  );
    
 
   Y1 = YforATemp(CalcFanPWMForATime(RoastMinutes))+ yshiftdown;
   X1 = Col1 + (PixelsPerMin * (RoastMinutes/4.0));
   myGLCD.fillCircle(X1, Y1, 5);
   myGLCD.setColor(BLACK);
   
  
}
void UpdateFanPWMValues() {
  myGLCD.setFont(BigFont);
  int rowheight = 20;
  int row = 450;
  int col = 30;
  int col1 = col + 100 ;
  int col2 = col1 + 100  ;

   if (FanSpeedPWMAutoMode == true)
   {
      myGLCD.setColor(LGBLUE) ;
   }
   else
   {
      myGLCD.setColor(AQUA);
   
   }
   myGLCD.printNumI(FanSpeedPWMStart,col , row, 3,' ');
   myGLCD.printNumI(FanSpeedPWM,col1 , row, 3,' ');
   myGLCD.printNumI((FanSpeedPWMStart-FanSpeedPWMAutoDecrease),col2 , row,3,' ');
       
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void StartLinebyTimeAndY(double timemins, int Y, int lineID, uint16_t color) {
  //Serial.println ("StartLineTandT id:");
  
 //Serial.println (temp);
 //Serial.println (lineID);
 //Serial.println(" color:");
  //Serial.println(color);
 //Serial.println("X");
  
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
      myLastGraphXPixels[X] = -1;
    }
  }
}

void AddLinebyTimeAndTemp(double timemins, int temp, int lineID) {
  uint16_t newY = (uint16_t)YforATemp(temp);
  uint16_t newX = (uint16_t)(PixelsPerMin * timemins);
 //Serial.print("AddLineByTimeAndTemp X:");
  //Serial.print(LastXforLineID[lineID]);Serial.print(" ");Serial.print(newX);Serial.print(" Y:");
  //Serial.print(LastYforLineID[lineID]);Serial.print(" ");Serial.print(newY);Serial.println(" ");
  //Serial.println(temp);
  myGLCD.setColor(LineColorforLineID[lineID]);//LineColorforLineID[lineID]);
  //x,y,x,y
  myGLCD.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY );
  LastXforLineID[lineID] = newX;
  LastYforLineID[lineID] = newY;
  if (lineID == ROLLAVGLINEID){
       myLastGraphYPixels[newX] = newY;
  }
}

void DrawSetPoint(int setpoint, uint16_t color){
    int Y = YforATemp(MySetPoints[setpoint].Temperature);
    int X = MySetPoints[setpoint].Minutes * PixelsPerMin;

   
    myGLCD.setColor(BLACK);
    myGLCD.fillRect(X-35, Y-8, X +10, Y- 12 - myGLCD.getFontYsize());
  
    myGLCD.setColor(WHITE);
    myGLCD.fillCircle(X, Y, 7);
    myGLCD.printNumI(setpoint, X - 30, Y - 20);
    myGLCD.printNumI(MySetPoints[setpoint].Temperature, X - 15, Y - 20);
}

void DrawMovedSetPoint(int setpoint){
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
  uint16_t newY = YforATemp(temp);
  myGLCD.setColor(LineColorforLineID[lineID]);
  //myGLCD.setColor(0,150,0);
  //Serial.print('a');Serial.print(newX);Serial.print( "   ");;Serial.print(timemins);Serial.print( "   ");Serial.println(temp);
  myGLCD.fillCircle(newX, newY, radius );
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
      //Serial.println ("DrawRealTime ");Serial.println(" color:");Serial.println(color);
      myGLCD.setColor(color);
      myGLCD.drawLine(LastXforLineID[1], LastYforLineID[1] , X, myLastGraphYPixels[X]);
      //BoldLine(LastXforLineID[1], LastYforLineID[1] , X, myLastGraphYPixels[X], color);
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
int YforATemp(double temp) {
  int result = 0;
  if ( temp < 0.0 ) {
     temp = 0.0;
  }  
  if (temp <= TempSplitLow) {
    result = (myGLCD.getDisplayYSize() - ((double)temp / TempPerPixL));
    //Serial.print("low t:");
  }
  else if (temp <= TempSplitHigh) {
    //Serial.print("mid t:");
    result = double(myGLCD.getDisplayYSize() - PixelYSplit) - ((double)(temp - TempSplitLow) / (double)TempPerPixM);
  } else {
    //Serial.print("high t:");
    result = double(myGLCD.getDisplayYSize() -  PixelYSplit2) + ((double)(TempSplitHigh - temp ) / (double)TempPerPixH);
  }
  //Serial.print (temp);
  //Serial.print ( "y:");
  //Serial.println(result);
  if (result < 0) {
    return 1;
  }
  else
  { return result;
  }


}

void myGLCD_printNumF(double Number, int col ,int row,  int Len, int Dec){
  
 
  if (Len == 5){
    Serial.println("AC5");
       
    Serial.println(Number);
    dtostrf(Number, Len, Dec, s6);
    Serial.println (s6 );
    myGLCD.print(s6,col, row);
  
  }
  else if (Len == 6)
  { 
    Serial.println("AC6");

    Serial.println(Number);
    dtostrf(Number, Len, Dec, s7);
    Serial.println (s7 );
    myGLCD.print(s7,col, row);
   
   }
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN          GRAPHSCREEN
//----------------------------------------------------------------------------------------------------------------------------------------------------


void graphProfile() {

  //draw scales
  myGLCD.setFont(SmallFont);
  //Calculate  setpoint accumlated minutes basedon current minute settings

  if (setpointschanged == true) {
    //read from memory
    int accumulatedbySetPoint = 0;
    int accumulatedMinutes = 0;
    byte m;
    EEPROM.get(RoastLength_EP, m);
    MySetPoints[EndingSetPoint].Minutes = m;
    //Serial.println ("Loading new setpoints:");
    if (MySetPoints[EndingSetPoint].Minutes < 10 or MySetPoints[EndingSetPoint].Minutes > 20)
    {


      MySetPoints[EndingSetPoint].Minutes = 16;
    }
    //Serial.println ("XX");
    MySetPoints[0].Minutes = 0;
    MySetPoints[1].Minutes = 4;

    if (MySetPoints[EndingSetPoint].Minutes >= 14)
    {
      //3 minute appart
      MySetPoints[2].Minutes = 7;
      MySetPoints[3].Minutes = 10;
      MySetPoints[4].Minutes = 13;
      TimeScreenLeft =  MySetPoints[EndingSetPoint].Minutes + 5;

    }
    else if (MySetPoints[EndingSetPoint].Minutes >= 11)
    {
      //2 minute appart
      MySetPoints[2].Minutes = 6;
      MySetPoints[3].Minutes = 8;
      MySetPoints[4].Minutes = 10;
      TimeScreenLeft =  MySetPoints[EndingSetPoint].Minutes + 3;

    }
    else
    {
      //1 or 2 minute appart
      MySetPoints[2].Minutes = 5;
      MySetPoints[3].Minutes = 7;
      MySetPoints[4].Minutes = 9;
      TimeScreenLeft =  MySetPoints[EndingSetPoint].Minutes + 2;

    }


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
        EEPROM.get(SETPOINTTEMP_EP[xSetPoint], MySetPoints[xSetPoint].Temperature);
        //Serial.println(MySetPoints[xSetPoint].Temperature);
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




    //perform some necesary calculations to size our scales to the display
    //yscale is done in 3 ranges
    TimeScreenLeft =  MySetPoints[EndingSetPoint].Minutes + 3;
    TempYMax = 800;
    TempSplitHigh = 440;
    TempSplitHigh = MySetPoints[EndingSetPoint].Temperature ;
    PixelYSplit2 = 400;//180;
    //  TempSplitLow = (MySetPoints[2].Temperature - Gain) ;
    TempSplitLow = (MySetPoints[1].Temperature ) ;

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
  myGLCD.setColor(100, 100, 100);

  //draw our x time axis
  myGLCD.drawLine(3 * PixelsPerMin , 0,  3 * PixelsPerMin, myGLCD.getDisplayYSize() - 30 );
  myGLCD.print("3min", (3 * PixelsPerMin) - 30, 460);
  myGLCD.drawLine(6 * PixelsPerMin , 0, 6 * PixelsPerMin , myGLCD.getDisplayYSize() - 30);
  myGLCD.print("6min", (6 * PixelsPerMin) - 30, 460);
  myGLCD.drawLine(9 * PixelsPerMin , 0, 9 * PixelsPerMin, myGLCD.getDisplayYSize() - 30);
  myGLCD.print("9min", (9 * PixelsPerMin) - 30, 460);
  myGLCD.drawLine(12 * PixelsPerMin , 0, 12 * PixelsPerMin,  myGLCD.getDisplayYSize() - 30 );
  myGLCD.print("12min", (12 * PixelsPerMin) - 30, 460);
  if (TimeScreenLeft > 15) {
    myGLCD.drawLine(15 * PixelsPerMin , 0, 15 * PixelsPerMin,  myGLCD.getDisplayYSize() - 30 );
    myGLCD.print("15min", (15 * PixelsPerMin) - 30, 460);
  }
  if (TimeScreenLeft > 18) {
    myGLCD.drawLine(18 * PixelsPerMin , 0, 18 * PixelsPerMin,  myGLCD.getDisplayYSize() - 30 );
    myGLCD.print("18min", (18 * PixelsPerMin) - 30, 460);
  }

  //draw y scale for 3 ranges
  int yaxislable = 30;
  int lastt = 0;
  //Low range A
  myGLCD.setColor(100, 100, 100);


  for (int t = 50; t < (TempSplitLow - 50); t = t + 50) {
    //Serial.println(t);
    myGLCD.drawLine(yaxislable,  YforATemp(t), myGLCD.getDisplayXSize(), YforATemp(t) );
    myGLCD.printNumI(t, 2 , YforATemp(t) - 5);
    lastt = t;
  }

  myGLCD.setColor(100, 100, 100);


  //med range
  //  myGLCD.setColor(WHITE);
  for (int t = TempSplitLow; t < (TempSplitHigh + 5); t = t + 5) {
    myGLCD.drawLine(yaxislable,  YforATemp(t), myGLCD.getDisplayXSize(),  YforATemp(t));
    myGLCD.printNumI(t, 2, YforATemp(t) - 5);
    lastt = t;
  }



  //high range
  int tstart = 500;
  if (TempSplitHigh < 380) {
    tstart = 400;
  }
  else if (TempSplitHigh < 420) {
    tstart = 450;
  }

  myGLCD.setColor(100, 100, 100);
  for (int t = tstart; t < TempYMax ; t = t + 100) {
    myGLCD.drawLine(yaxislable,  YforATemp(t), myGLCD.getDisplayXSize(),  YforATemp(t));
    myGLCD.printNumI(t, 2 , YforATemp(t) - 5);
  }



  //draw the profile by and setpoint
  StartLinebyTemp ( MySetPoints[0].Temperature, PROFILELINEID , WHITE);

  for (int Minutes = 1; Minutes <= MySetPoints[EndingSetPoint].Minutes; Minutes++) {
    AddLinebyTimeAndTemp(Minutes, MyMinuteTemperature[Minutes], PROFILELINEID);
  }

  for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++) {
    DrawSetPoint(xSetPoint, LineColorforLineID[PROFILELINEID]);
  }

  //Serial.println("AAC");

  ReDrawROLLAVGLINEFromArray(ORANGE);
  //Serial.println("AAD");

  DrawHorControlMenu();

  DrawHorFanMenu();

  //if sp is  selected, assume user is adjusting setpoint redraw the menu
  if (spSelected >= 0) {
    DrawVMenu(VmenuSetPointSelect, -1);
  }
  else
  {
    DrawVMenu(VmenuEmpty, -1);
  }
  //Serial.println("AAD");

  graphFanProfile();
  //ReDrawFANromArray(ORANGE);

  UpdateEachSecond(All);
  UpdateRealTime(All);
  setpointschanged = false;
  //Serial.println("AAE");

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
  if (bValuesOnly == false) {
    myGLCD.setColor(BLACK);
    myGLCD.fillRect(col - 5, row, col4 + (myGLCD.getFontYsize() * 5)  , row + (rowheight * 5));
  }
  //Serial.println("AAF");
  myGLCD.setColor(WHITE);

  myGLCD.setFont(BigFont);

  if (bValuesOnly == false) {
    myGLCD.print("Time:", col , row);
    myGLCD_printNumF(RoastMinutes, col2 , row, 5, 2);
    row = row + rowheight;
    //Serial.println("AAF2");
    myGLCD.print("Set :", col , row);
    myGLCD.printNumI(CurrentSetPointTemp, col2 , row, 5);
    row = row + rowheight;
    myGLCD.print("Duty:", col, row);
    myGLCD_printNumF(Duty, col2, row, 5, 2);
    row = row + rowheight;
    myGLCD.print("Err :", col , row);
    myGLCD.printNumI(Err, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print("IEr :", col  , row);
    myGLCD.printNumI(ErrI, col2, row, 5, ' ');
    myGLCD.setFont(SmallFont);
    row = rowstart;
    myGLCD.print("Gain:", col3 , row);
    myGLCD.printNumI(Gain, col4 , row, 6, ' ');
    row = row + rowheight;
    myGLCD.print("Int :", col3, row);
    myGLCD_printNumF(Integral, col4, row, 6, 2);
    row = row + rowheight;
    myGLCD.print("l/s :", col3, row);
    myGLCD.printNumI(LoopsPerSecond, col4 , row, 6, ' ') ;
    row = row + rowheight;
    myGLCD.print("Skp : ", col3, row);
    myGLCD.printNumI(Readingskipped, col4 , row, 6, ' ');
    row = row + rowheight;
  }
  else {
    //myGLCD.print("Time:", col , row);
    myGLCD_printNumF(RoastMinutes, col2 , row, 5, 2);
    row = row + rowheight;
    //Serial.println("AAF2");
    //myGLCD.print("Set :",col , row);
    myGLCD.printNumI(CurrentSetPointTemp, col2 , row, 5);
    row = row + rowheight;
    //myGLCD.print("Duty:", col, row);
    myGLCD_printNumF(Duty, col2, row, 5, 2);
    row = row + rowheight;
    //myGLCD.print("Err :",col , row);
    myGLCD.printNumI(Err, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print("IEr :",col  , row);
    myGLCD.printNumI(ErrI, col2, row, 5, ' ');
    myGLCD.setFont(SmallFont);
    row = rowstart;
    //myGLCD.print("Gain:",col3 , row);
    myGLCD.printNumI(Gain, col4 , row, 6, ' ');
    row = row + rowheight;
    //myGLCD.print("Int :", col3, row);
    myGLCD_printNumF(Integral, col4, row, 6, 2);
    row = row + rowheight;
    //myGLCD.print("l/s :",col3, row);
    myGLCD.printNumI(LoopsPerSecond, col4 , row, 6, ' ') ;
    row = row + rowheight;
    //myGLCD.print("Skp : ", col3, row);
    int R =   Readingskipped[0] + Readingskipped[1] + Readingskipped[2];
    myGLCD.printNumI(R, col4 , row, 6, ' ');
    row = row + rowheight;

  }
  //Serial.println("AAG");
}
void UpdateRealTime(boolean OnlyChanges) {

  if (lastStateUpdated != State || OnlyChanges == false) {
    lastStateUpdated = State;
    //Serial.print("Updated to new state");Serial.println(State);
    myGLCD.setFont(Grotesk24x48);
    int row = 50 - myGLCD.getFontYsize();
    int col = 40;
    switch (State) {
      case STATEROASTING:
        myGLCD.setColor(GREEN);
        //strncpy(StateName, "Roasting", 8);
        myGLCD.print("Roasting", col, row);
        break;
      case STATESTOPPED:
        myGLCD.setColor(RED);
        //strncpy(StateName, "Stopped ", 8);
        myGLCD.print("Stopped ", col, row);
        break;
      case STATECOOLING:
        myGLCD.setColor(BLUE);
        //strncpy(StateName, "Cooling ", 8);
        myGLCD.print( "Cooling ", col, row);
        break;
      case STATEOVERHEATED:
        myGLCD.setColor(RED);
        //strncpy(StateName, "HotCoil  ", 8);
        myGLCD.print ("HotCoil ", col, row);
        break;
      case STATENOFANCURRENT:
        myGLCD.setColor(RED);
        //strncpy(StateName, "NoFanCur   ", 8);
        myGLCD.print ("HotFan ", col, row);
        break;
      case DEBUGTOGGLE:
        myGLCD.setColor(BLUE);
        //strncpy(StateName, "DebugTog", 8);
        myGLCD.print ("Dbg-Tog ", col, row);
        break;
      case DEBUGCOIL:
        myGLCD.setColor(BLUE);
        //strncpy(StateName, "DebugTog", 8);
        myGLCD.print ("Dbg-Coil ", col, row);
        break;
      case DEBUGDUTY:
        myGLCD.setColor(BLUE);
        //strncpy(StateName, "DebugDut", 8);
        myGLCD.print ("Dbg-Duty", col, row);
        break;
      case STATEFANONLY:
        myGLCD.setColor(BLUE);
        //strncpy(StateName, "FanOnly ", 8);
        myGLCD.print ("FanOnly ", col, row);
        break;

      default:
        strncpy(StateName, "unk   ", 8);
        myGLCD.print ("unk     ", col, row);
        break;
    }
  }
  else {
    //Serial.println("no updated becaue states is same as last update");
  }

  if (newerrmsg == true || OnlyChanges == false) {
    myGLCD.setFont(BigFont);
    int row = 65 - myGLCD.getFontYsize() ;
    int col = 40;
    newerrmsg = false;


    if (lenlasterrmsg > 0) {
      myGLCD.setColor(BLACK);
      myGLCD.fillRect(col, row, myGLCD.getFontXsize() * (lenlasterrmsg + 1), row + myGLCD.getFontYsize());
      lenlasterrmsg = 0;
    }
    if (errmsg.length() > 0) {
      myGLCD.setColor(ORANGE);
      myGLCD.print(errmsg, col, row);
      lenlasterrmsg = errmsg.length();

    }
  }

}
void UpdateEachSecond(boolean bValuesOnly) {
  myGLCD.setFont(BigFont);

  int rowheight = 20;
  int row =  210 ;
  int col =  520 ; //lable ending with :
  int col2 = 600;  //number
  int col5 = 665; //F
  myGLCD.setBackColor(BLACK);
  myGLCD.setColor(VGA_WHITE);
  if (bValuesOnly == false) {
    row = row + rowheight;
    myGLCD.print("Tvg :", col , row);  myGLCD.print(" F", col5 , row);
    myGLCD.printNumI(TBeanAvgRoll.mean(), col2, row, 4, ' ');
    row = row + rowheight;
    myGLCD.print("T1  :", col , row);   myGLCD.print(" F", col5 , row);
    myGLCD.printNumI(TBean1, col2, row, 4, ' ');
    row = row + rowheight;
    myGLCD.print("T2  :", col, row);        myGLCD.print(" F", col5 , row);
    myGLCD.printNumI(TBean2, col2, row, 4, ' ');
    row = row + rowheight;
    myGLCD.print("Coil:", col, row); myGLCD.print(" F", col5 , row);
    myGLCD.printNumI(TCoil, col2, row, 4, ' ');
    row = row + rowheight;
    myGLCD.print("Cvg :", col, row); myGLCD.print(" F", col5 , row);
    myGLCD.printNumI(TCoilRoll.mean(), col2, row, 4, ' ');

  }
  else
  {
    row = row + rowheight;
    //myGLCD.print("Tvg :",col , row);  myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TBeanAvgRoll.mean(), col2, row, 4, ' ');
    row = row + rowheight;
    //myGLCD.print("T1  :",col , row);   myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TBean1, col2, row, 4, ' ');
    row = row + rowheight;
    //myGLCD.print("T2  :", col, row);        myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TBean2, col2, row, 4, ' ');
    row = row + rowheight;
    //          myGLCD.print("Coil:", col, row); myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TCoil, col2, row, 4, ' ');
    row = row + rowheight;
    //          myGLCD.print("Cvg:", col, row); myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TCoilRoll.mean(), col2, row, 4, ' ');

  }

}
void FanRestart()
{
  //            XStartFan_Last = 0;
  //
  //            X3Fan_Last = FanGraphColStart - 10;
  //            YBotFan_Last = Y1;
  //            XEndFan_Last = X2;

}
void graphFanProfile() {

  uint16_t fancolor = AQUA;
  uint16_t fanback = AQUA;

  myGLCD.setColor(fancolor);

  Serial.println("starting graphFanProfile");

  PixelsPerMinFan = (int)FanGraphXWidth / MySetPoints[EndingSetPoint].Minutes;

  int FanGraphTop = FanGraphBottom - FanGraphHeight;
  int FanXEnd = FanGraphXStart + ((PixelsPerMinFan * MySetPoints[EndingSetPoint].Minutes) ); //calc the ending of graph a max time


  int upperlineY = YforAFan(FanSpeedPWMStart)    ;
  int upperlineX2 =  FanGraphXStart + (PixelsPerMinFan * (FanSpeedPWNDelayDecreaseByMinutes )) ;
  int lowPWM = FanSpeedPWMStart - FanSpeedPWMAutoDecrease;
  int lowerlineY = YforAFan(lowPWM) ;
  int lowerlineX1 =  FanGraphXStart + ((PixelsPerMinFan * (FanSpeedPWNDecreaseByMinutes + FanSpeedPWNDelayDecreaseByMinutes)) )  ;
  int midPWM  =  FanSpeedPWMStart - FanSpeedPWMAutoDecrease / 2;
  int midPWMlineY = YforAFan(midPWM) ;
  int yTemp = 0;
  int xTemp = 0;

  myGLCD.setFont(SmallFont);

  int xlable = FanGraphXStart - myGLCD.getFontXsize() * 3 - 2;

  //block out area

  myGLCD.setFont(SmallFont);
  myGLCD.setColor(BLACK);

  myGLCD.fillRect(xlable, FanGraphTop  , FanXEnd , FanGraphBottom) ;
  myGLCD.setBackColor(BLACK);
  myGLCD.setColor(fancolor);
  myGLCD.drawRect(FanGraphXStart , FanGraphTop  , FanXEnd , FanGraphBottom );



  //draw vertical time grid lines and labels
  myGLCD.setColor(100, 100, 100);

  for (int t = 3; t < MySetPoints[EndingSetPoint].Minutes; t = t + 3) {
    int q = PixelsPerMinFan * t + FanGraphXStart;
    myGLCD.drawLine( q, FanGraphTop, q, FanGraphBottom );
    if (t < 10) {
      myGLCD.printNumI(t , q - myGLCD.getFontXsize() , FanGraphBottom - myGLCD.getFontYsize() - 1  );
      myGLCD.print ("m",  q, FanGraphBottom  - myGLCD.getFontYsize() - 1);
    } else
    {
      myGLCD.printNumI(t , q - (myGLCD.getFontXsize() * 1.33) , FanGraphBottom - myGLCD.getFontYsize() - 1   );
      myGLCD.print ("m",  q + (myGLCD.getFontXsize() * 1.33), FanGraphBottom  - myGLCD.getFontYsize() - 1);
    }
  }

  //draw horizontal pwm grid lines and labels
  myGLCD.setColor(100, 100, 100);
  int xgridpwm = 0;
  for (xgridpwm =FanGraphMinPWM; xgridpwm < FanGraphMaxPWM ; xgridpwm = xgridpwm + FanGraphHorGridSpacingPWM) {
    int Y = YforAFan(xgridpwm);  
    if (Y != FanGraphBottom and xgridpwm !=FanGraphTop) //Do not override top and bottom
    { 
        myGLCD.drawLine(FanGraphXStart, Y, FanXEnd, Y );
    }
    myGLCD.printNumI(xgridpwm    , xlable , Y - (myGLCD.getFontYsize() / 2)  );
  }

  if (xgridpwm < FanGraphMaxPWM) {
      if (FanGraphMaxPWM - xgridpwm >= 10) {
        myGLCD.printNumI(xgridpwm  , xlable , FanGraphTop - (myGLCD.getFontYsize() / 2)  );
      }
    
  }

  //draw fan profile
  myGLCD.setColor(WHITE);
  myGLCD.drawLine(FanGraphXStart, upperlineY, upperlineX2, upperlineY );
  myGLCD.drawLine(upperlineX2, upperlineY, lowerlineX1, lowerlineY );
  myGLCD.drawLine(lowerlineX1, lowerlineY , FanXEnd, lowerlineY  );

 //label fan profile start and end lables
  

  //draw probile settings lables
  //A
  myGLCD.print("A"  , upperlineX2 + 2                   , upperlineY + 3  );
  myGLCD.printNumI(FanSpeedPWNDelayDecreaseByMinutes , upperlineX2 + myGLCD.getFontXsize() + 3 , upperlineY + 3  );
  myGLCD.print("m"  , upperlineX2 + (myGLCD.getFontXsize() * 2) + 2 , upperlineY + 3  );

  myGLCD.printNumI(FanSpeedPWMStart  , upperlineX2 + 2 , upperlineY - (myGLCD.getFontYsize()) - 1  );
 
  //B
  int xtemp =  FanGraphXStart + (PixelsPerMinFan * (FanSpeedPWNDelayDecreaseByMinutes + FanSpeedPWNDecreaseByMinutes / 2 )) ;
  myGLCD.print("B"                          , xtemp     , midPWMlineY - myGLCD.getFontYsize() / 2  );
  myGLCD.printNumI(FanSpeedPWMAutoDecrease  , xtemp + myGLCD.getFontXsize()   , midPWMlineY - myGLCD.getFontYsize() / 2  );

  //C
  
  myGLCD.print("C"        , lowerlineX1     , lowerlineY + 3  );
  myGLCD.printNumI(FanSpeedPWNDelayDecreaseByMinutes + FanSpeedPWNDecreaseByMinutes,  lowerlineX1 + myGLCD.getFontXsize()  , lowerlineY + 3  );
  myGLCD.print("m"        ,lowerlineX1 + (myGLCD.getFontXsize() * 3) , lowerlineY + 3  );

  myGLCD.printNumI(lowPWM , lowerlineX1     , lowerlineY - (myGLCD.getFontYsize()) - 1  );
  

  Serial.println("here we go");



  ReDrawFANromArray(ORANGE);
  ////UpdateFanPWMValuesDisplay();
}

void UpdateFanPWMValuesDisplay() {
  myGLCD.setFont(BigFont);

  int rowheight = 20;
  int row = myHorFanButtonControl.rowstart - myGLCD.getFontXsize() - 1 ;
  int col = myHorFanButtonControl.colstart + myHorFanButtonControl.W;
  int col1 = col + myGLCD.getFontXsize() * 4 ;
  int col2 = col1 + myGLCD.getFontXsize() * 3  ;


  myGLCD.setColor(AQUA) ;

  myGLCD.printNumI(FanSpeedPWM, col , row);
  myGLCD.print("Dev", col1 , row, 3);
  myGLCD.printNumI(FanDeviation, col2 , row);


}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void StartLinebyTemp(int temp, int lineID, uint16_t color) {

  int Y = 0;
  if (temp > 0) {

    Y = YforATemp(temp);
  }
  else {
    Y = 480;
  }
  StartLinebyXAndY(0, Y, lineID, color);
}

void StartLinebyXAndY(int X, int Y, int lineID, uint16_t color) {

  LastXforLineID[lineID] = X;
  LastYforLineID[lineID] = Y;

  LineColorforLineID[lineID] = color;

  if (lineID == ROLLAVGLINEID) {
    for (int X = 0; X < LastPixelArrayCount; X++) {
      myLastTempGraphPixels[X].y = -1;
    }
    myLastTempGraphPixels[0].x = X;
    myLastTempGraphPixels[0].y = Y;
    myLastTempGraphPixelsP = 0;
  }

  if (lineID == FANSPEEDLINEID) {
    for (int X = 0; X < LastPixelArrayCount; X++) {
      myLastFanGraphPixels[X].y = -1;
    }
    myLastFanGraphPixels[0].x = X;
    myLastFanGraphPixels[0].y = Y;
    myLastFanGraphPixelsP = 0;
  }



}

void AddLinebyXY(uint16_t &newX, uint16_t &newY,  int lineID) {
  myGLCD.setColor(LineColorforLineID[lineID]);//LineColorforLineID[lineID]);
  //x,y,x,y
  myGLCD.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY );
  LastXforLineID[lineID] = newX;
  LastYforLineID[lineID] = newY;
  if (lineID == ROLLAVGLINEID) {
    if (SkipTempCount >= SkipTempLimit)
    {
          myLastTempGraphPixelsP++;
          Serial.print("save p"); Serial.print(myLastTempGraphPixelsP); Serial.print(" x"); Serial.print(newX); Serial.print(" y"); Serial.println(newY);

      myLastTempGraphPixels[ myLastTempGraphPixelsP].x = newX;
      myLastTempGraphPixels[ myLastTempGraphPixelsP].y = newY;
    }
    else
    {
      SkipTempCount++;
    }


  }
  if (lineID == FANSPEEDLINEID) {

    if (SkipFanCount >= SkipFanLimit)
    {
      SkipFanCount = 0;
      myLastFanGraphPixelsP++;
      
      myLastFanGraphPixels[ myLastFanGraphPixelsP].x = newX;
      myLastFanGraphPixels[ myLastFanGraphPixelsP].y = newY;
    }
    else
    {
      SkipFanCount++;
    }

  }
}

void AddLinebyTimeAndTemp(double timemins, int temp, int lineID) {
  uint16_t newY = (uint16_t)YforATemp(temp);
  uint16_t newX = (uint16_t)(PixelsPerMin * timemins);

  AddLinebyXY(newX, newY,  lineID);
}

void AddLinebyTimeAndFan(double timemins) {
  //Serial.print("FanSpeedPWM:");Serial.print(FanSpeedPWM); Serial.print(" Time:");Serial.println(timemins);
  uint16_t newY = (uint16_t)YforAFan(FanSpeedPWM);
  uint16_t newX = (uint16_t)(FanGraphXStart + (PixelsPerMinFan * timemins));

  AddLinebyXY(newX, newY,  FANSPEEDLINEID);
}

void DrawSetPoint(int setpoint, uint16_t color) {
  int Y = YforATemp(MySetPoints[setpoint].Temperature);
  int X = MySetPoints[setpoint].Minutes * PixelsPerMin;


  myGLCD.setColor(BLACK);
  myGLCD.fillRect(X - 35, Y - 8, X + 10, Y - 12 - myGLCD.getFontYsize());

  myGLCD.setColor(WHITE);
  myGLCD.fillCircle(X, Y, 7);
  myGLCD.printNumI(setpoint, X - 30, Y - 20);
  myGLCD.printNumI(MySetPoints[setpoint].Temperature, X - 15, Y - 20);
}

void DrawMovedSetPoint(int setpoint) {
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
  myGLCD.print("N:", col1, Row1Y); myGLCD.printNumI(MySetPoints[setpoint].TemperatureNew, col2, Row1Y );
  int delta = MySetPoints[setpoint].TemperatureNew - MySetPoints[setpoint].Temperature;
  myGLCD.print("D:", col1, Row2Y);
  if (delta > 0) {
    myGLCD.print("+", col2, Row2Y);
  }
  if (delta < 0) {
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
  LastXforLineID[ROLLAVGLINEID] = myLastTempGraphPixels[0].x;
  LastYforLineID[ROLLAVGLINEID] = myLastTempGraphPixels[0].y;
  //Serial.print ("ReDrawROLLAVGLINEFromArray ");Serial.println(sizeof(myLastTempGraphPixels));
  myGLCD.setColor(color);
  for (int X = 1; X < LastPixelArrayCount; X++) {
    if (myLastTempGraphPixels[X].y > -1 ) {
      Serial.print("draw p"); Serial.print(X); Serial.print(" x"); Serial.print(myLastTempGraphPixels[X].x); Serial.print(" y"); Serial.println(myLastTempGraphPixels[X].y);
      
       myGLCD.drawLine(LastXforLineID[1], LastYforLineID[1] , myLastTempGraphPixels[X].x, myLastTempGraphPixels[X].y);
      //BoldLine(LastXforLineID[1], LastYforLineID[1] , X, myLastGraphYPixels[X], color);
      LastXforLineID[ROLLAVGLINEID] = myLastTempGraphPixels[X].x;
      LastYforLineID[ROLLAVGLINEID] = myLastTempGraphPixels[X].y;
      myLastTempGraphPixelsP++;
    }
    else
    { break;
    }

  }
}
void ReDrawFANromArray(int color) {
  LastXforLineID[FANSPEEDLINEID] = myLastFanGraphPixels[0].x;
  LastYforLineID[FANSPEEDLINEID] = myLastFanGraphPixels[0].y;

  for (int X = 1; X < LastPixelArrayCount; X++) {
    if (myLastFanGraphPixels[X].y > -1 ) {
      //Serial.print("draw p"); Serial.print(X); Serial.print(" x"); Serial.print(myLastFanGraphPixels[X].x); Serial.print(" y"); Serial.println(myLastFanGraphPixels[X].y);
      myGLCD.drawLine(LastXforLineID[FANSPEEDLINEID], LastYforLineID[FANSPEEDLINEID] , myLastFanGraphPixels[X].x, myLastFanGraphPixels[X].y);
      LastXforLineID[FANSPEEDLINEID] = myLastFanGraphPixels[X].x;
      LastYforLineID[FANSPEEDLINEID] = myLastFanGraphPixels[X].y;

    }
    else {
      Serial.println ("Exit Draw Fan at:"); Serial.println(myLastFanGraphPixelsP);
      break;
    }

  }






  return;
  return;
  myGLCD.setColor(color);
  return;
  for (int X = 1; X < LastPixelArrayCount; X++) {
    if (myLastFanGraphPixels[X].y > -1 ) {
      //Serial.println ("DrawRealTime ");Serial.println(" color:");Serial.println(color);
      myGLCD.drawLine(LastXforLineID[FANSPEEDLINEID], LastYforLineID[FANSPEEDLINEID] , myLastFanGraphPixels[X].x, myLastFanGraphPixels[X].y);
      LastXforLineID[1] = myLastTempGraphPixels[X].x;
      LastYforLineID[1] = myLastTempGraphPixels[X].y;
      myLastFanGraphPixelsP++;
    }
    else {
      break;
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
int  YforATemp(double temp) {
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
void myGLCD_fillRect(rect& Rect) {
  myGLCD.fillRect(Rect.x, Rect.y, Rect.xmax, Rect.ymax);
}
void myGLCD_drawRect(rect& Rect) {
  myGLCD.drawRect(Rect.x, Rect.y, Rect.xmax, Rect.ymax);
}

void myGLCD_printNumF(double Number, int col , int row,  int Len, int Dec) {

  if (Len == 5) {
    //Serial.println("AC5");

    //Serial.println(Number);
    dtostrf(Number, Len, Dec, s6);
    //Serial.println (s6 );
    myGLCD.print(s6, col, row);

  }
  else if (Len == 6)
  {
    //Serial.println("AC6");

    //Serial.println(Number);
    dtostrf(Number, Len, Dec, s7);
    //Serial.println (s7 );
    myGLCD.print(s7, col, row);

  }
}

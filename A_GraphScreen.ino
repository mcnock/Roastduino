
byte timeSplitLow = 6;
float PixelsPerMinL;
float PixelsPerMinM;
//long PixelsPerMin;
int pixelSplitLow = 150;

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
    if (MySetPoints[EndingSetPoint].Minutes < 10 or MySetPoints[EndingSetPoint].Minutes > 20) {
      MySetPoints[EndingSetPoint].Minutes = 16;
    }
    //Serial.println ("XX");
    MySetPoints[0].Minutes = 0;
    MySetPoints[1].Minutes = 4;

    if (MySetPoints[EndingSetPoint].Minutes >= 14) {
      //3 minute appart
      MySetPoints[2].Minutes = 7;
      MySetPoints[3].Minutes = 10;
      MySetPoints[4].Minutes = 13;
      TimeScreenLeft = MySetPoints[EndingSetPoint].Minutes + 2;

    } else if (MySetPoints[EndingSetPoint].Minutes >= 11) {
      //2 minute appart
      MySetPoints[2].Minutes = 6;
      MySetPoints[3].Minutes = 8;
      MySetPoints[4].Minutes = 10;
      TimeScreenLeft = MySetPoints[EndingSetPoint].Minutes + 2;

    } else {
      //1 or 2 minute appart
      MySetPoints[2].Minutes = 5;
      MySetPoints[3].Minutes = 7;
      MySetPoints[4].Minutes = 9;
      TimeScreenLeft = MySetPoints[EndingSetPoint].Minutes + 1;
    }


    for (int xSetPoint = 0; xSetPoint < SetPointCount; xSetPoint++) {
      if (xSetPoint == 0) {
        //EEPROM.get(SETPOINTTEMP_EP[X],MySetPoints[X].Temperature);

        MySetPoints[0].Temperature = 200;
        MySetPoints[0].SpanMinutes = 0;
        MySetPoints[0].Minutes = 0;
        MyMinuteTemperature[0] = MySetPoints[0].Temperature;
      } else {
        EEPROM.get(SETPOINTTEMP_EP[xSetPoint], MySetPoints[xSetPoint].Temperature);
        //Serial.println(MySetPoints[xSetPoint].Temperature);
        MySetPoints[xSetPoint].SpanMinutes = MySetPoints[xSetPoint].Minutes - MySetPoints[xSetPoint - 1].Minutes;
        accumulatedbySetPoint = accumulatedbySetPoint + MySetPoints[xSetPoint].SpanMinutes;
        MySetPoints[xSetPoint].Minutes = accumulatedbySetPoint;

        //now calc the minute setpoint for the span leading to this setpoint
        double TempPerMinuteinSpan = ((double)(MySetPoints[xSetPoint].Temperature - MySetPoints[xSetPoint - 1].Temperature)) / MySetPoints[xSetPoint].SpanMinutes;
        for (int xSpanMinute = 1; xSpanMinute <= MySetPoints[xSetPoint].SpanMinutes; xSpanMinute++) {
          accumulatedMinutes = accumulatedMinutes + 1;
          MyMinuteTemperature[accumulatedMinutes] = MySetPoints[xSetPoint - 1].Temperature + (TempPerMinuteinSpan * xSpanMinute);
        }
      }
    }




    //perform some necesary calculations to size our scales to the display
    //yscale is done in 3 ranges
    //TimeScreenLeft = MySetPoints[EndingSetPoint].Minutes + 2;
    TempYMax = 800;
    TempSplitHigh = 440;
    TempSplitHigh = MySetPoints[EndingSetPoint].Temperature;
    PixelYSplit2 = 390;  //180;
    //  TempSplitLow = (MySetPoints[2].Temperature - Gain) ;
    TempSplitLow = (MySetPoints[1].Temperature);

    PixelYSplit = 150;  //90;

    TempPerPixL = TempSplitLow / PixelYSplit;
    TempPerPixM = (TempSplitHigh - TempSplitLow) / (PixelYSplit2 - PixelYSplit);
    TempPerPixH = (TempYMax - TempSplitHigh) / (480 - PixelYSplit2);
    //PixelsPerMin =  (int)(800 / TimeScreenLeft);

    PixelsPerMinL = (float)(pixelSplitLow / (float)timeSplitLow);

  }


  if (State == STATECOOLING) {
    TimeScreenLeft = MySetPoints[EndingSetPoint].Minutes + 4;
  } else {
    TimeScreenLeft = MySetPoints[EndingSetPoint].Minutes + 1;
  }
  PixelsPerMinM = (float)(800 - pixelSplitLow) / float((TimeScreenLeft - timeSplitLow));
  
  myGLCD.setColor(BLACK);
  myGLCD.setBackColor(BLACK);
  myGLCD.fillRect(0, 0, 800, 480);
  myGLCD.clrScr();
  myGLCD.setColor(GRAY);

  //draw our x time axis
  myGLCD.drawLine(XforATime(3), 0, XforATime(3), myGLCD.getDisplayYSize() - 30);
  myGLCD.print(F("3min"), XforATime(3) - 30, 460);
  myGLCD.drawLine(XforATime(6), 0, XforATime(6), myGLCD.getDisplayYSize() - 30);
  myGLCD.print(F("6min"), XforATime(6) - 30, 460);
  myGLCD.drawLine(XforATime(9), 0, XforATime(9), myGLCD.getDisplayYSize() - 30);
  myGLCD.print(F("9min"), XforATime(9) - 30, 460);
  myGLCD.drawLine(XforATime(12), 0, XforATime(12), myGLCD.getDisplayYSize() - 30);
  myGLCD.print(F("12min"), XforATime(12) - 30, 460);
  if (TimeScreenLeft > 15) {
    myGLCD.drawLine(XforATime(15), 0, XforATime(15), myGLCD.getDisplayYSize() - 30);
    myGLCD.print(F("15min"), XforATime(15) - 30, 460);
  }
  if (TimeScreenLeft > 18) {
    myGLCD.drawLine(XforATime(18), 0, XforATime(18), myGLCD.getDisplayYSize() - 30);
    myGLCD.print(F("18min"), XforATime(18) - 30, 460);
  }

  //draw y scale for 3 ranges
  int yaxislable = 30;
  //  int lastt = 0;
  //Low range A

  for (int t = 50; t < (TempSplitLow - 50); t = t + 50) {
    //Serial.println(t);
    int y = YforATemp(t);
    myGLCD.setColor(LIGHTGRAY);
    myGLCD.drawLine(yaxislable, y, myGLCD.getDisplayXSize(), y);
    myGLCD.setColor(GRAY);

    myGLCD.printNumI(t, 2, y - 5);
    //lastt = t;
  }


  HorScaleLineYCount = -1;  //we only need to capture med and high range

  //med range
  //  myGLCD.setColor(WHITE);
  for (int t = TempSplitLow; t < (TempSplitHigh + 5); t = t + 5) {
    HorScaleLineYCount++;
    int y = YforATemp(t);
    HorScaleLineY[HorScaleLineYCount] = y;

    myGLCD.setColor(LIGHTGRAY);
    myGLCD.drawLine(yaxislable, y, myGLCD.getDisplayXSize(), y);

    myGLCD.setColor(GRAY);
    myGLCD.printNumI(t, 2, y - 5);
    //lastt = t;
  }



  //high range
  int tstart = 500;
  if (TempSplitHigh < 380) {
    tstart = 400;
  } else if (TempSplitHigh < 420) {
    tstart = 450;
  }

  for (int t = tstart; t < TempYMax; t = t + 100) {
    HorScaleLineYCount++;
    int y = YforATemp(t);
    HorScaleLineY[HorScaleLineYCount] = y;

    myGLCD.setColor(LIGHTGRAY);
    myGLCD.drawLine(yaxislable, y, myGLCD.getDisplayXSize(), y);

    myGLCD.setColor(GRAY);
    myGLCD.printNumI(t, 2, y - 5);
  }



  //draw the profile by and setpoint
  StartLinebyTemp(MySetPoints[0].Temperature - 50, PROFILELINEID);

  for (unsigned int Minutes = 1; Minutes <= MySetPoints[EndingSetPoint].Minutes; Minutes++) {
    AddLinebyTimeAndTemp(Minutes, MyMinuteTemperature[Minutes], PROFILELINEID);
  }

  for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++) {
    DrawSetPoint(xSetPoint, LineColorforLineID[PROFILELINEID]);
  }

  DrawHorControlMenu();

  DrawHorFanMenu();

  //if sp is  selected, assume user is adjusting setpoint redraw the menu
  if (spSelected >= 0) {
    DrawVMenu(VmenuSetPointSelect, -1);
  } else {
    DrawVMenu(VerticalMenuShowing, myButtonVertMenus[VerticalMenuShowing].inputbutton);
  }

  graphFanProfile();

  UpdateTempDisplayArea(All);
  UpdateStateDisplayArea(All);
  setpointschanged = false;

  UpdateProgessDisplayArea(All);

  DrawLineFromHistoryArray(ROLLAVGLINEID);
}

void graphFanProfile() {

  uint16_t fancolor = GRAY;
  uint16_t fanback = DARKBLUE;

  myGLCD.setColor(fancolor);

  int FanGraphXWidth;
  //SpDebug("VerticalmenuShowing:" + String(VerticalMenuShowing));
  //if (VerticalMenuShowing == VmenuEmpty)
  //{
  //SpDebug("A");
  //   FanGraphXWidth = 800 - FanGraphXStart - 5 ;
  //}

  //else
  //{

  FanGraphXWidth = 800 - FanGraphXStart - 5;
  //}

  PixelsPerMinFan = (float)FanGraphXWidth / (float)MySetPoints[EndingSetPoint].Minutes;

  myGLCD.setFont(SmallFont);


  //block out area
  int FanXEnd = FanGraphXStart + ((PixelsPerMinFan * MySetPoints[EndingSetPoint].Minutes));  //calc the ending of graph a max time
  int FanGraphTop = FanGraphBottom - FanGraphHeight;
  int xlable = FanGraphXStart - myGLCD.getFontXsize() * 3 - 2;

  myGLCD.setFont(SmallFont);
  myGLCD.setColor(fanback);
  myGLCD.fillRect(xlable, FanGraphTop, FanXEnd + 8, FanGraphBottom);
  myGLCD.setBackColor(fanback);
  myGLCD.setColor(fancolor);
  myGLCD.drawRect(FanGraphXStart, FanGraphTop, FanXEnd, FanGraphBottom);

  //draw vertical time grid lines and labels
  //myGLCD.setColor(GREY);
  for (unsigned int t = 3; t < MySetPoints[EndingSetPoint].Minutes; t = t + 3) {
    int q = PixelsPerMinFan * t + FanGraphXStart;
    myGLCD.setColor(LIGHTGRAY);
    myGLCD.drawLine(q, FanGraphTop, q, FanGraphBottom);
    myGLCD.setColor(GRAY);
    if (t < 10) {
      myGLCD.printNumI(t, q - myGLCD.getFontXsize(), FanGraphBottom - myGLCD.getFontYsize() - 1);
      myGLCD.print(F("m"), q, FanGraphBottom - myGLCD.getFontYsize() - 1);
    } else {
      myGLCD.printNumI(t, q - (myGLCD.getFontXsize() * 1.33), FanGraphBottom - myGLCD.getFontYsize() - 1);
      myGLCD.print(F("m"), q + (myGLCD.getFontXsize() * 1.33), FanGraphBottom - myGLCD.getFontYsize() - 1);
    }
  }


  //draw horizontal pwm grid lines and labels
  myGLCD.setColor(LIGHTGRAY);
  int xgridpwm = 0;
  FanGraphMaxPWM = round5(FanSetPoints[0].PWM + 20);
  for (xgridpwm = FanGraphMinPWM; xgridpwm < FanGraphMaxPWM; xgridpwm = xgridpwm + FanGraphHorGridSpacingPWM) {
    int Y = YforAFan(xgridpwm);
    if (Y != FanGraphBottom and xgridpwm != FanGraphTop)  //Do not override top and bottom
    {
      myGLCD.setColor(LIGHTGRAY);
      myGLCD.drawLine(FanGraphXStart, Y, FanXEnd, Y);
    }
    myGLCD.setColor(GRAY);
    myGLCD.printNumI(xgridpwm, xlable, Y - (myGLCD.getFontYsize() / 2));
  }

  if (xgridpwm < FanGraphMaxPWM) {
    if (FanGraphMaxPWM - xgridpwm >= 10) {
      myGLCD.printNumI(xgridpwm, xlable, FanGraphTop - (myGLCD.getFontYsize() / 2));
    }
  }


  //draw fan profile
  myGLCD.setColor(GRAY);

  for (int i = 1; i < 4; i++) {
    //SpDebug("drawFanProfile x:" + String(XforAFanMin(FanSetPoints[i-1].Minutes)) + " y:" + String( YforAFan(FanSetPoints[i-1].PWM)) + " x2:" + String( XforAFanMin(FanSetPoints[i].Minutes)) + " y2:" + String(YforAFan(FanSetPoints[i].PWM)));
    myGLCD.drawLine(XforAFanMin(FanSetPoints[i - 1].Minutes), YforAFan(FanSetPoints[i - 1].PWM), XforAFanMin(FanSetPoints[i].Minutes), YforAFan(FanSetPoints[i].PWM));

    //SpDebug("drawFanLabel :" + String(FanSetPoints[i].PWM) + " x:" + String(XforAFanMin(FanSetPoints[i].Minutes)) + " y:" +  String(YforAFan(FanSetPoints[i].PWM - myGLCD.getFontYsize())));
  }
  myGLCD.setColor(GRAY);

  myGLCD.printNumI(FanSetPoints[0].PWM, XforAFanMin(FanSetPoints[0].Minutes) + 2, YforAFan(FanSetPoints[0].PWM) - myGLCD.getFontYsize());
  myGLCD.printNumI(FanSetPoints[1].PWM, XforAFanMin(FanSetPoints[1].Minutes), YforAFan(FanSetPoints[1].PWM) - myGLCD.getFontYsize());
  myGLCD.printNumI(FanSetPoints[1].Minutes, XforAFanMin(FanSetPoints[1].Minutes), YforAFan(FanSetPoints[1].PWM) + 3);
  myGLCD.printNumI(FanSetPoints[2].PWM, XforAFanMin(FanSetPoints[2].Minutes), YforAFan(FanSetPoints[2].PWM) - myGLCD.getFontYsize());
  myGLCD.printNumI(FanSetPoints[2].Minutes, XforAFanMin(FanSetPoints[2].Minutes), YforAFan(FanSetPoints[2].PWM) + 3);
  myGLCD.printNumI(FanSetPoints[3].PWM, XforAFanMin(FanSetPoints[3].Minutes) - (myGLCD.getFontXsize() * 3) - 3, YforAFan(FanSetPoints[3].PWM) - myGLCD.getFontYsize());


  DrawLineFromHistoryArray(FANSPEEDLINEID);
  ////UpdateFanPWMValuesDisplay();
}

void UpdateProgessDisplayArea(boolean bValuesOnly) {
  //this is top row
  myGLCD.setBackColor(BLACK);
  int rowstart = 70;
  int row = rowstart;
  int col = 40;
  myGLCD.setFont(BigFont);
  int rowheight = myGLCD.getFontYsize() * 1.1;
  int col2 = col + (5 * myGLCD.getFontXsize());
  myGLCD.setFont(SmallFont);
  int rowheight2 = myGLCD.getFontYsize() * 1.5;
  int col4 = col + (9 * myGLCD.getFontXsize());
  myGLCD.setColor(PALEYELLOW);
  myGLCD.setFont(BigFont);
  bool printlables = false;
  if (bValuesOnly == false) {
    myGLCD.print(F("Time:"), col, row);
    myGLCD_printNumF(MySetPoints[EndingSetPoint].Minutes - RoastMinutes, col2, row, 5, 2);
    row = row + rowheight;
    myGLCD.print(F("Temp:"), col, row);
    myGLCD.printNumI(TBeanAvgRoll.mean(), col2, row, 5);
    row = row + rowheight;
    myGLCD.print(F("Duty:"), col, row);
    myGLCD_printNumF(DutyTemp, col2, row, 5, 2);
    row = row + rowheight;
    myGLCD.print(F("Err :"), col, row);
    myGLCD.printNumI(ErrTemp, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("AccT:"), col, row);
    myGLCD.printNumI(RoastAcumHeat, col2, row, 5, ' ');
    row = row + rowheight2;
    myGLCD.setFont(SmallFont);
    myGLCD.print(F("Gain     :"), col, row);
    myGLCD.printNumI(GainTemp, col4, row, 6, ' ');
    row = row + rowheight2;
    myGLCD.print(F("Int      :"), col, row);
    myGLCD_printNumF(IntegralTemp, col4, row, 6, 2);
    row = row + rowheight2;
    myGLCD.print(F("loops/sec:"), col, row);
    myGLCD.printNumI(LoopsPerSecond, col4, row, 6, ' ');
    row = row + rowheight2;
    myGLCD.print(F("Skiptemps:"), col, row);
    int R = TempReadingskipped[0] + TempReadingskipped[1] + TempReadingskipped[2];
    myGLCD.printNumI(R, col4, row, 6, ' ');
    row = row + rowheight2;
    myGLCD.print(F("TooHotTem:"), col, row);
    myGLCD.printNumI(TEMPCOILTOOHOT, col4, row, 6, ' ');
    row = row + rowheight2;
    myGLCD.print(F("CooldwnT :"), col, row);
    myGLCD.printNumI(TEMPCOOLINGDONE, col4, row, 6, ' ');
  } else {
    //myGLCD.print(F("Time:"), col , row);
    myGLCD_printNumF(MySetPoints[EndingSetPoint].Minutes - RoastMinutes, col2, row, 5, 2);
    row = row + rowheight;
    //myGLCD.print(F("Temp:"), col , row);
    myGLCD.printNumI(TBeanAvgRoll.mean(), col2, row, 5);
    row = row + rowheight;
    //myGLCD.print(F("Duty:"), col, row);
    myGLCD_printNumF(DutyTemp, col2, row, 5, 2);
    row = row + rowheight;
    //myGLCD.print(F("Err :"), col , row);
    myGLCD.printNumI(ErrTemp, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("AccT:"), col  , row);
    myGLCD.printNumI(RoastAcumHeat, col2, row, 5, ' ');
    row = row + rowheight2;
    myGLCD.setFont(SmallFont);
    //myGLCD.print(F("Gain     :"), col , row);
    myGLCD.printNumI(GainTemp, col4, row, 6, ' ');
    row = row + rowheight2;
    //myGLCD.print(F("Int      :"), col, row);
    myGLCD_printNumF(IntegralTemp, col4, row, 6, 2);
    row = row + rowheight2;
    //myGLCD.print(F("loops/sec:"), col, row);
    myGLCD.printNumI(LoopsPerSecond, col4, row, 6, ' ');
    row = row + rowheight2;
    //myGLCD.print(F("Skiptemps:"), col, row);
    int R = TempReadingskipped[0] + TempReadingskipped[1] + TempReadingskipped[2];
    //myGLCD.printNumI(R, col4 , row, 6, ' ');
    row = row + rowheight2;
    //myGLCD.print(F("TooHotTem:"), col, row);
    myGLCD.printNumI(TEMPCOILTOOHOT, col4, row, 6, ' ');
    row = row + rowheight2;
    //myGLCD.print(F("CooldwnT :"), col, row);
    myGLCD.printNumI(TEMPCOOLINGDONE, col4, row, 6, ' ');
  }
}

void UpdateStateDisplayArea(boolean OnlyChanges) {

  if (lastStateUpdated != State || OnlyChanges == false) {
    lastStateUpdated = State;
    myGLCD.setFont(Grotesk24x48);

    int row = 50 - myGLCD.getFontYsize();
    int col = 40;
    myGLCD.setColor(StateColor[State]);
    myGLCD.print(StateName[State], col, row);

  } else {
    //Serial.println("no updated becaue states is same as last update");
  }

  if (newerrmsg == true || OnlyChanges == false) {
    myGLCD.setFont(BigFont);
    int row = 65 - myGLCD.getFontYsize();
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

void UpdateTempDisplayArea(boolean bValuesOnly) {

  myGLCD.setFont(SmallFont);
  int rowheight = myGLCD.getFontYsize() * 1.5;
  int row = 70;
  int col;
  col = 210;
 ;
  int col2 = col + 65;   //number
  int col5 = col2 + 68;  //lable

  myGLCD.setBackColor(BLACK);
  myGLCD.setColor(PALEYELLOW);
  if (bValuesOnly == false) {
    //myGLCD.setColor(BLACK);
    //myGLCD.fillRect(col - 5, row - 5 + rowheight, end, row + rowheight * 6);
    //myGLCD.setColor(WHITE);
    //row = row + rowheight;
    myGLCD.print(F("Temp1"), col, row);
    { myGLCD.print(F(" F"), col5, row); }
    myGLCD.printNumI(TBean1, col2, row, 4, ' ');
    row = row + rowheight;
    myGLCD.print(F("Temp2"), col, row);
    { myGLCD.print(F(" F"), col5, row); }
    myGLCD.printNumI(TBean2, col2, row, 4, ' ');
    row = row + rowheight;
    myGLCD.print(F("Coil "), col, row);
    { myGLCD.print(F(" F"), col5, row); }
    myGLCD.printNumI(TCoil, col2, row, 4, ' ');
    row = row + rowheight;
    myGLCD.print(F("CAvg "), col, row);
    { myGLCD.print(F(" F"), col5, row); }
    myGLCD.printNumI(TCoilRoll.mean(), col2, row, 4, ' ');
    row = row + rowheight;
    myGLCD.print(F("IEr :"), col, row);
    myGLCD.printNumI(ErrITemp, col2, row, 5, ' ');


  } else {  //this is copy of above with myLCD commented out

    //myGLCD.print("Temp1",col , row);   myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TBean1, col2, row, 4, ' ');
    row = row + rowheight;
    //myGLCD.print("Temp2", col, row);        myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TBean2, col2, row, 4, ' ');
    row = row + rowheight;
    //          myGLCD.print("Coil:", col, row); myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TCoil, col2, row, 4, ' ');
    row = row + rowheight;
    //          myGLCD.print("Cvg:", col, row); myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TCoilRoll.mean(), col2, row, 4, ' ');
    row = row + rowheight;
    //myGLCD.print(F("IEr :"), col, row);
    myGLCD.printNumI(ErrITemp, col2, row, 5, ' ');

  }
}

void UpdateFanPWMValuesDisplay() {

  myGLCD.setFont(BigFont);

  //int rowheight = 20;
  int row = myHorFanButtonControl.rowstart - myGLCD.getFontXsize() - 1;
  int col = myHorFanButtonControl.colstart + myHorFanButtonControl.W;
  int col1 = col + myGLCD.getFontXsize() * 4;
  int col2 = col1 + myGLCD.getFontXsize() * 3;

  myGLCD.setColor(AQUA);
  myGLCD.printNumI(FanSpeedPWM, col, row, 3, ' ');
  myGLCD.print(F("Dev"), col1, row, 3);
  myGLCD.printNumI(FanDeviation, col2, row, 3, ' ');
}

void StartLinebyTemp(int temp, int lineID) {

  uint16_t Y = 0;
  if (temp > 0) {

    Y = YforATemp(temp);
  } else {
    Y = 480;
  }
  StartLinebyXAndY(0, Y, lineID);
}

void StartLinebyXAndY(uint16_t X, uint16_t Y, int lineID) {

  LastforLineID[lineID].x = X;
  LastforLineID[lineID].y = Y;

  int HistoryID = GetHistoryIDfromLineID(lineID);
  if (HistoryID > -1) {
    //SpDebug("Reseting history for lineID:" + String(lineID));
    for (int i = 0; i < GraphHistory[HistoryID].ArraySize; i++) {
      GraphHistory[HistoryID].Pixels[i].y = -1;
    }
    GraphHistory[HistoryID].Pixels[0].x = X;
    GraphHistory[HistoryID].Pixels[0].y = Y;
    GraphHistory[HistoryID].PixelsP = 0;
    GraphHistory[HistoryID].SkipCount = 0;
  } else {
    //Serial.print(F("No history to reset starting lineID"));Serial.println(lineID);
  }
}

void AddLinebyXY(uint16_t &newX, uint16_t &newY, int lineID) {
  // Serial.print("Adding line by xy line id:");Serial.println(lineID);
  myGLCD.setColor(LineColorforLineID[lineID]);  //LineColorforLineID[lineID]);
  //x,y,x,y
  //if (lineID == ROLLMAXLINEID){
  //  Serial.print("Adding line by xy line id:");Serial.println(lineID);
  //SpDebug("Adding to lineid:\t" + String(lineID) + "\t from x:" + String(LastforLineID[lineID].x) + "\ty:" + String((int)LastforLineID[lineID].y) + "\t to x:" + String(newX) + "\t y:" + String(newY));

  myGLCD.drawLine((int)LastforLineID[lineID].x, (int)LastforLineID[lineID].y, (int)newX, (int)newY);
  if (LineBoldforLineID[lineID]) {
    BoldLine(LastforLineID[lineID].x, LastforLineID[lineID].y, newX, newY);
  }
  LastforLineID[lineID].x = newX;
  LastforLineID[lineID].y = newY;
  int HistoryID = GetHistoryIDfromLineID(lineID);
  if (HistoryID > -1) {
    if (GraphHistory[HistoryID].SkipCount >= GraphHistory[HistoryID].SkipLimit) {
      GraphHistory[HistoryID].SkipCount = 0;
      if (GraphHistory[HistoryID].PixelsP < (GraphHistory[HistoryID].ArraySize - 1)) {
        GraphHistory[HistoryID].PixelsP++;
        //SpDebug("Adding to lineid:\t" + String(lineID) + "\tx:" + String(newX) + "\ty:" + String(newY));
        GraphHistory[HistoryID].Pixels[GraphHistory[HistoryID].PixelsP].x = newX;
        GraphHistory[HistoryID].Pixels[GraphHistory[HistoryID].PixelsP].y = newY;
      } else {
        Serial.print(F("ERROR:LastTemp array size exceeded for LineID:"));
        Serial.println(lineID);
      }
    } else {
      //SpDebug("Skipping Adding to lineid:\t" + String(lineID) + " \tskipcount:" + String(GraphHistory[HistoryID].SkipCount));
      GraphHistory[HistoryID].SkipCount++;
    }
  } else {
    //Serial.print(F("Add to history  not available for lineID"));Serial.println(lineID);
  }
}

void AddLinebyTimeAndTemp(double timemins, int temp, int lineID) {
  uint16_t newY = YforATemp(temp);
  uint16_t newX = XforATime(timemins);

  AddLinebyXY(newX, newY, lineID);
}

void AddLinebyTimeAndFan(double timemins) {
  //Serial.print("FanSpeedPWM:");Serial.print(FanSpeedPWM);Serial.print(" Time:");Serial.println(timemins);
  uint16_t newY = (uint16_t)YforAFan(FanSpeedPWM);
  uint16_t newX = (uint16_t)(FanGraphXStart + (PixelsPerMinFan * timemins));

  AddLinebyXY(newX, newY, FANSPEEDLINEID);
}

void DrawLinebyTimeAndTemp(boolean log, double timemins1, int temp1, double timemins2, int temp2, int color) {
  if (log == true) {
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
  myGLCD.drawLine(XforATime(timemins1), (int)YforATemp(temp1), XforATime(timemins2), (int)YforATemp(temp2));
}

void DrawLineFromHistoryArray(int LineID) {
  int HistoryID = GetHistoryIDfromLineID(LineID);


  if (HistoryID > -1) {
    //SpDebug("Redrawing lineid:\t" + String(LineID));
    myGLCD.setColor(LineColorforLineID[LineID]);
    LastforLineID[LineID].x = GraphHistory[HistoryID].Pixels[0].x;
    LastforLineID[LineID].y = GraphHistory[HistoryID].Pixels[0].y;
    for (int i = 1; i < GraphHistory[HistoryID].ArraySize; i++) {
      if (GraphHistory[HistoryID].Pixels[i].y > -1) {
        //SpDebug("Redraw i:\t" + String(i) +
        //     " \tx:" + String(LastforLineID[LineID].x) +
        //     " \ty:" + String(LastforLineID[LineID].y) +
        //     " \txmax:" + String(GraphHistory[HistoryID].Pixels[i].x) +
        //     " \tymax:" + String(GraphHistory[HistoryID].Pixels[i].y)
        //     );
        myGLCD.drawLine((int)LastforLineID[LineID].x, (int)LastforLineID[LineID].y, (int)GraphHistory[HistoryID].Pixels[i].x, (int)GraphHistory[HistoryID].Pixels[i].y);
        if (LineBoldforLineID[LineID]) {
          BoldLine(LastforLineID[LineID].x, LastforLineID[LineID].y, GraphHistory[HistoryID].Pixels[i].x, GraphHistory[HistoryID].Pixels[i].y);
        }
        LastforLineID[LineID].x = GraphHistory[HistoryID].Pixels[i].x;
        LastforLineID[LineID].y = GraphHistory[HistoryID].Pixels[i].y;
      } else {
        //SpDebug("Exit ReDraw lineid:" + String(LineID) +  " at:" + String(i));
        break;
      }
    }
    return;
  } else {
    Serial.print(F("Draw from  history  not available for lineID"));
    Serial.println(LineID);
  }
}

void DrawSetPoint(int setpoint, uint16_t color) {
  int Y = YforATemp(MySetPoints[setpoint].Temperature);
  int X = XforATime(MySetPoints[setpoint].Minutes);


  //myGLCD.setColor(BLACK);
  // myGLCD.fillRect(X - 35, Y - 8, X + 10, Y - 12 - myGLCD.getFontYsize());

  myGLCD.setColor(YELLOW);
  myGLCD.fillCircle(X, Y, 5);
  myGLCD.setColor(WHITE);

  myGLCD.printNumI(setpoint, X + (myGLCD.getFontXsize() / 2), Y - (myGLCD.getFontYsize() * 2) - 5);
  myGLCD.print(F("#"), X - (myGLCD.getFontXsize() / 2), Y - (myGLCD.getFontYsize() * 2) - 5);

  myGLCD.printNumI(MySetPoints[setpoint].Temperature, X - (myGLCD.getFontXsize() * 1.5), Y - (myGLCD.getFontYsize()) - 5);
  myGLCD.printNumI(MySetPoints[setpoint].Minutes, X - (myGLCD.getFontXsize() / 2), Y + 2 + 5);
}

void DrawMovedSetPoint(int setpoint) {
  int spYnew = YforATemp(MySetPoints[setpoint].TemperatureNew);
  int spX = XforATime(MySetPoints[setpoint].Minutes);
  myGLCD.setColor(YELLOW);
  myGLCD.fillCircle(spX, spYnew, 7);
  myGLCD.setColor(WHITE);
  myGLCD.drawCircle(spX, spYnew, 7);

  int spYold = YforATemp(MySetPoints[setpoint].Temperature);
  myGLCD.setBackColor(BLACK);
  myGLCD.setFont(BigFont);

  int Row1Y = spYold - 15;
  int Row2Y = spYold + 5;
  //int Row3Y = spYold + 25;
  int col1 = spX + 8;
  int col2 = col1 + (myGLCD.getFontXsize() * 2);
  int col3 = col2 + (myGLCD.getFontXsize() * 1);
  //myGLCD.setColor(WHITE);
  //myGLCD.printNumI(MySetPoints[setpoint].Temperature, col2, Row1Y);
  myGLCD.setColor(YELLOW);
  myGLCD.print(F("N:"), col1, Row1Y);
  myGLCD.printNumI(MySetPoints[setpoint].TemperatureNew, col2, Row1Y);
  int delta = MySetPoints[setpoint].TemperatureNew - MySetPoints[setpoint].Temperature;
  myGLCD.print(F("D:"), col1, Row2Y);
  if (delta > 0) {
    myGLCD.print(F("+"), col2, Row2Y);
  }
  if (delta < 0) {
    myGLCD.print(F("-"), col2, Row2Y);
  }
  myGLCD.printNumI(abs(delta), col3, Row2Y);
}

void AddPointbyTimeAndTempAndLineID(double timemins, int temp, int lineID, int radius) {
  uint16_t newX = XforATime(timemins);
  uint16_t newY = YforATemp(temp);
  myGLCD.setColor(LineColorforLineID[lineID]);
  //myGLCD.setColor(0,150,0);
  //Serial.print('a');Serial.print(newX);Serial.print( "   ");;Serial.print(timemins);Serial.print( "   ");Serial.println(temp);
  myGLCD.fillCircle(newX, newY, radius);
  LastforLineID[lineID].x = newX;
  LastforLineID[lineID].y = newY;
}

void AddPointbyTimeandTemp(double timemins, int temp, int color, int radius) {
  uint16_t newX = (uint16_t)(XforATime(timemins));
  int newY = YforATemp(temp);
  myGLCD.setColor(color);
  myGLCD.fillCircle(newX, newY, radius);

  //Serial.println ("AddLineTandT line iD:");Serial.println (lineID);Serial.println(" time:");Serial.println(timemins);Serial.println("temp:");Serial.println(temp);Serial.println(" color:");Serial.println(LineColorforLineID[lineID]);
  //Serial.println ("newX:");Serial.println (newX);Serial.println ("pixelspermin:");Serial.println (PixelsPerMin);Serial.println(" newY:");Serial.println(newY);
  //myGLCD.drawLine(LastXforLineID[lineID], LastYforLineID[lineID], newX, newY, LineColorforLineID[lineID]);
}

void BoldLine(int x, int y, int newX, int newY) {
  myGLCD.drawLine(x, y + 1, newX, newY + 1);
  myGLCD.drawLine(x + 1, y + 1, newX + 1, newY + 1);
  myGLCD.drawLine(x + 1, y, newX + 1, newY);
}

void MoveLast3Point() {
  spSelected = 3;
  MoveAPoint(3);
  spSelected = 4;
  MoveAPoint(4);
  spSelected = 5;
  MoveAPoint(5);
  spSelected = 6;
}

void MoveAPoint(int SetPoint) {
  //Serial.print("MoveAPoint Setpoint:");Serial.print(SetPoint);Serial.print("moveamount:");Serial.println(moveamount);
  if (SetPoint > -1) {
    if (MySetPoints[SetPoint].TemperatureNew == 0) {
      MySetPoints[SetPoint].TemperatureNew = MySetPoints[SetPoint].Temperature;
    }
    //int lastnew = MySetPoints[SetPoint].TemperatureNew;
    MySetPoints[SetPoint].TemperatureNew = MySetPoints[SetPoint].TemperatureNew + moveamount;

    delay(100);
    DrawMovedSetPoint(SetPoint);
  }
}

void myGLCD_fillRect(rect &Rect) {
  myGLCD.fillRect(Rect.x, Rect.y, Rect.xmax, Rect.ymax);
}

void myGLCD_drawRect(rect &Rect) {
  myGLCD.drawRect(Rect.x, Rect.y, Rect.xmax, Rect.ymax);
}

void myGLCD_printNumF(double Number, int col, int row, int Len, int Dec) {

  if (Len == 5) {
    //Serial.println("AC5");

    //Serial.println(Number);
    dtostrf(Number, Len, Dec, s6);
    //Serial.println (s6 );
    myGLCD.print(s6, col, row);

  } else if (Len == 6) {
    //Serial.println("AC6");

    //Serial.println(Number);
    dtostrf(Number, Len, Dec, s7);
    //Serial.println (s7 );
    myGLCD.print(s7, col, row);
  }
}

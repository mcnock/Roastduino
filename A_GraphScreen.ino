double TempYMax = 600;
double TempSplitHigh = 460;
double TempSplitLow = 390;
double PixelYSplit2;
double PixelYSplit;
byte timeSplitLow = 6;
float PixelsPerMinL;
float PixelsPerMinM;
int pixelSplitLow = 150;
uint16_t fancolor = GRAY;
uint16_t fanback = DARKBLUE;

rect OpProgessDisplay = { 75, 0, 0, 0 };

rect OpDetailDisplay = { 610, 180, 0, 0 };

rect ConfigDisplay = { 450, 190, 0, 0 };


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
    MySetPoints[SetPointCount - 1].Minutes = m;
    //Serial.println ("Loading new setpoints:");
    if (MySetPoints[SetPointCount - 1].Minutes < 10 or MySetPoints[SetPointCount - 1].Minutes > 20) {
      MySetPoints[SetPointCount - 1].Minutes = 14;
    }
    //Serial.println ("XX");
    MySetPoints[0].Minutes = 0;
    if (SetPointCount == 3) {
      if (MySetPoints[SetPointCount - 1].Minutes >= 14) {
        MySetPoints[1].Minutes = 6;
        MySetPoints[2].Minutes = 10;
      } else if (MySetPoints[SetPointCount - 1].Minutes >= 12) {
        MySetPoints[2].Minutes = 8;
        MySetPoints[3].Minutes = 9;
      } else {
        MySetPoints[2].Minutes = 5;
        MySetPoints[3].Minutes = 7;
      }
    }
    if (SetPointCount == 4) {
      if (MySetPoints[SetPointCount - 1].Minutes >= 14) {
        MySetPoints[1].Minutes = 5;
        MySetPoints[2].Minutes = 8;
        MySetPoints[3].Minutes = 10;

      } else if (MySetPoints[SetPointCount - 1].Minutes >= 12) {
        MySetPoints[2].Minutes = 4;
        MySetPoints[3].Minutes = 7;
        MySetPoints[2].Minutes = 10;

      } else {
        MySetPoints[2].Minutes = 4;
        MySetPoints[3].Minutes = 6;
        MySetPoints[2].Minutes = 9;
      }
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
    //TimeScreenLeft = MySetPoints[SetPointCount - 1].Minutes + 2;
    TempYMax = 800;
    TempSplitHigh = 440;
    TempSplitHigh = MySetPoints[SetPointCount - 1].Temperature;
    PixelYSplit2 = 390;  //180;
    //  TempSplitLow = (MySetPoints[2].Temperature - Gain) ;
    TempSplitLow = (MySetPoints[1].Temperature);
    PixelYSplit = 190;  //90;
    TempPerPixL = TempSplitLow / PixelYSplit;
    TempPerPixM = (TempSplitHigh - TempSplitLow) / (PixelYSplit2 - PixelYSplit);
    TempPerPixH = (TempYMax - TempSplitHigh) / (480 - PixelYSplit2);
    //PixelsPerMin =  (int)(800 / TimeScreenLeft);
    PixelsPerMinL = (float)(pixelSplitLow / (float)timeSplitLow);
  }
  if (State == STATECOOLING) {
    TimeScreenLeft = MySetPoints[SetPointCount - 1].Minutes + 4;
  } else {
    TimeScreenLeft = MySetPoints[SetPointCount - 1].Minutes + 1;
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
  //  int lastt = 0;
  //Low range A
  HorScaleLineYCount = -1;
  int yaxislabelx = myGLCD.getDisplayXSize() - 30;
  int y = 0;
  for (int t = 50; t < (TempSplitLow - 10); t = t + 50) {
    y = YforATemp(t);
    HorScaleLineYCount++;
    HorScaleLineY[HorScaleLineYCount] = y;
    HorScaleLineYValue[HorScaleLineYCount] = t;
    myGLCD.setColor(LIGHTGRAY);
    myGLCD.drawLine(0, y, yaxislabelx, y);
    myGLCD.setColor(GRAY);
    myGLCD.printNumI(t, yaxislabelx, y - 5);
    //lastt = t;
  }
  //med range
  //  myGLCD.setColor(WHITE);
  for (int t = TempSplitLow; t < (TempSplitHigh - 4); t = t + 5) {
    y = YforATemp(t);
    HorScaleLineYCount++;
    HorScaleLineY[HorScaleLineYCount] = y;
    HorScaleLineYValue[HorScaleLineYCount] = t;
    myGLCD.setColor(LIGHTGRAY);
    myGLCD.drawLine(0, y, yaxislabelx, y);
    myGLCD.setColor(GRAY);
    myGLCD.printNumI(t, yaxislabelx, y - 5);
    
  }
  //high range
  
  for (int t = TempSplitHigh; t < TempYMax; t = t + 100) {
    y = YforATemp(t);
    HorScaleLineYCount++;
    HorScaleLineY[HorScaleLineYCount] = y;
    HorScaleLineYValue[HorScaleLineYCount] = t;
    myGLCD.setColor(LIGHTGRAY);
    myGLCD.drawLine(0, y, yaxislabelx, y);
    myGLCD.setColor(GRAY);
    myGLCD.printNumI(t, yaxislabelx, y - 5);
 
  }
  //draw the profile by and setpoint
  StartLinebyTemp(MySetPoints[0].Temperature, PROFILELINEID);
  for (unsigned int Minutes = 1; Minutes <= MySetPoints[SetPointCount - 1].Minutes; Minutes++) {
    AddLinebyTimeAndTemp(Minutes, MyMinuteTemperature[Minutes], PROFILELINEID);
  }
  for (int xSetPoint = 0; xSetPoint < SetPointCount; xSetPoint++) {
    DrawSetPoint(xSetPoint, LineColorforLineID[PROFILELINEID]);
  }
  DrawHorControlMenu();
  DrawHorFanMenu();
  graphFanProfile();

  DrawVMenu(MenuStatus.VmenuShowing);

  UpdateTempDisplayArea(All);
  UpdateStateDisplayArea(All);
  setpointschanged = false;

  UpdateProgessDisplayArea(All);
  UpdateConfigsDisplayArea(All);
  DrawLineFromHistoryArray(ROLLAVGLINEID);
}

void graphFanProfile() {
  PixelsPerMinFan = (float)FanGraphXWidth / (float)MySetPoints[SetPointCount - 1].Minutes;
  if (true == false) {
    myGLCD.setFont(SmallFont);
    myGLCD.setColor(fancolor);
    //block out area
    int FanXEnd = FanGraphXStart + ((PixelsPerMinFan * MySetPoints[SetPointCount - 1].Minutes));  //calc the ending of graph a max time
    int FanGraphTop = FanGraphBottom - FanGraphHeight;
    int xlable = FanGraphXStart - myGLCD.getFontXsize() * 3 - 2;
    myGLCD.setFont(SmallFont);
    myGLCD.setColor(fanback);
    //myGLCD.fillRect(xlable, FanGraphTop, FanXEnd + 8, FanGraphBottom);
    myGLCD.setBackColor(fanback);
    myGLCD.setColor(fancolor);
    myGLCD.drawRect(FanGraphXStart, FanGraphTop, FanXEnd, FanGraphBottom);
    //
    //draw vertical time grid lines and labels
    //myGLCD.setColor(GREY);
    for (unsigned int t = 3; t < MySetPoints[SetPointCount - 1].Minutes; t = t + 3) {
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
  } else {
    myGLCD.setFont(SmallFont);
    myGLCD.setColor(AQUA);
    myGLCD.setBackColor(BLACK);
  }
  //draw fan profile

  for (int i = 1; i < 4; i++) {
    //SpDebug("drawFanProfile x:" + String(XforAFanMin(FanSetPoints[i-1].Minutes)) + " y:" + String( YforAFan(FanSetPoints[i-1].PWM)) + " x2:" + String( XforAFanMin(FanSetPoints[i].Minutes)) + " y2:" + String(YforAFan(FanSetPoints[i].PWM)));
    myGLCD.drawLine(XforAFanMin(FanSetPoints[i - 1].Minutes), YforAFan(FanSetPoints[i - 1].PWM), XforAFanMin(FanSetPoints[i].Minutes), YforAFan(FanSetPoints[i].PWM));

    //SpDebug("drawFanLabel :" + String(FanSetPoints[i].PWM) + " x:" + String(XforAFanMin(FanSetPoints[i].Minutes)) + " y:" +  String(YforAFan(FanSetPoints[i].PWM - myGLCD.getFontYsize())));
  }
  myGLCD.setColor(GRAY);
  DrawFanSetPoint(0, fancolor);
  DrawFanSetPoint(1, fancolor);
  DrawFanSetPoint(2, fancolor);
  DrawFanSetPoint(3, fancolor);
  DrawLineFromHistoryArray(FANSPEEDLINEID);
}

void UpdateConfigsDisplayArea(boolean bValuesOnly) {
  //this is top row

  myGLCD.setFont(SmallFont);
  int rowheight = myGLCD.getFontYsize() * 1.1;
  int col = ConfigDisplay.x + 4;
  int row = ConfigDisplay.y + 2;
  int col2 = col + (myGLCD.getFontXsize() * 9);  //number
  if (ConfigDisplay.xmax == 0) {
    ConfigDisplay.xmax = col2 + (myGLCD.getFontXsize() * 6);
    ConfigDisplay.ymax = row + (rowheight * 6) + 2;
  }
  myGLCD.setBackColor(BLACK);
  myGLCD.setColor(PALEYELLOW);
  if (bValuesOnly == false) {
    myGLCD.setColor(BLACK);
    myGLCD_fillRect(ConfigDisplay);
    myGLCD.setColor(PALEYELLOW);
    myGLCD_drawRect(ConfigDisplay);
  }
  myGLCD.print(F("Configurations"), col, row);
  row = row + rowheight;
  myGLCD.print(F("T Gain   :"), col, row);
  myGLCD.printNumI(GainTemp, col2, row, 6, ' ');
  row = row + rowheight;
  myGLCD.print(F("T Intergal:"), col, row);
  myGLCD_printNumF(IntegralTemp, col2, row, 6, 2);
  row = row + rowheight;
  myGLCD.print(F("TooHotTemp:"), col, row);
  myGLCD.printNumI(TEMPCOILTOOHOT, col2, row, 6, ' ');
  row = row + rowheight;
  myGLCD.print(F("CooldwnT :"), col, row);
  myGLCD.printNumI(TEMPCOOLINGDONE, col2, row, 6, ' ');
  row = row + rowheight;
  myGLCD.print(F("FanG Bot :"), col, row);
  myGLCD.printNumI(FanGraphBottom, col2, row, 6, ' ');
}

void UpdateProgessDisplayArea(boolean bValuesOnly) {
  //this is top row

  myGLCD.setColor(PALEYELLOW);
  myGLCD.setBackColor(BLACK);
  myGLCD.setFont(arial_normal);
  int rowstart = OpProgessDisplay.x;
  int row = rowstart;
  int col = OpProgessDisplay.y;
  int rowheight = myGLCD.getFontYsize() * 1.1;
  int col2 = col + (5 * myGLCD.getFontXsize());
  int colend = col2 + (5 * myGLCD.getFontXsize());
  int DutyPercent = DutyTemp * 100;
  double FanPercent = (FanSpeedPWM / float(255)) * 100;
  if (bValuesOnly == false) {
    myGLCD.print(F("Time:"), col, row);
    myGLCD_printNumF(MySetPoints[SetPointCount - 1].Minutes - RoastMinutes, col2, row, 5, 1);
    row = row + rowheight;
    myGLCD.print(F("Temp:"), col, row);
    myGLCD.printNumI(TBeanAvgRoll.mean(), col2, row, 5);
    row = row + rowheight;
    myGLCD.print(F("Pwr%:"), col, row);
    myGLCD.printNumI(DutyPercent, col2, row, 5);
    row = row + rowheight;
    myGLCD.print(F("Err :"), col, row);
    myGLCD.printNumI(ErrTemp, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("AccT:"), col, row);
    myGLCD.printNumI(RoastAcumHeat, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("Fan%:"), col, row);
    myGLCD_printNumF(FanPercent, col2, row, 5, 1);
  } else {
    //myGLCD.print(F("Time:"), col , row);
    myGLCD_printNumF(MySetPoints[SetPointCount - 1].Minutes - RoastMinutes, col2, row, 5, 1);
    row = row + rowheight;
    //myGLCD.print(F("Temp:"), col , row);
    myGLCD.printNumI(TBeanAvgRoll.mean(), col2, row, 5);
    row = row + rowheight;
    //myGLCD.print(F("Pwr%:"), col, row);
    myGLCD.printNumI(DutyPercent, col2, row, 5);
    row = row + rowheight;
    //myGLCD.print(F("Err :"), col , row);
    myGLCD.printNumI(ErrTemp, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("AccT:"), col  , row);
    myGLCD.printNumI(RoastAcumHeat, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("Fan%:"), col, row);
    myGLCD_printNumF(FanPercent, col2, row, 5, 1);
  }

  myGLCD.drawRect(col, rowstart - 1, colend, row + rowheight + 1);
}

void UpdateStateDisplayArea(boolean bValuesOnly) {

  int row = 2;  //- myGLCD.getFontYsize();
  int col = 2;
  int rowheight = 0;
  bool drawwitherror = false;
  bool blankout = false;
  uint16_t blankoutcolor;

  if (bValuesOnly == true) {

    if (lastStateUpdated == State && ErrorStatus.newerrmsg == false) {
      //SpDebug("Skipped " + String(State));
      return;
    }
  }
  if (ErrorStatus.newerrmsg == false) {
    drawwitherror = ErrorStatus.lastdrawnwitherror;
  } else {
    blankout = true;
    ErrorStatus.newerrmsg = false;
    if (ErrorStatus.error == NoError) {
      blankoutcolor = BLACK;
      ErrorStatus.errorcleared = true;
      ErrorStatus.lastdrawnwitherror = false;
    } else {  //draw status and error
      blankoutcolor = YELLOW;
      ErrorStatus.lastdrawnwitherror = true;
      drawwitherror = true;
      ErrorStatus.errorcleared = false;
    }
  }


  if (blankout == true) {
    myGLCD.setColor(blankoutcolor);
    myGLCD.fillRect(0, 0, myHorControlMenuDef.colstart, OpProgessDisplay.x);
  }
  if (drawwitherror == false) {
    myGLCD.setFont(Grotesk24x48);
  } else {
    myGLCD.setFont(Grotesk16x32);
  }
  //draw status
  rowheight = myGLCD.getFontYsize();
  myGLCD.setColor(StateColor[State]);
  myGLCD.print(StateName[State], col, row);
  lastStateUpdated = State;
  //draw error
  if (drawwitherror == true) {
    myGLCD.print(ErrDisplay, 150, row);

    myGLCD.setFont(BigFont);
    row = row + rowheight;
    rowheight = myGLCD.getFontYsize();
    //memcpy_P(&myLocalbuttontext, &Vmenutext[butdefset.menuID][i], sizeof(buttontext));
    SpDebug("error found. ID is:" + String(errorlist[ErrorStatus.error].errorID) + " " + String(errorlist[ErrorStatus.error].line1) + " " + String(errorlist[ErrorStatus.error].line2));
    myGLCD.setColor(YELLOW);

    myGLCD.print(errorlist[ErrorStatus.error].line1, col, row);
    row = row + rowheight;
    myGLCD.print(errorlist[ErrorStatus.error].line2, col, row);
  }
}

void UpdateTempDisplayArea(boolean bValuesOnly) {

  myGLCD.setFont(SmallFont);
  int rowheight = myGLCD.getFontYsize() * 1.1;
  int col = OpDetailDisplay.x + 4;
  int row = OpDetailDisplay.y + 2;
  int col2 = col + (myGLCD.getFontXsize() * 9);  //number
  if (OpDetailDisplay.xmax == 0) {
    OpDetailDisplay.xmax = col2 + (myGLCD.getFontXsize() * 6);
    OpDetailDisplay.ymax = row + (rowheight * 10) + 2;
  }
  
  FreeMemory = freeMemory();
  
  myGLCD.setBackColor(BLACK);
  myGLCD.setColor(PALEYELLOW);
  if (bValuesOnly == false) {
    myGLCD.setColor(BLACK);
    myGLCD_fillRect(OpDetailDisplay);
    myGLCD.setColor(PALEYELLOW);
    myGLCD_drawRect(OpDetailDisplay);

    myGLCD.print(F("Oper Detail"), col, row);
    row = row + rowheight;
    myGLCD.print(F("Tavg:"), col, row);
    myGLCD.printNumI(TBeanAvgRoll.mean(), col2, row, 5);
    row = row + rowheight;
    myGLCD.print(F("Temp1"), col, row);
    //{ myGLCD.print(F(" F"), col5, row); }
    myGLCD.printNumI(TBean1, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("Temp2"), col, row);
    //{ myGLCD.print(F(" F"), col5, row); }
    myGLCD.printNumI(TBean2, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("Coil "), col, row);
    //{ myGLCD.print(F(" F"), col5, row); }
    myGLCD.printNumI(TCoil, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("CAvg "), col, row);
    //{ myGLCD.print(F(" F"), col5, row); }
    myGLCD.printNumI(TCoilRoll.mean(), col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("IEr :"), col, row);
    myGLCD.printNumI(ErrITemp, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("lp/sec:"), col, row);
    myGLCD.printNumI(LoopsPerSecond, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("Terrs:"), col, row);
    int R = TempReadingskipped[0] + TempReadingskipped[1] + TempReadingskipped[2];
    myGLCD.printNumI(R, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("Mem:"), col, row);
    myGLCD.printNumI(FreeMemory, col2, row, 5, ' ');
  } else {  //this is copy of above with myLCD commented out
    //myGLCD.print(F("Tavg:"), col, row);
    // myGLCD.print(F("Oper Detail"), col, row);
    row = row + rowheight;
    myGLCD.printNumI(TBeanAvgRoll.mean(), col2, row, 5);
    row = row + rowheight;
    //myGLCD.print("Temp1",col , row);   myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TBean1, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print("Temp2", col, row);        myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TBean2, col2, row, 5, ' ');
    row = row + rowheight;
    //          myGLCD.print("Coil:", col, row); myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TCoil, col2, row, 5, ' ');
    row = row + rowheight;
    //          myGLCD.print("Cvg:", col, row); myGLCD.print("F",col5 , row);
    myGLCD.printNumI(TCoilRoll.mean(), col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("IEr :"), col, row);
    myGLCD.printNumI(ErrITemp, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("lps/sec:"), col, row);
    myGLCD.printNumI(LoopsPerSecond, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("Skiptemps:"), col, row);
    int R = TempReadingskipped[0] + TempReadingskipped[1] + TempReadingskipped[2];
    myGLCD.printNumI(R, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("MemK:"), col, row);
    myGLCD.printNumI(FreeMemory, col2, row, 5, ' ');
  }
}

void UpdateFanPWMValuesDisplay(boolean bValuesOnly) {

  myGLCD.setFont(BigFont);
  // myGLCD.setFont(Retro8x16);
  //int rowheight = 20;
  //int row = (myHorControlMenuDef.H * 2) + myGLCD.getFontXsize() - 25;
  int row = myHorFanButtonControl.rowstart - myGLCD.getFontXsize() - 2;
  int col = myHorFanButtonControl.colstart;
  int col1 = col + myGLCD.getFontXsize() * 3;
  int col2 = col1 + myGLCD.getFontXsize() * 4;

  myGLCD.setBackColor(BLACK);

  myGLCD.setColor(AQUA);
  myGLCD.printNumI(FanSpeedPWM, col, row, 3, ' ');
  if (bValuesOnly == false) {
    myGLCD.print(F("Dev:"), col1, row);
  }
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

void AddLinebyXY(uint16_t& newX, uint16_t& newY, int lineID) {
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

void AddLinebyTimeAndDuty(double timemins) {
  //Serial.print("FanSpeedPWM:");Serial.print(FanSpeedPWM);Serial.print(" Time:");Serial.println(timemins);
  uint16_t newY = (uint16_t)YforAFan(FanSpeedPWM);
  uint16_t newX = (uint16_t)(FanGraphXStart + (PixelsPerMinFan * timemins));

  AddLinebyXY(newX, newY, FANSPEEDLINEID);
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

void DrawFanSetPoint(int setpoint, uint16_t color) {
  myGLCD.setFont(SmallFont);
  int Y = YforAFan(FanSetPoints[setpoint].PWM);
  int X = XforAFanMin(FanSetPoints[setpoint].Minutes);

  int xOffsetCircle = 4;
  int xOffset = 0;
  int xOffset2 = 0;
  switch (setpoint) {
    case 0:
      xOffset = 2;
      xOffset2 = 2;
      break;
    case 1:
    case 2:
      xOffset = -myGLCD.getFontXsize();
      xOffset2 = 0;//myGLCD.getFontXsize()/2;
      break;
    case 3:
      xOffset = -(myGLCD.getFontXsize() * 2);
      xOffset2 = -(myGLCD.getFontXsize());
      break;
  }

  myGLCD.setColor(color);
  myGLCD.fillCircle(X + xOffsetCircle, Y, 5);
  
  myGLCD.printNumI(FanSetPoints[setpoint].PWM, X + xOffset, Y - myGLCD.getFontYsize() - 5 );
  myGLCD.printNumI(FanSetPoints[setpoint].Minutes, X + xOffset2, Y + 5 );
}

void DrawSetPoint(int setpoint, uint16_t color) {
  int Y = YforATemp(MySetPoints[setpoint].Temperature);
  int X = XforATime(MySetPoints[setpoint].Minutes);
  
  int xOffset = 0;
  int xOffset2 = 0;
  int yOffset = 0;
  int circleoffset = 0;
  switch (setpoint) {
    case 0:
      xOffset = 15;
      xOffset2 = 0;
      circleoffset = 4;  
    case 1:
        break;
    case 2:
    break;
    case 3:
    break;
    case 4:
      xOffset = -10;
      xOffset2 = -10;
      break;
  }
  myGLCD.setColor(YELLOW);
  myGLCD.fillCircle(X + circleoffset, Y, 5);
  myGLCD.setColor(WHITE);

  myGLCD.printNumI(setpoint, X + (myGLCD.getFontXsize() / 2) + xOffset, Y - (myGLCD.getFontYsize() * 2) - 5);
  myGLCD.print(F("#"), X - (myGLCD.getFontXsize() / 2)  + xOffset, Y - (myGLCD.getFontYsize() * 2) - 5);
  myGLCD.printNumI(MySetPoints[setpoint].Temperature, X - (myGLCD.getFontXsize() * 1.5)  + xOffset, Y - (myGLCD.getFontYsize()) - 5);
  myGLCD.printNumI(MySetPoints[setpoint].Minutes, X - (myGLCD.getFontXsize() / 2)  + xOffset, Y + 2 + 5);

}

void DrawMovedSetPoint(int setpoint) {
  SpDebug("DrawMovedSetPoint");
  int spYnew = YforATemp(MySetPoints[setpoint].Temperature);
  int spX = XforATime(MySetPoints[setpoint].Minutes);
  myGLCD.setColor(YELLOW);
  myGLCD.fillCircle(spX, spYnew, 7);
  myGLCD.setColor(WHITE);
  myGLCD.drawCircle(spX, spYnew, 7);

  int spYold = YforATemp(MySetPoints[setpoint].Temperature);
  myGLCD.setBackColor(BLACK);
  myGLCD.setFont(BigFont);

  int Row1Y = spYold - 10;
  //int Row2Y = spYold + 5;
  //int col1 = spX - (myGLCD.getFontXsize() * 3);
  int col1 = spX - (myGLCD.getFontXsize() * 3);
  int col2 = col1 + (myGLCD.getFontXsize() * 1);
  //int col3 = col2 + (myGLCD.getFontXsize() * 1);
  myGLCD.setColor(YELLOW);
  //myGLCD.print(F("N:"), col1, Row1Y);
  //myGLCD.printNumI(MySetPoints[setpoint].TemperatureNew, col2, Row1Y);
  int delta = MySetPoints[setpoint].TemperatureNew - MySetPoints[setpoint].Temperature;
  //myGLCD.print(F("D:"), col1, Row2Y);
  if (delta > 0) {
    myGLCD.print(F("+"), col1, Row1Y);
  }
  if (delta < 0) {
    myGLCD.print(F("-"), col1, Row1Y);
  }
  myGLCD.printNumI(abs(delta), col2, Row1Y);
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
}

void MoveAFanPointsPWM(int SetPoint) {
  Serial.print("MoveAPointFan Setpoint:");
  Serial.print(SetPoint);
  Serial.print("ActiveAdjustment.moveamount:");
  Serial.println(ActiveAdjustment.moveamount);
  if (SetPoint > -1) {
    FanSetPoints[SetPoint].PWM = FanSetPoints[SetPoint].PWM + ActiveAdjustment.moveamount;
    EEPROM.put(FanSetPoints_EP[SetPoint], FanSetPoints[SetPoint]);
    delay(100);
    myGLCD.setFont(SmallFont);
    DrawFanSetPoint(SetPoint, AQUA);
  }
}

void MoveAFanPointsTime(int SetPoint) {
  Serial.print("MoveAPointFanPointsTime Setpoint:");
  Serial.print(SetPoint);
  Serial.print("ActiveAdjustment.moveamount:");
  Serial.println(ActiveAdjustment.moveamount);
  if (SetPoint > -1) {
    FanSetPoints[SetPoint].Minutes = FanSetPoints[SetPoint].Minutes + ActiveAdjustment.moveamount;
    EEPROM.put(FanSetPoints_EP[SetPoint], FanSetPoints[SetPoint]);
    delay(100);
    myGLCD.setFont(SmallFont);
    DrawFanSetPoint(SetPoint, AQUA);
  }
}
void MoveAPoint(int SetPoint) {
  //Serial.print("MoveAPoint Setpoint:"); Serial.print(SetPoint); Serial.print("ActiveAdjustment.moveamount:"); Serial.println(ActiveAdjustment.moveamount);
  if (SetPoint > -1) {
    MySetPoints[SetPoint].Temperature = MySetPoints[SetPoint].Temperature + ActiveAdjustment.moveamount;

    EEPROM.put(SETPOINTTEMP_EP[SetPoint], MySetPoints[SetPoint].Temperature);
    delay(100);
    myGLCD.setFont(SmallFont);
    DrawSetPoint(SetPoint, YELLOW);
    // DrawMovedSetPoint(SetPoint);
  }
}

void myGLCD_fillRect(rect& Rect) {
  myGLCD.fillRect(Rect.x, Rect.y, Rect.xmax, Rect.ymax);
}

void myGLCD_drawRect(rect& Rect) {
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

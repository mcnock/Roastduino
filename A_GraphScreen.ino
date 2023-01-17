const double TempYMax = 600;
const double PixelY_SplitHigh = 430;
const double PixelYSplitLow = 150;
double TempSplitHigh;
double TempSplitLow;
float PixelsPerMinL;
float PixelsPerMinM;
const byte timeSplitLow = 6;
const int pixelXSplitLow = 150;
const uint16_t fancolor = GRAY;
const uint16_t fanback = DARKBLUE;

void graphProfile() {
  //draw scales
  myGLCD.setFont(SmallFont);
  //Calculate  setpoint accumlated minutes basedon current minute settings
  if (Setpointschanged == true) {
    //read from memory
    int accumulatedbySetPoint = 0;
    int accumulatedMinutes = 0;
    byte m;
    EEPROM.get(ROASTLENGTH_int_EP, m);
    MySetPoints[SetPointCount - 1].Minutes = m;
    //SPDEBUG("Loading " + String(SetPointCount) + " new setpoints with length " + String(MySetPoints[SetPointCount - 1].Minutes));
    if (MySetPoints[SetPointCount - 1].Minutes < 10 or MySetPoints[SetPointCount - 1].Minutes > 20) {
      MySetPoints[SetPointCount - 1].Minutes = 14;
    }
    //Set the minutes based on number of setpoint and length of roast;
    MySetPoints[0].Minutes = 0;
    if (SetPointCount == 4) {
      if (MySetPoints[SetPointCount - 1].Minutes >= 14) {
        MySetPoints[1].Minutes = 6;
        MySetPoints[2].Minutes = 10;
      } else if (MySetPoints[SetPointCount - 1].Minutes >= 12) {
        MySetPoints[1].Minutes = 8;
        MySetPoints[2].Minutes = 9;
      } else {
        MySetPoints[1].Minutes = 5;
        MySetPoints[2].Minutes = 7;
      }
    }
    if (SetPointCount == 5) {
      if (MySetPoints[SetPointCount - 1].Minutes >= 14) {
        MySetPoints[1].Minutes = 6;

        MySetPoints[2].Minutes = 9;
        MySetPoints[3].Minutes = 12;

      } else if (MySetPoints[SetPointCount - 1].Minutes >= 12) {
        MySetPoints[1].Minutes = 4;
        MySetPoints[2].Minutes = 7;
        MySetPoints[3].Minutes = 10;

      } else {
        MySetPoints[1].Minutes = 4;
        MySetPoints[2].Minutes = 6;
        MySetPoints[3].Minutes = 10;
      }
    }
    //read setpoint temps from eprom
    for (int xSetPoint = 0; xSetPoint < SetPointCount; xSetPoint++) {
      EEPROM.get(TempSetpoint_int_EP[xSetPoint], MySetPoints[xSetPoint].Temperature);
      if (xSetPoint == 0) {
        MySetPoints[0].SpanMinutes = 0;
        MySetPoints[0].Minutes = 0;
        MyMinuteTemperature[0] = MySetPoints[0].Temperature;
      } else {
        MySetPoints[xSetPoint].SpanMinutes = MySetPoints[xSetPoint].Minutes - MySetPoints[xSetPoint - 1].Minutes;
        accumulatedbySetPoint = accumulatedbySetPoint + MySetPoints[xSetPoint].SpanMinutes;
        MySetPoints[xSetPoint].Minutes = accumulatedbySetPoint;
        //now calc the minute setpoint for the span leading to this setpoint
        double TempPerMinuteinSpan = ((double)(MySetPoints[xSetPoint].Temperature - MySetPoints[xSetPoint - 1].Temperature)) / MySetPoints[xSetPoint].SpanMinutes;
        for (int xSpanMinute = 1; xSpanMinute <= MySetPoints[xSetPoint].SpanMinutes; xSpanMinute++) {
          accumulatedMinutes = accumulatedMinutes + 1;
          MyMinuteTemperature[accumulatedMinutes] = MySetPoints[xSetPoint - 1].Temperature + (TempPerMinuteinSpan * xSpanMinute);
        }
        //SPDEBUG("setpoint : " + String(xSetPoint) + " minutes " + String(MySetPoints[xSetPoint].Minutes) + " span_minutes:" + String(MySetPoints[xSetPoint].SpanMinutes) + " temp" + String(MySetPoints[xSetPoint].Temperature));
      }
    }
    //perform some necesary calculations to size our scales to the display
    //yscale is done in 3 ranges
    //TimeScreenLeft = MySetPoints[SetPointCount - 1].Minutes + 2;
    TempSplitHigh = MySetPoints[SetPointCount - 1].Temperature + 10;
    //PixelY_SplitHigh = 440;  //180;
    //  TempSplitLow = (MySetPoints[2].Temperature - Gain) ;
    TempSplitLow = (MySetPoints[1].Temperature);
    //PixelYSplitLow = 190;  //90;
    TempPerPixL = TempSplitLow / PixelYSplitLow;
    ;
    TempPerPixM = (TempSplitHigh - TempSplitLow) / (PixelY_SplitHigh - PixelYSplitLow);
    TempPerPixH = (TempYMax - TempSplitHigh) / (480 - PixelY_SplitHigh);
    //PixelsPerMin =  (int)(800 / TimeScreenLeft);
    PixelsPerMinL = (float)(pixelXSplitLow / (float)timeSplitLow);
  }
  //if (State == STATECOOLING) {
  // TimeScreenLeft = MySetPoints[SetPointCount - 1].Minutes + 4;
  // } else {
  TimeScreenLeft = MySetPoints[SetPointCount - 1].Minutes + 1;
  //}
  PixelsPerMinM = (float)(800 - pixelXSplitLow) / float((TimeScreenLeft - timeSplitLow));
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
  for (int t = 50; t < (TempSplitLow - 5); t = t + 50) {
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
  for (int t = TempSplitLow; t <= (TempSplitHigh); t = t + 5) {
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

  for (int t = TempSplitHigh + 5; t < TempYMax; t = t + 100) {
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
  SPDEBUG("Drawing profile. Temp of setpoint[0] is:" + String(MySetPoints[0].Temperature));
  StartLinebyTemp(MySetPoints[0].Temperature, PROFILELINEID);
  for (unsigned int Minutes = 1; Minutes <= MySetPoints[SetPointCount - 1].Minutes; Minutes++) {
    AddLinebyTimeAndTemp(Minutes, MyMinuteTemperature[Minutes], PROFILELINEID);
  }
  for (int xSetPoint = 0; xSetPoint < SetPointCount; xSetPoint++) {
    DrawTempSetPoint(xSetPoint, LineColorforLineID[PROFILELINEID], REGULAR);
  }
  DrawHorControlMenu();

  if (FanLegacy) {
    DrawHorFanMenu();
    graphFanProfile();
  }
  DrawVMenu(MenuStatus.VmenuShowing);
  UpdateErrorDisplayArea(ALL);
  UpdateOpDetailsDisplayArea(ALL);
  UpdateProgressDisplayArea(ALL);
  UpdateConfigsDisplayArea(ALL);
  DrawLineFromHistoryArray(ROLLAVGLINEID);
  DrawLineFromHistoryArray(COILLINEID);
  Setpointschanged = false;
}

void graphFanProfile() {

  PixelsPerFlowRate = (float)FanGraphXWidth / (float)MySetPoints[SetPointCount - 1].Minutes;
  if (true == false) {
    myGLCD.setFont(SmallFont);
    myGLCD.setColor(fancolor);
    //block out area
    int FanXEnd = FanGraphXStart + ((PixelsPerFlowRate * MySetPoints[SetPointCount - 1].Minutes));  //calc the ending of graph a max time
    int FanGraphTop = FlowSetPointGraphBottompx - FlowSetPointGraphHeightpx;
    int xlable = FanGraphXStart - myGLCD.getFontXsize() * 3 - 2;
    myGLCD.setFont(SmallFont);
    myGLCD.setColor(fanback);
    //myGLCD.fillRect(xlable, FanGraphTop, FanXEnd + 8, FlowSetPointGraphBottompx);
    myGLCD.setBackColor(fanback);
    myGLCD.setColor(fancolor);
    myGLCD.drawRect(FanGraphXStart, FanGraphTop, FanXEnd, FlowSetPointGraphBottompx);
    //
    //draw vertical time grid lines and labels
    //myGLCD.setColor(GREY);
    for (unsigned int t = 3; t < MySetPoints[SetPointCount - 1].Minutes; t = t + 3) {
      int q = PixelsPerFlowRate * t + FanGraphXStart;
      myGLCD.setColor(LIGHTGRAY);
      myGLCD.drawLine(q, FanGraphTop, q, FlowSetPointGraphBottompx);
      myGLCD.setColor(GRAY);
      if (t < 10) {
        myGLCD.printNumI(t, q - myGLCD.getFontXsize(), FlowSetPointGraphBottompx - myGLCD.getFontYsize() - 1);
        myGLCD.print(F("m"), q, FlowSetPointGraphBottompx - myGLCD.getFontYsize() - 1);
      } else {
        myGLCD.printNumI(t, q - (myGLCD.getFontXsize() * 1.33), FlowSetPointGraphBottompx - myGLCD.getFontYsize() - 1);
        myGLCD.print(F("m"), q + (myGLCD.getFontXsize() * 1.33), FlowSetPointGraphBottompx - myGLCD.getFontYsize() - 1);
      }
    }

    //draw horizontal pwm grid lines and labels
    myGLCD.setColor(LIGHTGRAY);
    int xgridpwm = 0;
    FlowSetPointMax = round5(FlowSetPoints[0].flow);
    for (xgridpwm = FlowSetPointMin; xgridpwm < FlowSetPointMax; xgridpwm = xgridpwm + FanGraphHorGridSpacingPWM) {
      int Y = YforAFlow(xgridpwm);
      if (Y != FlowSetPointGraphBottompx and xgridpwm != FanGraphTop)  //Do not override top and bottom
      {
        myGLCD.setColor(LIGHTGRAY);
        myGLCD.drawLine(FanGraphXStart, Y, FanXEnd, Y);
      }
      myGLCD.setColor(GRAY);
      myGLCD.printNumI(xgridpwm, xlable, Y - (myGLCD.getFontYsize() / 2));
    }

    if (xgridpwm < FlowSetPointMax) {
      if (FlowSetPointMax - xgridpwm >= 10) {
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
    //SPDEBUG("drawFanProfile x:" + String(XforAFanMin(FlowSetPoints[i-1].Minutes)) + " y:" + String( YforAFlow(FlowSetPoints[i-1].PWM)) + " x2:" + String( XforAFanMin(FlowSetPoints[i].Minutes)) + " y2:" + String(YforAFlow(FlowSetPoints[i].PWM)));
    myGLCD.drawLine(XforAFanMin(FlowSetPoints[i - 1].Minutes), YforAFlow(FlowSetPoints[i - 1].flow), XforAFanMin(FlowSetPoints[i].Minutes), YforAFlow(FlowSetPoints[i].flow));

    //SPDEBUG("drawFanLabel :" + String(FlowSetPoints[i].PWM) + " x:" + String(XforAFanMin(FlowSetPoints[i].Minutes)) + " y:" +  String(YforAFlow(FlowSetPoints[i].PWM - myGLCD.getFontYsize())));
  }
  myGLCD.setColor(GRAY);
  DrawFanSetPoint(0, fancolor, false);
  DrawFanSetPoint(1, fancolor, false);
  DrawFanSetPoint(2, fancolor, false);
  DrawFanSetPoint(3, fancolor, false);
  DrawLineFromHistoryArray(FANSPEEDLINEID);
}

void UpdateConfigsDisplayArea(boolean bVALUESONLY) {
  //this is top row

  if (TouchStatus.objectpressID == PressConfigDisplayBox){
    return;
  }
  myGLCD.setFont(SmallFont);
  int rowheight = myGLCD.getFontYsize() * 1.1;
  int col = DisplayBoxes[ConfigDisplay].Rect.x + 4;
  int row = DisplayBoxes[ConfigDisplay].Rect.y + 2;
  int col2 = col + (myGLCD.getFontXsize() * 9);  //number
  if (DisplayBoxes[ConfigDisplay].Rect.xmax == 0) {
    DisplayBoxes[ConfigDisplay].Rect.xmax = col2 + (myGLCD.getFontXsize() * 6);
    DisplayBoxes[ConfigDisplay].Rect.ymax = row + (rowheight * 13) + 2;
  }
  myGLCD.setBackColor(BLACK);
  myGLCD.setColor(PALEYELLOW);
  if (bVALUESONLY == false) {
    myGLCD.setColor(BLACK);
    myGLCD_fillRect(DisplayBoxes[ConfigDisplay].Rect);
    myGLCD.setColor(PALEYELLOW);
    myGLCD_drawRect(DisplayBoxes[ConfigDisplay].Rect);
  }
  myGLCD.print(F("Configurations"), col, row);
  row = row + rowheight;
  myGLCD.print(F("T Gain   :"), col, row);
  myGLCD.printNumI(GainTemp, col2, row, 6, ' ');
  row = row + rowheight;
  myGLCD.print(F("T Intergal:"), col, row);
  myGLCD_printNumF(IntegralTemp, col2, row, 6, 2);
  row = row + rowheight;
  myGLCD.print(F("Fsp  0m:"), col, row);
  myGLCD_printNumF(FlowSetPoints[0].flow, col2, row, 6, 1);
  row = row + rowheight;
  myGLCD.print(F("Fsp  7m:"), col, row);
  myGLCD_printNumF(FlowSetPoints[1].flow, col2, row, 6, 1);
  row = row + rowheight;
  myGLCD.print(F("Fsp 11m:"), col, row);
  myGLCD_printNumF(FlowSetPoints[2].flow, col2, row, 6, 1);
  row = row + rowheight;
  myGLCD.print(F("Fsp 14m:"), col, row);
  myGLCD_printNumF(FlowSetPoints[3].flow, col2, row, 6, 1);
  row = row + rowheight;
  myGLCD.print(F("F Gain   :"), col, row);
  myGLCD.printNumI(GainFlow, col2, row, 6, ' ');
  row = row + rowheight;
  myGLCD.print(F("F Intergal:"), col, row);
  myGLCD_printNumF(IntegralFlow, col2, row, 6, 2);
  row = row + rowheight;
  myGLCD.print(F("F Der:"), col, row);
  myGLCD_printNumF(MaxPercentChangePerSecondFlow, col2, row, 6, 2);
  row = row + rowheight;
  myGLCD.print(F("TooHotTemp:"), col, row);
  myGLCD.printNumI(TempCoilTooHot, col2, row, 6, ' ');
  row = row + rowheight;
  myGLCD.print(F("CooldwnT :"), col, row);
  myGLCD.printNumI(TempCoolingDone, col2, row, 6, ' ');
  row = row + rowheight;
  myGLCD.print(F("CoolBurst:"), col, row);
  myGLCD_printNumF(FanCoolingBoostPercent, col2, row, 5, 1);
}

void UpdateProgressDisplayArea(boolean bVALUESONLY) {
  myGLCD.setBackColor(BLACK);
  if (bVALUESONLY == ALL || LastStateUpdated != State) {
    myGLCD.setFont(Grotesk24x48);
    myGLCD.setColor(StateColor[State]);
    myGLCD.print(StateName[State], 2, 2);
    LastStateUpdated = State;
  }
  myGLCD.setColor(PALEYELLOW);
  myGLCD.setFont(arial_normal);
  int rowstart = DisplayBoxes[OpProgessDisplay].Rect.y + 2;
  int row = rowstart;
  int col = DisplayBoxes[OpProgessDisplay].Rect.x + 1;
  int rowheight = myGLCD.getFontYsize() * 1.1;
  int col2 = col + (5 * myGLCD.getFontXsize());
  int colend = col2 + (5 * myGLCD.getFontXsize());

  if (DisplayBoxes[OpProgessDisplay].Rect.xmax == 0) {
    DisplayBoxes[OpProgessDisplay].Rect.xmax = colend + 2;
    DisplayBoxes[OpProgessDisplay].Rect.ymax = rowstart + (rowheight * 10) + 2;
  }
  //myGLCD_drawRect(col, rowstart - 1, colend, row + rowheight + 1);
  if (bVALUESONLY == ALL || LastStateUpdated != State) {
    myGLCD.setColor(BLACK);
    myGLCD_fillRect(DisplayBoxes[OpProgessDisplay].Rect);
    myGLCD.setColor(PALEYELLOW);
    myGLCD_drawRect(DisplayBoxes[OpProgessDisplay].Rect);
  }
  myGLCD.setBackColor(BLACK);
  myGLCD.setColor(PALEYELLOW);
  int DutyPercent = DutyTemp * 100;
  double FanPercent;
  if (FanLegacy == true) {
    FanPercent = (FanSpeed254PWM / float(255)) * 100;
  } else {
    FanPercent = DutyFan * 100;
  }
  if (bVALUESONLY == false) {
    myGLCD.print(F("Time:"), col, row);
    myGLCD_printNumF(MySetPoints[SetPointCount - 1].Minutes - RoastMinutes, col2, row, 5, 1);
    row = row + rowheight;
    myGLCD.print(F("Temp:"), col, row);
    myGLCD.printNumI(TBeanAvgRoll.mean(), col2, row, 5);
    row = row + rowheight;
    myGLCD.print(F("Pwr%:"), col, row);
    myGLCD.printNumI(DutyPercent, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("sp:"), col, row);
    myGLCD_printNumF(CurrentSetPointTemp, col2, row, 5, 1);
    row = row + rowheight;
    myGLCD.print(F("Err :"), col, row);
    myGLCD.printNumI(ErrTemp, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("AccTk:"), col, row);
    myGLCD.printNumI(RoastAcumHeat / 1000, col2, row, 5, ' ');
    row = row + rowheight;
    row = row + rowheight;
    myGLCD.print(F("Fan%:"), col, row);
    myGLCD_printNumF(FanPercent, col2, row, 5, 1);
    row = row + rowheight;
    myGLCD.print(F("F SP:"), col, row);
    myGLCD_printNumF(BeanYflowsetpoint, col2, row, 5, 1);
    row = row + rowheight;
    myGLCD.print(F("Flow:"), col, row);
    myGLCD_printNumF(sq(BeanYflow_avg.mean()), col2, row, 5, 1);
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
    //myGLCD.print(F("sp:"), col, row);
    myGLCD_printNumF(CurrentSetPointTemp, col2, row, 5, 1);
    row = row + rowheight;
    //myGLCD.print(F("Err :"), col , row);
    myGLCD.printNumI(ErrTemp, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("AccT:"), col  , row);
    myGLCD.printNumI(RoastAcumHeat, col2, row, 5, ' ');
    row = row + rowheight;
    row = row + rowheight;
    //myGLCD.print(F("Fan%:"), col, row);
    myGLCD_printNumF(FanPercent, col2, row, 5, 1);
    row = row + rowheight;
    //myGLCD.print(F("F SP:"), col, row);
    myGLCD_printNumF(BeanYflowsetpoint, col2, row, 5, 1);
    row = row + rowheight;
    //myGLCD.print(F("Flow:"), col, row);
    myGLCD_printNumF(sq(BeanYflow_avg.mean()), col2, row, 5, 1);
  }
}

void UpdateErrorDisplayArea(boolean bVALUESONLY) {

  myGLCD.setFont(arial_bold);
  int rowheight = myGLCD.getFontYsize();
  int row = HorControlMenuDef.H + 1;  //- myGLCD.getFontYsize();
  int col = 2;
  uint16_t blankoutcolor = BLACK;

  if (bVALUESONLY == true && ErrorStatus.newerrmsg == false) {
    return;
  }

  if (ErrorStatus.error == NoError || ErrorStatus.newerrmsg == true) {
    myGLCD.setColor(BLACK);
    myGLCD.fillRect(col, row, 650, row + rowheight + 3);
    ErrorStatus.newerrmsg = false;
    //ErrorStatus.errorcleared = true;
  }


  if (ErrorStatus.error != NoError) {
    //SPDEBUG("error found. ID is:" + String(ErrorList[ErrorStatus.error].errorID) + " " + String(ErrorList[ErrorStatus.error].line1) + " " + String(ErrorList[ErrorStatus.error].line2));
    int widewidth = (strlen(ErrorList[ErrorStatus.error].line1) + 1) * myGLCD.getFontYsize();
    if (widewidth > 650) {
      myGLCD.setFont(Retro8x16);
    } else {
      myGLCD.setFont(arial_bold);
    }
    myGLCD.setColor(YELLOW);
    myGLCD.print(ErrorList[ErrorStatus.error].line1, col + 1, row + 3);
  } else {

    myGLCD.setFont(Retro8x16);
    myGLCD.setBackColor(BLACK);
    myGLCD.setColor(GRAY);
    switch (DisplayMessageToShow) {
      case DisplayDefault:
        myGLCD.print(F("Press 3 seconds on buttons for more info..."), col + 1, row + 3);
        break;
      case DisplayMsgAdvanced:
        myGLCD.print(F("Roast has been shortened..."), col + 1, row + 3);
        break;
      case DisplayMsgRetarded:
        myGLCD.print(F("Roast has been extended..."), col + 1, row + 3);
        break;
    }
  }
}

void UpdateOpDetailsDisplayArea(boolean bVALUESONLY) {

  if (TouchStatus.objectpressID == PressOpDetailBox){
    return;
  }
  myGLCD.setFont(SmallFont);
  int rowheight = myGLCD.getFontYsize() * 1.1;
  int col = DisplayBoxes[OpDetailDisplay].Rect.x + 4;
  int row = DisplayBoxes[OpDetailDisplay].Rect.y + 2;
  int col2 = col + (myGLCD.getFontXsize() * 9);  //number
  if (DisplayBoxes[OpDetailDisplay].Rect.xmax == 0) {
    DisplayBoxes[OpDetailDisplay].Rect.xmax = col2 + (myGLCD.getFontXsize() * 6);
    DisplayBoxes[OpDetailDisplay].Rect.ymax = row + (rowheight * 12) + 2;
  }
  FreeMemory = freeMemory();
  myGLCD.setBackColor(BLACK);
  myGLCD.setColor(PALEYELLOW);
  if (bVALUESONLY == false) {
    myGLCD.setColor(BLACK);
    myGLCD_fillRect(DisplayBoxes[OpDetailDisplay].Rect);
    myGLCD.setColor(PALEYELLOW);
    myGLCD_drawRect(DisplayBoxes[OpDetailDisplay].Rect);
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
    myGLCD.printNumI(TCoilAvgRoll.mean(), col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("IEr :"), col, row);
    myGLCD.printNumI(ErrITemp, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("lp/sec:"), col, row);
    myGLCD.printNumI(LoopsPerSecond, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("T&F/s:"), col, row);
    myGLCD.printNumI(TempSensorReadsPerSecond, col2 - (myGLCD.getFontXsize() * 2), row, 3, ' ');
    myGLCD.printNumI(BeanOpticalFlowReadsPerSecond, col2 + (myGLCD.getFontXsize() * 2), row, 3, ' ');
    row = row + rowheight;
    myGLCD.print(F("Terrs:"), col, row);
    int R = thermocouples[0].Readingskipped + thermocouples[1].Readingskipped + thermocouples[2].Readingskipped;
    myGLCD.printNumI(R, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("Mem:"), col, row);
    myGLCD.printNumI(FreeMemory, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("SSRs"), col, row);
    myGLCD.print(SSRStatus[SSR1Status].status, col2 - (myGLCD.getFontXsize() * 2), row);
    myGLCD.print(SSRStatus[SSR2Status].status, col2 + (myGLCD.getFontXsize() * 2), row);

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
    myGLCD.printNumI(TCoilAvgRoll.mean(), col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("IEr :"), col, row);
    myGLCD.printNumI(ErrITemp, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("lps/sec:"), col, row);
    myGLCD.printNumI(LoopsPerSecond, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("T&F/s:"), col, row);
    myGLCD.printNumI(TempSensorReadsPerSecond, col2 - (myGLCD.getFontXsize() * 2), row, 3, ' ');
    myGLCD.printNumI(BeanOpticalFlowReadsPerSecond, col2 + (myGLCD.getFontXsize() * 2), row, 3, ' ');
    row = row + rowheight;
    //myGLCD.print(F("Skiptemps:"), col, row);
    int R = thermocouples[0].Readingskipped + thermocouples[1].Readingskipped + thermocouples[2].Readingskipped;
    myGLCD.printNumI(R, col2, row, 5, ' ');
    row = row + rowheight;
    //myGLCD.print(F("MemK:"), col, row);
    myGLCD.printNumI(FreeMemory, col2, row, 5, ' ');
    row = row + rowheight;
    myGLCD.print(F("SSRs"), col, row);
    myGLCD.print(SSRStatus[SSR1Status].status, (col2 - (myGLCD.getFontXsize() * 2)), row);
    myGLCD.print(SSRStatus[SSR2Status].status, (col2 + (myGLCD.getFontXsize() * 2)), row);
  }
}

void UpdateFanPWMValuesDisplay(boolean bVALUESONLY) {

  myGLCD.setFont(BigFont);
  // myGLCD.setFont(Retro8x16);
  //int rowheight = 20;
  //int row = (HorControlMenuDef.H * 2) + myGLCD.getFontXsize() - 25;
  int row = HorFanMenuDef.rowstart - myGLCD.getFontXsize() - 2;
  int col = HorFanMenuDef.colstart;
  int col1 = col + myGLCD.getFontXsize() * 3;
  int col2 = col1 + myGLCD.getFontXsize() * 4;

  myGLCD.setBackColor(BLACK);

  myGLCD.setColor(AQUA);
  myGLCD.printNumI(FanSpeed254PWM, col, row, 3, ' ');
  if (bVALUESONLY == false) {
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
    for (int i = 0; i < GraphHistory[HistoryID].ArraySize; i++) {
      GraphHistory[HistoryID].Pixels[i].y = 0;
    }
    GraphHistory[HistoryID].Pixels[0].x = map(X, 0, 800, 0, 255);
    GraphHistory[HistoryID].Pixels[0].y = map(Y, 0, 480, 0, 255);
    GraphHistory[HistoryID].PixelsP = 0;
  } else {
    //Serial.print(F("No history to reset starting lineID"));Serial.println(lineID);
  }
}

void AddLinebyXY(uint16_t& newX, uint16_t& newY, int lineID) {
  myGLCD.setColor(LineColorforLineID[lineID]);  //LineColorforLineID[lineID]);
  //SPDEBUG("Adding to lineid:\t" + String(lineID) + "\t from x:" + String(LastforLineID[lineID].x) + "\ty:" + String((int)LastforLineID[lineID].y) + "\t to x:" + String(newX) + "\t y:" + String(newY));
  myGLCD.drawLine((int)LastforLineID[lineID].x, (int)LastforLineID[lineID].y, (int)newX, (int)newY);
  if (LineBoldforLineID[lineID]) {
    BoldLine(LastforLineID[lineID].x, LastforLineID[lineID].y, newX, newY);
  }
  LastforLineID[lineID].x = newX;
  LastforLineID[lineID].y = newY;
  int HistoryID = GetHistoryIDfromLineID(lineID);
  if (HistoryID > -1) {
    int minX = GraphHistory[HistoryID].pixelperlogging * GraphHistory[HistoryID].PixelsP;
    if (newX >= minX) {
      if (GraphHistory[HistoryID].PixelsP < (GraphHistory[HistoryID].ArraySize - 1)) {
        GraphHistory[HistoryID].PixelsP++;
        //SPDEBUG("Adding to lineid:\t" + String(lineID) + "\tx:" + String(newX) + "\ty:" + String(newY));
        GraphHistory[HistoryID].Pixels[GraphHistory[HistoryID].PixelsP].x = map(newX, 0, 800, 0, 255);
        GraphHistory[HistoryID].Pixels[GraphHistory[HistoryID].PixelsP].y = map(newY, 0, 480, 0, 255);
      } else {
        Serial.print(F("ERROR:LastTemp array size exceeded for LineID:"));
        Serial.println(lineID);
      }
    } else {
      //SPDEBUG("Skipping Adding to lineid:\t" + String(lineID) + " \tskipcount:" + String(GraphHistory[HistoryID].SkipCount));
      //GraphHistory[HistoryID].SkipCount++;
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
  //Serial.print("FanSpeed254PWM:");Serial.print(FanSpeed254PWM);Serial.print(" Time:");Serial.println(timemins);
  uint16_t newY = (uint16_t)YforAFlow(FanSpeed254PWM);
  uint16_t newX = (uint16_t)(FanGraphXStart + (PixelsPerFlowRate * timemins));

  AddLinebyXY(newX, newY, FANSPEEDLINEID);
}

void AddLinebyTimeAndFan(double timemins) {
  //Serial.print("FanSpeed254PWM:");Serial.print(FanSpeed254PWM);Serial.print(" Time:");Serial.println(timemins);
  uint16_t newY = (uint16_t)YforAFlow(FanSpeed254PWM);
  uint16_t newX = (uint16_t)(FanGraphXStart + (PixelsPerFlowRate * timemins));

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
    //SPDEBUG("Redrawing lineid:\t" + String(LineID));
    myGLCD.setColor(LineColorforLineID[LineID]);
    LastforLineID[LineID].x = map(GraphHistory[HistoryID].Pixels[0].x, 0, 255, 0, 800);
    LastforLineID[LineID].y = map(GraphHistory[HistoryID].Pixels[0].y, 0, 255, 0, 480);
    for (int i = 1; i < GraphHistory[HistoryID].PixelsP; i++) {
      //if (GraphHistory[HistoryID].Pixels[i].y > 0) {
      int xMapped = map(GraphHistory[HistoryID].Pixels[i].x, 0, 255, 0, 800);
      int yMapped = map(GraphHistory[HistoryID].Pixels[i].y, 0, 255, 0, 480);
      myGLCD.drawLine((int)LastforLineID[LineID].x, (int)LastforLineID[LineID].y, xMapped, yMapped);
      if (LineBoldforLineID[LineID]) {
        BoldLine(LastforLineID[LineID].x, LastforLineID[LineID].y, GraphHistory[HistoryID].Pixels[i].x, GraphHistory[HistoryID].Pixels[i].y);
      }
      LastforLineID[LineID].x = xMapped;
      LastforLineID[LineID].y = yMapped;
      //} else {
      //SPDEBUG("Exit ReDraw lineid:" + String(LineID) +  " at:" + String(i));
      //break;
      //}
    }
    return;
  } else {
    //Serial.print(F("Draw from  history  not available for lineID"));
    //Serial.println(LineID);
  }
}

void DrawFanSetPoint(int setpoint, uint16_t color, boolean beingmoved) {

  myGLCD.setFont(SmallFont);
  int Y = YforAFlow(FlowSetPoints[setpoint].flow);
  int X = XforAFanMin(FlowSetPoints[setpoint].Minutes);

  int xOffsetCircle = 0;
  int xOffset = 0;
  int xOffset2 = 0;
  int xOffsetmoved = 0;
  switch (setpoint) {
    case 0:
      xOffset = 2;
      xOffset2 = 2;
      xOffsetCircle = 5;
      xOffsetmoved = myGLCD.getFontXsize() * 3;
      break;
    case 1:
    case 2:
      xOffset = -myGLCD.getFontXsize();
      xOffset2 = 0;  //myGLCD.getFontXsize()/2;
      xOffsetmoved = -(myGLCD.getFontXsize() * 3) - 2;
      break;
    case 3:
      xOffset = -(myGLCD.getFontXsize() * 2);
      xOffset2 = -myGLCD.getFontXsize();
      xOffsetmoved = -(myGLCD.getFontXsize() * 3) - 2;
      break;
  }

  if (beingmoved == false) {
    xOffsetmoved = 0;
  }
  int col1 = X + xOffset + xOffsetmoved;
  int col2 = X + xOffset2 + xOffsetmoved;
  int row1 = Y - myGLCD.getFontYsize() - 5;
  int row2 = Y + 6;

  if (beingmoved == true) {
    myGLCD.setColor(BLACK);
    myGLCD.fillRect(col1 - 2, row1 - 6, (col1 + myGLCD.getFontXsize() * 3) + 1, (row2 + (myGLCD.getFontXsize() * 2) + 4));
    myGLCD.setColor(AQUA);
    myGLCD.drawRect(col1 - 2, row1 - 5, (col1 + myGLCD.getFontXsize() * 3) + 1, (row2 + (myGLCD.getFontXsize() * 2) + 4));
  }

  myGLCD.setColor(color);
  myGLCD.fillCircle(X + xOffsetCircle + xOffsetmoved, Y, 5);
  myGLCD.printNumI(FlowSetPoints[setpoint].flow, col1, row1);
  myGLCD.printNumI(FlowSetPoints[setpoint].Minutes, col2, row2);
}

void DrawTempSetPoint(int setpoint, uint16_t color, boolean beingmoved) {
  int Y = YforATemp(MySetPoints[setpoint].Temperature);
  int X = XforATime(MySetPoints[setpoint].Minutes);
  myGLCD.setFont(SmallFont);

  int xOffset = 0;
  int xOffsetmoved = 0;
  int yOffset = 0;
  int circleoffset = 0;
  switch (setpoint) {
    case 0:
      xOffset = myGLCD.getFontXsize() * 0;
      circleoffset = 5;
      xOffsetmoved = myGLCD.getFontXsize() * 4;
      break;
    case 1:
    case 2:
    case 3:
      xOffset = -myGLCD.getFontXsize() * 1.5;
      xOffsetmoved = myGLCD.getFontXsize() * 4;
      break;
    case 4:
      xOffset = -myGLCD.getFontXsize() * 2;
      xOffsetmoved = -myGLCD.getFontXsize() * 4;
      break;
  }
  if (beingmoved == true) {
    xOffset = xOffset + xOffsetmoved;
    circleoffset = circleoffset + xOffsetmoved;
  }
  int row1 = Y - (myGLCD.getFontYsize() * 2) - 5;
  int row2 = Y - (myGLCD.getFontYsize()) - 5;
  int row3 = Y + 2 + 5;
  int col1 = X + xOffset;
  int col2 = X + xOffset + myGLCD.getFontXsize() / 2;
  if (beingmoved == true) {
    myGLCD.setColor(GRAY);
    myGLCD.fillRect(col1 - 3, row1 - myGLCD.getFontXsize(), (col1 + myGLCD.getFontXsize() * 3) + 2, (row3 + (myGLCD.getFontXsize() * 2)));
    myGLCD.setColor(BLACK);
    myGLCD.drawRect(col1 - 3, row1 - myGLCD.getFontXsize(), (col1 + myGLCD.getFontXsize() * 3) + 2, (row3 + (myGLCD.getFontXsize() * 2)));
  }
  myGLCD.setColor(YELLOW);
  myGLCD.fillCircle(X + circleoffset, Y, 5);
  myGLCD.setColor(WHITE);
  myGLCD.print(F("#"), col2, row1);
  myGLCD.printNumI(setpoint, (col2 + myGLCD.getFontXsize()), row1);
  myGLCD.printNumI(MySetPoints[setpoint].Temperature, col1, row2);
  if (MySetPoints[setpoint].Minutes < 10) {
    myGLCD.printNumI(MySetPoints[setpoint].Minutes, col2, row3);
    myGLCD.print(F("m"), col2 + (myGLCD.getFontXsize() * 1), row3);
  } else {
    myGLCD.printNumI(MySetPoints[setpoint].Minutes, col1, row3);
    myGLCD.print(F("m"), col1 + (myGLCD.getFontXsize() * 2), row3);
  }
}

void AddPointbyTimeAndTemp(double timemins, int temp, int lineID, int radius) {
  uint16_t newX = XforATime(timemins);
  uint16_t newY = YforATemp(temp);
  myGLCD.setColor(LineColorforLineID[lineID]);
  //myGLCD.setColor(0,150,0);
  SPDEBUG("addingpoint x:" + String(newX) + "  y:" + String(newY));
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

void MoveAFanPointsPWM(int SetPoint) {
  Serial.print("MoveAPointFan Setpoint:");
  Serial.print(SetPoint);
  Serial.print("ActiveAdjustment.moveamount:");
  Serial.println(ActiveAdjustment.moveamount);
  if (SetPoint > -1) {
    FlowSetPoints[SetPoint].flow = FlowSetPoints[SetPoint].flow + ActiveAdjustment.moveamount;
    EEPROM.put(Flowsetpoints_int_EP[SetPoint], FlowSetPoints[SetPoint]);
    delay(100);
    myGLCD.setFont(SmallFont);
    DrawFanSetPoint(SetPoint, AQUA, true);
  }
}

void MoveAFanPointsTime(int SetPoint) {
  Serial.print("MoveAPointFanPointsTime Setpoint:");
  Serial.print(SetPoint);
  Serial.print("ActiveAdjustment.moveamount:");
  Serial.println(ActiveAdjustment.moveamount);
  if (SetPoint > -1) {
    FlowSetPoints[SetPoint].Minutes = FlowSetPoints[SetPoint].Minutes + ActiveAdjustment.moveamount;
    EEPROM.put(Flowsetpoints_int_EP[SetPoint], FlowSetPoints[SetPoint]);
    delay(100);
    myGLCD.setFont(SmallFont);
    DrawFanSetPoint(SetPoint, AQUA, false);
  }
}

void MoveAPoint(int SetPoint) {
  //Serial.print("MoveAPoint Setpoint:"); Serial.print(SetPoint); Serial.print("ActiveAdjustment.moveamount:"); Serial.println(ActiveAdjustment.moveamount);
  if (SetPoint > -1) {
    MySetPoints[SetPoint].Temperature = MySetPoints[SetPoint].Temperature + ActiveAdjustment.moveamount;
    EEPROM.put(TempSetpoint_int_EP[SetPoint], MySetPoints[SetPoint].Temperature);
    delay(100);
    myGLCD.setFont(SmallFont);
    DrawTempSetPoint(SetPoint, YELLOW, BEINGMOVED);
    // DrawMovedSetPoint(SetPoint);
  }
}

void myGLCD_fillRect(rect& Rect) {
  myGLCD.fillRect(Rect.x, Rect.y, Rect.xmax, Rect.ymax);
}

void myGLCD_fillRect_wBuffer(rect& Rect, byte buffer) {
  myGLCD.fillRect(Rect.x - buffer, Rect.y - buffer, Rect.xmax + buffer, Rect.ymax + buffer);
}
void myGLCD_drawRect(rect& Rect) {
  myGLCD.drawRect(Rect.x, Rect.y, Rect.xmax, Rect.ymax);
}

void myGLCD_printNumF(double Number, int col, int row, int Len, int Dec) {

  dtostrf(Number, Len, Dec, CharArrays[Len - 1]);
  myGLCD.print(CharArrays[Len - 1], col, row);
  return;
  if (Len == 5) {
    dtostrf(Number, Len, Dec, s6);
    myGLCD.print(s6, col, row);
  } else if (Len == 6) {
    dtostrf(Number, Len, Dec, s7);
    myGLCD.print(s7, col, row);
  }
}

void UpdateDisplayBoxLocation(byte DisplayIDBeingMoved) {
  displaybox* BoxedBeingMoved = &DisplayBoxes[DisplayIDBeingMoved];
  //blank out the prior box
  myGLCD.setColor(BLACK);

  myGLCD_fillRect_wBuffer(BoxedBeingMoved->Rect, 1);
  myGLCD.setColor(PALEYELLOW);

  int revisedDragx = TouchStatus.dragx;
  int revisedDragy = TouchStatus.dragy;

  SPDEBUG("Draw new id:" + String(BoxedBeingMoved->DisplayboxID) + " x delta:" + String(revisedDragx) + " y delta:" + String(revisedDragy));
  BoxedBeingMoved->Rect.x = BoxedBeingMoved->Rect.x + revisedDragx;
  BoxedBeingMoved->Rect.y = BoxedBeingMoved->Rect.y + revisedDragy;
  BoxedBeingMoved->Rect.xmax = BoxedBeingMoved->Rect.xmax + revisedDragx;
  BoxedBeingMoved->Rect.ymax = BoxedBeingMoved->Rect.ymax + revisedDragy;
  SPDEBUG("Draw new: xnew:" + String(BoxedBeingMoved->Rect.x) + " y new:" + String(BoxedBeingMoved->Rect.y));
TouchStatus.objectpressID = PressNone;
  switch (BoxedBeingMoved->DisplayboxID) {
    case OpProgessDisplay:
      {

        break;
      }

    case ConfigDisplay:
      {
        UpdateConfigsDisplayArea(ALL);

        break;
      }

    case OpDetailDisplay:
      {
        UpdateOpDetailsDisplayArea(ALL);

        break;
      }
  }
  
  EEPROM.put(BoxedBeingMoved->X_EP, BoxedBeingMoved->Rect.x);
  EEPROM.put(BoxedBeingMoved->Y_EP, BoxedBeingMoved->Rect.y);
}
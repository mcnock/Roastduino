
//-------------------------------------------------  ---------------------------------------------------------------------------------------------------
// MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS
//----------------------------------------------------------------------------------------------------------------------------------------------------
int vButWidth = 60;
int hButWidth = 70;
int ButHeight = 53;
void intializeMenus() {
  for (int i = 0; i < VmenuCount; i++) {
    //Serial.print("VMenu");Serial.println(i);
    //this make a name
    buttonsetdef* bsd = &myButtonVertMenus[i];
    bsd->ButtonCount = 9;
    bsd->rowstart = 0;
    bsd->W = vButWidth;
    bsd->H = ButHeight;
    bsd->vertical = true;
    bsd->colstart = myGLCD.getDisplayXSize() - bsd->W - 2;
    bsd->menuID = i;
    //bsd->pClickHandler = ProcessVmenuButtonClick;
    SetMenuBoundingRect(myButtonVertMenus[i]);
  }

  //myButtonVertMenus[VmenuEmpty].ButtonCount = 1;

  myHorControlMenuDef.H = ButHeight;
  myHorControlMenuDef.W = hButWidth;
  myHorControlMenuDef.rowstart = 0;
  myHorControlMenuDef.colstart = 360;
  myHorControlMenuDef.vertical = false;
  myHorControlMenuDef.ButtonCount = 6;
  myHorControlMenuDef.menuID = HmenuCTRL;
  myHorControlMenuDef.pClickHandler = ProcessHorControlMenu;

  myHorControlMenuDef.colstart = (800 - vButWidth - (hButWidth * myHorControlMenuDef.ButtonCount));

  SetMenuBoundingRect(myHorControlMenuDef);

  myHorFanButtonControl.H = 40;
  myHorFanButtonControl.W = 50;
  myHorFanButtonControl.rowstart = 480 - 40 - 20;
  myHorFanButtonControl.colstart = 800 - (40 * 6) - vButWidth;
  myHorFanButtonControl.ButtonCount = 5;
  myHorFanButtonControl.vertical = false;
  myHorFanButtonControl.menuID = HmenuFAN;
  SetMenuBoundingRect(myHorFanButtonControl);

  myHorFanButtonControl.pClickHandler = ProcessHorFanMenu;
}

void DrawHorControlMenu() {
  DrawMenuButtons(myHorControlMenuDef);
}

void ProcessHorControlMenu(int i) {
  //Serial.print("ProcessConrol");Serial.println (i);
  if (ErrorStatus.error == ErrorNeedFanFirst) {
    ErrorStatus.lasterror = ErrorStatus.error;
    ErrorStatus.error = NoError;
    ErrorStatus.newerrmsg = true;
    UpdateErrorDisplayArea(ValuesOnly);
  }
  switch (i) {
    case 0:
      if (State == STATEFANONLY) {
        newState = STATEROASTING;
      } else if ((State == STATENOFANCURRENT || State == STATEOVERHEATED || State == STATECOOLING)) {
        newState = STATEROASTING;
        //Serial.println("ReStart Detected!");
      } else {
        ErrorStatus.newerrmsg = true;
        ErrorStatus.error = ErrorNeedFanFirst;
        UpdateErrorDisplayArea(ValuesOnly);
      }
      break;
    case 1:
      if (State != STATESTOPPED) {
        newState = STATESTOPPED;
      }
      Serial.println(F("Stop Called!"));
      break;
    case 2:
      if (State == STATESTOPPED) {
        newState = STATEFANONLY;
      }

      break;
    case 3:
    

      graphProfile();

      break;
    case 4:  //rtd roaste
      if (State == STATEROASTING || State == STATECOOLING) {
        RoastTime.add(60);
      }
      break;
    case 5:  //advancee roaste
      if (State == STATEROASTING && RoastTime.elapsed() > 61) {
        RoastTime.add(-60);

      } else if (State == STATECOOLING && RoastTime.elapsed() > 61) {
        RoastTime.add(-60);
        if (RoastTime.elapsed() / 60 < MySetPoints[SetPointCount - 1].Minutes) {
          //SpDebug("Adding 1 minute roast time when cooling. restartng roast requested");
          RoastRestartNeeded = true;
        } else {
          //SpDebug("Adding 1 minute roast time when cooling. restartng roast not yet neccesary");
        }
      }

      break;
    case 6:

      break;
  }
}

void DrawHorFanMenu() {

  DrawMenuButtons(myHorFanButtonControl);
}

void ProcessHorFanMenu(int i) {
  //Serial.print("ProcessFanMenu:");Serial.println (i);
  int change = 0;
  switch (i) {
    case 0:
      break;
    case 1:
      //decrease quickly
      change = -5;
      break;
    case 2:
      change = -1;
      break;
    case 3:
      change = 1;
      break;
    case 4:
      change = 5;
      break;
    case 5:

      break;
  }
  if (change != 0) {

    switch (State) {
      case STATEROASTING:
      case STATEFANONLY:
      case STATECOOLING:
        FanDeviation = FanDeviation + change;
        SetAndSendFanPWMForATime(RoastMinutes);

        break;
    }
  }
}

void DrawVMenu(int iMenu) {
  bool menuchange = true;
  //spDebug ("DrawVmenu:" + String(iMenu));
  if (iMenu == -1) {
    iMenu = VmenuEmpty;
    MenuStatus.VmenuPrior = VmenuBase;
    MenuStatus.VmenuShowing = VmenuEmpty;
    menuchange = false;
  } else {
    //SpDebug("Setting prior from " + String(MenuStatus.VmenuPrior) + " to :" + String(MenuStatus.VmenuShowing));
    if (MenuStatus.VmenuShowing == iMenu) {
      menuchange = false;
    } else {
      MenuStatus.VmenuPrior = MenuStatus.VmenuShowing;
      MenuStatus.VmenuShowing = iMenu;
      MenuStatus.VmenubuttonClicked = MenuStatus.ButtonClicked;
    }
  }

  //boolean bRedraw = false;
  if (iMenu == VmenuEmpty) {
    if (menuchange == true) {
      myGLCD.setColor(BLACK);
      int xstart = 800 - myButtonVertMenus[VmenuBase].W;
      //myGLCD.drawRect(xstart,myButtonVertMenus[Vmenubase].H,800,480);
      myGLCD.fillRect(xstart - 1, 0, 800, 480);
      // bRedraw = true;
      myGLCD.setFont(SmallFont);

      for (int i = 0; i < HorScaleLineYCount; i++) {
        //SpDebug("menuRedrawYScale:" + String(i)+ " x:"+  String(xstart) + " y:" + String(HorScaleLineY[i]));
        myGLCD.setColor(LIGHTGRAY);
        myGLCD.drawLine(xstart - 2, HorScaleLineY[i], 800 - 30, HorScaleLineY[i]);
        myGLCD.setColor(GRAY);
        myGLCD.printNumI(HorScaleLineYValue[i], 800 - 30, HorScaleLineY[i] - 5);
      }
      UpdateFanPWMValuesDisplay(All);
      DrawHorFanMenu();
    }
  }

  DrawMenuButtons(myButtonVertMenus[iMenu]);

  //if (bRedraw) {
  //  UpdateOpDetailsDisplayArea(All);
  //  graphFanProfile();
  //}
}

void ProcessVmenuButtonClick() {
  MenuStatus.VmenubuttonClicked = MenuStatus.ButtonClicked;
  buttondef mybutton = myButtonVertMenus[MenuStatus.VmenuShowing].buttondefs[MenuStatus.VmenubuttonClicked];
  //SpDebug("ProcessVmenuButtonClick  button id:" + String(mybutton.butID) + " action:" + String(mybutton.action) + " adjustmentvalueset:" + String(mybutton.adjustmentvalueset));
  int i = MenuStatus.ButtonClicked;
  //SpDebug("ProcessVmenuButtonClick_ button id " + String(mybutton.butID) +  " action:" + String(mybutton.action) + " adjustmentvalueset:" + String(mybutton.adjustmentvalueset));
  //SpDebug("ProcessVmenuButtonClick_ buttonclicked " + String(MenuStatus.ButtonClicked));
  if (MenuStatus.ButtonClicked == 0) {
    if (MenuStatus.VmenuShowing == VmenuSetPointSelect) {
      for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++) {
        MySetPoints[xSetPoint].TemperatureNew = 0;
      }
      ActiveAdjustment.spSelected = -1;
    }
    if (mybutton.action == VmenuFindPrior) {
      DrawVMenu(MenuStatus.VmenuPrior);
    } else {
      DrawVMenu(mybutton.action);
    }
    return;
  }
  if (mybutton.action > ActionAdjustments) {
    //SpDebug("button.action:" + String(mybutton.action) + " adjustmentvalueset:" + String(mybutton.adjustmentvalueset));
    //set up editing of any value
    ActiveAdjustment.spSelected = -1;
    ActiveAdjustment.ButtonWhenCalled = MenuStatus.ButtonClicked;
    ActiveAdjustment.VmenuWhenCalled = MenuStatus.VmenuShowing;
    ActiveAdjustment.name = mybutton.action;
    ActiveAdjustment.adjustmentvalueset = mybutton.adjustmentvalueset;
    DrawVMenu(VmenuAdjustValue);
    return;
  }
  switch (mybutton.action) {
    case ActionSelectAdustmentValue:
      ProcessAnAdjustment();
      break;
    case ActionShowSetpointSelectMenu:
      DrawVMenu(VmenuSetPointSelect);
      break;
    case ActionShowSetpointFanMenu:
      DrawVMenu(VmenuFan);
      break;
    case ActionResetToDefaultOnNextStart:
      if (ErrorStatus.error == ErrorPleaseRestart) {
        EEPROM.update(SETDEFAULT_EP, (byte)1);
        ErrorStatus.newerrmsg = true;
        ErrorStatus.error = NoError;
        UpdateErrorDisplayArea(ValuesOnly);

      } else {
        EEPROM.put(SETDEFAULT_EP, 0);
        ErrorStatus.newerrmsg = true;
        ErrorStatus.error = ErrorPleaseRestart;
        UpdateErrorDisplayArea(ValuesOnly);
      }

      break;


    default:
      break;
  }
}

void ProcessAnAdjustment() {
  //SpDebug("ProcessAdjustmentMenuClick  VmenuWhenCalled:" + String(ActiveAdjustment.VmenuWhenCalled) + " adjustment name:" + String(ActiveAdjustment.name) + " adjustmentvalueset:" + String(ActiveAdjustment.adjustmentvalueset) + "  button:" + String(MenuStatus.VmenubuttonClicked));
  if (ActiveAdjustment.VmenuWhenCalled != MenuStatus.VmenuShowing) {
    //possible error
  }
  ActiveAdjustment.moveamount = 0;
  switch (MenuStatus.VmenubuttonClicked) {
    case 2:
      ActiveAdjustment.moveamount = AdustmentValues[ActiveAdjustment.adjustmentvalueset][0];
      break;
    case 3:
      ActiveAdjustment.moveamount = AdustmentValues[ActiveAdjustment.adjustmentvalueset][1];
      break;
    case 4:
      ActiveAdjustment.moveamount = AdustmentValues[ActiveAdjustment.adjustmentvalueset][2];
      break;
    case 5:
      ActiveAdjustment.moveamount = -AdustmentValues[ActiveAdjustment.adjustmentvalueset][2];
      break;
    case 6:
      ActiveAdjustment.moveamount = -AdustmentValues[ActiveAdjustment.adjustmentvalueset][1];
      break;
    case 7:
      ActiveAdjustment.moveamount = -AdustmentValues[ActiveAdjustment.adjustmentvalueset][0];
      break;
    default:
      break;
  }
  //SpDebug("move amount for button click will be " + String(ActiveAdjustment.moveamount));
  if (ActiveAdjustment.moveamount == 0) {
    return;
  }
  int spSelected;
  switch (ActiveAdjustment.name) {
    case ActionAdjustIntegralTemp:
      IntegralTemp = IntegralTemp + ActiveAdjustment.moveamount;
      if (IntegralTemp < 0.0) {
        IntegralTemp = 0.0;
      }
      EEPROM.put(INTEGRALTEMP_EP, IntegralTemp);
      UpdateConfigsDisplayArea(ValuesOnly);
      break;
    case ActionAdjustGainTemp:
      //SpDebug("here");
      GainTemp = GainTemp + ActiveAdjustment.moveamount;
      if (GainTemp < 10) {
        GainTemp = 0;
      }
      EEPROM.put(GAINTEMP_EP, GainTemp);
      UpdateConfigsDisplayArea(ValuesOnly);
      break;
    case ActionAdjustSetpointTemp:
      setpointschanged = true;

      if ((ActiveAdjustment.ButtonWhenCalled >= 1) & (ActiveAdjustment.ButtonWhenCalled <= 5)) {
        //SpDebug("set point selected is:" + String(spSelected));
        spSelected = ActiveAdjustment.ButtonWhenCalled - 1;
        MoveAPoint(spSelected);
      } else if (ActiveAdjustment.ButtonWhenCalled == 6) {
        MoveAPoint(SetPointCount - 1);
        MoveAPoint(SetPointCount - 2);
        setpointschanged = true;

      } else if (ActiveAdjustment.ButtonWhenCalled == 7) {
        MoveAPoint(SetPointCount - 1);
        MoveAPoint(SetPointCount - 2);
        MoveAPoint(SetPointCount - 3);
       
        setpointschanged = true;
      }
      break;
    case ActionAdjustTempToHot:
      TEMPCOILTOOHOT = TEMPCOILTOOHOT + ActiveAdjustment.moveamount;
      EEPROM.put(TOOHOTTEMP_EP, TEMPCOILTOOHOT);
      UpdateConfigsDisplayArea(ValuesOnly);
      break;
    case ActionAdjustCoolDownTemp:
      TEMPCOOLINGDONE = TEMPCOOLINGDONE + ActiveAdjustment.moveamount;
      EEPROM.put(COOLDOWNTEMP_EP, TEMPCOOLINGDONE);
      UpdateConfigsDisplayArea(ValuesOnly);
      //#define COOLDOWNTEMP_EP 64
      break;
    case ActionAdjustFanGraphPixelBottom:
      FanGraphBottom = FanGraphBottom - ActiveAdjustment.moveamount;
      EEPROM.put(FANGRAPHBOTTOM_EP, FanGraphBottom);
      UpdateConfigsDisplayArea(ValuesOnly);
      //FANGRAPHBOTTOM_EP
      break;
    case ActionAdjustSetpointFan:
      if ((ActiveAdjustment.ButtonWhenCalled >= 0) & (ActiveAdjustment.ButtonWhenCalled <= 4)) {
        spSelected = ActiveAdjustment.ButtonWhenCalled - 1;
        MoveAFanPointsPWM(spSelected);
      }
      if ((ActiveAdjustment.ButtonWhenCalled >= 5) & (ActiveAdjustment.ButtonWhenCalled <= 6)) {
        spSelected = ActiveAdjustment.ButtonWhenCalled - 4;
        MoveAFanPointsTime(spSelected);
      }

      break;
    default:
      break;
  }
}

void SetMenuBoundingRect(struct buttonsetdef& butdefset) {
  for (int i = 0; i < butdefset.ButtonCount; i++) {
    if (butdefset.vertical == true) {
      butdefset.buttondefs[i].Rect.y = butdefset.rowstart + butdefset.H * i;
      butdefset.buttondefs[i].Rect.x = butdefset.colstart;
    } else {
      butdefset.buttondefs[i].Rect.y = butdefset.rowstart;
      butdefset.buttondefs[i].Rect.x = butdefset.colstart + butdefset.W * i;
    }
    //butdefset.buttondefs[i].h = butdefset.H;  //CAN BE REFACTORED OUT
    //butdefset.buttondefs[i].w = butdefset.W;  //CAN BE REFACTORED OUT
    butdefset.buttondefs[i].Rect.xmax = butdefset.buttondefs[i].Rect.x + butdefset.W;
    butdefset.buttondefs[i].Rect.ymax = butdefset.buttondefs[i].Rect.y + butdefset.H;
  }
  if (butdefset.vertical == false) {
    //Serial.println("Here");
    //Serial.println(butdefset.colstart);
    butdefset.bounding.x = butdefset.colstart;
    //Serial.println(butdefset.W);

    butdefset.bounding.xmax = butdefset.colstart + butdefset.W * butdefset.ButtonCount;
    //Serial.println(butdefset.rowstart);

    butdefset.bounding.y = butdefset.rowstart;
    //Serial.println(butdefset.H);

    butdefset.bounding.ymax = butdefset.rowstart + butdefset.H;
  } else {
    butdefset.bounding.x = butdefset.colstart;
    butdefset.bounding.xmax = butdefset.colstart + butdefset.W;
    butdefset.bounding.y = butdefset.rowstart;
    butdefset.bounding.ymax = butdefset.rowstart + butdefset.H * butdefset.ButtonCount;
  }
}

void DrawMenuButtons(buttonsetdef& butdefset) {
  if (butdefset.ButtonCount == -1) {

    for (int i = 0; i < MaxButtonCount; i++) {
      DrawMenuButton(butdefset, i, false);
    }
    //SpDebug("cacled menu button count for menu " + String(butdefset.menuID) + " is:" + String(butdefset.ButtonCount));
  } else {
    //SpDebug("found a menu button count for menu " + String(butdefset.menuID) + " is:" + String(butdefset.ButtonCount));

    for (int i = 0; i < butdefset.ButtonCount; i++) {
      DrawMenuButton(butdefset, i, false);
    }
  }
}

void DrawMenuButton(buttonsetdef& butdefset, int i, boolean toggletooltip) {
  //SpDebug("drawing menuid " + String(butdefset.menuID) + " button index " + String(i) + " with butID " + String(mybut.butID) );
  if (butdefset.buttondefs[i].butID == -1) {
    //SpDebug("looking up details");
    memcpy_P(&myLocalbuttontext, &Vmenutext[butdefset.menuID][i], sizeof(buttontext));
    butdefset.buttondefs[i].action = myLocalbuttontext.action;
    butdefset.buttondefs[i].adjustmentvalueset = myLocalbuttontext.adjustmentvalueset;
    butdefset.buttondefs[i].butID = myLocalbuttontext.butID;
    //SpDebug(" found action " + String(butdefset.buttondefs[i].action) + " and butID " + String(butdefset.buttondefs[i].butID) );
    //SpDebug("A is adjustment:" + String(butdefset.IsAdjustment) + " button id" + String(i));
    if (butdefset.buttondefs[i].action == ActionGetLableFromPrior && butdefset.buttondefs[i].ToolTipShowing == toggletooltip) {
      memcpy_P(&myLocalbuttontext, &Vmenutext[MenuStatus.VmenuPrior][MenuStatus.VmenubuttonClicked], sizeof(buttontext));
    }
  } else {
    if (butdefset.buttondefs[i].action == ActionGetLableFromPrior && butdefset.buttondefs[i].ToolTipShowing == toggletooltip) {
      memcpy_P(&myLocalbuttontext, &Vmenutext[MenuStatus.VmenuPrior][MenuStatus.VmenubuttonClicked], sizeof(buttontext));
    } else {
      memcpy_P(&myLocalbuttontext, &Vmenutext[butdefset.menuID][i], sizeof(buttontext));
    }
  }
  if (butdefset.buttondefs[i].butID < 0) {
    return;
  }

  if (butdefset.buttondefs[i].action == ActionSelectAdustmentValue) {
    switch (i) {
      case 2:
        strcpy(myLocalbuttontext.label, "+");
        strcat(myLocalbuttontext.label, AdustmentValuesLabels[ActiveAdjustment.adjustmentvalueset][0].label);
        break;
      case 3:
        strcpy(myLocalbuttontext.label, "+");
        strcat(myLocalbuttontext.label, AdustmentValuesLabels[ActiveAdjustment.adjustmentvalueset][1].label);
        break;
      case 4:
        strcpy(myLocalbuttontext.label, "+");
        strcat(myLocalbuttontext.label, AdustmentValuesLabels[ActiveAdjustment.adjustmentvalueset][2].label);
        break;
      case 5:
        strcpy(myLocalbuttontext.label, "-");
        strcat(myLocalbuttontext.label, AdustmentValuesLabels[ActiveAdjustment.adjustmentvalueset][2].label);
        break;
      case 6:
        strcpy(myLocalbuttontext.label, "-");
        strcat(myLocalbuttontext.label, AdustmentValuesLabels[ActiveAdjustment.adjustmentvalueset][1].label);
        break;
      case 7:
        strcpy(myLocalbuttontext.label, "-");
        strcat(myLocalbuttontext.label, AdustmentValuesLabels[ActiveAdjustment.adjustmentvalueset][0].label);
        break;
      default:
        break;
    }
  }


  //Serial.print(" id:");Serial.print(myLocalbuttontext.key);Serial.print(" label:");Serial.println(myLocalbuttontext.label);
  myGLCD.setColor(myLocalbuttontext.color);
  myGLCD_fillRect(butdefset.buttondefs[i].Rect);
  myGLCD.setColor(BLACK);
  myGLCD_drawRect(butdefset.buttondefs[i].Rect);
  myGLCD.setBackColor(myLocalbuttontext.color);
  myGLCD.setColor(BLACK);
  int xOffset = 0;
  int yOffset = 0;
  int xT = 0;

  if (toggletooltip == false || butdefset.buttondefs[i].ToolTipShowing == true) {
    //Serial.print("gettting button text for VmenuID:");Serial.print(butdefset.menuID);Serial.print(" button:");Serial.println(i);
    butdefset.buttondefs[i].ToolTipShowing = false;
    if (butdefset.vertical == true && i == 0) {
      myGLCD.setFont(Font16x16);
    } else {
      myGLCD.setFont(Retro8x16);  //main button. 6 chars.
    }
    xOffset = (butdefset.W - (strlen(myLocalbuttontext.label) * myGLCD.getFontXsize())) / 2;
    yOffset = (butdefset.H - myGLCD.getFontYsize()) / 2;

    if (myLocalbuttontext.butID == DrawLablesRotated) {
      //spDebug("myGLCD.getFontXsize:" + String(myGLCD.getFontXsize()) + " myGLCD.getFontYsize:" + String(myGLCD.getFontYsize()) + " strlen:" +  String(strlen(myLocalbuttontext.label)));
      int xOffsetrotated = butdefset.W / 2 + (myGLCD.getFontXsize() / 2);
      int yOffsetrotated = butdefset.H / 2 - (myGLCD.getFontYsize() * .7);
      myGLCD.print(myLocalbuttontext.label, butdefset.buttondefs[i].Rect.x + xOffsetrotated, butdefset.buttondefs[i].Rect.y + yOffsetrotated, 90);
    } else {
      myGLCD.print(myLocalbuttontext.label, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset, 0);
    }
  } else {  //tool tip 2 or 3 line x 12 char
    myGLCD.setFont(Retro8x16);
    xOffset = (butdefset.W - (strlen(myLocalbuttontext.tip1) * myGLCD.getFontXsize())) / 2;
    butdefset.buttondefs[i].ToolTipShowing = true;
    if (butdefset.H >= 50) {
      xT = 10;
      yOffset = (butdefset.H / 3 - myGLCD.getFontYsize()) / 2;
      myGLCD.print(myLocalbuttontext.tip1, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset);
      xOffset = (butdefset.W + xT - (strlen(myLocalbuttontext.tip2) * myGLCD.getFontXsize())) / 2;
      myGLCD.print(myLocalbuttontext.tip2, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset + butdefset.H / 3);
      xOffset = (butdefset.W - (strlen(myLocalbuttontext.tip3) * myGLCD.getFontXsize())) / 2;
      myGLCD.print(myLocalbuttontext.tip3, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset + (butdefset.H / 3 * 2));
    } else {
      yOffset = (butdefset.H / 2 - myGLCD.getFontYsize()) / 2;
      myGLCD.print(myLocalbuttontext.tip1, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset);
      xOffset = (butdefset.W - (strlen(myLocalbuttontext.tip2) * myGLCD.getFontXsize())) / 2;
      myGLCD.print(myLocalbuttontext.tip2, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset + butdefset.H / 2);
    }
  }
}

void OutlineClickedButton(uint16_t color) {
  uint16_t priorcolor = myGLCD.getColor();
  myGLCD.setColor(color);
  myGLCD_drawRect(MenuStatus.TouchButtonSet->buttondefs[MenuStatus.ButtonClicked].Rect);
  //myGLCD_drawRect(butdefset->buttondefs[i].Rect);
  myGLCD.setColor(priorcolor);
}

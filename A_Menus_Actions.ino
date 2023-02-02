
//-------------------------------------------------  ---------------------------------------------------------------------------------------------------
// MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS
//----------------------------------------------------------------------------------------------------------------------------------------------------
int vButWidth = 60;
int hButWidth = 70;
int ButHeight = 53;
void intializeMenus() {
  for (int i = 0; i < VMENUCOUNT; i++) {
    //Serial.print("VMenu");Serial.println(i);
    //this make a name
    buttonsetdef* bsd = &VertMenuDefs[i];
    bsd->ButtonCount = 9;
    bsd->rowstart = 0;
    bsd->W = vButWidth;
    bsd->H = ButHeight;
    bsd->vertical = true;
    bsd->colstart = myGLCD.getDisplayXSize() - bsd->W - 2;
    bsd->menuID = i;
    //bsd->pClickHandler = ProcessVmenuButtonClick;
    SetMenuBoundingRect(VertMenuDefs[i]);
  }
  HorControlMenuDef.H = ButHeight;
  HorControlMenuDef.W = hButWidth;
  HorControlMenuDef.rowstart = 0;
  HorControlMenuDef.colstart = 360;
  HorControlMenuDef.vertical = false;
  HorControlMenuDef.ButtonCount = 6;
  HorControlMenuDef.menuID = HMENUCTRL;
  HorControlMenuDef.pClickHandler = ProcessHorControlMenu;
  HorControlMenuDef.colstart = (800 - vButWidth - (hButWidth * HorControlMenuDef.ButtonCount));

  SetMenuBoundingRect(HorControlMenuDef);

}
void DrawHorControlMenu() {
  DrawMenuButtons(HorControlMenuDef);
}
void ProcessHorControlMenu(int i) {
  //Serial.print("ProcessConrol");Serial.println (i);
  if (ErrorStatus.error == ErrorNeedFanFirst) {
    ErrorStatus.lasterror = ErrorStatus.error;
    ErrorStatus.error = NoError;
    ErrorStatus.newerrmsg = true;
    UpdateErrorDisplayArea(VALUESONLY);
  }
  switch (i) {
    case 0:
      if (State == STATEFANONLY) {
        NewState = STATEROASTING;
      } else if ((State == STATENOFANCURRENT || State == STATEOVERHEATED || State == STATECOOLING)) {
        NewState = STATEROASTING;
        //Serial.println("ReStart Detected!");
      } else {
        ErrorStatus.newerrmsg = true;
        ErrorStatus.error = ErrorNeedFanFirst;
        UpdateErrorDisplayArea(VALUESONLY);
      }
      break;
    case 1:
      if (State != STATESTOPPED) {
        NewState = STATESTOPPED;
      }
      SERIALPRINTLN_OP(F("Stop Called!"));
      break;
    case 2:
      if (State == STATESTOPPED) {
        NewState = STATEFANONLY;
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
          //SPDEBUG("Adding 1 minute roast time when cooling. restartng roast requested");
          RoastRestartNeeded = true;
        } else {
          //SPDEBUG("Adding 1 minute roast time when cooling. restartng roast not yet neccesary");
        }
      }

      break;
    case 6:

      break;
  }
}

void DrawVMenu(int iMenu) {
  bool menuchange = true;
  //SPDEBUG ("DrawVmenu:" + String(iMenu));
  if (iMenu == -1) {
    iMenu = VMENUEMPTY;
    MenuStatus.VmenuPrior = VMENUBASE;
    MenuStatus.VmenuShowing = VMENUEMPTY;
    menuchange = false;
  } else {
    //SPDEBUG("Setting prior from " + String(MenuStatus.VmenuPrior) + " to :" + String(MenuStatus.VmenuShowing));
    if (MenuStatus.VmenuShowing == iMenu) {
      menuchange = false;
    } else {
      MenuStatus.VmenuPrior = MenuStatus.VmenuShowing;
      MenuStatus.VmenuShowing = iMenu;
      MenuStatus.VmenubuttonClicked = MenuStatus.ButtonClicked;
    }
  }

  //boolean bRedraw = false;
  if (iMenu == VMENUEMPTY) {
    if (menuchange == true) {
      myGLCD.setBackColor(BLACK);

      myGLCD.setColor(BLACK);
      int xstart = 800 - VertMenuDefs[VMENUBASE].W;
      //myGLCD.drawRect(xstart,VertMenuDefs[Vmenubase].H,800,480);
      myGLCD.fillRect(xstart - 1, 0, 800, 480);
      // bRedraw = true;
      myGLCD.setFont(SmallFont);

      for (int i = 0; i < HorScaleLineYCount; i++) {
        //SPDEBUG("menuRedrawYScale:" + String(i)+ " x:"+  String(xstart) + " y:" + String(HorScaleLineY[i]));
        myGLCD.setColor(LIGHTGRAY);
        myGLCD.drawLine(xstart - 2, HorScaleLineY[i], 800 - 30, HorScaleLineY[i]);
        myGLCD.setColor(GRAY);
        myGLCD.printNumI(HorScaleLineYValue[i], 800 - 30, HorScaleLineY[i] - 5);
      }
     
    }
  }

  DrawMenuButtons(VertMenuDefs[iMenu]);
}

void ProcessVmenuButtonClick() {
  MenuStatus.VmenubuttonClicked = MenuStatus.ButtonClicked;
  buttondef mybutton = VertMenuDefs[MenuStatus.VmenuShowing].buttondefs[MenuStatus.VmenubuttonClicked];
  //SPDEBUG("ProcessVmenuButtonClick  button id:" + String(mybutton.butID) + " action:" + String(mybutton.action) + " adjustmentvalueset:" + String(mybutton.adjustmentvalueset));
  int i = MenuStatus.ButtonClicked;
  //SPDEBUG("ProcessVmenuButtonClick_ button id " + String(mybutton.butID) +  " action:" + String(mybutton.action) + " adjustmentvalueset:" + String(mybutton.adjustmentvalueset));
  //SPDEBUG("ProcessVmenuButtonClick_ buttonclicked " + String(MenuStatus.ButtonClicked));
  if (MenuStatus.ButtonClicked == 0) {  // user clicked on top button
    if (MenuStatus.VmenuShowing == VMENUADJUSTVALUE) {
      switch (ActiveAdjustment.name) {
        case ACTIONADJUSTFANMANUAL:
          {
            if (FanManual == true) { FanManual = false; }
            break;
          }
        case ACTIONADJUSTTIMEMANUAL:
          {
            if (TimeManual == true) { TimeManual = false; }
            break;
          }
      }
    }
    if (MenuStatus.VmenuShowing == VMENUSETPOINTSELECT) {
      ActiveAdjustment.spSelected = -1;
    }
    if (MenuStatus.VmenuShowing == VMENUDEBUG) {
      StateDebug = 0;
    }

    if (mybutton.action == VMENUFINDPRIOR) {
      DrawVMenu(MenuStatus.VmenuPrior);
    } else {
      DrawVMenu(mybutton.action);
    }

    return;
  }
  if (mybutton.action > ACTIONADJUSTMENTS) {
    //SPDEBUG("button.action:" + String(mybutton.action) + " adjustmentvalueset:" + String(mybutton.adjustmentvalueset));
    //set up editing of any value
    ActiveAdjustment.spSelected = -1;
    ActiveAdjustment.ButtonWhenCalled = MenuStatus.ButtonClicked;
    ActiveAdjustment.VmenuWhenCalled = MenuStatus.VmenuShowing;
    ActiveAdjustment.name = mybutton.action;
    ActiveAdjustment.adjustmentvalueset = mybutton.adjustmentvalueset;
    DrawVMenu(VMENUADJUSTVALUE);
    return;
  }
  switch (mybutton.action) {
    case ACTIONSELECTADUSTMENTVALUE:
      ProcessAnAdjustment();
      break;
    case ACTIONSHOWSETPOINTSELECTMENU:
      DrawVMenu(VMENUSETPOINTSELECT);
      break;
    case ACTIONSHOWSETPOINTFANMENU:
      DrawVMenu(VMENUFAN);
      break;
    case ACTIONRESETTODEFAULTONNEXTSTART:
      if (ErrorStatus.error == ErrorPleaseRestart) {
        EEPROM.put(LOADDEFAULTS_EP, false);  //undo
        ErrorStatus.newerrmsg = true;
        ErrorStatus.error = NoError;
        UpdateErrorDisplayArea(VALUESONLY);
      } else {
        EEPROM.put(LOADDEFAULTS_EP, true);
        ErrorStatus.newerrmsg = true;
        ErrorStatus.error = ErrorPleaseRestart;
        UpdateErrorDisplayArea(VALUESONLY);
      }

      break;


    default:
      break;
  }
}

void ProcessAnAdjustment() {
  //SPDEBUG("ProcessAdjustmentMenuClick  VmenuWhenCalled:" + String(ActiveAdjustment.VmenuWhenCalled) + " adjustment name:" + String(ActiveAdjustment.name) + " adjustmentvalueset:" + String(ActiveAdjustment.adjustmentvalueset) + "  button:" + String(MenuStatus.VmenubuttonClicked));
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
  //SPDEBUG("move amount for button click will be " + String(ActiveAdjustment.moveamount));
  if (ActiveAdjustment.moveamount == 0) {
    return;
  }
  int spSelected;
  switch (ActiveAdjustment.name) {
    case ACTIONADJUSTINTEGRALTEMP:
      IntegralTemp = IntegralTemp + ActiveAdjustment.moveamount;
      if (IntegralTemp < 0.0) {
        IntegralTemp = 0.0;
      }
      EEPROM.put(INTEGRALTEMP_EP, IntegralTemp);
      UpdateConfigsDisplayArea(VALUESONLY);
      break;
    case ACTIONADJUSTGAINTEMP:
      //SPDEBUG("here");
      GainTemp = GainTemp + ActiveAdjustment.moveamount;
      if (GainTemp < 10) {
        GainTemp = 0;
      }
      EEPROM.put(GAINTEMP_EP, GainTemp);
      UpdateConfigsDisplayArea(VALUESONLY);
      break;

    case ACTIONADJUSTSETPOINTTEMP:
      Setpointschanged = true;

      if ((ActiveAdjustment.ButtonWhenCalled >= 1) & (ActiveAdjustment.ButtonWhenCalled <= 5)) {
        //SPDEBUG("set point selected is:" + String(spSelected));
        spSelected = ActiveAdjustment.ButtonWhenCalled - 1;
        MoveAPoint(spSelected);
      } else if (ActiveAdjustment.ButtonWhenCalled == 6) {
        MoveAPoint(SetPointCount - 1);
        MoveAPoint(SetPointCount - 2);
        Setpointschanged = true;

      } else if (ActiveAdjustment.ButtonWhenCalled == 7) {
        MoveAPoint(SetPointCount - 1);
        MoveAPoint(SetPointCount - 2);
        MoveAPoint(SetPointCount - 3);

        Setpointschanged = true;
      }
      break;
    case ACTIONADJUSTTEMPTOHOT:
      TempCoilTooHot = TempCoilTooHot + ActiveAdjustment.moveamount;
      EEPROM.put(TOOHOTTEMP_EP, TempCoilTooHot);
      UpdateConfigsDisplayArea(VALUESONLY);
      break;
    case ACTIONADJUSTCOOLDOWNTEMP:
      TempCoolingDone = TempCoolingDone + ActiveAdjustment.moveamount;
      EEPROM.put(COOLDOWNTEMP_EP, TempCoolingDone);
      UpdateConfigsDisplayArea(VALUESONLY);
      //#define COOLDOWNTEMP_EP 64
      break;
    case ActionAdjustFanGraphPixelBottom:
      FlowSetPointGraphBottompx = FlowSetPointGraphBottompx - ActiveAdjustment.moveamount;
      EEPROM.put(FlowSetPointGraphBottompx_EP, FlowSetPointGraphBottompx);
      UpdateConfigsDisplayArea(VALUESONLY);
      //FlowSetPointGraphBottompx_EP
      break;
    case ACTIONADJUSTSETPOINTFLOW:
      if ((ActiveAdjustment.ButtonWhenCalled >= 0) & (ActiveAdjustment.ButtonWhenCalled <= 4)) {
        spSelected = ActiveAdjustment.ButtonWhenCalled - 1;
        FlowSetPoints[spSelected].flow = FlowSetPoints[spSelected].flow + ActiveAdjustment.moveamount;
        EEPROM.put(Flowsetpoints_int_EP[spSelected], FlowSetPoints[spSelected]);

        if (ActiveAdjustment.moveamount > 0) {
          if (spSelected < 3) {
            for (byte i = spSelected + 1; i <= 3; i++) {
              if (FlowSetPoints[i].flow < FlowSetPoints[spSelected].flow) {
                FlowSetPoints[i].flow = FlowSetPoints[spSelected].flow;
                EEPROM.put(Flowsetpoints_int_EP[i], FlowSetPoints[i]);
              }
            }
          }
        }

        if (ActiveAdjustment.moveamount < 0) {
          if (spSelected > 0) {
            for (int i = spSelected - 1; i > -1; i--) {
              if (FlowSetPoints[i].flow > FlowSetPoints[spSelected].flow) {
                FlowSetPoints[i].flow = FlowSetPoints[spSelected].flow;
                EEPROM.put(Flowsetpoints_int_EP[i], FlowSetPoints[i]);
              }
            }
          }
        }
        UpdateConfigsDisplayArea(VALUESONLY);
      }
      if ((ActiveAdjustment.ButtonWhenCalled >= 5) & (ActiveAdjustment.ButtonWhenCalled <= 6)) {
        spSelected = ActiveAdjustment.ButtonWhenCalled - 4;
        //MoveAFanPointsTime(spSelected);
      }

      break;
    case ACTIONADJUSTTEMPDUTY:
      if (State == STATESTOPPED) {
        StateDebug = DEBUGDUTY;
        DutyTemp = RangeAdouble(DutyTemp + ActiveAdjustment.moveamount, 0.0, 1.0);
        //SPDEBUG("DutyTemp:" + String(DutyTemp));
      }
      break;
    case ACTIONADJUSTINTEGRALFLOW:
      IntegralFlow = IntegralFlow + ActiveAdjustment.moveamount;
      if (IntegralFlow < 0.0) {
        IntegralFlow = 0.0;
      }
      EEPROM.put(INTEGRALFLOW_EP, IntegralFlow);
      UpdateConfigsDisplayArea(VALUESONLY);
      break;
    case ACTIONADJUSTFANMANUAL:
      //SPDEBUG("here");
      FanManual = true;
      DutyFan = RangeAdouble(DutyFan + ActiveAdjustment.moveamount, 0.0, 1.0);
      UpdateProgressDisplayArea(VALUESONLY);
      sendFan_D_Wire();
      break;
    case ACTIONADJUSTCOOLBURST:
      FanCoolingBoostPercent = FanCoolingBoostPercent + ActiveAdjustment.moveamount;
      UpdateConfigsDisplayArea(VALUESONLY);
      break;
    case ACTIONADJUSTGAINFLOW:
      //SPDEBUG("here");
      GainFlow = GainFlow + ActiveAdjustment.moveamount;
      if (GainFlow < 1) {
        GainFlow = 0;
      }
      EEPROM.put(GAINFLOW_EP, GainFlow);
      UpdateConfigsDisplayArea(VALUESONLY);
      break;
    case ACTIONADJUSTMAXPERCENTCHANGEPERSECONDFLOW:
      //SPDEBUG("here");
      MaxPercentChangePerSecondFlow = MaxPercentChangePerSecondFlow + ActiveAdjustment.moveamount;
      EEPROM.put(MAXPERCENTCHANGEPERSECONDFLOW_float_EP, MaxPercentChangePerSecondFlow);
      UpdateConfigsDisplayArea(VALUESONLY);
      break;
    case ACTIONADJUSTTIMEMANUAL:
      //SPDEBUG("here");
      TimeManual = true;
      RoastMinutes = RoastMinutes;
      UpdateProgressDisplayArea(VALUESONLY);
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

    for (int i = 0; i < MAXBUTTONCOUNT; i++) {
      DrawMenuButton(butdefset, i, false);
    }
    //SPDEBUG("cacled menu button count for menu " + String(butdefset.menuID) + " is:" + String(butdefset.ButtonCount));
  } else {
    //SPDEBUG("found a menu button count for menu " + String(butdefset.menuID) + " is:" + String(butdefset.ButtonCount));
    for (int i = 0; i < butdefset.ButtonCount; i++) {
      DrawMenuButton(butdefset, i, false);
    }
  }
}

void DrawMenuButton(buttonsetdef& butdefset, int i, boolean toggletooltip) {
  //SPDEBUG("drawing menuid " + String(butdefset.menuID) + " button index " + String(i) + " with butID " + String(mybut.butID) );

  int butIDCalced = (butdefset.menuID * 10) + i;

  if (butdefset.buttondefs[i].butID == -1) {  //this means it has never been expanded
    memcpy_P(&myLocalbuttontext, &Vmenutext[butdefset.menuID][i], sizeof(buttontext));
    butdefset.buttondefs[i].action = myLocalbuttontext.action;
    butdefset.buttondefs[i].adjustmentvalueset = myLocalbuttontext.adjustmentvalueset;
    butdefset.buttondefs[i].butID = myLocalbuttontext.butID;
    //SPDEBUG(" found action " + String(butdefset.buttondefs[i].action) + " and butID " + String(butdefset.buttondefs[i].butID) );
    //SPDEBUG("A is adjustment:" + String(butdefset.IsAdjustment) + " button id" + String(i));
    if (butdefset.buttondefs[i].action == ACTIONGETLABLEFROMPRIOR && butdefset.buttondefs[i].ToolTipShowing == toggletooltip) {
      memcpy_P(&myLocalbuttontext, &Vmenutext[MenuStatus.VmenuPrior][MenuStatus.VmenubuttonClicked], sizeof(buttontext));
    }
  } else {
    if (butdefset.buttondefs[i].action == ACTIONGETLABLEFROMPRIOR && butdefset.buttondefs[i].ToolTipShowing == toggletooltip) {
      memcpy_P(&myLocalbuttontext, &Vmenutext[MenuStatus.VmenuPrior][MenuStatus.VmenubuttonClicked], sizeof(buttontext));
    } else {
      memcpy_P(&myLocalbuttontext, &Vmenutext[butdefset.menuID][i], sizeof(buttontext));
    }
  }
  if (butdefset.buttondefs[i].butID < 0) {
    return;
  }

  if (butdefset.buttondefs[i].butID != butIDCalced) {
    SERIALPRINT_ERR(F("Error DrawMenuButton butID "));
    SERIALPRINT_ERR(butdefset.buttondefs[i].butID); 
    SERIALPRINT_ERR(F( " does not matched calced butID "));
    SERIALPRINTLN_ERR(butIDCalced);
  }

  if (butdefset.buttondefs[i].action == ACTIONSELECTADUSTMENTVALUE) {
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
      myGLCD.setFont(FONT16X16);
    } else {
      myGLCD.setFont(Retro8x16);  //main button. 6 chars.
    }
    xOffset = (butdefset.W - (strlen(myLocalbuttontext.label) * myGLCD.getFontXsize())) / 2;
    yOffset = (butdefset.H - myGLCD.getFontYsize()) / 2;

    if (myLocalbuttontext.butID == DrawLablesRotated) {
      //SPDEBUG("myGLCD.getFontXsize:" + String(myGLCD.getFontXsize()) + " myGLCD.getFontYsize:" + String(myGLCD.getFontYsize()) + " strlen:" +  String(strlen(myLocalbuttontext.label)));
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

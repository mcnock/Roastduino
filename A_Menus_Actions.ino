

//-------------------------------------------------  ---------------------------------------------------------------------------------------------------
// MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS
//----------------------------------------------------------------------------------------------------------------------------------------------------
int vButWidth = 60;
int hButWidth = 70;
int ButHeight = 50;
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
  myHorFanButtonControl.W = 80;
  myHorFanButtonControl.rowstart = 410;
  myHorFanButtonControl.colstart = 60;
  myHorFanButtonControl.ButtonCount = 4;
  myHorFanButtonControl.vertical = false;
  myHorFanButtonControl.menuID = HmenuFAN;
  SetMenuBoundingRect(myHorFanButtonControl);
  myHorFanButtonControl.pClickHandler = ProcessHorFanMenu;
}

void DrawHorControlMenu() {
  //these ar the buttoms on top row starting about 1/2 across screen
  DrawMenuButtons(myHorControlMenuDef);
}

void ProcessHorControlMenu(int i) {
  //Serial.print("ProcessConrol");Serial.println (i);
  if (errmsg == "Must be in Fan Only to start a roast") {
    errmsg = "";
    newerrmsg = true;
  }
  switch (i) {
    case 0:
      if (State == STATEFANONLY) {
        newState = STATEROASTING;
      } else if ((State == STATENOFANCURRENT || State == STATEOVERHEATED || State == STATECOOLING)) {
        newState = STATEROASTING;
        //Serial.println("ReStart Detected!");
      } else {
        newerrmsg = true;
        errmsg = "Must be in Fan Only to start a roast";
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
      //Serial.println("Redraw graph detected");
      for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++) {
        MySetPoints[xSetPoint].TemperatureNew = 0;
      }


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
        if (RoastTime.elapsed() / 60 < MySetPoints[EndingSetPoint].Minutes) {
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
  //these ar the buttoms on top row starting about 1/2 across screen

  DrawMenuButtons(myHorFanButtonControl);
}

void ProcessHorFanMenu(int i) {
  //Serial.print("ProcessFanMenu:");Serial.println (i);
  int change = 0;
  switch (i) {
    case 0:
      //decrease quickly
      change = -5;
      break;
    case 1:
      change = -1;
      break;
    case 2:
      change = 1;
      break;
    case 3:
      change = 5;
      break;
    case 4:

      break;
  }
  if (change != 0) {
    if (State == STATEROASTING) {

      FanDeviation = FanDeviation + change;

      //the following call will corrext the FanDeviation if it is too large
      SetAndSendFanPWMForATime(RoastMinutes);
      //DrawFanGraph();
    } else {
      if (State == STATEFANONLY) {
        FanSpeedPWM = FanSpeedPWM + change;

        if (FanSpeedPWM > 254) {
          FanSpeedPWM = 254;
        }
        if (FanSpeedPWM < 0) {
          FanSpeedPWM = 0;
        }

        FanDeviation = FanSpeedPWM - FanSetPoints[0].PWM;

        sendFanPWM_Wire();
      }
      //DrawFanGraph();
      //delay(5);
    }
  }
}

void DrawVMenu(int iMenu, int iButton) {

  if (iMenu == -1) {
    iMenu = VmenuEmpty;
    MenuStatus.VmenuPrior = VmenuEmpty;
    MenuStatus.VmenuShowing = VmenuBase;
  }
  if (myButtonVertMenus[MenuStatus.VmenuShowing].IsAdjustment == false) {
    SpDebug("Setting parent to :" + String(MenuStatus.VmenuShowing));
    myButtonVertMenus[iMenu].VmenuParent = MenuStatus.VmenuShowing;
  } else {
    SpDebug("leaving parent alone ");
  }

  MenuStatus.VmenuPrior = MenuStatus.VmenuShowing;
  MenuStatus.VmenuShowing = iMenu;

  MenuStatus.VmenubuttonClicked = iButton;
  boolean bRedraw = false;
  if (iMenu == VmenuEmpty) {
    myGLCD.setColor(BLACK);
    int xstart = 800 - myButtonVertMenus[Vmenubase].W;
    //myGLCD.drawRect(xstart,myButtonVertMenus[Vmenubase].H,800,480);
    myGLCD.fillRect(xstart - 1, myButtonVertMenus[Vmenubase].H, 800, 480);
    bRedraw = true;
    myGLCD.setColor(LIGHTGRAY);

    for (int i = 0; i < HorScaleLineYCount; i++) {
      //SpDebug("menuRedrawYScale:" + String(i)+ " x:"+  String(xstart) + " y:" + String(HorScaleLineY[i]));
      myGLCD.drawLine(xstart - 2, HorScaleLineY[i], 800, HorScaleLineY[i]);
    }
    myGLCD.setColor(BLACK);


  } else if (MenuStatus.VmenuPrior == VmenuEmpty) {
    //bRedraw = true;
  }
  SpDebug("Drawing vMenu :" + String(MenuStatus.VmenuShowing));
  DrawMenuButtons(myButtonVertMenus[iMenu]);

  if (bRedraw) {
    UpdateTempDisplayArea(All);
    graphFanProfile();
  }
}

void ProcessVmenuButtonClick_() {

  buttondef mybutton = myButtonVertMenus[MenuStatus.VmenuShowing].buttondefs[MenuStatus.ButtonClicked];
  int i = MenuStatus.ButtonClicked;
  //SpDebug("ProcessVmenuButtonClick button:" + String(MenuStatus.ButtonClicked) + " showing:" + String(MenuStatus.VmenuShowing) + " parent" + String(myButtonVertMenus[MenuStatus.VmenuShowing].VmenuParent));
  if (MenuStatus.ButtonClicked == 0) {
    if (MenuStatus.VmenuShowing == VmenuSetPointSelect) {
      for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++) {
        MySetPoints[xSetPoint].TemperatureNew = 0;
      }
      spSelected = -1;
    }
    DrawVMenu(myButtonVertMenus[MenuStatus.VmenuShowing].VmenuParent, 0);
    return;
  }
  //SpDebug("button.action:" + String(mybutton.action) + " adjustmentvalueset:" + String(mybutton.adjustmentvalueset));
  if (mybutton.action > ActionAdjustments) {
    //set up editing of any value
    ActiveAdjustment.VmenuWhenCalled = MenuStatus.VmenuShowing;
    ActiveAdjustment.name = mybutton.action;
    ActiveAdjustment.adjustmentvalueset = mybutton.adjustmentvalueset;
    myButtonVertMenus[VmenuAdjustValue].IsAdjustment = true;
    DrawVMenu(VmenuAdjustValue, i);
    return;
  }

  switch (mybutton.action) {
    case ActionProvideValueForAdjustment:
      ProcessAdjustment();
      break;
    case ActionShowSetpointSelectMenu:
      DrawVMenu(VmenuSetPointSelect, -1);
      break;
    case ActionShowSetpointFanMenu:
      DrawVMenu(VmenuFan, -1);
      break;


    default:
      break;
  }
}

void ClearActiveAdjustment() {

  ActiveAdjustment.moveamount = -1;
  ActiveAdjustment.spSelected = -1;
  ActiveAdjustment.name = -1;
}

void ProcessAdjustment() {
  if (ActiveAdjustment.VmenuWhenCalled != MenuStatus.VmenuShowing) {
    //possible error
  }
  if (ActiveAdjustment.moveamount != 0) {
    switch (ActiveAdjustment.name) {
      case ActionAdjustIntegralTemp:
        IntegralTemp = IntegralTemp + ActiveAdjustment.moveamount;
        if (IntegralTemp < 0.0) {
          IntegralTemp = 0.0;
        }
        EEPROM.update(INTEGRALTEMP_EP, (int)(IntegralTemp * 100));
        UpdateProgessDisplayArea(All);


        break;
      case ActionAdjustGainTemp:
        GainTemp = GainTemp + moveamount;
        if (GainTemp < 10) {
          GainTemp = 0;
        }
        EEPROM.update(GAINTEMP_EP, GainTemp);
        UpdateProgessDisplayArea(All);
        break;
      case ActionAdjustSetpointTemp:
        setpointschanged = true;
        if ((spSelected >= 0) & (spSelected <= 5)) {
          MoveAPoint(spSelected);
        }
        if (spSelected == 6) {
          MoveLast3Point();
        }
        break;
      case ActionAdjustFan:
        break;
      case ActionAdjustRoastLength:
        break;
      case ActionAdjustSetpointFan:

        break;
      default:
        break;
    }
  }
}

void ProcessDebugVMenu(int i) {
  //Serial.print("ProcessVertMenu3:");Serial.println(i);
  //    buttonsetdef* bsd = &myButtonVertMenus[VerticalMenuShowing];
  if (errmsg == "Must be in state stopped to debug" || errmsg == "Must be in statefanonly to debug coils") {
    errmsg = "";
    newerrmsg = true;
  }
  switch (i) {
    case VBUT0:
      if (State == DEBUGTOGGLE || State == DEBUGDUTY) {
        newState = STATESTOPPED;
      }
      DrawVMenu(myButtonVertMenus[MenuStatus.VmenuShowing].VmenuParent, 0);
      break;
    case VBUT1:
      if (State == DEBUGTOGGLE || State == DEBUGDUTY) {
        newState = STATESTOPPED;
      }

      break;
    case VBUT3:  //coil 1
    case VBUT4:  //coil 2
      if (State == STATEFANONLY || State == DEBUGCOIL) {
        // myButtonVertMenus[VmenuOnOff].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
        // myButtonVertMenus[VmenuOnOff].buttondefs[1].AlternateLableID.ButtonID = i;
        //D/rawVMenu(VmenuOnOff, i);
        // newState = DEBUGCOIL;
      } else {
        newerrmsg = true;
        errmsg = "Must be in statefanonly to debug coils";
      }
      break;

    case VBUT5:  //vib
    case VBUT6:  //fan

      if (State == STATESTOPPED || State == DEBUGTOGGLE || State == DEBUGDUTY) {
        // myButtonVertMenus[VmenuOnOff].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
        // myButtonVertMenus[VmenuOnOff].buttondefs[1].AlternateLableID.ButtonID = i;
        // DrawVMenu(VmenuOnOff, i);
        // newState = DEBUGTOGGLE;
      } else {
        newerrmsg = true;
        errmsg = "Must be in state stopped to debug";
      }
      break;
    case VBUT7:  //duty
      if (State == STATESTOPPED || State == DEBUGTOGGLE || State == DEBUGDUTY) {
        //myButtonVertMenus[VmenuAjd_01].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
        //myButtonVertMenus[VmenuAjd_01].buttondefs[1].AlternateLableID.ButtonID = i;
        //DrawVMenu(VmenuAjd_01, i);
        newState = DEBUGDUTY;

      } else {
        newerrmsg = true;
        errmsg = "Must be in state stopped to debug";
      }
    case VBUT8:  //temp
      //myButtonVertMenus[VMenuAdj_1_5_10_V].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
      //myButtonVertMenus[VMenuAdj_1_5_10_V].buttondefs[1].AlternateLableID.ButtonID = i;
      //DrawVMenu(VMenuAdj_1_5_10_V, i);
      break;
  }
}

void ProcessOnOffVMenu(int iButPressed) {
  //Serial.print("ProcessOnOffVMenu:");Serial.println(i);
  int ONOFF = HIGH;
  switch (iButPressed) {
    case VBUT0:

      break;
    case VBUT1:
      break;
    case VBUT2:  //on
      ONOFF = HIGH;
      break;
    case VBUT3:  //off
      ONOFF = LOW;
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
  }


  if (iButPressed == 2 || iButPressed == 3) {
    switch (MenuStatus.VbuttonClicked) {
      case 3:  //ss 1
        Serial.println(F("C:ONOFF"));

        digitalWrite(SSR1_p7, ONOFF);
        break;
      case 4:  //ss 2
        digitalWrite(SSR2_p6, ONOFF);
        break;
      case 5:  //vib
        //            digitalWrite(VIBRELAYp, !ONOFF);
        break;
      case 6:  //fan
        if (FanSpeedPWM == 0) {
          FanSpeedPWM = FanSetPoints[0].PWM;
        }
        digitalWrite(FANRELAYp_2, !ONOFF);
        break;
    }
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
  if (butdefset.ButtonCount = -1) {
  
    for (int i = 0; i < MaxButtonCount; i++) {
      DrawMenuButton(butdefset, i, false);
    }
    SpDebug("menu button count for menu " + String(butdefset.menuID) + " is:" + String(butdefset.ButtonCount));
  } else {
    for (int i = 0; i < butdefset.ButtonCount; i++) {
      DrawMenuButton(butdefset, i, false);
    }
  }
}

void DrawMenuButton(buttonsetdef& butdefset, int i, boolean toggletooltip) {


  if (butdefset.vertical == true) {
    //Serial.print("gettting button text for VmenuID:");Serial.print(butdefset.menuID);Serial.print(" button:");Serial.println(i);
  } else {
    //Serial.print("gettting button text for HmenuID:");Serial.print(butdefset.menuID);Serial.print(" button:");Serial.println(i);
  }

  if (butdefset.buttondefs[i].action == -1) {
    memcpy_P(&myArrayLocal, &Vmenutext[butdefset.menuID][i], sizeof(buttontext));
    butdefset.buttondefs[i].action = myArrayLocal.action;
    butdefset.buttondefs[i].adjustmentvalueset = myArrayLocal.adjustmentvalueset;
    SpDebug("A is adjustment:" + String(butdefset.IsAdjustment) + " button id" + String(i));
    if (butdefset.IsAdjustment == true && i == 1) {
      memcpy_P(&myArrayLocal, &Vmenutext[MenuStatus.VmenuPrior][MenuStatus.VmenubuttonClicked], sizeof(buttontext));
    }

  } else {
    if (butdefset.IsAdjustment == true && i == 1) {
      memcpy_P(&myArrayLocal, &Vmenutext[MenuStatus.VmenuPrior][MenuStatus.VmenubuttonClicked], sizeof(buttontext));
    } else {
      memcpy_P(&myArrayLocal, &Vmenutext[butdefset.menuID][i], sizeof(buttontext));
    }
  }

  if (myArrayLocal.key > 0) {
    butdefset.ButtonCount = i + 1;
  } else {
    return;
  }





  Serial.print(" id:");
  Serial.print(myArrayLocal.key);
  Serial.print(" label:");
  Serial.println(myArrayLocal.label);

  myGLCD.setColor(myArrayLocal.color);
  myGLCD_fillRect(butdefset.buttondefs[i].Rect);
  myGLCD.setColor(BLACK);
  myGLCD_drawRect(butdefset.buttondefs[i].Rect);

  myGLCD.setBackColor(myArrayLocal.color);
  myGLCD.setColor(BLACK);
  int xOffset = 0;
  int yOffset = 0;
  int xT = 0;

  //this is normal
  if (toggletooltip == false || butdefset.buttondefs[i].ToolTipShowing == true) {
    //Serial.print("gettting button text for VmenuID:");Serial.print(butdefset.menuID);Serial.print(" button:");Serial.println(i);

    butdefset.buttondefs[i].ToolTipShowing = false;
    if (butdefset.vertical == true && i != 0) {
      myGLCD.setFont(Retro8x16);
    } else {
      myGLCD.setFont(BigFont);  //main button. 6 chars.
    }
    xOffset = (butdefset.W - (strlen(myArrayLocal.label) * myGLCD.getFontXsize())) / 2;
    yOffset = (butdefset.H - myGLCD.getFontYsize()) / 2;
    myGLCD.print(myArrayLocal.label, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset);
  } else {  //tool tip 2 or 3 line x 12 char

    myGLCD.setFont(Retro8x16);
    xOffset = (butdefset.W - (strlen(myArrayLocal.tip1) * myGLCD.getFontXsize())) / 2;
    butdefset.buttondefs[i].ToolTipShowing = true;
    // if (butdefset.vertical == true) {
    //   xT = 10;
    //   yOffset = (butdefset.H / 3 - myGLCD.getFontYsize()) / 2;
    //   myGLCD.print(myArrayLocal.tip1, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset);
    //   xOffset = (butdefset.W + xT - (strlen(myArrayLocal.tip2) * myGLCD.getFontXsize())) / 2;
    //   myGLCD.print(myArrayLocal.tip2, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset + butdefset.H / 3);
    //   xOffset = (butdefset.W - (strlen(myArrayLocal.tip3) * myGLCD.getFontXsize())) / 2;
    //   myGLCD.print(myArrayLocal.tip3, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset + (butdefset.H / 3 * 2));

    // } else {
    yOffset = (butdefset.H / 2 - myGLCD.getFontYsize()) / 2;
    myGLCD.print(myArrayLocal.tip1, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset);
    xOffset = (butdefset.W - (strlen(myArrayLocal.tip2) * myGLCD.getFontXsize())) / 2;
    myGLCD.print(myArrayLocal.tip2, butdefset.buttondefs[i].Rect.x + xOffset, butdefset.buttondefs[i].Rect.y + yOffset + butdefset.H / 2);
  }
}

void OutlineClickedButton(uint16_t color) {
  uint16_t priorcolor = myGLCD.getColor();
  myGLCD.setColor(color);
  myGLCD_drawRect(MenuStatus.TouchButtonSet->buttondefs[MenuStatus.ButtonClicked].Rect);
  //myGLCD_drawRect(butdefset->buttondefs[i].Rect);
  myGLCD.setColor(priorcolor);
}

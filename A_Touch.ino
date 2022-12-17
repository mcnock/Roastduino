
//----------------------------------------------------------------------------------------------------------------------------------------------------
// TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH
//----------------------------------------------------------------------------------------------------------------------------------------------------
void MenuTouchClickDetected() {

  OutlineClickedButton(BLACK);

  if (MenuStatus.IsVmenu == false) {
    //Serial.print("Touch Click calling menuID:");Serial.print(TouchButtonSet->menuID);Serial.print(" buttonID:");Serial.println(MenuStatus.ButtonClicked);
    MenuStatus.TouchButtonSet->pClickHandler(MenuStatus.ButtonClicked);
  } else {
    //Serial.println("Touch Click calling ProcessVmenuButtonClick_()" );
    ProcessVmenuButtonClick();
  }
}

void MenuTouchLongPressDetected() {
  OutlineClickedButton(BLACK);
  buttonsetdef *TouchButtonSet;
  TouchButtonSet = MenuStatus.TouchButtonSet;
  DrawMenuButton(*TouchButtonSet, MenuStatus.ButtonClicked, true);
}


boolean DetectTouch() {
  myTouch.read();
  int16_t Xtouch = myTouch.getX();
  int16_t Ytouch = myTouch.getY();
  // int Ytouchinv = myGLCD.getDisplayYSize() - Ytouch;
  switch (TouchStatus.objectpress) {
    case PressMenu:
      return true;
      break;
    case PressCongurationBox:
      
      if (TouchStatus.dragy > 0) {
        myGLCD.setColor(BLACK);
        
        myGLCD.drawRect(ConfigDisplay.x - TouchStatus.dragx, ConfigDisplay.y - TouchStatus.dragy, ConfigDisplay.xmax - TouchStatus.dragx, ConfigDisplay.ymax - TouchStatus.dragy);
      }
      
      TouchStatus.dragx = TouchStatus.Xdown - Xtouch;
      TouchStatus.dragy = TouchStatus.Ydown - Ytouch;
      myGLCD.setColor(PALEYELLOW);
      myGLCD.drawRect(ConfigDisplay.x - TouchStatus.dragx, ConfigDisplay.y - TouchStatus.dragy, ConfigDisplay.xmax - TouchStatus.dragx, ConfigDisplay.ymax - TouchStatus.dragy);
      delay(100);
      return true;
      break;
    case PressOperDetailBox:
      if (TouchStatus.dragy > 0) {
        myGLCD.setColor(BLACK);
        myGLCD.drawRect(OpDetailDisplay.x - TouchStatus.dragx, OpDetailDisplay.y - TouchStatus.dragy, OpDetailDisplay.xmax - TouchStatus.dragx, OpDetailDisplay.ymax - TouchStatus.dragy);
      }
      TouchStatus.dragx = TouchStatus.Xdown - Xtouch;
      TouchStatus.dragy = TouchStatus.Ydown - Ytouch;
      myGLCD.setColor(PALEYELLOW);
      myGLCD.drawRect(OpDetailDisplay.x - TouchStatus.dragx, OpDetailDisplay.y - TouchStatus.dragy, OpDetailDisplay.xmax - TouchStatus.dragx, OpDetailDisplay.ymax - TouchStatus.dragy);
      delay(100);
      return true;
      break;
  }
  if (InRect(Xtouch, Ytouch, &myHorControlMenuDef.bounding)) {
    MenuStatus.ButtonClicked = WhatButton(Xtouch, Ytouch, &myHorControlMenuDef);
    MenuStatus.TouchButtonSet = &myHorControlMenuDef;
    MenuStatus.IsVmenu = false;
    TouchStatus.objectpress = PressMenu;
    OutlineClickedButton(WHITE);
  } else if (InRect(Xtouch, Ytouch, &myHorFanButtonControl.bounding)) {
    MenuStatus.ButtonClicked = WhatButton(Xtouch, Ytouch, &myHorFanButtonControl);
    MenuStatus.TouchButtonSet = &myHorFanButtonControl;
    MenuStatus.IsVmenu = false;
    TouchStatus.objectpress = PressMenu;
    OutlineClickedButton(WHITE);
  } else if (InRect(Xtouch, Ytouch, &myButtonVertMenus[MenuStatus.VmenuShowing].bounding)) {
    MenuStatus.ButtonClicked = WhatButton(Xtouch, Ytouch, &myButtonVertMenus[MenuStatus.VmenuShowing]);
    MenuStatus.TouchButtonSet = &myButtonVertMenus[MenuStatus.VmenuShowing];
    MenuStatus.IsVmenu = true;
    TouchStatus.objectpress = PressMenu;
    OutlineClickedButton(WHITE);
  } else if (InRect(Xtouch, Ytouch, &OpDetailDisplay)) {
    TouchStatus.objectpress = PressOperDetailBox;
    TouchStatus.Xdown = Xtouch;
    TouchStatus.Ydown = Ytouch;
    TouchStatus.dragy = 0;
    TouchStatus.dragx = 0;
    myGLCD.setColor(BLUE);
    myGLCD_drawRect(OpDetailDisplay);
    //    spDebug("Starting drag with click at X" + String(Xtouch) + " Y" + String(Ytouch));
  } else if (InRect(Xtouch, Ytouch, &ConfigDisplay)) {
    TouchStatus.objectpress = PressCongurationBox;
    TouchStatus.Xdown = Xtouch;
    TouchStatus.Ydown = Ytouch;
    TouchStatus.dragy = 0;
    TouchStatus.dragx = 0;
    myGLCD.setColor(BLUE);
    myGLCD_drawRect(ConfigDisplay);
    //    spDebug("Starting drag with click at X" + String(Xtouch) + " Y" + String(Ytouch));
  }
  if (TouchStatus.objectpress > 0) {
    return true;
  } else {
    if (TouchStatus.objectpress == PressMenu) {
      OutlineClickedButton(BLACK);
      TouchStatus.objectpress = 0;
    }

    return false;
  }
}

boolean InRect(int &x, int &y, struct rect *Rect) {

  if (x > Rect->x && x < Rect->xmax) {
    //Serial.println("Found X");
    if (y > Rect->y && y < Rect->ymax) {
      //Serial.println("Found Y");
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}


int WhatButton(int &x, int &y, struct buttonsetdef *butdefset) {
  //Serial.print ("what menu count:");Serial.println( butdefset->Count);

  for (int i = 0; i < butdefset->ButtonCount; i++) {
    //Serial.println ("looking at index:");Serial.println(i);
    //SerialprintRect(&butsetdef->buttondefs[i].Rect);
    if (InRect(x, y, &butdefset->buttondefs[i].Rect) == true) {
      //Serial.print("Found:");Serial.println(i);
      return i;
    }
  }
  return -1;
}

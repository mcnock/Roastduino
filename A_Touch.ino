
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
  switch (TouchStatus.objectpressID) {
    case PressMenu:

      return true;
      break;
    case PressConfigDisplayBox:
      
      DrawBoxMove(PressConfigDisplayBox, Xtouch,Ytouch);

      // TouchStatus.dragx = Xtouch - TouchStatus.Xdown;
      // TouchStatus.dragy = TouchStatus.Ydown - Ytouch;
      // if (millis() > NextDragRedrawTime) {
      //   if (abs(TouchStatus.dragylastdrawn - TouchStatus.dragy) > 5 || abs(TouchStatus.dragylastdrawn = TouchStatus.dragy) > 5) {
      //     if (TouchStatus.dragylastdrawn != 0 || TouchStatus.dragxlastdrawn != 0) {
      //       myGLCD.setColor(BLACK);
      //       myGLCD.drawRect(DisplayBoxes[ConfigDisplay].Rect.x + TouchStatus.dragxlastdrawn, DisplayBoxes[ConfigDisplay].Rect.y - TouchStatus.dragylastdrawn, DisplayBoxes[ConfigDisplay].Rect.xmax + TouchStatus.dragxlastdrawn, DisplayBoxes[ConfigDisplay].Rect.ymax - TouchStatus.dragylastdrawn);
      //     }

      //     myGLCD.setColor(PALEYELLOW);
      //     TouchStatus.dragylastdrawn = TouchStatus.dragy;
      //     TouchStatus.dragxlastdrawn = TouchStatus.dragx;
      //     myGLCD.drawRect(DisplayBoxes[ConfigDisplay].Rect.x + TouchStatus.dragx, DisplayBoxes[ConfigDisplay].Rect.y - TouchStatus.dragy, DisplayBoxes[ConfigDisplay].Rect.xmax + TouchStatus.dragx, DisplayBoxes[ConfigDisplay].Rect.ymax - TouchStatus.dragy);
      //   }
      //   NextDragRedrawTime = millis() + DragRedrawWait;
      // }
      return true;
      break;
    case PressOpDetailBox:
      DrawBoxMove(PressOpDetailBox, Xtouch,Ytouch);

      // if (millis() > NextDragRedrawTime) {
      //   if (TouchStatus.dragylastdrawn != 0 || TouchStatus.dragxlastdrawn != 0) {
      //     myGLCD.setColor(BLACK);
      //     myGLCD.drawRect(DisplayBoxes[OpDetailDisplay].Rect.x + TouchStatus.dragxlastdrawn, DisplayBoxes[OpDetailDisplay].Rect.y + TouchStatus.dragylastdrawn, DisplayBoxes[OpDetailDisplay].Rect.xmax + TouchStatus.dragxlastdrawn, DisplayBoxes[OpDetailDisplay].Rect.ymax + TouchStatus.dragylastdrawn);
      //   }
      //   TouchStatus.dragx = Xtouch - TouchStatus.Xdown;
      //   TouchStatus.dragy = Ytouch - TouchStatus.Ydown ;
      //   myGLCD.setColor(PALEYELLOW);
      //     if ((DisplayBoxes[OpDetailDisplay].Rect.y + TouchStatus.dragy) < 60){
      //           TouchStatus.dragy = 60 - DisplayBoxes[OpDetailDisplay].Rect.y;
      //     }
      //     if ((DisplayBoxes[OpDetailDisplay].Rect.ymax + TouchStatus.dragy) > 440){
      //           TouchStatus.dragy = 440 - DisplayBoxes[OpDetailDisplay].Rect.ymax;
      //     }

      //     if ((DisplayBoxes[OpDetailDisplay].Rect.x + TouchStatus.dragx) < 10){
      //           TouchStatus.dragx = 10 - DisplayBoxes[OpDetailDisplay].Rect.x;
      //     }

      //     if ((DisplayBoxes[OpDetailDisplay].Rect.xmax + TouchStatus.dragx) > 730){
      //           TouchStatus.dragx = 740 - DisplayBoxes[OpDetailDisplay].Rect.xmax;
      //     }
      //   myGLCD.drawRect(DisplayBoxes[OpDetailDisplay].Rect.x + TouchStatus.dragx, DisplayBoxes[OpDetailDisplay].Rect.y + TouchStatus.dragy, DisplayBoxes[OpDetailDisplay].Rect.xmax + TouchStatus.dragx, DisplayBoxes[OpDetailDisplay].Rect.ymax + TouchStatus.dragy);
      //   TouchStatus.dragylastdrawn = TouchStatus.dragy;
      //   TouchStatus.dragxlastdrawn = TouchStatus.dragx;
      //   //delay(100);
      //   NextDragRedrawTime = millis() + DragRedrawWait;
      // }

      return true;
      break;
  }

  if (InRect(Xtouch, Ytouch, &HorControlMenuDef.bounding)) {
    MenuStatus.ButtonClicked = WhatButton(Xtouch, Ytouch, &HorControlMenuDef);
    MenuStatus.TouchButtonSet = &HorControlMenuDef;
    MenuStatus.IsVmenu = false;
    TouchStatus.objectpressID = PressMenu;
    OutlineClickedButton(WHITE);
  } else if (InRect(Xtouch, Ytouch, &HorFanMenuDef.bounding)) {
    MenuStatus.ButtonClicked = WhatButton(Xtouch, Ytouch, &HorFanMenuDef);
    MenuStatus.TouchButtonSet = &HorFanMenuDef;
    MenuStatus.IsVmenu = false;
    TouchStatus.objectpressID = PressMenu;
    OutlineClickedButton(WHITE);
  } else if (InRect(Xtouch, Ytouch, &VertMenuDefs[MenuStatus.VmenuShowing].bounding)) {
    MenuStatus.ButtonClicked = WhatButton(Xtouch, Ytouch, &VertMenuDefs[MenuStatus.VmenuShowing]);
    MenuStatus.TouchButtonSet = &VertMenuDefs[MenuStatus.VmenuShowing];
    MenuStatus.IsVmenu = true;
    TouchStatus.objectpressID = PressMenu;
    OutlineClickedButton(WHITE);
  } else if (InRect(Xtouch, Ytouch, &DisplayBoxes[OpDetailDisplay].Rect)) {
    TouchStatus.objectpressID = PressOpDetailBox;
    TouchStatus.Xdown = Xtouch;
    TouchStatus.Ydown = Ytouch;
    TouchStatus.dragy = 0;
    TouchStatus.dragx = 0;
    TouchStatus.dragylastdrawn = 0;
    TouchStatus.dragxlastdrawn = 0;

    myGLCD.setColor(BLUE);
    myGLCD_drawRect(DisplayBoxes[OpDetailDisplay].Rect);
    //    SPDEBUG("Starting drag with click at X" + String(Xtouch) + " Y" + String(Ytouch));
  } else if (InRect(Xtouch, Ytouch, &DisplayBoxes[ConfigDisplay].Rect)) {
    TouchStatus.objectpressID = PressConfigDisplayBox;
    TouchStatus.Xdown = Xtouch;
    TouchStatus.Ydown = Ytouch;
    TouchStatus.dragy = 0;
    TouchStatus.dragx = 0;
    TouchStatus.dragylastdrawn = 0;
    TouchStatus.dragxlastdrawn = 0;

    myGLCD.setColor(BLUE);
    myGLCD_drawRect(DisplayBoxes[ConfigDisplay].Rect);
    //    SPDEBUG("Starting drag with click at X" + String(Xtouch) + " Y" + String(Ytouch));
  }
  if (TouchStatus.objectpressID != PressNone) {
    return true;
  } else {
    if (TouchStatus.objectpressID == PressMenu) {
      OutlineClickedButton(BLACK);
      TouchStatus.objectpressID = PressNone;
    }

    return false;
  }
}

void DrawBoxMove(int BoxBeingMoved, int16_t Xtouch, int16_t Ytouch) {
  if (millis() > NextDragRedrawTime) {
    if (TouchStatus.dragylastdrawn != 0 || TouchStatus.dragxlastdrawn != 0) {
      myGLCD.setColor(BLACK);
      myGLCD.drawRect(DisplayBoxes[BoxBeingMoved].Rect.x + TouchStatus.dragxlastdrawn, DisplayBoxes[BoxBeingMoved].Rect.y + TouchStatus.dragylastdrawn, DisplayBoxes[BoxBeingMoved].Rect.xmax + TouchStatus.dragxlastdrawn, DisplayBoxes[BoxBeingMoved].Rect.ymax + TouchStatus.dragylastdrawn);
    }
    TouchStatus.dragx = Xtouch - TouchStatus.Xdown;
    TouchStatus.dragy = Ytouch - TouchStatus.Ydown;
    myGLCD.setColor(PALEYELLOW);
    if ((DisplayBoxes[BoxBeingMoved].Rect.y + TouchStatus.dragy) < 60) {
      TouchStatus.dragy = 60 - DisplayBoxes[BoxBeingMoved].Rect.y;
    }
    if ((DisplayBoxes[BoxBeingMoved].Rect.ymax + TouchStatus.dragy) > 440) {
      TouchStatus.dragy = 440 - DisplayBoxes[BoxBeingMoved].Rect.ymax;
    }

    if ((DisplayBoxes[BoxBeingMoved].Rect.x + TouchStatus.dragx) < 10) {
      TouchStatus.dragx = 10 - DisplayBoxes[BoxBeingMoved].Rect.x;
    }

    if ((DisplayBoxes[BoxBeingMoved].Rect.xmax + TouchStatus.dragx) > 720) {
      TouchStatus.dragx = 720 - DisplayBoxes[BoxBeingMoved].Rect.xmax;
    }
    myGLCD.drawRect(DisplayBoxes[BoxBeingMoved].Rect.x + TouchStatus.dragx, DisplayBoxes[BoxBeingMoved].Rect.y + TouchStatus.dragy, DisplayBoxes[BoxBeingMoved].Rect.xmax + TouchStatus.dragx, DisplayBoxes[BoxBeingMoved].Rect.ymax + TouchStatus.dragy);
    TouchStatus.dragylastdrawn = TouchStatus.dragy;
    TouchStatus.dragxlastdrawn = TouchStatus.dragx;
    //delay(100);
    NextDragRedrawTime = millis() + DragRedrawWait;
  }
}
boolean InRect(int &x, int &y, struct rect *Rect) {
  //SPDEBUG("InRec x:" +String(x) + " y:" + String(y));
  //Serial_println_rect(*Rect);
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

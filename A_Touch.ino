
//----------------------------------------------------------------------------------------------------------------------------------------------------
// TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH
//----------------------------------------------------------------------------------------------------------------------------------------------------
void TouchClickDetected (){

  OutlineClickedButton(BLACK);

 if (MenuStatus.IsVmenu == false){ 
      //Serial.print("Touch Click calling menuID:");Serial.print(TouchButtonSet->menuID);Serial.print(" buttonID:");Serial.println(MenuStatus.ButtonClicked);
      MenuStatus.TouchButtonSet->pClickHandler(MenuStatus.ButtonClicked);
  } else{
      //Serial.println("Touch Click calling ProcessVmenuButtonClick_()" );
      ProcessVmenuButtonClick_();
  }

}


void TouchLongPressDetected() {

  OutlineClickedButton ( BLACK);
  buttonsetdef* TouchButtonSet;
  TouchButtonSet = MenuStatus.TouchButtonSet;
  DrawMenuButton(*TouchButtonSet , MenuStatus.ButtonClicked, true);
 
}


boolean DetectTouch() {
  myTouch.read();
  int16_t Xtouch = myTouch.getX();
  int16_t Ytouch = myTouch.getY();

  
  // int Ytouchinv = myGLCD.getDisplayYSize() - Ytouch;
  //Serial.print("touch detected X");Serial.print(Xtouch);Serial.print(" Y");Serial.println(Ytouch);

  if (InRect(Xtouch, Ytouch, &myHorControlMenuDef.bounding)) {
    
    MenuStatus.ButtonClicked = WhatButton(Xtouch, Ytouch, &myHorControlMenuDef);
    MenuStatus.TouchButtonSet = &myHorControlMenuDef;
    MenuStatus.IsVmenu = false;
    
    ;
  } else if (InRect(Xtouch, Ytouch, &myHorFanButtonControl.bounding)) {
    
    MenuStatus.ButtonClicked = WhatButton(Xtouch, Ytouch, &myHorFanButtonControl);
    MenuStatus.TouchButtonSet = &myHorFanButtonControl;
    MenuStatus.IsVmenu = false;
  
  }

  else if (InRect(Xtouch, Ytouch, &myButtonVertMenus[MenuStatus.VmenuShowing].bounding)) {
    MenuStatus.ButtonClicked = WhatButton(Xtouch, Ytouch, &myButtonVertMenus[MenuStatus.VmenuShowing]);
    MenuStatus.TouchButtonSet = &myButtonVertMenus[MenuStatus.VmenuShowing];
    MenuStatus.IsVmenu = true;  
  } else {
    //Serial.print ("looking but no match vmenu is");Serial.println(VerticalMenuShowing);
    return false;
  }
  
  
  OutlineClickedButton(WHITE);

  
  return true;
}

void xProcessTouch(int Xtouch, int Ytouch) {


  //int Ytouchinv = myGLCD.getDisplayYSize() - Ytouch;
  //Serial.print("touch detected X");Serial.print(Xtouch);Serial.print(" Y");Serial.println(Ytouch);

  if (InRect(Xtouch, Ytouch, &myHorControlMenuDef.bounding)) {
    //Serial.println("found hor1 buttons");
    int i = WhatButton(Xtouch, Ytouch, &myHorControlMenuDef);
    ProcessHorControlMenu(i);
    return;
  }
  if (InRect(Xtouch, Ytouch, &myHorFanButtonControl.bounding)) {
    //Serial.println("found hor2 buttons");
    int j = WhatButton(Xtouch, Ytouch, &myHorFanButtonControl);
    ProcessHorFanMenu(j);
    return;
  }

  if (InRect(Xtouch, Ytouch, &myButtonVertMenus[MenuStatus.VmenuShowing].bounding)) {
    //Serial.print("found vertical1  buttons");
    int k = WhatButton(Xtouch, Ytouch, &myButtonVertMenus[MenuStatus.VmenuShowing]);
    myButtonVertMenus[MenuStatus.VmenuShowing].pClickHandler(k);
    return;
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

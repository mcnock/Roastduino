
//----------------------------------------------------------------------------------------------------------------------------------------------------
// TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH
//----------------------------------------------------------------------------------------------------------------------------------------------------
void   TouchClick()
{

     OutlineMenuButton(TouchButtonSet,TouchButton,BLACK);

     //Serial.print("Touch Click calling menuID:");Serial.print(TouchButtonSet->menuID);Serial.print(" buttonID:");Serial.println(TouchButton);
     
     TouchButtonSet->pClickHandler(TouchButton);

}


void   TouchLongPress()
{

       OutlineMenuButton(TouchButtonSet,TouchButton,BLACK);
    
       DrawMenuButton(*TouchButtonSet, TouchButton, true);
}
             

boolean DetectTouch()
{
       myTouch.read();
       int16_t Xtouch = myTouch.getX();
       int16_t Ytouch = myTouch.getY();
          
     
       // int Ytouchinv = myGLCD.getDisplayYSize() - Ytouch;
        //Serial.print("touch detected X");Serial.print(Xtouch);Serial.print(" Y");Serial.println(Ytouch);

    if (InRect(Xtouch, Ytouch, &myHorControlMenuDef.bounding)) {
        TouchButton = WhatButton(Xtouch, Ytouch, &myHorControlMenuDef);
        TouchButtonSet = &myHorControlMenuDef;
  
    }
    else if (InRect(Xtouch, Ytouch, &myHorFanButtonControl.bounding)) {
        TouchButton = WhatButton(Xtouch, Ytouch, &myHorFanButtonControl);
        TouchButtonSet = &myHorFanButtonControl;
        
    }  
   
    else if (InRect(Xtouch, Ytouch, &myButtonVertMenus[VerticalMenuShowing].bounding)) {
        TouchButton = WhatButton(Xtouch, Ytouch, &myButtonVertMenus[VerticalMenuShowing]);
        TouchButtonSet = &myButtonVertMenus[VerticalMenuShowing];
        //myButtonVertMenus[VerticalMenuShowing].ClickHandler(TouchButton);
        
    } else
    {
  //Serial.print ("looking but no match vmenu is");Serial.println(VerticalMenuShowing);
     return false;
    
    }
//Serial.print("found button presss menu:");Serial.print(TouchButtonSet->menuID);Serial.print(" button");Serial.println(TouchButton);
    OutlineMenuButton(TouchButtonSet,TouchButton,WHITE);
    return true;
}

void ProcessTouch(int Xtouch, int Ytouch)
{

    
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
   
    if (InRect(Xtouch, Ytouch, &myButtonVertMenus[VerticalMenuShowing].bounding)) {
      //Serial.print("found vertical1  buttons");
        int k = WhatButton(Xtouch, Ytouch, &myButtonVertMenus[VerticalMenuShowing]);
        myButtonVertMenus[VerticalMenuShowing].pClickHandler(k);
        return;
    }

}

boolean InRect(int &x, int &y, struct rect *Rect)
{

  if (x > Rect->x && x < Rect->xmax) {
   //Serial.println("Found X");
    if (y > Rect->y && y < Rect->ymax) {
      //Serial.println("Found Y");
      return true;
    }
    else
    {
      return false;
    }
  }
  else {
    return false;
  }
}


int WhatButton(int &x, int &y, struct buttonsetdef *butdefset)
{
  //Serial.print ("what menu count:");Serial.println( butdefset->Count);

  for (int i = 0 ; i < butdefset->ButtonCount; i++) {
    //Serial.println ("looking at index:");Serial.println(i);
    //SerialprintRect(&butsetdef->buttondefs[i].Rect);
    if (InRect(x, y, &butdefset->buttondefs[i].Rect) == true) {
        //Serial.print("Found:");Serial.println(i);
        return i;
    }
  }
  return -1;
}

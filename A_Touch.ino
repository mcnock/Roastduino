
//----------------------------------------------------------------------------------------------------------------------------------------------------
// TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH
//----------------------------------------------------------------------------------------------------------------------------------------------------


void ProcessTouch(int Xtouch, int Ytouch)
{
    int Ytouchinv = myGLCD.getDisplayYSize() - Ytouch;
    //Serial.print("touch detected X"); Serial.print(Xtouch);Serial.print(" Y");Serial.println(Ytouch);

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
        VerticalButtonPrior = k;
        myButtonVertMenus[VerticalMenuShowing].ClickHandler(k);
        return;
    }

}

boolean InRect(int &x, int &y, struct rect *Rect)
{

  if (x > Rect->xmin && x < Rect->xmax) {
   //Serial.println("Found X");
    if (y > Rect->ymin && y < Rect->ymax) {
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
  //Serial.print ("what menu count:"); Serial.println( butdefset->Count);

  for (int i = 0 ; i < butdefset->Count; i++) {
    //Serial.println ("looking at index:"); Serial.println(i);
    //SerialprintRect(&butsetdef->buttondefs[i].Rect);
    if (InRect(x, y, &butdefset->buttondefs[i].Rect) == true) {
        //Serial.print("Found:");Serial.println(i);
        myGLCD.setColor(WHITE);
        myGLCD.drawRect(butdefset->buttondefs[i].x, butdefset->buttondefs[i].y, butdefset->buttondefs[i].w + butdefset->buttondefs[i].x, butdefset->buttondefs[i].h + butdefset->buttondefs[i].y);
        myGLCD.setColor(BLACK);
        delay(250);

        myGLCD.drawRect(butdefset->buttondefs[i].x, butdefset->buttondefs[i].y, butdefset->buttondefs[i].w + butdefset->buttondefs[i].x, butdefset->buttondefs[i].h + butdefset->buttondefs[i].y);


         return i;
    }
  }
  return -1;
}

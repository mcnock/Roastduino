
//----------------------------------------------------------------------------------------------------------------------------------------------------
// TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH          TOUCH
//----------------------------------------------------------------------------------------------------------------------------------------------------


void ProcessTouch(int Xtouch, int Ytouch)
{
    int Ytouchinv = myGLCD.getDisplayYSize() - Ytouch;

    if (InRect(Xtouch, Ytouch, &myHorizontalButtonControl.bounding)) {
        Serial.println("found hor buttons");
        int i = WhatMenu(Xtouch, Ytouch, &myHorizontalButtonControl);
        //Serial.print("menu control found: "); Serial.println(i);
        ProcessHorizontal(i);
        return;
    }
  
   
        if (InRect(Xtouch, Ytouch, &myButtonVertMenus[VerticalMenuShowing].bounding)) {
            Serial.print("found vertical1  buttons");
            int i = WhatMenu(Xtouch, Ytouch, &myButtonVertMenus[VerticalMenuShowing]);
            //Serial.print("Vmenu1 found index: "); Serial.println(i);
            switch (VerticalMenuShowing)
            {
            case (0):
                ProcessVertMenu0(i);
                break;
            case(1):
                ProcessVertMenu1(i);
                break;
            case(2):
                ProcessVertMenu2(i);
                break;

            default:
                break;
            }
            return;
        }

}

boolean InRect(int &x, int &y, struct rect *Rect)
{

  if (x > Rect->xmin && x < Rect->xmax) {
   // Serial.println("Found X");
    if (y > Rect->ymin && y < Rect->ymax) {
     // Serial.println("Found Y");
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


int WhatMenu(int &x, int &y, struct buttonsetdef *butdefset)
{
  Serial.print ("what menu count:"); Serial.println( butdefset->Count);

  for (int i = 0 ; i < butdefset->Count; i++) {
    //Serial.print ("looking at index:"); Serial.println(i);
    //SerialprintRect(&butsetdef->buttondefs[i].Rect);
    if (InRect(x, y, &butdefset->buttondefs[i].Rect) == true) {
        Serial.print("Found:");Serial.print(i);
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

//-------------------------------------------------  ---------------------------------------------------------------------------------------------------
// MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS
//----------------------------------------------------------------------------------------------------------------------------------------------------


void DrawControlButtons() {
  int height = 25;
  int width = 35;
  int row = 0;
  int col = 130;
  myButtonControl.Count = 5;
  if (myButtonControl.buttondefs == 0) {
    //Serial.println ("Allocating myButtonControl");
    myButtonControl.buttondefs = (buttondef*) malloc(myButtonControl.Count * sizeof(buttondef));

    int i = 0;
    i = 0; myButtonControl.buttondefs[i].x = col + (width * i); myButtonControl.buttondefs[i].y = row; myButtonControl.buttondefs[i].h = height;
    myButtonControl.buttondefs[i].w = width; myButtonControl.buttondefs[i].color = GREEN; strcpy(myButtonControl.buttondefs[i].label, "Strt");  myButtonControl.buttondefs[i].fontsize = 1;
    i = 1; myButtonControl.buttondefs[i].x = col + (width * i); myButtonControl.buttondefs[i].y = row; myButtonControl.buttondefs[i].h = height;
    myButtonControl.buttondefs[i].w = width; myButtonControl.buttondefs[i].color = RED; strcpy(myButtonControl.buttondefs[i].label, "Stop");  myButtonControl.buttondefs[i].fontsize = 1;
    i = 2; myButtonControl.buttondefs[i].x = col + (width * i); myButtonControl.buttondefs[i].y = row; myButtonControl.buttondefs[i].h = height;
    myButtonControl.buttondefs[i].w = width; myButtonControl.buttondefs[i].color = BLUE; strcpy(myButtonControl.buttondefs[i].label, "Fan ");  myButtonControl.buttondefs[i].fontsize = 1;
    i = 3; myButtonControl.buttondefs[i].x = col + (width * i); myButtonControl.buttondefs[i].y = row; myButtonControl.buttondefs[i].h = height;
    myButtonControl.buttondefs[i].w = width; myButtonControl.buttondefs[i].color = WHITE; strcpy(myButtonControl.buttondefs[i].label, "Refr");  myButtonControl.buttondefs[i].fontsize = 1;
    i = 4; myButtonControl.buttondefs[i].x = col + (width * i); myButtonControl.buttondefs[i].y = row; myButtonControl.buttondefs[i].h = height;
    myButtonControl.buttondefs[i].w = width; myButtonControl.buttondefs[i].color = WHITE; strcpy(myButtonControl.buttondefs[i].label, "T");  myButtonControl.buttondefs[i].fontsize = 1;

    SetMenuBoundingRect(myButtonControl);

  }
  //Serial.println ("Drawing myButtonControl");
  DrawButtons(myButtonControl);
}

void ProcessControl(int i) {
  //Serial.println (i);
  switch (i) {
    case 0:
      if (State == AMSTOPPED || State == AMFANONLY  ) {
        newState = AMROASTING;
        //Serial.println("Start Detected!");
      }
      else  if (State == AMOVERHEATEDCOIL || State == AMOVERHEATEDFAN || State == AMAUTOCOOLING ) {
        newState = AMROASTING;
        //Serial.println("ReStart Detected!");
      }
      break;
    case 1:
      if (State != AMSTOPPED) {
        newState = AMSTOPPED;
      //Serial.println("Stop Detected!");
      }

      break;
    case 2:
      if ( State == AMSTOPPED) {
      //Serial.println("Fan On Detected");
        newState = AMFANONLY;
      }
      break;
    case 3:
    //Serial.println("Redraw graph detected");
      graphProfile();

      break;
    case 4:
      //Serial.println("Touch Toggle");
      ShowTouch = !ShowTouch;
      FlashButton(myButtonControl,i);
      break;

  }


}

void DrawMenu1() {
  int menuH = 25;
  int width = 30;
  int row = 240;
  int col = 320 - width;
  myButtonMenu1.Count = 1;
  if (myButtonMenu1.buttondefs == 0) {
    //Serial.println ("Allocating myButtonMenu1");
    myButtonMenu1.buttondefs = (buttondef*) malloc(myButtonMenu1.Count * sizeof(buttondef));
    int i = 0;
    myButtonMenu1.buttondefs[i].x = col; myButtonMenu1.buttondefs[i].y = row - (menuH * 1); myButtonMenu1.buttondefs[i].h = menuH; myButtonMenu1.buttondefs[i].w = width; myButtonMenu1.buttondefs[i].color = GREEN; strcpy(myButtonMenu1.buttondefs[i].label, "L&S");  myButtonMenu1.buttondefs[i].fontsize = 1;
    //i = 1;
    //myButtonMenu1[i].x = col; myButtonMenu1[i].y = row - (menuH * 2); myButtonMenu1[i].h = menuH; myButtonMenu1[i].w = width; myButtonMenu1[i].color = WHITE; strcpy(myButtonMenu1[i].label,"B"); myButtonMenu1[i].fontsize = 1;

    SetMenuBoundingRect(myButtonMenu1);


  }
  //Serial.print ("Drawing menu1 with count:");Serial.print(myButtonMenu1Count);
  DrawButtons(myButtonMenu1);
}

void ProcessMenu1(int i) {
  //Serial.println (i);
  switch (i) {
    case 0:
      DrawMenu2();
      break;
    case 1:

      break;
    case 2:

      break;
    case 3:

      break;
  }

}

void DrawMenu2() {
  Menu2Showing = true;
  int width = 40;
  int col = 320 - width;
  myButtonMenu2.Count = 9;
  int menuH = 240 / 9;
  int row = 240 - menuH;

  if (myButtonMenu2.buttondefs == 0) {
    //Serial.println ("Allocating myButtonMenu2");
    myButtonMenu2.buttondefs = (buttondef*) malloc(myButtonMenu2.Count * sizeof(buttondef));
    int i = 0; strcpy(myButtonMenu2.buttondefs[i].label, "Ld F"); myButtonMenu2.buttondefs[i].color = GREEN; myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
    i = 1; strcpy(myButtonMenu2.buttondefs[i].label, "Ld V");  myButtonMenu2.buttondefs[i].color = GREEN;  myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
    i = 2; strcpy(myButtonMenu2.buttondefs[i].label, "Ld FC"); myButtonMenu2.buttondefs[i].color = GREEN;  myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
    i = 3; strcpy(myButtonMenu2.buttondefs[i].label, "Ld C");  myButtonMenu2.buttondefs[i].color = GREEN;  myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
    i = 4; strcpy(myButtonMenu2.buttondefs[i].label, "Sv F");  myButtonMenu2.buttondefs[i].color = YELLOW; myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
    i = 5; strcpy(myButtonMenu2.buttondefs[i].label, "Sv V");  myButtonMenu2.buttondefs[i].color = YELLOW; myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
    i = 6; strcpy(myButtonMenu2.buttondefs[i].label, "Sv FC"); myButtonMenu2.buttondefs[i].color = YELLOW; myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
    i = 7; strcpy(myButtonMenu2.buttondefs[i].label, "Sv C");  myButtonMenu2.buttondefs[i].color = YELLOW; myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
    i = 8; strcpy(myButtonMenu2.buttondefs[i].label, "X");     myButtonMenu2.buttondefs[i].color = WHITE;  myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;

    SetMenuBoundingRect(myButtonMenu2);

  }
  DrawButtons(myButtonMenu2);
}

void ProcessMenu2(int i) {
  //Serial.println (i);
  switch (i) {
    case 0:
    case 1:
    case 2:
    case 3:
      Load(i);
      FlashButton(myButtonMenu2,i);
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      FlashButton(myButtonMenu2,i);
      Save(i);
      break;
    case 8:
      Menu2Showing = false;
      //Serial.println("Close menu 2");
      graphProfile();
      break;
    default:
      Menu2Showing = false;
      break;

  }

}

void SetMenuBoundingRect(buttonsetdef &butdefset) {

  for (int i = 0 ; i < butdefset.Count; i++) {
    butdefset.buttondefs[i].Rect.xmin = butdefset.buttondefs[i].x;
    butdefset.buttondefs[i].Rect.ymin = butdefset.buttondefs[i].y;
    butdefset.buttondefs[i].Rect.xmax = butdefset.buttondefs[i].x + butdefset.buttondefs[i].w;
    butdefset.buttondefs[i].Rect.ymax = butdefset.buttondefs[i].y + butdefset.buttondefs[i].h;
    //Serial.print("butdefs[i].Rect.xmin");Serial.println(butdefs[i].Rect.xmin);
    //Serial.print("butdefs[i].Rect.ymin");Serial.println(butdefs[i].Rect.ymin);
    //Serial.print("butdefs[i].Rect.xmax");Serial.println(butdefs[i].Rect.xmax);
    //Serial.print("butdefs[i].Rect.ymax");Serial.println(butdefs[i].Rect.ymax);


    if ((butdefset.buttondefs[i].x + butdefset.buttondefs[i].w) > butdefset.bounding.xmax) {
      butdefset.bounding.xmax = (butdefset.buttondefs[i].x + butdefset.buttondefs[i].w);
    }
    if ((butdefset.buttondefs[i].y + butdefset.buttondefs[i].h) > butdefset.bounding.ymax) {
      butdefset.bounding.ymax = (butdefset.buttondefs[i].y + butdefset.buttondefs[i].h);
    }
    if (butdefset.buttondefs[i].x < butdefset.bounding.xmin) {
      butdefset.bounding.xmin = butdefset.buttondefs[i].x;
    }
    if (butdefset.buttondefs[i].y < butdefset.bounding.ymin) {
      butdefset.bounding.ymin  = butdefset.buttondefs[i].y;
    }



  }
  //Serial.print ("MenuShowingXmin:");Serial.println (bounding.xmin);
  //Serial.print ("MenuShowingYmin:");Serial.println (bounding.ymin);
  //Serial.print ("MenuShowingXmax:");Serial.println (bounding.xmax);
  //Serial.print ("MenuShowingYmax:");Serial.println (bounding.ymax);



}

void FlashButton(buttonsetdef &butdefset, int i)
{
  tft.fillRect(butdefset.buttondefs[i].x, butdefset.buttondefs[i].y, butdefset.buttondefs[i].w, butdefset.buttondefs[i].h, MAGENTA);
  delay(250); 
  DrawButton(butdefset,i);
}
void DrawButton(buttonsetdef &butdefset, int i)
  {
    //draw button
    //Serial.print ("Drawing buttons x:");Serial.print (butdefs[i].x), Serial.print (" y:");Serial.print ( butdefs[i].y);Serial.print (" w:");Serial.print ( butdefs[i].w) ;Serial.print (" h:");Serial.println ( butdefs[i].h);

    tft.fillRect(butdefset.buttondefs[i].x, butdefset.buttondefs[i].y, butdefset.buttondefs[i].w, butdefset.buttondefs[i].h, butdefset.buttondefs[i].color);
    tft.drawRect(butdefset.buttondefs[i].x, butdefset.buttondefs[i].y, butdefset.buttondefs[i].w, butdefset.buttondefs[i].h, BLACK);
    tft.setTextColor(BLACK, butdefset.buttondefs[i].color);


    //draw text
    //Serial.print ("sizer of label:");Serial.print (strlen( butdefs[i].label));Serial.print( "Label:");Serial.println(butdefs[i].label);
    //Serial.println(butdefs[i].fontsize);
    int xOffset = (butdefset.buttondefs[i].w - (strlen(butdefset.buttondefs[i].label) * butdefset.buttondefs[i].fontsize * 8)) / 2;
    int yOffset = (butdefset.buttondefs[i].h - (butdefset.buttondefs[i].fontsize * 10)) / 2;
    
    //Serial.print ("Drawing text x:");Serial.print (xOffset);Serial.print (" y:");Serial.print(yOffset);Serial.print (" text:");Serial.println ( butdefs[i].label);

    tft.setCursor(butdefset.buttondefs[i].x + xOffset + 2 , butdefset.buttondefs[i].y + yOffset + 2);
    tft.println(butdefset.buttondefs[i].label);
    
  }

void DrawButtons(buttonsetdef &butdefset) {

  //Serial.print ("Drawing buttons start. Count:");Serial.println(count);

  for (int i = 0 ; i < butdefset.Count; i++) {
     DrawButton(butdefset,i);
  }
}

void DrawBox(int x, int y, int w, int h, uint16_t color) {

  tft.fillRect(x, y, w, h, color);
  tft.drawRect(x, y, w, h, BLACK);


}


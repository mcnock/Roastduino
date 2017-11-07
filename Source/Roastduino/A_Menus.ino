//-------------------------------------------------  ---------------------------------------------------------------------------------------------------
// MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS
//----------------------------------------------------------------------------------------------------------------------------------------------------


void DrawControlButtons(){
  int height = 30;
  int width = 40;
  int row = 0;
  int col = 140;
  myButtonControlCount = 4;
  if (myButtonControl == 0) {
  //Serial.println ("Allocating myButtonControl");
    myButtonControl = (buttondef*) malloc(myButtonControlCount * sizeof(buttondef));

    int i = 0;
    i = 0; myButtonControl[i].x = col + (width * i); myButtonControl[i].y = row; myButtonControl[i].h = height;
    myButtonControl[i].w = width; myButtonControl[i].color = GREEN; strcpy(myButtonControl[i].label, "Strt");  myButtonControl[i].fontsize = 1;
    i = 1; myButtonControl[i].x = col + (width * i); myButtonControl[i].y = row; myButtonControl[i].h = height;
    myButtonControl[i].w = width; myButtonControl[i].color = RED; strcpy(myButtonControl[i].label, "Stop");  myButtonControl[i].fontsize = 1;
    i = 2; myButtonControl[i].x = col + (width * i); myButtonControl[i].y = row; myButtonControl[i].h = height;
    myButtonControl[i].w = width; myButtonControl[i].color = BLUE; strcpy(myButtonControl[i].label, "Fan ");  myButtonControl[i].fontsize = 1;
    i = 3; myButtonControl[i].x = col + (width * i); myButtonControl[i].y = row; myButtonControl[i].h = height;
    myButtonControl[i].w = width; myButtonControl[i].color = WHITE; strcpy(myButtonControl[i].label, "Refr");  myButtonControl[i].fontsize = 1;

    SetMenuBoundingRect(myButtonControl, myButtonControlCount, myButtonControlrect);

  }
//Serial.println ("Drawing myButtonControl");
  DrawButtons(myButtonControl, myButtonControlCount);
}

void ProcessControl(int i){
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
        Serial.println("Stop Detected!");
      }

      break;
    case 2:
      if ( State == AMSTOPPED) {
        Serial.println("Fan On Detected");
        newState = AMFANONLY;
      }
      break;
    case 3:
       Serial.println("Redraw graph detected");
       graphProfile();
 
      break;
    
  }


}

void DrawMenu1(){
  int menuH = 25;
  int width = 30;
  int row = 240;
  int col = 320 - width;
  myButtonMenu1Count = 1;
  if (myButtonMenu1 == 0) {
  //Serial.println ("Allocating myButtonMenu1");
    myButtonMenu1 = (buttondef*) malloc(myButtonMenu1Count * sizeof(buttondef));
    int i = 0;
    myButtonMenu1[i].x = col; myButtonMenu1[i].y = row - (menuH * 1); myButtonMenu1[i].h = menuH; myButtonMenu1[i].w = width; myButtonMenu1[i].color = GREEN; strcpy(myButtonMenu1[i].label, "L&S");  myButtonMenu1[i].fontsize = 1;
    //i = 1;
    //myButtonMenu1[i].x = col; myButtonMenu1[i].y = row - (menuH * 2); myButtonMenu1[i].h = menuH; myButtonMenu1[i].w = width; myButtonMenu1[i].color = WHITE; strcpy(myButtonMenu1[i].label,"B"); myButtonMenu1[i].fontsize = 1;

    SetMenuBoundingRect(myButtonMenu1, myButtonMenu1Count, myButtonMenu1rect);


  }
//Serial.print ("Drawing menu1 with count:");Serial.print(myButtonMenu1Count);
  DrawButtons(myButtonMenu1, myButtonMenu1Count);
}

void ProcessMenu1(int i){
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

void DrawMenu2(){
  Menu2Showing = true;
  int width = 40;
  int col = 320 - width;
  myButtonMenu2Count = 9;
  int menuH = 240 / 9;
  int row = 240 - menuH;

  if (myButtonMenu2 == 0) {
  //Serial.println ("Allocating myButtonMenu2");
    myButtonMenu2 = (buttondef*) malloc(myButtonMenu2Count * sizeof(buttondef));
    int i = 0; strcpy(myButtonMenu2[i].label, "Ld F"); myButtonMenu2[i].color = GREEN; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
    i = 1; strcpy(myButtonMenu2[i].label, "Ld V"); myButtonMenu2[i].color = GREEN; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
    i = 2; strcpy(myButtonMenu2[i].label, "Ld FC"); myButtonMenu2[i].color = GREEN; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
    i = 3; strcpy(myButtonMenu2[i].label, "Ld C"); myButtonMenu2[i].color = GREEN; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
    i = 4; strcpy(myButtonMenu2[i].label, "Sv F"); myButtonMenu2[i].color = YELLOW; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
    i = 5; strcpy(myButtonMenu2[i].label, "Sv V"); myButtonMenu2[i].color = YELLOW; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
    i = 6; strcpy(myButtonMenu2[i].label, "Sv FC"); myButtonMenu2[i].color = YELLOW; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
    i = 7; strcpy(myButtonMenu2[i].label, "Sv C"); myButtonMenu2[i].color = YELLOW; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
    i = 8; strcpy(myButtonMenu2[i].label, "X"); myButtonMenu2[i].color = WHITE; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
    
    SetMenuBoundingRect(myButtonMenu2, myButtonMenu2Count, myButtonMenu2rect);

  }
  DrawButtons(myButtonMenu2, myButtonMenu2Count);
}

void ProcessMenu2(int i){
//Serial.println (i);
  switch (i) {
    case 0:

      break;
    case 1:

      break;
    case 2:

      break;
    case 3:

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

      Menu2Showing = false;
    //Serial.println("Close menu 2");
      graphProfile();
      
      break;
   default:
     Menu2Showing = false;
     break;
 
  }

}

void SetMenuBoundingRect(buttondef butdefs[], int count, rect &bounding){

  for (int i = 0 ; i < count; i++) {
    butdefs[i].Rect.xmin = butdefs[i].x;
    butdefs[i].Rect.ymin = butdefs[i].y;
    butdefs[i].Rect.xmax = butdefs[i].x + butdefs[i].w;
    butdefs[i].Rect.ymax = butdefs[i].y + butdefs[i].h;
  //Serial.print("butdefs[i].Rect.xmin");Serial.println(butdefs[i].Rect.xmin);
  //Serial.print("butdefs[i].Rect.ymin");Serial.println(butdefs[i].Rect.ymin);
  //Serial.print("butdefs[i].Rect.xmax");Serial.println(butdefs[i].Rect.xmax);
  //Serial.print("butdefs[i].Rect.ymax");Serial.println(butdefs[i].Rect.ymax);


    if ((butdefs[i].x + butdefs[i].w) > bounding.xmax) {
      bounding.xmax = (butdefs[i].x + butdefs[i].w);
    }
    if ((butdefs[i].y + butdefs[i].h) > bounding.ymax) {
      bounding.ymax = (butdefs[i].y + butdefs[i].h);
    }
    if (butdefs[i].x < bounding.xmin) {
      bounding.xmin = butdefs[i].x;
    }
    if (butdefs[i].y < bounding.ymin) {
      bounding.ymin  = butdefs[i].y;
    }



  }
//Serial.print ("MenuShowingXmin:");Serial.println (bounding.xmin);
//Serial.print ("MenuShowingYmin:");Serial.println (bounding.ymin);
//Serial.print ("MenuShowingXmax:");Serial.println (bounding.xmax);
//Serial.print ("MenuShowingYmax:");Serial.println (bounding.ymax);



}

void DrawButtons(buttondef butdefs[], int &count) {

//Serial.print ("Drawing buttons start. Count:");Serial.println(count);

  for (int i = 0 ; i < count; i++) {
    //draw button
  //Serial.print ("Drawing buttons x:");Serial.print (butdefs[i].x), Serial.print (" y:");Serial.print ( butdefs[i].y);Serial.print (" w:");Serial.print ( butdefs[i].w) ;Serial.print (" h:");Serial.println ( butdefs[i].h);

    tft.fillRect(butdefs[i].x, butdefs[i].y, butdefs[i].w, butdefs[i].h, butdefs[i].color);
    tft.drawRect(butdefs[i].x, butdefs[i].y, butdefs[i].w, butdefs[i].h, BLACK);
    tft.setTextColor(BLACK, butdefs[i].color);


    //draw text
  //Serial.print ("sizer of label:");Serial.print (strlen( butdefs[i].label));Serial.print( "Label:");Serial.println(butdefs[i].label);
  //Serial.println(butdefs[i].fontsize);
    int xOffset = (butdefs[i].w - (strlen(butdefs[i].label) * butdefs[i].fontsize * 8)) / 2;
    int yOffset = (butdefs[i].h - (butdefs[i].fontsize * 10)) / 2;
  //Serial.print ("Drawing text x:");Serial.print (xOffset);Serial.print (" y:");Serial.print(yOffset);Serial.print (" text:");Serial.println ( butdefs[i].label);

    tft.setCursor(butdefs[i].x + xOffset + 2 , butdefs[i].y + yOffset + 2);
    tft.println(butdefs[i].label);
  }
}

void DrawBox(int x, int y, int w, int h, uint16_t color){

  tft.fillRect(x, y, w, h, color);
  tft.drawRect(x, y, w, h, BLACK);


}


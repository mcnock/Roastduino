//----------------------------------------------------------------------------------------------------------------------------------------------------
// MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS
//----------------------------------------------------------------------------------------------------------------------------------------------------

void HideButtons()
{
  MenuShowing = 0;
  graphProfile();
}
void DrawMenu1()
{
  int menuH = 25;
  int width = 30;
  int row = 240;
  int col = 320 - width;
  MenuShowing = 1;
  if (myButtonMenu1 == 0) {
    Serial.println ("Allocating myButtonMenu1");
    myButtonMenu1 = (buttondef*) malloc(1 * sizeof(buttondef));
    myButtonMenu1Count = 1;
    int i = 0;
    myButtonMenu1[i].x = col; myButtonMenu1[i].y = row - (menuH * 1); myButtonMenu1[i].h = menuH; myButtonMenu1[i].w = width; myButtonMenu1[i].color = GREEN;strcpy(myButtonMenu1[i].label,"L&S");  myButtonMenu1[i].fontsize = 1;
    //i = 1;
    //myButtonMenu1[i].x = col; myButtonMenu1[i].y = row - (menuH * 2); myButtonMenu1[i].h = menuH; myButtonMenu1[i].w = width; myButtonMenu1[i].color = WHITE; strcpy(myButtonMenu1[i].label,"B"); myButtonMenu1[i].fontsize = 1;
  }
  Serial.println ("Drawing menu1");
  DrawButtons(myButtonMenu1, 2);
}


void DrawMenu2()
{
  MenuShowing = 2;
  int menuH = 30;
  int row = 240 - menuH;
  int width = 40;
  int col = 320 - width;
  
if (myButtonMenu2 == 0) {
    Serial.println ("Allocating myButtonMenu2");
    myButtonMenu2 = (buttondef*) malloc(8 * sizeof(buttondef));
    myButtonMenu2Count = 8;
    int i = 0;strcpy(myButtonMenu2[i].label,"Ld F"); myButtonMenu2[i].color = GREEN; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
     i = 1;strcpy(myButtonMenu2[i].label,"Ld V"); myButtonMenu2[i].color = GREEN; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
     i = 2;strcpy(myButtonMenu2[i].label,"Ld FC"); myButtonMenu2[i].color = GREEN; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
     i = 3;strcpy(myButtonMenu2[i].label,"Ld C"); myButtonMenu2[i].color = GREEN; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
     i = 4;strcpy(myButtonMenu2[i].label,"Sv F"); myButtonMenu2[i].color = WHITE; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
     i = 5;strcpy(myButtonMenu2[i].label,"Sv V"); myButtonMenu2[i].color = WHITE; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
     i = 6;strcpy(myButtonMenu2[i].label,"Sv FC"); myButtonMenu2[i].color = WHITE; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;
     i = 7;strcpy(myButtonMenu2[i].label,"Sv C"); myButtonMenu2[i].color = WHITE; myButtonMenu2[i].x = col; myButtonMenu2[i].y = row - (menuH * i); myButtonMenu2[i].h = menuH; myButtonMenu2[i].w = width;  myButtonMenu2[i].fontsize = 1;


  
  }

  DrawButtons(myButtonMenu2, 8);
}

void DrawButtons(buttondef butdefs[], int count) {

  Serial.print ("Drawing buttons start. Count:"); Serial.println(count);

  MenuShowingXmax = 0;
  MenuShowingYmax = 0;
  MenuShowingXmin = 320;
  MenuShowingYmin = 240;

  for (int i = 0 ; i < count; i++) {
    //draw button
    Serial.print ("Drawing buttons x:"); Serial.print (butdefs[i].x), Serial.print (" y:"); Serial.print ( butdefs[i].y); Serial.print (" w:"); Serial.print ( butdefs[i].w) ; Serial.print (" h:"); Serial.println ( butdefs[i].h);

    tft.fillRect(butdefs[i].x, butdefs[i].y, butdefs[i].w, butdefs[i].h, butdefs[i].color);
    tft.drawRect(butdefs[i].x, butdefs[i].y, butdefs[i].w, butdefs[i].h, BLACK);
    tft.setTextColor(BLACK, butdefs[i].color);


    //draw text
    Serial.print ("sizer of label:"); Serial.print (strlen( butdefs[i].label)); Serial.print( "Label:"); Serial.println(butdefs[i].label);
    Serial.println(butdefs[i].fontsize);
    int xOffset = (butdefs[i].w - (strlen( butdefs[i].label) * butdefs[i].fontsize * 8)) / 2;
    int yOffset = (butdefs[i].h - (butdefs[i].fontsize * 10)) / 2;
    Serial.print ("Drawing text x:"); Serial.print (xOffset); Serial.print (" y:"); Serial.print(yOffset); Serial.print (" text:"); Serial.println ( butdefs[i].label);

    tft.setCursor(butdefs[i].x + xOffset + 2 , butdefs[i].y + yOffset + 2);
    tft.println(butdefs[i].label);

    //capture min and max of menu (for touch)
    if ((butdefs[i].x + butdefs[i].w) > MenuShowingXmax) {
      MenuShowingXmax = (butdefs[i].x + butdefs[i].w);
    }
    if ((butdefs[i].y + butdefs[i].h) > MenuShowingYmax) {
      MenuShowingYmax = (butdefs[i].y + butdefs[i].h);
    }
    if (butdefs[i].x < MenuShowingXmin) {
      MenuShowingXmin = butdefs[i].x;
    }
    if (butdefs[i].y < MenuShowingYmin) {
      MenuShowingYmin  = butdefs[i].y;
    }



  }
  Serial.print ("MenuShowingXmin:"); Serial.println (MenuShowingXmin);
  Serial.print ("MenuShowingYmin:"); Serial.println (MenuShowingYmin);
  Serial.print ("MenuShowingXmax:"); Serial.println (MenuShowingXmax);
  Serial.print ("MenuShowingYmax:"); Serial.println (MenuShowingYmax);


}


void DrawBox(int x, int y, int w, int h, uint16_t color)
{

  tft.fillRect(x, y, w, h, color);
  tft.drawRect(x, y, w, h, BLACK);



}


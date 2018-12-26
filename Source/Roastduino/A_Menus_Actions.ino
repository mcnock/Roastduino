//-------------------------------------------------  ---------------------------------------------------------------------------------------------------
// MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS
//----------------------------------------------------------------------------------------------------------------------------------------------------


void DrawControlButtons() {
  //these ar the buttoms on top row starting about 1/2 across screen
  int height = 25;
  int width = 35;
  int row = 0;
  int col = 130;
  myHorizontalButtonControl.Count = 5;
  if (myHorizontalButtonControl.buttondefs == 0) {
    //Serial.println ("Allocating myHorizontalButtonControl");
    myHorizontalButtonControl.buttondefs = (buttondef*) malloc(myHorizontalButtonControl.Count * sizeof(buttondef));

    int i = 0;
    i = 0; myHorizontalButtonControl.buttondefs[i].x = col + (width * i); myHorizontalButtonControl.buttondefs[i].y = row; myHorizontalButtonControl.buttondefs[i].h = height;
    myHorizontalButtonControl.buttondefs[i].w = width; myHorizontalButtonControl.buttondefs[i].color = GREEN; strcpy(myHorizontalButtonControl.buttondefs[i].label, "Strt");  myHorizontalButtonControl.buttondefs[i].fontsize = 1;
    i = 1; myHorizontalButtonControl.buttondefs[i].x = col + (width * i); myHorizontalButtonControl.buttondefs[i].y = row; myHorizontalButtonControl.buttondefs[i].h = height;
    myHorizontalButtonControl.buttondefs[i].w = width; myHorizontalButtonControl.buttondefs[i].color = RED; strcpy(myHorizontalButtonControl.buttondefs[i].label, "Stop");  myHorizontalButtonControl.buttondefs[i].fontsize = 1;
    i = 2; myHorizontalButtonControl.buttondefs[i].x = col + (width * i); myHorizontalButtonControl.buttondefs[i].y = row; myHorizontalButtonControl.buttondefs[i].h = height;
    myHorizontalButtonControl.buttondefs[i].w = width; myHorizontalButtonControl.buttondefs[i].color = BLUE; strcpy(myHorizontalButtonControl.buttondefs[i].label, "Fan ");  myHorizontalButtonControl.buttondefs[i].fontsize = 1;
    i = 3; myHorizontalButtonControl.buttondefs[i].x = col + (width * i); myHorizontalButtonControl.buttondefs[i].y = row; myHorizontalButtonControl.buttondefs[i].h = height;
    myHorizontalButtonControl.buttondefs[i].w = width; myHorizontalButtonControl.buttondefs[i].color = WHITE; strcpy(myHorizontalButtonControl.buttondefs[i].label, "Refr");  myHorizontalButtonControl.buttondefs[i].fontsize = 1;
    i = 4; myHorizontalButtonControl.buttondefs[i].x = col + (width * i); myHorizontalButtonControl.buttondefs[i].y = row; myHorizontalButtonControl.buttondefs[i].h = height;
    myHorizontalButtonControl.buttondefs[i].w = width; myHorizontalButtonControl.buttondefs[i].color = WHITE; strcpy(myHorizontalButtonControl.buttondefs[i].label, "T");  myHorizontalButtonControl.buttondefs[i].fontsize = 1;

    SetMenuBoundingRect(myHorizontalButtonControl);

  }
  //Serial.println ("Drawing myHorizontalButtonControl");
  DrawButtons(myHorizontalButtonControl);
}


void EndRoastToggleAction() {
  Serial.println("Toggle end action");
  if (EndingSetPoint == 4) {
    EndingSetPoint = 5;
  }
  else {
    EndingSetPoint = 4;
  }

  graphProfile();
}

void FanAction() {
  Serial.println("Fan action");

  if ( State == STATESTOPPED) {
    newState = STATEFANONLY;
  }
}
void RefreshAction() {
  graphProfile();
}
void StartAction() {
  if (State == STATESTOPPED || State == STATEFANONLY  ) {
    newState = STATEROASTING;
    //Serial.println("Start Detected!");
  }
  else if (not(State == STATENOFANCURRENT || State == STATEOVERHEATED || State == STATECOOLING )) {
    newState = STATEROASTING;
    //Serial.println("ReStart Detected!");
  }
}

void EndAction() {
  if (State != STATESTOPPED) {
    newState = STATESTOPPED;
    //Serial.println("Stop Detected!");
  }

}
void ShowTouchAction() {
  //Serial.println("Touch Toggle");
  ShowTouch = !ShowTouch;

}
void ProcessHorizontal(int i) {
  //Serial.println (i);
  switch (i) {
    case 0:
      StartAction();
      FlashButton(myHorizontalButtonControl, i);

      break;
    case 1:
      EndAction();
      FlashButton(myHorizontalButtonControl, i);

      break;
    case 2:
      FanAction();
      FlashButton(myHorizontalButtonControl, i);

      break;
    case 3:
      Serial.println("Redraw graph detected");

      RefreshAction();
      FlashButton(myHorizontalButtonControl, i);

      break;
    case 4:
      ShowTouchAction();
      FlashButton(myHorizontalButtonControl, i);

      break;

  }


}
void DrawVertMenu1() {
  //these are vertical on right side starting at bottom going up
  int menuH = 30;
  int width = 25;
  int row = 230;
  int col = 320 - width;
  myButtonVertMenu1.Count = 4;
  if (myButtonVertMenu1.buttondefs == 0) {
    //Serial.println ("Allocating myButtonVertMenu1");
    myButtonVertMenu1.buttondefs = (buttondef*) malloc(myButtonVertMenu1.Count * sizeof(buttondef));
    int i = 0;
    myButtonVertMenu1.buttondefs[i].x = col; myButtonVertMenu1.buttondefs[i].y = row - (menuH * (i + 1)); myButtonVertMenu1.buttondefs[i].h = menuH; myButtonVertMenu1.buttondefs[i].w = width; myButtonVertMenu1.buttondefs[i].color = GREEN; strcpy(myButtonVertMenu1.buttondefs[i].label, "T+");  myButtonVertMenu1.buttondefs[i].fontsize = 1;
    i = 1;
    myButtonVertMenu1.buttondefs[i].x = col; myButtonVertMenu1.buttondefs[i].y = row - (menuH * (i + 1)); myButtonVertMenu1.buttondefs[i].h = menuH; myButtonVertMenu1.buttondefs[i].w = width; myButtonVertMenu1.buttondefs[i].color = GREEN; strcpy(myButtonVertMenu1.buttondefs[i].label, "T-");  myButtonVertMenu1.buttondefs[i].fontsize = 1;
    i = 2;
    myButtonVertMenu1.buttondefs[i].x = col; myButtonVertMenu1.buttondefs[i].y = row - (menuH * (i + 1)); myButtonVertMenu1.buttondefs[i].h = menuH; myButtonVertMenu1.buttondefs[i].w = width; myButtonVertMenu1.buttondefs[i].color = GREEN; strcpy(myButtonVertMenu1.buttondefs[i].label, "L+");  myButtonVertMenu1.buttondefs[i].fontsize = 1;
    i = 3;
    myButtonVertMenu1.buttondefs[i].x = col; myButtonVertMenu1.buttondefs[i].y = row - (menuH * (i + 1)); myButtonVertMenu1.buttondefs[i].h = menuH; myButtonVertMenu1.buttondefs[i].w = width; myButtonVertMenu1.buttondefs[i].color = GREEN; strcpy(myButtonVertMenu1.buttondefs[i].label, "L-");  myButtonVertMenu1.buttondefs[i].fontsize = 1;


    SetMenuBoundingRect(myButtonVertMenu1);


  }
  //Serial.print ("Drawing menu1 with count:");Serial.print(myButtonVertMenu1Count);
  DrawButtons(myButtonVertMenu1);
}

void MoveLast4Point(int i) {
  Serial.print ("MoveLast4Points"); Serial.println (i);
  MySetPoints[2].Temperature = MySetPoints[2].Temperature + (i * 0.5);
  SaveTempEprom(MySetpointsEprom[2] , MySetPoints[2].Temperature);
  MySetPoints[3].Temperature = MySetPoints[3].Temperature + (i * 0.75);
  SaveTempEprom(MySetpointsEprom[3] , MySetPoints[3].Temperature);
  MySetPoints[4].Temperature = MySetPoints[4].Temperature + i;
  SaveTempEprom(MySetpointsEprom[4] , MySetPoints[4].Temperature);
  MySetPoints[5].Temperature = MySetPoints[5].Temperature + i;
  SaveTempEprom(MySetpointsEprom[5] , MySetPoints[5].Temperature);
  //drawprofileline();
  delay(100);
}

void MoveAPoint(int SetPoint, int amount) {
  if (SetPoint > 0) {
    Serial.print(SetPoint); Serial.print("  ");
    Serial.print(MySetPoints[SetPoint].Temperature); Serial.print(" to ");
    MySetPoints[SetPoint].Temperature = MySetPoints[SetPoint].Temperature + amount;
    Serial.println(MySetPoints[SetPoint].Temperature);
    if (MySetPoints[SetPoint].Temperature < 254) {
      MySetPoints[SetPoint].Temperature = 255;
    }
    SaveTempEprom(MySetpointsEprom[SetPoint] , MySetPoints[SetPoint].Temperature);
    delay(100);
  }
}
void MoveGain(int amount) {
  Gain = Gain + amount;
  EEPROM.update(GAIN_EP, Gain);
  delay(100);
}
void MoveIntegral(int amount) {
  Integral = Integral + (float(amount) / 10);
  delay(100);
  EEPROM.update(INTEGRAL_EP , (int)(Integral * 100));
}
void MoveTime(int jsetpointPlusOrMInus) {
  int i = abs(jsetpointPlusOrMInus);
  int editmade = 0;
  if (jsetpointPlusOrMInus > 0) {
    if (i > 0 and i < 5 and MySetPoints[i + 1].SpanMinutes > 1 ) {
      Serial.print ("Increasing "); Serial.print (i); Serial.print (" to ");
      Serial.println (MySetPoints[i].Minutes + 1);
      MySetPoints[i].Minutes = MySetPoints[i].Minutes + 1;
      MySetPoints[i].SpanMinutes = MySetPoints[i].SpanMinutes + 1;
      MySetPoints[i + 1].SpanMinutes = MySetPoints[i + 1].SpanMinutes - 1;
      editmade = 1;
    }
    else if (i == 5) {
      MySetPoints[i].Minutes = MySetPoints[i].Minutes + 1;
      MySetPoints[i].SpanMinutes = MySetPoints[i].SpanMinutes + 1;
      editmade = 1;
    }
  }
  else
  {
    if (i > 0 and i <= 5 and MySetPoints[i].SpanMinutes > 1 ) {
      //Serial.println ("Adusting time increase 1 to 3");
      MySetPoints[i].Minutes = MySetPoints[i].Minutes - 1;
      MySetPoints[i].SpanMinutes = MySetPoints[i].SpanMinutes - 1;
      MySetPoints[i + 1].SpanMinutes = MySetPoints[i + 1].SpanMinutes + 1;
      editmade = 1;
    }
  }
  if (editmade == 1)
  {

    EEPROM.update( MySpanMinutesEprom[i] ,  MySetPoints[i].SpanMinutes);
    EEPROM.update( MySpanMinutesEprom[i + 1] , MySetPoints[i + 1].SpanMinutes);

    graphProfile();
  }
}


void ClearIntergalSum() {
  IntegralSum = 0;
}


void ProcessVertMenu1(int i) {
  //Serial.print ("ProcessVertMenu1:");Serial.println (i);
  switch (i) {
    case 0:
      MoveLast4Point(2);
      drawprofileline();
      //write code to raise last 3 set points
      break;
    case 1:
      MoveLast4Point(-2);
      drawprofileline() ;
      //write code to lower last 3 set points
      break;
    case 2:

      break;
    case 3:

      break;
  }

}

//void DrawMenu2() {
//  //not useed in cde right now
//  Menu2Showing = true;
//  int width = 40;
//  int col = 320 - width;
//  myButtonMenu2.Count = 9;
//  int menuH = 240 / 9;
//  int row = 240 - menuH;
//
//  if (myButtonMenu2.buttondefs == 0) {
//    //Serial.println ("Allocating myButtonMenu2");
//    myButtonMenu2.buttondefs = (buttondef*) malloc(myButtonMenu2.Count * sizeof(buttondef));
//    int i = 0; strcpy(myButtonMenu2.buttondefs[i].label, "Ld F"); myButtonMenu2.buttondefs[i].color = GREEN; myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
//    i = 1; strcpy(myButtonMenu2.buttondefs[i].label, "Ld V");  myButtonMenu2.buttondefs[i].color = GREEN;  myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
//    i = 2; strcpy(myButtonMenu2.buttondefs[i].label, "Ld FC"); myButtonMenu2.buttondefs[i].color = GREEN;  myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
//    i = 3; strcpy(myButtonMenu2.buttondefs[i].label, "Ld C");  myButtonMenu2.buttondefs[i].color = GREEN;  myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
//    i = 4; strcpy(myButtonMenu2.buttondefs[i].label, "Sv F");  myButtonMenu2.buttondefs[i].color = YELLOW; myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
//    i = 5; strcpy(myButtonMenu2.buttondefs[i].label, "Sv V");  myButtonMenu2.buttondefs[i].color = YELLOW; myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
//    i = 6; strcpy(myButtonMenu2.buttondefs[i].label, "Sv FC"); myButtonMenu2.buttondefs[i].color = YELLOW; myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
//    i = 7; strcpy(myButtonMenu2.buttondefs[i].label, "Sv C");  myButtonMenu2.buttondefs[i].color = YELLOW; myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
//    i = 8; strcpy(myButtonMenu2.buttondefs[i].label, "X");     myButtonMenu2.buttondefs[i].color = WHITE;  myButtonMenu2.buttondefs[i].x = col; myButtonMenu2.buttondefs[i].y = row - (menuH * i); myButtonMenu2.buttondefs[i].h = menuH; myButtonMenu2.buttondefs[i].w = width;  myButtonMenu2.buttondefs[i].fontsize = 1;
//
//    SetMenuBoundingRect(myButtonMenu2);
//
//  }
//  DrawButtons(myButtonMenu2);
////}
//
//void ProcessMenu2(int i) {
//  //Serial.println (i);
//  switch (i) {
//    case 0:
//    case 1:
//    case 2:
//    case 3:
//      Load(i);
//      FlashButton(myButtonMenu2, i);
//      break;
//    case 4:
//    case 5:
//    case 6:
//    case 7:
//      FlashButton(myButtonMenu2, i);
//      Save(i);
//      break;
//    case 8:
//      Menu2Showing = false;
//      Serial.println("Close menu 2");
//      graphProfile();
//      break;
//    default:
//      Menu2Showing = false;
//      break;
//
//  }
//
//}

void SetMenuBoundingRect(struct buttonsetdef &butdefset) {

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
  DrawButton(butdefset, i);
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
    DrawButton(butdefset, i);
  }
}

void DrawBox(int x, int y, int w, int h, uint16_t color) {

  tft.fillRect(x, y, w, h, color);
  tft.drawRect(x, y, w, h, BLACK);


}

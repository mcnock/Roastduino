//-------------------------------------------------  ---------------------------------------------------------------------------------------------------
// MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS
//----------------------------------------------------------------------------------------------------------------------------------------------------


void ClearIntergalSum() {
  IntegralSum = 0;
}

void DrawHorMenu1() {
  //these ar the buttoms on top row starting about 1/2 across screen
  int height = 50;
  int width = 90;
  int row = 0;
  int col = 270;
  myHorizontalButtonControl.Count = 5;
  myGLCD.setBackColor(BLACK);
 
  if (myHorizontalButtonControl.buttondefs == 0) {
    //Serial.println ("Allocating myHorizontalButtonControl");
    myHorizontalButtonControl.buttondefs = (buttondef*) malloc(myHorizontalButtonControl.Count * sizeof(buttondef));

    int i = 0;
    i = 0; myHorizontalButtonControl.buttondefs[i].x = col + (width * i); myHorizontalButtonControl.buttondefs[i].y = row; myHorizontalButtonControl.buttondefs[i].h = height;
    myHorizontalButtonControl.buttondefs[i].w = width; myHorizontalButtonControl.buttondefs[i].color = GREEN; strcpy(myHorizontalButtonControl.buttondefs[i].label, "Start"); 
    myHorizontalButtonControl.buttondefs[i].fontsize = 2;
    i = 1; myHorizontalButtonControl.buttondefs[i].x = col + (width * i); myHorizontalButtonControl.buttondefs[i].y = row; myHorizontalButtonControl.buttondefs[i].h = height;
    myHorizontalButtonControl.buttondefs[i].w = width; myHorizontalButtonControl.buttondefs[i].color = RED; strcpy(myHorizontalButtonControl.buttondefs[i].label, "Stop");
    myHorizontalButtonControl.buttondefs[i].fontsize = 2;
    i = 2; myHorizontalButtonControl.buttondefs[i].x = col + (width * i); myHorizontalButtonControl.buttondefs[i].y = row; myHorizontalButtonControl.buttondefs[i].h = height;
    myHorizontalButtonControl.buttondefs[i].w = width; myHorizontalButtonControl.buttondefs[i].color = BLUE; strcpy(myHorizontalButtonControl.buttondefs[i].label, "Fan ");
    myHorizontalButtonControl.buttondefs[i].fontsize = 2;
    i = 3; myHorizontalButtonControl.buttondefs[i].x = col + (width * i); myHorizontalButtonControl.buttondefs[i].y = row; myHorizontalButtonControl.buttondefs[i].h = height;
    myHorizontalButtonControl.buttondefs[i].w = width; myHorizontalButtonControl.buttondefs[i].color = WHITE; strcpy(myHorizontalButtonControl.buttondefs[i].label, "Refr");
    myHorizontalButtonControl.buttondefs[i].fontsize = 2;
    i = 4; myHorizontalButtonControl.buttondefs[i].x = col + (width * i); myHorizontalButtonControl.buttondefs[i].y = row; myHorizontalButtonControl.buttondefs[i].h = height;
    myHorizontalButtonControl.buttondefs[i].w = width; myHorizontalButtonControl.buttondefs[i].color = WHITE; strcpy(myHorizontalButtonControl.buttondefs[i].label, "Tch"); 
    myHorizontalButtonControl.buttondefs[i].fontsize = 2;

    SetMenuBoundingRect(myHorizontalButtonControl);

  }
  //Serial.println ("Drawing myHorizontalButtonControl");
  DrawButtons(myHorizontalButtonControl);
}

void ProcessHorizontal(int i) {
  //Serial.println (i);
  switch (i) {
    case 0:
      StartAction();

      break;
    case 1:
      EndAction();
  
      break;
    case 2:
      FanAction();
     
      break;
    case 3:
      Serial.println("Redraw graph detected");
      for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++)
      {
          MySetPoints[xSetPoint].TemperatureNew = 0;
      }


      RefreshAction();
     
      break;
    case 4:
   
      break;

  }


}

void initializeButtonDefs()
{
    for (int i = 0; i < 4; i++)
    {
        buttonsetdef* bsd = &myButtonVertMenus[i];
        bsd->Count = 9;
        bsd->rowstart = 0;
        bsd->W = 65;
        bsd->H = 50;
        bsd->vertical = true;
        bsd->colstart = myGLCD.getDisplayXSize() - bsd->W - 2;
        switch (i) {
        case 0:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                strcpy(bsd->buttondefs[0].label, ">>"); bsd->buttondefs[0].color = GREEN;
                strcpy(bsd->buttondefs[1].label, "");    bsd->buttondefs[1].color = BLACK;
                strcpy(bsd->buttondefs[2].label, "");    bsd->buttondefs[2].color = BLACK;
                strcpy(bsd->buttondefs[3].label, "");    bsd->buttondefs[3].color = BLACK;
                strcpy(bsd->buttondefs[4].label, "");    bsd->buttondefs[4].color = BLACK;
                strcpy(bsd->buttondefs[5].label, "");    bsd->buttondefs[5].color = BLACK;
                strcpy(bsd->buttondefs[6].label, "");    bsd->buttondefs[6].color = BLACK;
                strcpy(bsd->buttondefs[7].label, "");   bsd->buttondefs[7].color = BLACK;
                strcpy(bsd->buttondefs[8].label, "");     bsd->buttondefs[8].color = BLACK;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
        case 1:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                strcpy(bsd->buttondefs[0].label, ">>");    bsd->buttondefs[0].color = GREEN;
                strcpy(bsd->buttondefs[1].label, "sp1");    bsd->buttondefs[1].color = GREEN;
                strcpy(bsd->buttondefs[2].label, "sp2");    bsd->buttondefs[2].color = GREEN;
                strcpy(bsd->buttondefs[3].label, "sp3");    bsd->buttondefs[3].color = GREEN;
                strcpy(bsd->buttondefs[4].label, "sp4");    bsd->buttondefs[4].color = GREEN;
                strcpy(bsd->buttondefs[5].label, "sp5");    bsd->buttondefs[5].color = GREEN;
                strcpy(bsd->buttondefs[6].label, "ls4");    bsd->buttondefs[6].color = GREEN;
                strcpy(bsd->buttondefs[7].label, "RST");     bsd->buttondefs[7].color = RED;
                strcpy(bsd->buttondefs[8].label, "");        bsd->buttondefs[8].color = BLACK;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
        case 2:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                strcpy(bsd->buttondefs[0].label, "BCK");   bsd->buttondefs[0].color = GREEN;
                strcpy(bsd->buttondefs[1].label,  "");     bsd->buttondefs[1].color = RED;
                strcpy(bsd->buttondefs[2].label, "+1");    bsd->buttondefs[2].color = GREEN;
                strcpy(bsd->buttondefs[3].label, "+3");    bsd->buttondefs[3].color = GREEN;
                strcpy(bsd->buttondefs[4].label, "+5");    bsd->buttondefs[4].color = GREEN;
                strcpy(bsd->buttondefs[5].label, "-1");    bsd->buttondefs[5].color = GREEN;
                strcpy(bsd->buttondefs[6].label, "-3");    bsd->buttondefs[6].color = GREEN;
                strcpy(bsd->buttondefs[7].label, "-5");    bsd->buttondefs[7].color = GREEN;
                strcpy(bsd->buttondefs[8].label, "SAVE");    bsd->buttondefs[8].color = RED;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
        case 3:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));

                strcpy(bsd->buttondefs[0].label, ">>");   bsd->buttondefs[0].color = GREEN;
                strcpy(bsd->buttondefs[1].label, "1-ON");     bsd->buttondefs[1].color = ORANGE;
                strcpy(bsd->buttondefs[2].label, "1-OFF");    bsd->buttondefs[2].color = ORANGE;
                strcpy(bsd->buttondefs[3].label, "2-ON");    bsd->buttondefs[3].color = ORANGE;
                strcpy(bsd->buttondefs[4].label, "2-OFF");    bsd->buttondefs[4].color = ORANGE;
                strcpy(bsd->buttondefs[5].label, "V-ON");    bsd->buttondefs[5].color = ORANGE;
                strcpy(bsd->buttondefs[6].label, "V-OFF");    bsd->buttondefs[6].color = ORANGE;
                strcpy(bsd->buttondefs[7].label, "");    bsd->buttondefs[7].color = BLACK;
                strcpy(bsd->buttondefs[8].label, "");    bsd->buttondefs[8].color = BLACK;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }

        }
    }
}

    void DrawVertMenu(int i) {
    VerticalMenuShowing = i;
    DrawButtons(myButtonVertMenus[i]);
}

void ProcessVertMenu0(int i) {
    //Serial.println ("ProcessVertMenu1:");Serial.println (i);
    switch (i) {
    case 0:
        DrawVertMenu(1);
        break;
    default:
        break;
    }

}
void ProcessVertMenu1(int i) {
    //Serial.println ("ProcessVertMenu1:");Serial.println (i);
    switch (i) {
    case 0:        spSelected = -1;
        DrawVertMenu(3);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        spSelected = i;   
        strcpy(myButtonVertMenus[2].buttondefs[1].label, myButtonVertMenus[1].buttondefs[i].label);
        DrawVertMenu(2);
        break;
    case 7:
       for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++)
        {
            
           Serial.println("savingdefaults");

                MySetPoints[xSetPoint].TemperatureNew = 0;
               EEPROM.put(MySetpointsEprom[xSetPoint], MySetPoints[xSetPoint].TemperatureDefault);

        }
       spSelected = 0;
       graphProfile();
        break;
    default:
        break;
    }

}

void ProcessVertMenu2(int i) {
    Serial.println ("ProcessVertMenu2:");Serial.println (i);
    moveamount = 0;
    
    switch (i) {
    case 0:
        for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++)
        {
            MySetPoints[xSetPoint].TemperatureNew = 0;
        }
        spSelected = 0;
        DrawVertMenu(1);
        break;
    case 2:
        
        moveamount=1;
        break;
    case 3:
        moveamount=3;
        break;
    case 4:
        moveamount=5;
        break;
    case 5:
        moveamount=-1;
        break;
    case 6:
        moveamount=-3;
        break;
    case 7:
        moveamount = -5;
        break;

    case 8:
        saveChangedSetpoints();
        DrawVertMenu(1);
        break;
    
    default:
        
        break;
    }

    if (moveamount != 0 ){
        if (spSelected >= 0 & spSelected <=5) {
           MoveAPoint(spSelected);
          }
      if (spSelected == 6 ) {
         MoveLast4Point();
      }
    }

}

void ProcessVertMenu3(int i) {
    Serial.println("ProcessVertMenue:");Serial.println(i);
    moveamount = 0;
    switch (i) {
    case 0:
        DrawVertMenu(0);
        break;
    case 1:
        digitalWrite(SSR1p, HIGH);
        break;
    case 2:
        digitalWrite(SSR1p, LOW);
        break;
    case 3:
        digitalWrite(SSR2p, HIGH);
        break;
    case 4:
        digitalWrite(SSR2p, LOW);
        break;
    case 5:
        digitalWrite(FANRELAYp, RELAYON);
        break;
    case 6:
        digitalWrite(FANRELAYp, RELAYON); 
        break;
    case 7:
        moveamount = -5;
        break;

    case 8:
        saveChangedSetpoints();
        DrawVertMenu(1);
        break;

    default:

        break;
    }

    if (moveamount != 0) {
        if (spSelected >= 0 & spSelected <= 5) {
            MoveAPoint(spSelected);
        }
        if (spSelected == 6) {
            MoveLast4Point();
        }
    }

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

    if (State == STATESTOPPED) {
        newState = STATEFANONLY;
    }
}
void RefreshAction() {
    graphProfile();
}
void StartAction() {
    if (State == STATESTOPPED || State == STATEFANONLY) {
        newState = STATEROASTING;
        //Serial.println("Start Detected!");
    }
    else if (! (State == STATENOFANCURRENT || State == STATEOVERHEATED || State == STATECOOLING)) {
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
void saveChangedSetpoints(){

    for (int xSetPoint = 1; xSetPoint < 6; xSetPoint++)
        {
         if (MySetPoints[xSetPoint].TemperatureNew != 0)
         {
             MySetPoints[xSetPoint].Temperature = MySetPoints[xSetPoint].TemperatureNew;
             MySetPoints[xSetPoint].TemperatureNew = 0;
             
             EEPROM.write(MySetpointsEprom[xSetPoint], MySetPoints[xSetPoint].Temperature);
             

         }
      }
      
     
        
     
}
void MoveLast4Point() {
    spSelected = 2;
    MoveAPoint(2);
    spSelected = 3;
    MoveAPoint(3);
    spSelected = 4;
    MoveAPoint(4);
    spSelected = 5;
    MoveAPoint(5);
    spSelected = 6;
}
void MoveAPoint(int SetPoint) {
  if (SetPoint > 0) {
      if (MySetPoints[SetPoint].TemperatureNew == 0) {
         MySetPoints[SetPoint].TemperatureNew = MySetPoints[SetPoint].Temperature;
     }
    int lastnew = MySetPoints[SetPoint].TemperatureNew;
    MySetPoints[SetPoint].TemperatureNew = MySetPoints[SetPoint].TemperatureNew + moveamount;

    delay(100);
    DrawMovedSetPoint(SetPoint);
    
  }
}
void MoveGain(int amount) {
  Gain = Gain + amount;
  EEPROM.update(GAIN_EP, Gain);
  delay(100);
}
void MoveIntegral(int amount) {
  Integral = Integral + (float(amount) / 20);
  delay(100);
  EEPROM.update(INTEGRAL_EP , (int)(Integral * 100));
}
void MoveIntegralSum(int amount) {
  IntegralSum = IntegralSum + amount;
}

void SetMenuBoundingRect(struct buttonsetdef &butdefset) {
  for (int i = 0 ; i < butdefset.Count; i++) {
      if (butdefset.vertical == true) {
          butdefset.buttondefs[i].h = butdefset.H;
          butdefset.buttondefs[i].w = butdefset.W;

          butdefset.buttondefs[i].y = butdefset.rowstart + butdefset.H * i;
          butdefset.buttondefs[i].x = butdefset.colstart ;
      }
      else {
         // butdefset.buttondefs[i].x = butdefset.rowstart ;
        //  butdefset.buttondefs[i].y = butdefset.colstart + butdefset.W * i;

      }
 
    butdefset.buttondefs[i].Rect.ymin = butdefset.buttondefs[i].y;
    butdefset.buttondefs[i].Rect.xmax = butdefset.buttondefs[i].x + butdefset.buttondefs[i].w;
    butdefset.buttondefs[i].Rect.ymax = butdefset.buttondefs[i].y + butdefset.buttondefs[i].h;



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
  //Serial.println ("MenuShowingXmin:");Serial.println (bounding.xmin);
  //Serial.println ("MenuShowingYmin:");Serial.println (bounding.ymin);
  //Serial.println ("MenuShowingXmax:");Serial.println (bounding.xmax);
  //Serial.println ("MenuShowingYmax:");Serial.println (bounding.ymax);



}


void DrawButton(buttonsetdef& butdefset, int i)
{
    //draw button
    //Serial.println ("Drawing buttons x:");Serial.println (butdefset.buttondefs[i].x), Serial.println (" y:");Serial.println ( butdefset.buttondefs[i].y);Serial.println (" w:");Serial.println ( butdefset.buttondefs[i].w) ;
    //Serial.println (" h:");Serial.println ( butdefset.buttondefs[i].h);

    myGLCD.setColor(butdefset.buttondefs[i].color);
    myGLCD.fillRect(butdefset.buttondefs[i].x, butdefset.buttondefs[i].y, butdefset.buttondefs[i].w + butdefset.buttondefs[i].x, butdefset.buttondefs[i].h + butdefset.buttondefs[i].y);
    myGLCD.setColor(BLACK);
    myGLCD.drawRect(butdefset.buttondefs[i].x, butdefset.buttondefs[i].y, butdefset.buttondefs[i].w + butdefset.buttondefs[i].x, butdefset.buttondefs[i].h + butdefset.buttondefs[i].y);


    //draw text
    //Serial.println ("sizer of label:");Serial.println (strlen( butdefs[i].label));Serial.println( "Label:");Serial.println(butdefs[i].label);
    //Serial.println(butdefs[i].fontsize);
 

    myGLCD.setFont(BigFont);
  
   

     
  int xOffset = (butdefset.buttondefs[i].w - (strlen(butdefset.buttondefs[i].label) * myGLCD.getFontXsize()))/2;
  int yOffset = (butdefset.buttondefs[i].h - myGLCD.getFontXsize()) / 2;
  myGLCD.setColor(BLACK);

  myGLCD.setBackColor(butdefset.buttondefs[i].color);
  myGLCD.print(butdefset.buttondefs[i].label,butdefset.buttondefs[i].x + xOffset   , butdefset.buttondefs[i].y + yOffset );
  myGLCD.setBackColor(BLACK);

}

void DrawButtons(buttonsetdef &butdefset) {

  //Serial.println ("Drawing buttons start. Count:");Serial.println(count);

    for (int i = 0; i < butdefset.Count; i++) {
        
      DrawButton(butdefset, i);
  }
}

void DrawBox(int x, int y, int w, int h, uint16_t color) {

//  myGLCD.fillRect(x, y, w, h, color);
//  myGLCD.drawRect(x, y, w, h, BLACK);


}

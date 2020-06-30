#define Vmenubase  0
#define VmenuSetPointSelect  1
#define VmenuSetPointValue   2
#define VmenuDebug       3  
#define VmenuZeroAmps        4
#define VmenuOnOff           5


//-------------------------------------------------  ---------------------------------------------------------------------------------------------------
// MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS
//----------------------------------------------------------------------------------------------------------------------------------------------------


void ClearIntergalSum() {
  IntegralSum = 0;
}

void DrawControlMenu() {
  //these ar the buttoms on top row starting about 1/2 across screen
  int height = 50;
  int width = 90;
  int row = 0;
  int col = 270;
  myControlMenuDef.vertical = false;
  myControlMenuDef.Count = 5;
 
  if (myControlMenuDef.buttondefs == 0) {
    //Serial.println ("Allocating myControlMenuDef");
    myControlMenuDef.buttondefs = (buttondef*) malloc(myControlMenuDef.Count * sizeof(buttondef));

    int i = 0;
    i = 0; myControlMenuDef.buttondefs[i].x = col + (width * i); myControlMenuDef.buttondefs[i].y = row; myControlMenuDef.buttondefs[i].h = height;
    myControlMenuDef.buttondefs[i].w = width; myControlMenuDef.buttondefs[i].color = GREEN; strcpy(myControlMenuDef.buttondefs[i].label, "Start"); 
    myControlMenuDef.buttondefs[i].fontsize = 2;
    i = 1; myControlMenuDef.buttondefs[i].x = col + (width * i); myControlMenuDef.buttondefs[i].y = row; myControlMenuDef.buttondefs[i].h = height;
    myControlMenuDef.buttondefs[i].w = width; myControlMenuDef.buttondefs[i].color = RED; strcpy(myControlMenuDef.buttondefs[i].label, "Stop");
    myControlMenuDef.buttondefs[i].fontsize = 2;
    i = 2; myControlMenuDef.buttondefs[i].x = col + (width * i); myControlMenuDef.buttondefs[i].y = row; myControlMenuDef.buttondefs[i].h = height;
    myControlMenuDef.buttondefs[i].w = width; myControlMenuDef.buttondefs[i].color = BLUE; strcpy(myControlMenuDef.buttondefs[i].label, "Fan ");
    myControlMenuDef.buttondefs[i].fontsize = 2;
    i = 3; myControlMenuDef.buttondefs[i].x = col + (width * i); myControlMenuDef.buttondefs[i].y = row; myControlMenuDef.buttondefs[i].h = height;
    myControlMenuDef.buttondefs[i].w = width; myControlMenuDef.buttondefs[i].color = WHITE; strcpy(myControlMenuDef.buttondefs[i].label, "Refr");
    myControlMenuDef.buttondefs[i].fontsize = 2;
    i = 4; myControlMenuDef.buttondefs[i].x = col + (width * i); myControlMenuDef.buttondefs[i].y = row; myControlMenuDef.buttondefs[i].h = height;
    myControlMenuDef.buttondefs[i].w = width; myControlMenuDef.buttondefs[i].color = BLACK; strcpy(myControlMenuDef.buttondefs[i].label, ""); 
    myControlMenuDef.buttondefs[i].fontsize = 2;
    //Serial.println("ControlMenu");
    myControlMenuDef.ClickHandler = ProcessControlMenu;
    SetMenuBoundingRect(myControlMenuDef);
  }
  //Serial.println ("Drawing myControlMenuDef");
  DrawButtons(myControlMenuDef);
}

void ProcessControlMenu(int i) {
  //Serial.print("ProcessConrol");Serial.println (i);
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
      //Serial.println("Redraw graph detected");
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

void DrawFanMenu() {
  //these ar the buttoms on top row starting about 1/2 across screen
  int height = 50;
  int width = 50;
  int row = 400;
  int col = 30;
  myFanButtonControl.Count = 5;
  myGLCD.setBackColor(BLACK);
 
  if (myFanButtonControl.buttondefs == 0) {
    //Serial.println ("Allocating myControlMenuDef");
    myFanButtonControl.buttondefs = (buttondef*) malloc(myFanButtonControl.Count * sizeof(buttondef));
    int i = 0;
    i = 0; myFanButtonControl.buttondefs[i].x = col + (width * i); myFanButtonControl.buttondefs[i].y = row; myFanButtonControl.buttondefs[i].h = height;
    myFanButtonControl.buttondefs[i].w = width; myFanButtonControl.buttondefs[i].color = AQUA; strcpy(myFanButtonControl.buttondefs[i].label, "<<"); 
    myFanButtonControl.buttondefs[i].fontsize = 2;

    i = 1; myFanButtonControl.buttondefs[i].x = col + (width * i); myFanButtonControl.buttondefs[i].y = row; myFanButtonControl.buttondefs[i].h = height;
    myFanButtonControl.buttondefs[i].w = width; myFanButtonControl.buttondefs[i].color = AQUA; strcpy(myFanButtonControl.buttondefs[i].label, "<"); 
    myFanButtonControl.buttondefs[i].fontsize = 2;

    i = 2; myFanButtonControl.buttondefs[i].x = col + (width * i); myFanButtonControl.buttondefs[i].y = row; myFanButtonControl.buttondefs[i].h = height;
    myFanButtonControl.buttondefs[i].w = width; myFanButtonControl.buttondefs[i].color = AQUA; strcpy(myFanButtonControl.buttondefs[i].label, ">"); 
    myFanButtonControl.buttondefs[i].fontsize = 2;

    i = 3; myFanButtonControl.buttondefs[i].x = col + (width * i); myFanButtonControl.buttondefs[i].y = row; myFanButtonControl.buttondefs[i].h = height;
    myFanButtonControl.buttondefs[i].w = width; myFanButtonControl.buttondefs[i].color = AQUA; strcpy(myFanButtonControl.buttondefs[i].label, ">>"); 
    myFanButtonControl.buttondefs[i].fontsize = 2;
    SetMenuBoundingRect(myFanButtonControl);
    
    i = 4; myFanButtonControl.buttondefs[i].x = col + (width * i); myFanButtonControl.buttondefs[i].y = row; myFanButtonControl.buttondefs[i].h = height;
    myFanButtonControl.buttondefs[i].w = width; myFanButtonControl.buttondefs[i].color = AQUA; strcpy(myFanButtonControl.buttondefs[i].label, "S"); 
    myFanButtonControl.buttondefs[i].fontsize = 2;
    SetMenuBoundingRect(myFanButtonControl);

    myControlMenuDef.ClickHandler = ProcessFanMenu;
  }
  //Serial.println ("Drawing myControlMenuDef");

  DrawButtons(myFanButtonControl);
}

void ProcessFanMenu(int i) {
 //Serial.print("ProcessFanMenu:"); Serial.println (i);
  int change = 0;
  switch (i) {
    case 0:
      //decrease quickly
      change = -5;
      break;
    case 1:
      change = -1;
       break;
    case 2:
      change = 1;
      break;
    case 3:
      change = 5;       
      break;
    case 4:
      FanSpeedPWMStart = FanSpeedPWM;
      EEPROM.write(FANSPEED_EP,FanSpeedPWMStart);
      break;
  }

  if (change != 0){
      
      if (FanSpeedPWMAutoMode == true){
           if (RoastMinutes < FanSpeedPWNDelayDecreaseByMinutes){
                FanSpeedPWMStart = FanSpeedPWMStart + change;
           }
           else
           {
               FanSpeedPWMAutoDecrease = FanSpeedPWMAutoDecrease  + change;
           }
           //Serial.println("AMinutes");
           //Serial.println(RoastMinutes);
           //SetFanPWMForATime(RoastMinutes);
           DrawFanDecrease(AQUA);
      }
      else
      {
        FanSpeedPWM = FanSpeedPWM + change;  
        if (FanSpeedPWM > 255){FanSpeedPWM = 255;}
        if (FanSpeedPWM < 0){FanSpeedPWM = 0;}
        analogWrite(FanPWMp, FanSpeedPWM);
          UpdateFanPWMBut();
          updateFanOutputResistance();
          delay(10);
      }
  }
  
}

void intializeVMenus(){
    for (int i = 0; i < 6; i++)
    {
       // Serial.print("VMenu");Serial.println(i);
        int j= 0;
        buttonsetdef* bsd = &myButtonVertMenus[i];
        bsd->Count = 9;
        bsd->rowstart = 0;
        bsd->W = 65;
        bsd->H = 50;
        bsd->vertical = true;
        bsd->colstart = myGLCD.getDisplayXSize() - bsd->W - 2;
        switch (i) {
        case Vmenubase:
            if (bsd->buttondefs == 0) {
              
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                strcpy(bsd->buttondefs[0].label, ">>"); bsd->buttondefs[0].color = GREEN;
                strcpy(bsd->buttondefs[1].label, "<<");    bsd->buttondefs[1].color = GREEN;
                strcpy(bsd->buttondefs[2].label, "");    bsd->buttondefs[2].color = BLACK;
                strcpy(bsd->buttondefs[3].label, "");    bsd->buttondefs[3].color = BLACK;
                strcpy(bsd->buttondefs[4].label, "");    bsd->buttondefs[4].color = BLACK;
                strcpy(bsd->buttondefs[5].label, "");    bsd->buttondefs[5].color = BLACK;
                strcpy(bsd->buttondefs[6].label, "");    bsd->buttondefs[6].color = BLACK;
                strcpy(bsd->buttondefs[7].label, "");   bsd->buttondefs[7].color = BLACK;
                strcpy(bsd->buttondefs[8].label, "");     bsd->buttondefs[8].color = BLACK;
                bsd->ClickHandler = ProcessBaseVMenu;
                bsd->nextMenu = VmenuSetPointSelect;
                bsd->backMenu = VmenuDebug;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
        case VmenuSetPointSelect:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                strcpy(bsd->buttondefs[0].label, ">>");    bsd->buttondefs[0].color = GREEN;
                strcpy(bsd->buttondefs[1].label, "<<");    bsd->buttondefs[1].color = GREEN;
                strcpy(bsd->buttondefs[2].label, "sp1");    bsd->buttondefs[2].color = YELLOW;
                strcpy(bsd->buttondefs[3].label, "sp2");    bsd->buttondefs[3].color = YELLOW;
                strcpy(bsd->buttondefs[4].label, "sp3");    bsd->buttondefs[4].color = YELLOW;
                strcpy(bsd->buttondefs[5].label, "sp4");    bsd->buttondefs[5].color = YELLOW;
                strcpy(bsd->buttondefs[6].label, "sp5");    bsd->buttondefs[6].color = YELLOW;
                strcpy(bsd->buttondefs[7].label, "ls4");    bsd->buttondefs[7].color = YELLOW;
                strcpy(bsd->buttondefs[8].label, "FanD");    bsd->buttondefs[8].color = YELLOW;
                bsd->ClickHandler = ProcessSetPointSelectVMenu; 
                bsd->nextMenu = VmenuZeroAmps;
                bsd->backMenu = Vmenubase;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
        case VmenuSetPointValue:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                strcpy(bsd->buttondefs[0].label, "<<");   bsd->buttondefs[0].color = GREEN;
                strcpy(bsd->buttondefs[1].label,  "");     bsd->buttondefs[1].color = RED;
                strcpy(bsd->buttondefs[2].label, "+1");    bsd->buttondefs[2].color = ORANGE;
                strcpy(bsd->buttondefs[3].label, "+3");    bsd->buttondefs[3].color = ORANGE;
                strcpy(bsd->buttondefs[4].label, "+5");    bsd->buttondefs[4].color = ORANGE;
                strcpy(bsd->buttondefs[5].label, "-1");    bsd->buttondefs[5].color = ORANGE;
                strcpy(bsd->buttondefs[6].label, "-3");    bsd->buttondefs[6].color = ORANGE;
                strcpy(bsd->buttondefs[7].label, "-5");    bsd->buttondefs[7].color = ORANGE;
                strcpy(bsd->buttondefs[8].label, "SAVE");    bsd->buttondefs[8].color = RED;
                bsd->ClickHandler = ProcessSetPointAdjustmentVMenu; 
                bsd->nextMenu = -1;
                bsd->backMenu = VmenuSetPointSelect;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
      case VmenuZeroAmps:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                strcpy(bsd->buttondefs[0].label, ">>");   bsd->buttondefs[0].color = GREEN;
                strcpy(bsd->buttondefs[1].label, "<<");     bsd->buttondefs[1].color = GREEN;
                strcpy(bsd->buttondefs[2].label, "0 Fn");    bsd->buttondefs[2].color = ORANGE;
                strcpy(bsd->buttondefs[3].label, "0 C1");    bsd->buttondefs[3].color = ORANGE;
                strcpy(bsd->buttondefs[4].label, "0 C2");    bsd->buttondefs[4].color = ORANGE;
                strcpy(bsd->buttondefs[5].label, "0ALL");    bsd->buttondefs[5].color = ORANGE;
                strcpy(bsd->buttondefs[6].label, "CLR0");    bsd->buttondefs[6].color = ORANGE;
                strcpy(bsd->buttondefs[7].label, "RSSP");    bsd->buttondefs[7].color = ORANGE;
                strcpy(bsd->buttondefs[8].label, "");    bsd->buttondefs[8].color = BLACK;
                bsd->ClickHandler = ProcessZeroAmpMetersVMenu; 
                bsd->nextMenu = VmenuDebug;
                bsd->backMenu = VmenuSetPointSelect;

                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
      case VmenuDebug:
            if (bsd->buttondefs == 0) {
               j = 0;
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                strcpy(bsd->buttondefs[j].label, ">>");   bsd->buttondefs[j].color = GREEN;
                j++;strcpy(bsd->buttondefs[j].label, "<<");     bsd->buttondefs[j].color = GREEN;
                j++;strcpy(bsd->buttondefs[j].label, "TOG");     bsd->buttondefs[j].color = ORANGE;
                j++;strcpy(bsd->buttondefs[j].label, "C1");     bsd->buttondefs[j].color = ORANGE;
                j++;strcpy(bsd->buttondefs[j].label, "C2");    bsd->buttondefs[j].color = ORANGE;
                j++;strcpy(bsd->buttondefs[j].label, "Vib");    bsd->buttondefs[j].color = ORANGE;
                j++;strcpy(bsd->buttondefs[j].label, "Fan");    bsd->buttondefs[j].color = ORANGE;
                j++;strcpy(bsd->buttondefs[j].label, "");    bsd->buttondefs[j].color = BLACK;
                j++;strcpy(bsd->buttondefs[j].label, "");    bsd->buttondefs[j].color = BLACK;
                bsd->ClickHandler = ProcessDebugVMenu;
                bsd->nextMenu = Vmenubase;
                bsd->backMenu = VmenuZeroAmps;

                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
      case VmenuOnOff:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                j=0;strcpy(bsd->buttondefs[j].label, "<<");   bsd->buttondefs[j].color = GREEN;
                j=1;strcpy(bsd->buttondefs[j].label,  "");     bsd->buttondefs[j].color = RED;
                j=2;strcpy(bsd->buttondefs[j].label, "ON");    bsd->buttondefs[j].color = GREEN;
                j=3;strcpy(bsd->buttondefs[j].label, "OFF");    bsd->buttondefs[j].color = GREEN;
                j=4;strcpy(bsd->buttondefs[j].label, "");    bsd->buttondefs[j].color = BLACK;
                j=5;strcpy(bsd->buttondefs[j].label, "");    bsd->buttondefs[j].color = BLACK;
                j=6;strcpy(bsd->buttondefs[j].label, "");    bsd->buttondefs[j].color = BLACK;
                j=7;strcpy(bsd->buttondefs[j].label, "");    bsd->buttondefs[j].color = BLACK;
                j=8;strcpy(bsd->buttondefs[j].label, "");    bsd->buttondefs[j].color = BLACK;
                
                
                bsd->ClickHandler = ProcessOnOffVMenu; 
                bsd->nextMenu = -1;
                bsd->backMenu = VmenuDebug;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
 
        }
    }
    
}


 void DrawVMenu(int i) {
    VerticalMenuShowing = i;
    DrawButtons(myButtonVertMenus[i]);
}

void ProcessBaseVMenu(int i) {
    //Serial.println ("ProcessVertMenu1:");Serial.println (i);
    switch (i) {
    case 0:
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu);
        break;
    case 1:
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu);
        break;
    default:
        break;
    }

}

void ProcessSetPointSelectVMenu(int i) {
                //  strcpy(bsd->buttondefs[0].label, ">>");    bsd->buttondefs[0].color = GREEN;
                //strcpy(bsd->buttondefs[1].label, "<<");    bsd->buttondefs[1].color = GREEN;
                //strcpy(bsd->buttondefs[2].label, "sp1");    bsd->buttondefs[2].color = YELLOW;
                //strcpy(bsd->buttondefs[3].label, "sp2");    bsd->buttondefs[3].color = YELLOW;
                //strcpy(bsd->buttondefs[4].label, "sp3");    bsd->buttondefs[4].color = YELLOW;
                //strcpy(bsd->buttondefs[5].label, "sp4");    bsd->buttondefs[5].color = YELLOW;
                //strcpy(bsd->buttondefs[6].label, "sp5");    bsd->buttondefs[6].color = YELLOW;
                //strcpy(bsd->buttondefs[7].label, "ls4");    bsd->buttondefs[7].color = YELLOW;
                //strcpy(bsd->buttondefs[8].label, "FanD");    bsd->buttondefs[8].color = YELLOW;
    //Serial.println ("ProcessVertMenu1:");Serial.println (i);
    switch (i) {
    case 0:        
        spSelected = -1;
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu);
        break;
    case 1:
        spSelected = -1;
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu);
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        spSelected = i-1;   
        strcpy(myButtonVertMenus[VmenuSetPointValue].buttondefs[1].label, myButtonVertMenus[1].buttondefs[i].label);
        myButtonVertMenus[VmenuSetPointValue].inputfrompriormenu = i;
        DrawVMenu(VmenuSetPointValue);
        break;
    case 8:
        spSelected = 9;
        strcpy(myButtonVertMenus[2].buttondefs[1].label, myButtonVertMenus[1].buttondefs[i].label);
        DrawVMenu(2);
        break;
    default:
        break;
    }

}

void ProcessSetPointAdjustmentVMenu(int i) {
    //Serial.print ("ProcessSetPointAdjustmentVMenu:");Serial.println (i);
    //Serial.print ("spSelected:");Serial.print (spSelected);
    moveamount = 0;    
    switch (i) {
    case 0:
        for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++)
        {
            MySetPoints[xSetPoint].TemperatureNew = 0;
        }
        spSelected = -1;
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu);
    
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
        DrawVMenu(Vmenubase);
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
       if (spSelected == 9 ) {
          FanSpeedPWMAutoDecrease = FanSpeedPWMAutoDecrease + moveamount;
           updateFanOutputResistance();
            UpdateFanPWMBut();
            spSelected =-1;
       }
    }

}
void ProcessDebugVMenu(int i) {
   // Serial.print("ProcessVertMenu3:");Serial.println(i);
    switch (i) {
        case 0:        
          DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu);
        break;
    case 1:
          DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu);
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        strcpy(myButtonVertMenus[VmenuOnOff].buttondefs[1].label, myButtonVertMenus[VerticalMenuShowing].buttondefs[i].label);
        myButtonVertMenus[VmenuOnOff].inputfrompriormenu = i;
        DrawVMenu(VmenuOnOff);
        break;
    case 7:
        break;
    case 8:
        break;
    }
}
void ProcessOnOffVMenu(int i) {
   Serial.print("ProcessOnOffVMenu:");Serial.println(i);
   int ONOFF = HIGH; 
    switch (i) {
        case 0:        
          DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu);
        break;
    case 1:
        break;
    case 2: //on
        ONOFF = HIGH;
        break;
    case 3: //off
        ONOFF = LOW;
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
        break;
    }

    if (i == 2 || i == 3){
      switch (myButtonVertMenus[VerticalMenuShowing].inputfrompriormenu){
        case 3: //ss 1
            digitalWrite(SSR1p, ONOFF);
            break;
        case 4: //ss 2
            digitalWrite(SSR2p, ONOFF);
            break;
        case 5: //vib
            digitalWrite(VIBRELAYp, !ONOFF);
            break;
        case 6: //fan
            digitalWrite(FANRELAYp, !ONOFF);
            break;
        }
            

    }
}
void ProcessZeroAmpMetersVMenu(int i) {
   Serial.print("ProcessVertMenu4:");Serial.println(i);
    moveamount = 0;
    switch (i) {
    case 0:
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu);
        break;
    case 1:
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu);
        break;
    case 2:
        CurrentFanOffset = CurrentFanOffset + CurrentFan ;
        AvgFanCurrent.clear();
        break;
    case 3:
        CurrentHeat1Offset =   CurrentHeat1Offset + CurrentHeat1;
        AvgCoil1Amp.clear();
        break;
    case 4:
        CurrentHeat2Offset = CurrentHeat2Offset + CurrentHeat2;
        AvgCoil2Amp.clear(); 
        break;
    case 5:
        CurrentFanOffset = CurrentFanOffset + CurrentFan ;
        AvgFanCurrent.clear();
        CurrentHeat1Offset =   CurrentHeat1Offset + CurrentHeat1;
        AvgCoil1Amp.clear();
        CurrentHeat2Offset = CurrentHeat2Offset + CurrentHeat2;
        AvgCoil2Amp.clear(); 
    
        break;
    case 6:
        CurrentFanOffset = 0; 
        CurrentHeat1Offset = 0; 
        CurrentHeat2Offset = 0;
        
        break;
    case 7:
       for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++)
        {
               MySetPoints[xSetPoint].TemperatureNew = 0;
               EEPROM.put(SETPOINTTEMP_EP[xSetPoint], MySetPoints[xSetPoint].TemperatureDefault);
        }
       graphProfile();
        break;

    case 8:
        break;
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
    //Serial.println("Fan action");
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

    for (int xSetPoint = 0; xSetPoint < 6; xSetPoint++)
        {
         if (MySetPoints[xSetPoint].TemperatureNew != 0)
         {
             MySetPoints[xSetPoint].Temperature = MySetPoints[xSetPoint].TemperatureNew;
             MySetPoints[xSetPoint].TemperatureNew = 0;
             
             EEPROM.write(SETPOINTTEMP_EP[xSetPoint], MySetPoints[xSetPoint].Temperature);
             

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
  //Serial.print("MoveAPoint Setpoint:");Serial.print(SetPoint);Serial.print("moveamount:");Serial.println(moveamount);  
  if (SetPoint > -1) {
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
  
    butdefset.buttondefs[i].Rect.xmin = butdefset.buttondefs[i].x;
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
  //Serial.print ("MenuShowingXmin:");Serial.println (butdefset.bounding.xmin);
 // Serial.print ("MenuShowingYmin:");Serial.println (butdefset.bounding.ymin);
  //Serial.print ("MenuShowingXmax:");Serial.println (butdefset.bounding.xmax);
  //Serial.print ("MenuShowingYmax:");Serial.println (butdefset.bounding.ymax);



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

    DrawButtonText(butdefset,i);
 

}

void DrawButtonText(buttonsetdef& butdefset, int i){
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



//-------------------------------------------------  ---------------------------------------------------------------------------------------------------
// MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS
//----------------------------------------------------------------------------------------------------------------------------------------------------


void ClearIntergalSum() {
  IntegralSum = 0;
}

void DrawHorControlMenu() {
  //these ar the buttoms on top row starting about 1/2 across screen
  int height = 40;
  int width = 90;
  int row = 0;
  int col = 360;
  myHorControlMenuDef.vertical = false;
  myHorControlMenuDef.Count = 4;
 
  if (myHorControlMenuDef.buttondefs == 0) {
    //Serial.println ("Allocating myHorControlMenuDef");
    myHorControlMenuDef.buttondefs = (buttondef*) malloc(myHorControlMenuDef.Count * sizeof(buttondef));

    int i = 0;
    i = 0; myHorControlMenuDef.buttondefs[i].x = col + (width * i); myHorControlMenuDef.buttondefs[i].y = row; myHorControlMenuDef.buttondefs[i].h = height;
    myHorControlMenuDef.buttondefs[i].w = width; myHorControlMenuDef.buttondefs[i].color = GREEN; strcpy(myHorControlMenuDef.buttondefs[i].label, "Start"); 
    myHorControlMenuDef.buttondefs[i].fontsize = 2;
    i = 1; myHorControlMenuDef.buttondefs[i].x = col + (width * i); myHorControlMenuDef.buttondefs[i].y = row; myHorControlMenuDef.buttondefs[i].h = height;
    myHorControlMenuDef.buttondefs[i].w = width; myHorControlMenuDef.buttondefs[i].color = RED; strcpy(myHorControlMenuDef.buttondefs[i].label, "Stop");
    myHorControlMenuDef.buttondefs[i].fontsize = 2;
    i = 2; myHorControlMenuDef.buttondefs[i].x = col + (width * i); myHorControlMenuDef.buttondefs[i].y = row; myHorControlMenuDef.buttondefs[i].h = height;
    myHorControlMenuDef.buttondefs[i].w = width; myHorControlMenuDef.buttondefs[i].color = BLUE; strcpy(myHorControlMenuDef.buttondefs[i].label, "Fan ");
    myHorControlMenuDef.buttondefs[i].fontsize = 2;
    i = 3; myHorControlMenuDef.buttondefs[i].x = col + (width * i); myHorControlMenuDef.buttondefs[i].y = row; myHorControlMenuDef.buttondefs[i].h = height;
    myHorControlMenuDef.buttondefs[i].w = width; myHorControlMenuDef.buttondefs[i].color = WHITE; strcpy(myHorControlMenuDef.buttondefs[i].label, "Refr");
    myHorControlMenuDef.buttondefs[i].fontsize = 2;
    myHorControlMenuDef.ClickHandler = ProcessHorControlMenu;
    SetMenuBoundingRect(myHorControlMenuDef);
  }
  //Serial.println ("Drawing myHorControlMenuDef");
  DrawButtons(myHorControlMenuDef);
}

void ProcessHorControlMenu(int i) {
  //Serial.print("ProcessConrol");Serial.println (i);
  if (errmsg == "Must be in Fan Only to start a roast"){
       errmsg = "";
       newerrmsg = true;
  }
 
    switch (i) {
    case 0:
          
    
       
       if ( State == STATEFANONLY) {
              newState = STATEROASTING;
       }
       else if ((State == STATENOFANCURRENT || State == STATEOVERHEATED || State == STATECOOLING)) {
              newState = STATEROASTING;
              //Serial.println("ReStart Detected!");
       }
       else{
           newerrmsg = true;
           errmsg = "Must be in Fan Only to start a roast";
      
        
       }
       break;
    case 1:
      if (State != STATESTOPPED) {
        newState = STATESTOPPED;
        //Serial.println("Stop Detected!");
    }
  
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

void DrawHorFanMenu() {
  //these ar the buttoms on top row starting about 1/2 across screen
  int height = 50;
  int width = 50;
  int row = 400;
  int col = 30;
  myHorFanButtonControl.Count = 5;
  myGLCD.setBackColor(BLACK);
 
  if (myHorFanButtonControl.buttondefs == 0) {
    //Serial.println ("Allocating myHorControlMenuDef");
    myHorFanButtonControl.buttondefs = (buttondef*) malloc(myHorFanButtonControl.Count * sizeof(buttondef));
    int i = 0;
    i = 0; myHorFanButtonControl.buttondefs[i].x = col + (width * i); myHorFanButtonControl.buttondefs[i].y = row; myHorFanButtonControl.buttondefs[i].h = height;
    myHorFanButtonControl.buttondefs[i].w = width; myHorFanButtonControl.buttondefs[i].color = AQUA; strcpy(myHorFanButtonControl.buttondefs[i].label, "<<"); 
    myHorFanButtonControl.buttondefs[i].fontsize = 2;

    i = 1; myHorFanButtonControl.buttondefs[i].x = col + (width * i); myHorFanButtonControl.buttondefs[i].y = row; myHorFanButtonControl.buttondefs[i].h = height;
    myHorFanButtonControl.buttondefs[i].w = width; myHorFanButtonControl.buttondefs[i].color = AQUA; strcpy(myHorFanButtonControl.buttondefs[i].label, "<"); 
    myHorFanButtonControl.buttondefs[i].fontsize = 2;

    i = 2; myHorFanButtonControl.buttondefs[i].x = col + (width * i); myHorFanButtonControl.buttondefs[i].y = row; myHorFanButtonControl.buttondefs[i].h = height;
    myHorFanButtonControl.buttondefs[i].w = width; myHorFanButtonControl.buttondefs[i].color = AQUA; strcpy(myHorFanButtonControl.buttondefs[i].label, ">"); 
    myHorFanButtonControl.buttondefs[i].fontsize = 2;

    i = 3; myHorFanButtonControl.buttondefs[i].x = col + (width * i); myHorFanButtonControl.buttondefs[i].y = row; myHorFanButtonControl.buttondefs[i].h = height;
    myHorFanButtonControl.buttondefs[i].w = width; myHorFanButtonControl.buttondefs[i].color = AQUA; strcpy(myHorFanButtonControl.buttondefs[i].label, ">>"); 
    myHorFanButtonControl.buttondefs[i].fontsize = 2;
    SetMenuBoundingRect(myHorFanButtonControl);
    
    i = 4; myHorFanButtonControl.buttondefs[i].x = col + (width * i); myHorFanButtonControl.buttondefs[i].y = row; myHorFanButtonControl.buttondefs[i].h = height;
    myHorFanButtonControl.buttondefs[i].w = width; myHorFanButtonControl.buttondefs[i].color = AQUA; strcpy(myHorFanButtonControl.buttondefs[i].label, "S"); 
    myHorFanButtonControl.buttondefs[i].fontsize = 2;
    SetMenuBoundingRect(myHorFanButtonControl);

    myHorFanButtonControl.ClickHandler = ProcessHorFanMenu;
  }
  //Serial.println ("Drawing myHorFanButtonMenuDef");

  DrawButtons(myHorFanButtonControl);
}

void ProcessHorFanMenu(int i) {
 //Serial.print("ProcessFanMenu:");Serial.println (i);
  int change = 0;
  switch (i) {
    case 0:
      //decrease quickly
      change = -10;
      break;
    case 1:
      change = -3;
       break;
    case 2:
      change = 3;
      break;
    case 3:
      change = 10;       
      break;
    case 4:
      FanSpeedPWMStart = FanSpeedPWM;
      EEPROM.write(FanSpeedPWMStart_EP,FanSpeedPWMStart);
      FanSpeedPWMAutoDecreaseStart = FanSpeedPWMAutoDecrease;
      EEPROM.write(FanSpeedPWMAutoDecrease_EP,FanSpeedPWMAutoDecrease);
      DrawFanGraph_ex(true);
      break;
  }
  if (change != 0){    
      if (FanSpeedPWMAutoMode == true){
          
           
           if (RoastMinutes < (FanSpeedPWNDelayDecreaseByMinutes + FanSpeedPWNDecreaseByMinutes/2)){
                 //first half we adjust start and let slope be what is asked for or be trimmed
                 FanSpeedPWMStart = FanSpeedPWMStart + change;
           }
           else
           {
               //later half we adjust the slope and if slope is not sufficent the start
               //in the second half we let the slope grow or shrink relative to initial setting

                   FanSpeedPWMAutoDecrease = FanSpeedPWMAutoDecrease  - change;

                  

                   //if the decrease cannot be accomoated with slope change, apply it to start
                   if (FanSpeedPWMAutoDecrease < 0)
                   {
                        FanSpeedPWMStart = FanSpeedPWMStart + change;
                        FanSpeedPWMAutoDecrease = 0;
                   }
                   
               
           }

         //we trim too large of slope
          if (FanSpeedPWMAutoDecrease > FanSpeedPWMStart)   
          {
              FanSpeedPWMAutoDecrease = FanSpeedPWMStart ; 
         }
             
          if (FanSpeedPWMStart > 254){FanSpeedPWMStart = 254;}
          if (FanSpeedPWMStart < 10){FanSpeedPWMStart = 10;}
           sendFanPWM_Wire();
           DrawFanGraph();
      }
      else
      {
        FanSpeedPWM = FanSpeedPWM + change;  
          
        if (FanSpeedPWM > 254){FanSpeedPWM = 254;}
        if (FanSpeedPWM < 0){FanSpeedPWM = 0;}
     
        sendFanPWM_Wire();
        DrawFanGraph();
        //delay(5);
      }
      
      }
  
  
}

void intializeVMenus(){
    for (int i = 0; i < 9; i++)
    {
       //Serial.print("VMenu");Serial.println(i);
        int j= 0;
        buttonsetdef* bsd = &myButtonVertMenus[i];
        bsd->Count = 10;
        bsd->rowstart = 0;
        bsd->W = 90;
        bsd->H = 50;
        bsd->vertical = true;
        bsd->colstart = myGLCD.getDisplayXSize() - bsd->W - 2;
        switch (i) {
        case Vmenubase:
            if (bsd->buttondefs == 0) {
              
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                strcpy(bsd->buttondefs[0].label, ">>"); bsd->buttondefs[0].color = GREEN;
                strcpy(bsd->buttondefs[1].label, "<<");    bsd->buttondefs[1].color = GREEN;
                strcpy(bsd->buttondefs[2].label, "Gain");    bsd->buttondefs[2].color = YELLOW;
                strcpy(bsd->buttondefs[3].label, "Int");    bsd->buttondefs[3].color = YELLOW;
                strcpy(bsd->buttondefs[4].label, "SPs");    bsd->buttondefs[4].color = YELLOW;
                strcpy(bsd->buttondefs[5].label, "Fan");    bsd->buttondefs[5].color = YELLOW;
                strcpy(bsd->buttondefs[6].label, "rT+1");    bsd->buttondefs[6].color = RED; //roast time this roaste
                strcpy(bsd->buttondefs[7].label, "rT-1");   bsd->buttondefs[7].color = RED;  //roast time this roaste
                strcpy(bsd->buttondefs[8].label, "cCut");     bsd->buttondefs[8].color = GREEN; //temp cout
                bsd->ClickHandler = ProcessBaseVMenu;
                bsd->nextMenu = VmenuDebug;
                bsd->backMenu = VmenuDebug;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
      case VmenuZeroAmps:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                strcpy(bsd->buttondefs[0].label, ">>");   bsd->buttondefs[0].color = GREEN; //1
                strcpy(bsd->buttondefs[1].label, "<<");     bsd->buttondefs[1].color = GREEN; //2
                strcpy(bsd->buttondefs[2].label, "0 Fn");    bsd->buttondefs[2].color = ORANGE;
                strcpy(bsd->buttondefs[3].label, "0 C1");    bsd->buttondefs[3].color = ORANGE;
                strcpy(bsd->buttondefs[4].label, "0 C2");    bsd->buttondefs[4].color = ORANGE;
                strcpy(bsd->buttondefs[5].label, "0ALL");    bsd->buttondefs[5].color = ORANGE;
                strcpy(bsd->buttondefs[6].label, "CLR0");    bsd->buttondefs[6].color = ORANGE;
                strcpy(bsd->buttondefs[7].label, "RSSP");    bsd->buttondefs[7].color = ORANGE;
                strcpy(bsd->buttondefs[8].label, "");    bsd->buttondefs[8].color = BLACK;
                bsd->ClickHandler = ProcessZeroAmpMetersVMenu; 
                bsd->nextMenu = VmenuDebug;
                bsd->backMenu = Vmenubase;

                SetMenuBoundingRect(myButtonVertMenus[i]);
            }

      case VmenuDebug:
            if (bsd->buttondefs == 0) {
               
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                j=0;strcpy(bsd->buttondefs[j].label, ">>");   bsd->buttondefs[j].color = GREEN;
                j=1;strcpy(bsd->buttondefs[j].label, "<<");     bsd->buttondefs[j].color = GREEN;
                j=2;strcpy(bsd->buttondefs[j].label, "DBG");     bsd->buttondefs[j].color = ORANGE;
                j=3;strcpy(bsd->buttondefs[j].label, "C1");     bsd->buttondefs[j].color = ORANGE;
                j=4;strcpy(bsd->buttondefs[j].label, "C2");    bsd->buttondefs[j].color = ORANGE;
                j=5;strcpy(bsd->buttondefs[j].label, "Vib");    bsd->buttondefs[j].color = ORANGE;
                j=6;strcpy(bsd->buttondefs[j].label, "Fan");    bsd->buttondefs[j].color = ORANGE;
                j=7;strcpy(bsd->buttondefs[j].label, "Duty");    bsd->buttondefs[j].color = ORANGE;
                j=8;strcpy(bsd->buttondefs[j].label, "Temp");    bsd->buttondefs[j].color = ORANGE;
                bsd->ClickHandler = ProcessDebugVMenu;
                bsd->nextMenu = Vmenubase;
                bsd->backMenu = Vmenubase;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }

        case VmenuSetPointSelect:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                j=0;strcpy(bsd->buttondefs[j].label, "<<");    bsd->buttondefs[j].color = GREEN;
                j=1;strcpy(bsd->buttondefs[j].label, "sp1");    bsd->buttondefs[j].color = YELLOW;
                j=2;strcpy(bsd->buttondefs[j].label, "sp2");    bsd->buttondefs[j].color = YELLOW;
                j=3;strcpy(bsd->buttondefs[j].label, "sp3");    bsd->buttondefs[j].color = YELLOW;
                j=4;strcpy(bsd->buttondefs[j].label, "sp4");    bsd->buttondefs[j].color = YELLOW;
                j=5;strcpy(bsd->buttondefs[j].label, "sp5");    bsd->buttondefs[j].color = YELLOW;
                j=6;strcpy(bsd->buttondefs[j].label, "ls3");    bsd->buttondefs[j].color = YELLOW;
                j=7;strcpy(bsd->buttondefs[j].label, "T+1");    bsd->buttondefs[j].color = GREEN;  //roaste time saved
                j=8;strcpy(bsd->buttondefs[j].label, "T-1");    bsd->buttondefs[j].color = GREEN; //roast time saved
                bsd->ClickHandler = ProcessSetPointSelectVMenu; 
                bsd->nextMenu = -1;
                bsd->backMenu = Vmenubase;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
      case VmenuFan:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                j=0;strcpy(bsd->buttondefs[j].label, "<<");   bsd->buttondefs[j].color = GREEN;
                j=1;strcpy(bsd->buttondefs[j].label, "A+1");     bsd->buttondefs[j].color = GREEN;
                j=2;strcpy(bsd->buttondefs[j].label, "A-1");    bsd->buttondefs[j].color = GREEN;
                j=3;strcpy(bsd->buttondefs[j].label, "B+1");    bsd->buttondefs[j].color = GREEN;
                j=4;strcpy(bsd->buttondefs[j].label, "B+5");    bsd->buttondefs[j].color = GREEN;
                j=5;strcpy(bsd->buttondefs[j].label, "B-1");    bsd->buttondefs[j].color = GREEN;
                j=6;strcpy(bsd->buttondefs[j].label, "B-5");    bsd->buttondefs[j].color = GREEN;
                j=7;strcpy(bsd->buttondefs[j].label, "C+1");    bsd->buttondefs[j].color = GREEN;
                j=8;strcpy(bsd->buttondefs[j].label, "C-1");    bsd->buttondefs[j].color = GREEN;               
                bsd->ClickHandler = ProcessFanVmenu; 
                bsd->nextMenu = -1;
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
       case VmenuAjd_01:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                j=0;strcpy(bsd->buttondefs[j].label, "<<");   bsd->buttondefs[j].color = GREEN;
                j=1;strcpy(bsd->buttondefs[j].label,  "");     bsd->buttondefs[j].color = RED;
                j=2;strcpy(bsd->buttondefs[j].label, "+.01");    bsd->buttondefs[j].color = GREEN;
                j=3;strcpy(bsd->buttondefs[j].label, "+.05");    bsd->buttondefs[j].color = GREEN;
                j=4;strcpy(bsd->buttondefs[j].label, "+.10");    bsd->buttondefs[j].color = GREEN;
                j=5;strcpy(bsd->buttondefs[j].label, "-.01");    bsd->buttondefs[j].color = GREEN;
                j=6;strcpy(bsd->buttondefs[j].label, "-.05");    bsd->buttondefs[j].color = GREEN;
                j=7;strcpy(bsd->buttondefs[j].label, "-.10");    bsd->buttondefs[j].color = GREEN;
                j=8;strcpy(bsd->buttondefs[j].label, "");    bsd->buttondefs[j].color = BLACK;               
                bsd->ClickHandler = ProcessAdjust_01Vmenu; 
                bsd->nextMenu = -1;
                bsd->backMenu = VmenuDebug;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
      
       case VMenuAdj_1_5_10_V:
            if (bsd->buttondefs == 0) {
                bsd->buttondefs = (buttondef*)malloc(bsd->Count * sizeof(buttondef));
                j=0;strcpy(bsd->buttondefs[j].label, "<<");   bsd->buttondefs[j].color = GREEN;
                j=1;strcpy(bsd->buttondefs[j].label,  "");     bsd->buttondefs[j].color = RED;
                j=2;strcpy(bsd->buttondefs[j].label, "+1");    bsd->buttondefs[j].color = GREEN;
                j=3;strcpy(bsd->buttondefs[j].label, "+5");    bsd->buttondefs[j].color = GREEN;
                j=4;strcpy(bsd->buttondefs[j].label, "+10");    bsd->buttondefs[j].color = GREEN;
                j=5;strcpy(bsd->buttondefs[j].label, "-1");    bsd->buttondefs[j].color = GREEN;
                j=6;strcpy(bsd->buttondefs[j].label, "-5");    bsd->buttondefs[j].color = GREEN;
                j=7;strcpy(bsd->buttondefs[j].label, "-10");    bsd->buttondefs[j].color = GREEN;
                j=8;strcpy(bsd->buttondefs[j].label, "");    bsd->buttondefs[j].color = BLACK;               
                bsd->ClickHandler = ProcessAdj_1_5_10_VMenu; 
                bsd->nextMenu = -1;
                bsd->backMenu = VmenuDebug;
                SetMenuBoundingRect(myButtonVertMenus[i]);
            }
 
       }   
    }
    
}

 void DrawVMenu(int iMenu, int iButton) {
    VerticalMenuPrior = VerticalMenuShowing; 
    VerticalMenuShowing = iMenu;
    myButtonVertMenus[iMenu].inputbutton = iButton;
    DrawButtons(myButtonVertMenus[iMenu]);
}

void ProcessBaseVMenu(int i) {
    //Serial.println ("ProcessVertMenu1:");Serial.println (i);
  
    switch (i) {
    case VBUT0:
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu,-1);
        break;
    case VBUT1:
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu,-1);
        break;
    case VBUT2:
        strcpy(myButtonVertMenus[VMenuAdj_1_5_10_V].buttondefs[1].label, myButtonVertMenus[VerticalMenuShowing].buttondefs[i].label);    
        DrawVMenu(VMenuAdj_1_5_10_V,i);
        break;
    case VBUT3:
       
        strcpy(myButtonVertMenus[VmenuAjd_01].buttondefs[1].label, myButtonVertMenus[VerticalMenuShowing].buttondefs[i].label);
        DrawVMenu(VmenuAjd_01, i);
        break;
    case VBUT4:
        DrawVMenu(VmenuSetPointSelect,-1);
        break;
    case VBUT5:
        DrawVMenu(VmenuFan,-1);
        break;
    case VBUT6: //roast time
        if (State == STATEROASTING || State == STATECOOLING)
        {
          RoastTime.add(60);
        }
        break;
    case VBUT7: //roast time
        if (State == STATEROASTING && RoastTime.elapsed()> 61)
        {
            RoastTime.add(-60);
        }
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
                //strcpy(bsd->buttondefs[7].label, "ls3");    bsd->buttondefs[7].color = YELLOW;
                //strcpy(bsd->buttondefs[8].label, "FanD");    bsd->buttondefs[8].color = YELLOW;
    //Serial.println ("ProcessVertMenu1:");Serial.println (i);
    switch (i) {
    case 0:        
        spSelected = -1;
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu,-1);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        spSelected = i;   
        strcpy(myButtonVertMenus[VmenuSetPointValue].buttondefs[1].label, myButtonVertMenus[1].buttondefs[i].label);
        DrawVMenu(VmenuSetPointValue,i);
        break;
    case 7:
         if (MySetPoints[EndingSetPoint].Minutes < 20 )
        {
           MySetPoints[EndingSetPoint].Minutes = MySetPoints[EndingSetPoint].Minutes + 1;
           Serial.print("F7");Serial.println(MySetPoints[EndingSetPoint].Minutes);
          byte m = MySetPoints[EndingSetPoint].Minutes;
         
           EEPROM.write(RoastLength_EP,m);
           setpointschanged = true;
           graphProfile(); 
           DrawVMenu(VmenuSetPointSelect,i);
        
        }
       
        break;
    case 8:
         if (MySetPoints[EndingSetPoint].Minutes > 10 )
        {
          MySetPoints[EndingSetPoint].Minutes = MySetPoints[EndingSetPoint].Minutes - 1;
          byte m = MySetPoints[EndingSetPoint].Minutes;
          EEPROM.write(RoastLength_EP,m);
          setpointschanged = true;
          graphProfile();
          DrawVMenu(VmenuSetPointSelect,i);        }
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
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu,-1);
        
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
        DrawVMenu(Vmenubase,-1);
        break;
    default:
        break;
    }

    if (moveamount != 0 ){
        setpointschanged = true;
        if (spSelected >= 0 & spSelected <=5) {
           MoveAPoint(spSelected);
          }
        if (spSelected == 6 ) {
          MoveLast3Point();
       }
       if (spSelected == 9 ) {
          FanSpeedPWMAutoDecrease = FanSpeedPWMAutoDecrease + moveamount;
           sendFanPWM_Wire();
           DrawFanGraph();
           
           spSelected =-1;
       }
    }

}

void ProcessAdjust_01Vmenu(int i) {
    //Serial.print ("ProcessSetPointAdjustmentVMenu:");Serial.println (i);    
    double moveamount = 0;    
    switch (i) {
    case 0:
        newState = STATESTOPPED;
        DrawVMenu(VerticalMenuPrior,-1);   
        break;
    case 2:
        moveamount=.01;
        break;
    case 3:
        moveamount=.05;
        break;
    case 4:
        moveamount=.10;
        break;
    case 5:
        moveamount=-.01;
        break;
    case 6:
        moveamount=-.05;
        break;
    case 7:
        moveamount=-.10;
        break;
    case 8:
        break;
    default:
        break;
    }
    
             
    if (moveamount != 0.0 ){
     //Serial.print("moveamount");Serial.println(moveamount);
     //Serial.print("VerticalMenuPrior ");Serial.println(VerticalMenuPrior );
     //Serial.print("VerticalButtonPrior  ");Serial.println(myButtonVertMenus[VerticalMenuShowing].inputbutton   );
      
      
      if (VerticalMenuPrior == VmenuDebug && myButtonVertMenus[VerticalMenuShowing].inputbutton == VBUT7 ){
          Duty = Duty + moveamount;
       
      }
      if (VerticalMenuPrior == Vmenubase && myButtonVertMenus[VerticalMenuShowing].inputbutton  == VBUT3 ){
       //Serial.print("int");Serial.println(Integral);
         Integral = Integral + moveamount;
         if (Integral < 0.0){Integral = 0.0;}
         EEPROM.update(INTEGRAL_EP , (int)(Integral * 100));
         UpdateDisplayDetailA(true);
      }
      
       
    }

}

void ProcessDebugVMenu(int i) {
     //Serial.print("ProcessVertMenu3:");Serial.println(i);
//    buttonsetdef* bsd = &myButtonVertMenus[VerticalMenuShowing];     
    if (errmsg == "Must be in state stopped to debug" || errmsg == "Must be in statefanonly to debug coils"){
        errmsg = "";
        newerrmsg = true;
    }
    switch (i) {
     case VBUT0:        
          if (State == DEBUGTOGGLE || State == DEBUGDUTY){
               newState = STATESTOPPED; 
          }
          //DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu);
          DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu,-1);
        break;
    case VBUT1:
          if (State == DEBUGTOGGLE || State == DEBUGDUTY){
           newState = STATESTOPPED; 
          }  
          DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu,-1);
        break;
    case VBUT3:  //coil 1
    case VBUT4:  //coil 2
        if (State == STATEFANONLY || State == DEBUGCOIL) {

              strcpy(myButtonVertMenus[VmenuOnOff].buttondefs[1].label, myButtonVertMenus[VerticalMenuShowing].buttondefs[i].label);
                           DrawVMenu(VmenuOnOff,i);
              newState = DEBUGCOIL;
       }
       else
       {
          newerrmsg = true;
          errmsg = "Must be in statefanonly to debug coils";
       }
       break;
    
    case VBUT5:  //vib
    case VBUT6:  //fan
    
    if (State == STATESTOPPED || State == DEBUGTOGGLE || State == DEBUGDUTY) {
              strcpy(myButtonVertMenus[VmenuOnOff].buttondefs[1].label, myButtonVertMenus[VerticalMenuShowing].buttondefs[i].label);
                           DrawVMenu(VmenuOnOff,i);
              newState = DEBUGTOGGLE;
       }
       else
       {
          newerrmsg = true;
          errmsg = "Must be in state stopped to debug";
       }
        break;
    case VBUT7: //duty
        if (State == STATESTOPPED || State == DEBUGTOGGLE || State == DEBUGDUTY) {
              strcpy(myButtonVertMenus[VmenuAjd_01].buttondefs[1].label, myButtonVertMenus[VerticalMenuShowing].buttondefs[i].label);
              DrawVMenu(VmenuAjd_01, i);
              newState = DEBUGDUTY;

       }
       else
       {
          newerrmsg = true;
          errmsg = "Must be in state stopped to debug";
       }
    case VBUT8:  //temp
          strcpy(myButtonVertMenus[VMenuAdj_1_5_10_V].buttondefs[1].label, myButtonVertMenus[VerticalMenuShowing].buttondefs[i].label);    
          DrawVMenu(VMenuAdj_1_5_10_V,i);
        break;
    }
}

void ProcessFanVmenu(int i) {
    //Serial.print ("ProcessSetPointAdjustmentVMenu:");Serial.println (i);
    //Serial.print ("spSelected:");Serial.print (spSelected);
    moveamount = 0;    
    switch (i) {
    case 0:
      
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu,-1);
        
        break;
    case 1:  //A+
        if (FanSpeedPWNDelayDecreaseByMinutes < 5){
        FanSpeedPWNDelayDecreaseByMinutes = FanSpeedPWNDelayDecreaseByMinutes + 1;
        EEPROM.write(FanSpeedPWNDelayDecreaseByMinutes_EP,FanSpeedPWNDelayDecreaseByMinutes);}

        
        break;
   
    case 2: //A-
        if (FanSpeedPWNDelayDecreaseByMinutes > 0 ){
        FanSpeedPWNDelayDecreaseByMinutes = FanSpeedPWNDelayDecreaseByMinutes - 1;
        EEPROM.write(FanSpeedPWNDelayDecreaseByMinutes_EP,FanSpeedPWNDelayDecreaseByMinutes);}

        
        break;
    case 3: //B+
        if (FanSpeedPWMAutoDecrease < 200 && (FanSpeedPWMStart - FanSpeedPWMAutoDecrease) > 1 ){
            FanSpeedPWMAutoDecrease = FanSpeedPWMAutoDecrease + 1;

            if (FanSpeedPWMAutoDecrease > FanSpeedPWMStart){
                FanSpeedPWMAutoDecrease = FanSpeedPWMStart;
            }

            EEPROM.write(FanSpeedPWMAutoDecrease_EP,FanSpeedPWMAutoDecrease);}

               break;
    case 4: //B++
     if (FanSpeedPWMAutoDecrease < 195 && (FanSpeedPWMStart - FanSpeedPWMAutoDecrease) > 5 ){
        FanSpeedPWMAutoDecrease = FanSpeedPWMAutoDecrease + 5;
        if (FanSpeedPWMAutoDecrease > FanSpeedPWMStart){
            FanSpeedPWMAutoDecrease = FanSpeedPWMStart;
        }

            EEPROM.write(FanSpeedPWMAutoDecrease_EP,FanSpeedPWMAutoDecrease);}
               break;
        break;
    case 5://B-
        if (FanSpeedPWMAutoDecrease > 0  ){
            FanSpeedPWMAutoDecrease = FanSpeedPWMAutoDecrease - 1;

            EEPROM.write(FanSpeedPWMAutoDecrease_EP,FanSpeedPWMAutoDecrease);}
        break;
    case 6:
        if (FanSpeedPWMAutoDecrease > 5  ){
        FanSpeedPWMAutoDecrease = FanSpeedPWMAutoDecrease - 5;

            EEPROM.write(FanSpeedPWMAutoDecrease_EP,FanSpeedPWMAutoDecrease);}
        break;
    case 7:
        if (FanSpeedPWNDecreaseByMinutes < (MySetPoints[EndingSetPoint].Minutes - FanSpeedPWNDelayDecreaseByMinutes)){
        FanSpeedPWNDecreaseByMinutes = FanSpeedPWNDecreaseByMinutes + 1;
            EEPROM.write(FanSpeedPWNDecreaseByMinutes_EP,FanSpeedPWNDecreaseByMinutes);}
    break;
    case 8:
        if (FanSpeedPWNDecreaseByMinutes > 0 ){
        FanSpeedPWNDecreaseByMinutes = FanSpeedPWNDecreaseByMinutes - 1;
        EEPROM.write(FanSpeedPWNDecreaseByMinutes_EP,FanSpeedPWNDecreaseByMinutes);}
        break;
    default:
        break;
    }

    DrawFanGraph();
    
  }

void ProcessOnOffVMenu(int iButPressed) {
   //Serial.print("ProcessOnOffVMenu:");Serial.println(i);
   int ONOFF = HIGH; 
    switch ( iButPressed) {
        case VBUT0: 
          DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu, -1);
        break;
    case VBUT1:
        break;
    case VBUT2: //on
        ONOFF = HIGH;
        break;
    case VBUT3: //off
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

    
    if (iButPressed == 2 ||  iButPressed == 3){
      switch (myButtonVertMenus[VerticalMenuShowing].inputbutton) {
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
            if (FanSpeedPWM == 0) {FanSpeedPWM = FanSpeedPWMStart ;}
            digitalWrite(FANRELAYp, !ONOFF);
            break;
        }
    }
}

void ProcessAdj_1_5_10_VMenu(int i) {
    //Serial.print ("ProcessSetPointAdjustmentVMenu:");Serial.println (i);
    //Serial.print ("spSelected:");Serial.print (spSelected);
    int moveamount = 0;    
    switch (i) {
    case 0:
        newState = STATESTOPPED;
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu, -1);   
        break;
    case 2:
        moveamount=1;
        break;
    case 3:
        moveamount=5;
        break;
    case 4:
        moveamount=10;
        break;
    case 5:
        moveamount=-1;
        break;
    case 6:
        moveamount=-5;
        break;
    case 7:
        moveamount=-10;
        break;
    case 8:
        break;
    default:
        break;
    }


    if (moveamount != 0.0 ){

      if (VerticalMenuPrior == Vmenubase && myButtonVertMenus[VerticalMenuShowing].inputbutton  == VBUT2 ){
         Gain = Gain + moveamount;
         if (Gain < 10){Gain = 0;}
         EEPROM.update(GAIN_EP , Gain);
         UpdateDisplayDetailA(true);
      }
      else if (VerticalMenuPrior == Vmenubase && myButtonVertMenus[VerticalMenuShowing].inputbutton  == VBUT8 ){
         TEMPCOILTOOHOT = TEMPCOILTOOHOT + moveamount;
         UpdateDisplayDetailA(true);
      }
    
      
      manualtemp = manualtemp + moveamount; //this many not be needed
    
    }

}

void ProcessZeroAmpMetersVMenu(int i) {
   //Serial.print("ProcessVertMenu4:");Serial.println(i);
    moveamount = 0;
    switch (i) {
    case 0:
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu, -1);
        break;
    case 1:
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu, -1);
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
//Serial.println("Toggle end action");
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
void MoveLast3Point() {
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
 //Serial.print ("MenuShowingYmin:");Serial.println (butdefset.bounding.ymin);
  //Serial.print ("MenuShowingXmax:");Serial.println (butdefset.bounding.xmax);
  //Serial.print ("MenuShowingYmax:");Serial.println (butdefset.bounding.ymax);



}

void DrawButton(buttonsetdef& butdefset, int i)
{
    //draw button
    //Serial.println ("Drawing buttons x:");Serial.println (butdefset.buttondefs[i].x),Serial.println (" y:");Serial.println ( butdefset.buttondefs[i].y);Serial.println (" w:");Serial.println ( butdefset.buttondefs[i].w) ;
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

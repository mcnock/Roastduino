

//-------------------------------------------------  ---------------------------------------------------------------------------------------------------
// MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS          MENUS
//----------------------------------------------------------------------------------------------------------------------------------------------------


void ClearIntergalSum() {
  IntegralSum = 0;
}

void DrawHorControlMenu() {
  //these ar the buttoms on top row starting about 1/2 across screen
  DrawMenuButtons(myHorControlMenuDef);
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
       
      } 
      Serial.println(F("Stop Called!"));
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
  DrawMenuButtons(myHorFanButtonControl);
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
      //DrawFanGraph_ex(true);
      break;
  }
  if (change != 0){    
      if (FanSpeedPWMAutoMode == true){
          
          FanDeviation = FanDeviation + change;
          
          //the following call will corrext the FanDeviation if it is too large
          SetAndSendFanPWMForATime(RoastMinutes);
          //DrawFanGraph();
      }
      else
      {
        if (State == STATEFANONLY){
            FanSpeedPWM = FanSpeedPWM + change;  
              
            if (FanSpeedPWM > 254){FanSpeedPWM = 254;}
            if (FanSpeedPWM < 0){FanSpeedPWM = 0;}
             
             FanDeviation = FanSpeedPWM -FanSpeedPWMStart;
              
            sendFanPWM_Wire();
        }
            //DrawFanGraph();
            //delay(5);
      }
      
      }
  
  
}

void intializeMenus(){
    for (int i = 0; i < 10; i++)
    {
       //Serial.print("VMenu");Serial.println(i);
        //this make a name
        buttonsetdef* bsd = &myButtonVertMenus[i];
        bsd->ButtonCount = 9;
        bsd->rowstart = 0;
        bsd->W = 80;
        bsd->H = 50;
        bsd->vertical = true;
        bsd->colstart = myGLCD.getDisplayXSize() - bsd->W - 2;
        bsd->menuID = i;
        switch (i) {
        case Vmenubase:{
                bsd->ClickHandler = ProcessBaseVMenu;
                bsd->nextMenu = VmenuDebug;
                bsd->backMenu = VmenuEmpty;
                break;
        }
         case VmenuDebug:{
                bsd->ClickHandler = ProcessDebugVMenu;
                bsd->nextMenu = VmenuEmpty;
                bsd->backMenu = Vmenubase;
            break;
            }

        case VmenuSetPointSelect:{
                bsd->ClickHandler = ProcessSetPointSelectVMenu; 
                bsd->nextMenu = -1;
                bsd->backMenu = Vmenubase;
            break;
            }
        case VmenuFan:{
                bsd->ClickHandler = ProcessFanVmenu; 
                bsd->nextMenu = -1;
                bsd->backMenu = Vmenubase;
            break;
            }
        case VmenuAdj_1_3_5:{
                bsd->ClickHandler = Process_135_VMenu; 
                bsd->nextMenu = -1;
                bsd->backMenu = VmenuSetPointSelect;
           break;
           }

        case VmenuOnOff:{
                bsd->ClickHandler = ProcessOnOffVMenu; 
                bsd->nextMenu = -1;
                bsd->backMenu = VmenuDebug;
            break;
            }
        case VmenuAjd_01:{
                bsd->ClickHandler = ProcessAdjust_01Vmenu; 
                bsd->nextMenu = -1;
                bsd->backMenu = VmenuDebug;
            break;
            }
       
       case VMenuAdj_1_5_10_V:{
                bsd->ClickHandler = ProcessAdj_1_5_10_VMenu; 
                bsd->nextMenu = -1;
                bsd->backMenu = VmenuDebug;
            break;
            }
       case VmenuEmpty:{
                bsd->ClickHandler = ProcessEmptyVmenu; 
                bsd->nextMenu = Vmenubase;
                bsd->ButtonCount = 1;
                bsd->backMenu = VmenuDebug;
            break;
            }
         
       } 
       SetMenuBoundingRect(myButtonVertMenus[i]);  
    }
  myHorControlMenuDef.H = 40;
  myHorControlMenuDef.W = 90;
  myHorControlMenuDef.rowstart = 0;
  myHorControlMenuDef.colstart = 360;
  myHorControlMenuDef.vertical = false;
  myHorControlMenuDef.ButtonCount = 4;
  myHorControlMenuDef.menuID = HmenuCTRL;
  myHorControlMenuDef.ClickHandler = ProcessHorControlMenu;
  SetMenuBoundingRect(myHorControlMenuDef);
  
  myHorFanButtonControl.H = 40;
  myHorFanButtonControl.W = 80;
  myHorFanButtonControl.rowstart = 410;
  myHorFanButtonControl.colstart = 60;
  myHorFanButtonControl.ButtonCount = 4;
  myHorFanButtonControl.vertical = false;
  myHorFanButtonControl.menuID = HmenuFAN;
  SetMenuBoundingRect(myHorFanButtonControl);
  myHorFanButtonControl.ClickHandler = ProcessHorFanMenu;
  
 
}


 void DrawVMenu(int iMenu, int iButton) {
    VerticalMenuPrior = VerticalMenuShowing; 
    VerticalMenuShowing = iMenu;
    myButtonVertMenus[iMenu].inputbutton = iButton;
    //Serial.print("draw v menu");Serial.println(iMenu);
    //Serial_println_rect(myButtonVertMenus[iMenu].bounding);
    
    if (iMenu == VmenuEmpty && iButton != -1)
    {
        //Serial.println ("blacking out");
    
        //Serial_println_rect(myButtonVertMenus[Vmenubase].bounding);
        myGLCD.setColor(BLACK);
        myGLCD_drawRect(myButtonVertMenus[Vmenubase].bounding);
        myGLCD_fillRect(myButtonVertMenus[Vmenubase].bounding);
    }
    DrawMenuButtons(myButtonVertMenus[iMenu]);
}

void ProcessEmptyVmenu(int i)
{
    DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu,-1);
}
void ProcessBaseVMenu(int i) {
    //Serial.println ("ProcessVertMenu1:");Serial.println (i);
  
    switch (i) {
    case VBUT0:
        DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu,0);
        break;
    case VBUT1:
        //DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu,-1);
                break;
    case VBUT2:
        myButtonVertMenus[VMenuAdj_1_5_10_V].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
        myButtonVertMenus[VMenuAdj_1_5_10_V].buttondefs[1].AlternateLableID.ButtonID = i;
        DrawVMenu(VMenuAdj_1_5_10_V,i);
        break;
    case VBUT3:
         
        myButtonVertMenus[VmenuAjd_01].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
        myButtonVertMenus[VmenuAjd_01].buttondefs[1].AlternateLableID.ButtonID = i;
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
        myButtonVertMenus[VmenuAdj_1_3_5].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
        myButtonVertMenus[VmenuAdj_1_3_5].buttondefs[1].AlternateLableID.ButtonID = i;
 
        DrawVMenu(VmenuAdj_1_3_5,i);
        break;
    case 7:
         if (MySetPoints[EndingSetPoint].Minutes < 20 )
        {
           MySetPoints[EndingSetPoint].Minutes = MySetPoints[EndingSetPoint].Minutes + 1;
          //Serial.print("F7");Serial.println(MySetPoints[EndingSetPoint].Minutes);
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

void Process_135_VMenu(int i) {
    //Serial.print ("ProcessSetPointAdjustmentVMenu:");Serial.println (i);
    //Serial.print ("spSelected:");Serial.print (spSelected);
    moveamount = 0;    
    switch (i) {
    case 0:
        if (VerticalMenuPrior == VmenuSetPointSelect) {
            for (int xSetPoint = 1; xSetPoint < SetPointCount; xSetPoint++)
            {
                MySetPoints[xSetPoint].TemperatureNew = 0;
            }
            spSelected = -1;
            DrawVMenu(myButtonVertMenus[VerticalMenuShowing].backMenu,-1);
        }
        if (VerticalMenuPrior == VmenuFan) {
           
            DrawVMenu(VmenuFan,-1);
        }
        
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
        if (VerticalMenuPrior == VmenuSetPointSelect) {
            saveChangedSetpoints();
            graphProfile();
        }
        
        break;
        if (VerticalMenuPrior == VmenuFan) {
           
            DrawVMenu(VmenuFan,-1);
        }

    default:
        break;
    }

    if (moveamount != 0 ){
      if (VerticalMenuPrior == VmenuSetPointSelect) {
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
           graphFanProfile();
           
           spSelected =-1;
       }
      }
      else if (VerticalMenuPrior == VmenuFan)
      {
           //Serial.print("fan125 button clicked:");Serial.println(i);
           //Serial.print("fan125 inputbutton:");Serial.println(myButtonVertMenus[VerticalMenuShowing].inputbutton);
           if (myButtonVertMenus[VerticalMenuShowing].inputbutton == 3) {
              FanSpeedPWMStart = FanSpeedPWMStart + moveamount;

            if (FanSpeedPWMStart > 254){
                 FanSpeedPWMStart = 254;
            }
            if (FanSpeedPWMStart < 125){
                 FanSpeedPWMStart = 125;
            }

            EEPROM.write(FanSpeedPWMStart_EP,FanSpeedPWMStart);
           }
           if (myButtonVertMenus[VerticalMenuShowing].inputbutton == 4) {
           FanSpeedPWMAutoDecrease = FanSpeedPWMAutoDecrease + moveamount;

            if (FanSpeedPWMAutoDecrease + FanGraphMinPWM > FanSpeedPWMStart ){
                FanSpeedPWMAutoDecrease = FanSpeedPWMStart -  FanGraphMinPWM;
                 Serial.print("A:");Serial.println(FanGraphMinPWM);
                  Serial.print("Aa:");Serial.println(FanSpeedPWMStart);
    
                  Serial.print("B:");Serial.println(FanSpeedPWMAutoDecrease);
          
            }
            if (FanSpeedPWMAutoDecrease < 10){
                FanSpeedPWMAutoDecrease = 10;
            }

            EEPROM.write(FanSpeedPWMAutoDecrease_EP,FanSpeedPWMAutoDecrease);
           
           }
           graphFanProfile();
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
          DrawVMenu(myButtonVertMenus[VerticalMenuShowing].nextMenu,0);
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
              myButtonVertMenus[VmenuOnOff].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
              myButtonVertMenus[VmenuOnOff].buttondefs[1].AlternateLableID.ButtonID = i;
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
              myButtonVertMenus[VmenuOnOff].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
              myButtonVertMenus[VmenuOnOff].buttondefs[1].AlternateLableID.ButtonID = i;
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
              myButtonVertMenus[VmenuAjd_01].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
              myButtonVertMenus[VmenuAjd_01].buttondefs[1].AlternateLableID.ButtonID = i;
              DrawVMenu(VmenuAjd_01, i);
              newState = DEBUGDUTY;

       }
       else
       {
          newerrmsg = true;
          errmsg = "Must be in state stopped to debug";
       }
    case VBUT8:  //temp
          myButtonVertMenus[VMenuAdj_1_5_10_V].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
          myButtonVertMenus[VMenuAdj_1_5_10_V].buttondefs[1].AlternateLableID.ButtonID = i;
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
    case 3: //A
        myButtonVertMenus[VmenuAdj_1_3_5].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
        myButtonVertMenus[VmenuAdj_1_3_5].buttondefs[1].AlternateLableID.ButtonID = i;
        DrawVMenu(VmenuAdj_1_3_5,i);
        break;
    case 4: //B
        myButtonVertMenus[VmenuAdj_1_3_5].buttondefs[1].AlternateLableID.MenuID = VerticalMenuShowing;
        myButtonVertMenus[VmenuAdj_1_3_5].buttondefs[1].AlternateLableID.ButtonID = i;
        DrawVMenu(VmenuAdj_1_3_5,i);
        break;
    case 5://C+
     if (FanSpeedPWNDecreaseByMinutes < (MySetPoints[EndingSetPoint].Minutes - FanSpeedPWNDelayDecreaseByMinutes)){
        FanSpeedPWNDecreaseByMinutes = FanSpeedPWNDecreaseByMinutes + 1;
            EEPROM.write(FanSpeedPWNDecreaseByMinutes_EP,FanSpeedPWNDecreaseByMinutes);}
        break;
    
    case 6: //c-
     if (FanSpeedPWNDecreaseByMinutes > 0 ){
        FanSpeedPWNDecreaseByMinutes = FanSpeedPWNDecreaseByMinutes - 1;
        EEPROM.write(FanSpeedPWNDecreaseByMinutes_EP,FanSpeedPWNDecreaseByMinutes);}
        break;
    case 7: //C
      break;
    case 8:
        break;
    default:
        break;
    }

    graphFanProfile();
    
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
            Serial.println(F("C:ONOFF"));

            digitalWrite(SSR1_p7, ONOFF);
            break;
        case 4: //ss 2
            digitalWrite(SSR2_p6, ONOFF);
            break;
        case 5: //vib
//            digitalWrite(VIBRELAYp, !ONOFF);
            break;
        case 6: //fan
            if (FanSpeedPWM == 0) {FanSpeedPWM = FanSpeedPWMStart ;}
            digitalWrite(FANRELAYp_2, !ONOFF);
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
             
             EEPROM.put(SETPOINTTEMP_EP[xSetPoint], MySetPoints[xSetPoint].Temperature);
             

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
  for (int i = 0 ; i < butdefset.ButtonCount; i++) {
      if (butdefset.vertical == true) {
          butdefset.buttondefs[i].Rect.y = butdefset.rowstart + butdefset.H * i;
          butdefset.buttondefs[i].Rect.x = butdefset.colstart ;
      }
      else {
          butdefset.buttondefs[i].Rect.y = butdefset.rowstart;
          butdefset.buttondefs[i].Rect.x = butdefset.colstart + butdefset.W * i;      
      }
      //butdefset.buttondefs[i].h = butdefset.H;  //CAN BE REFACTORED OUT
      //butdefset.buttondefs[i].w = butdefset.W;  //CAN BE REFACTORED OUT
      butdefset.buttondefs[i].Rect.xmax = butdefset.buttondefs[i].Rect.x + butdefset.W;
      butdefset.buttondefs[i].Rect.ymax = butdefset.buttondefs[i].Rect.y + butdefset.H;

   
  }
  if (butdefset.vertical == false) {
          //Serial.println("Here");
          //Serial.println(butdefset.colstart);
          butdefset.bounding.x = butdefset.colstart;
          //Serial.println(butdefset.W);
          
          butdefset.bounding.xmax = butdefset.colstart + butdefset.W * butdefset.ButtonCount ;
          //Serial.println(butdefset.rowstart);
          
          butdefset.bounding.y =  butdefset.rowstart;
          //Serial.println(butdefset.H);
         
          butdefset.bounding.ymax = butdefset.rowstart + butdefset.H ;
       }
   else { 
          butdefset.bounding.x = butdefset.colstart;
          butdefset.bounding.xmax = butdefset.colstart + butdefset.W ; 
          butdefset.bounding.y =  butdefset.rowstart;
          butdefset.bounding.ymax = butdefset.rowstart + butdefset.H * butdefset.ButtonCount;      
   } 
}

void DrawMenuButtons(buttonsetdef &butdefset) {

   
    
    for (int i = 0; i < butdefset.ButtonCount; i++) {
        
        DrawButton(butdefset, i, false);
    }
}

void DrawButton(buttonsetdef& butdefset, int i, boolean toggletooltip){    
    int xTipOffset;
    int yTipOffset;
    rect DrawRect;

    //memcpy_P(&DrawRect,&butdefset.buttondefs[i].Rect,sizeof(DrawRect));
    
    if (butdefset.vertical == true) {
       //Serial.print("gettting button text for VmenuID:");Serial.print(butdefset.menuID);Serial.print(" button:");Serial.println(i);
    }
    else{
       //Serial.print("gettting button text for HmenuID:");Serial.print(butdefset.menuID);Serial.print(" button:");Serial.println(i);
     
    }

    if (butdefset.buttondefs[i].AlternateLableID.ButtonID > -1){
        memcpy_P(&myArrayLocal, &Vmenutext[butdefset.buttondefs[i].AlternateLableID.MenuID][butdefset.buttondefs[i].AlternateLableID.ButtonID], sizeof( buttontext));
    }
    else
    {
        memcpy_P(&myArrayLocal, &Vmenutext[butdefset.menuID][i], sizeof( buttontext));
    }
    
    
    //Serial.print(" id:");Serial.print(myArrayLocal.key);Serial.print(" label:");Serial.println(myArrayLocal.label);
 
    myGLCD.setColor(myArrayLocal.color);
    myGLCD_fillRect(butdefset.buttondefs[i].Rect);
    myGLCD.setColor(BLACK);
    myGLCD_drawRect(butdefset.buttondefs[i].Rect);

    myGLCD.setBackColor(myArrayLocal.color);
    myGLCD.setColor(BLACK);
    int xOffset =0;
    int yOffset =0;
    int xT = 0;
    
    //this is normal
    if (toggletooltip == false || butdefset.buttondefs[i].ToolTipShowing == true){
        butdefset.buttondefs[i].ToolTipShowing = false;
        myGLCD.setFont(BigFont); //main button. 6 chars.
        xOffset = (butdefset.W - (strlen(myArrayLocal.label) * myGLCD.getFontXsize()))/2;      
        yOffset = (butdefset.H - myGLCD.getFontYsize()) / 2;
        myGLCD.print(myArrayLocal.label,butdefset.buttondefs[i].Rect.x + xOffset   , butdefset.buttondefs[i].Rect.y + yOffset );
    }
    else
    {    //tool tip 2 or 3 line x 12 char
        
        myGLCD.setFont(Retro8x16);
        xOffset = (butdefset.W - (strlen(myArrayLocal.tip1) * myGLCD.getFontXsize()))/2;
        butdefset.buttondefs[i].ToolTipShowing = true;      
        if (butdefset.vertical == true) {
            xT = 10;
            yOffset = (butdefset.H/3 - myGLCD.getFontYsize())/ 2;
            myGLCD.print(myArrayLocal.tip1,butdefset.buttondefs[i].Rect.x + xOffset   , butdefset.buttondefs[i].Rect.y + yOffset );
            xOffset = (butdefset.W + xT - (strlen(myArrayLocal.tip2) * myGLCD.getFontXsize()))/2;      
            myGLCD.print(myArrayLocal.tip2,butdefset.buttondefs[i].Rect.x + xOffset    , butdefset.buttondefs[i].Rect.y + yOffset + butdefset.H/3  );
            xOffset = (butdefset.W - (strlen(myArrayLocal.tip3) * myGLCD.getFontXsize()))/2; 
          myGLCD.print(myArrayLocal.tip3,butdefset.buttondefs[i].Rect.x + xOffset  , butdefset.buttondefs[i].Rect.y + yOffset + (butdefset.H/3 * 2)  );
        
        }
        else
        {
            yOffset = (butdefset.H/2 - myGLCD.getFontYsize())/ 2;
            myGLCD.print(myArrayLocal.tip1,butdefset.buttondefs[i].Rect.x + xOffset   , butdefset.buttondefs[i].Rect.y + yOffset );
            xOffset = (butdefset.W - (strlen(myArrayLocal.tip2) * myGLCD.getFontXsize()))/2;      
            myGLCD.print(myArrayLocal.tip2,butdefset.buttondefs[i].Rect.x + xOffset   , butdefset.buttondefs[i].Rect.y + yOffset + butdefset.H/2);
         
        }
    }
}

void OutlineButton(buttonsetdef* butdefset, int i,uint16_t  color){
        uint16_t  priorcolor = myGLCD.getColor();
        myGLCD.setColor(color);
        myGLCD_drawRect(butdefset->buttondefs[i].Rect);
        myGLCD.setColor(priorcolor);
    

}

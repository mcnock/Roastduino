void processSerial()
{
if (Serial.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial.read();
    int incomingByte2 = 0;
    int incomingByte3 = 0;
    delay(100);
    if (Serial.available() > 0) {
      incomingByte2 = Serial.read();
 
    }
    delay(100);
 //     Serial.println("E:End  s:GetStatus c:GetConfiguration t:temperatures  t[1,2,o]:1secondtemps,3secondtemps,off");
 //     Serial.println("F:Fan[S,i,I,d,D]:Start,Incr,Decr   P[i,I,d,D]:ProfileIncrDec]    R:Roast[S,A,R]:Start,AddMinute,RemMinute");
 //     Serial.println("GXX:SetGain   IXX:SetIntegral");
    
     if (Serial.available() > 0) {
      incomingByte3 = Serial.read();
    }  
    // say what you got:
   switch (incomingByte){
    case 63: //?
    { 
      Serial.println("E:End  s:GetStatus c:GetConfiguration t:temperatures  t[1,2,o]:1secondtemps,3secondtemps,off");
      Serial.println("F:Fan[S,i,I,d,D]:Start,Incr,Decr   R:Roast[S,A,R]:Start,AddMinute,RemMinute   P[i,I,d,D]:ProfileIncrDec]");
      Serial.println("GXX:SetGain   IXX:SetIntegral");
      
      break;
    }
    case 70: //F
    { 
     int v = 0;
     int b = FanSpeedPWM;;
     switch (incomingByte2){
     case 83: //S
       {
           Serial.print ("Fan On ");  
           ProcessHorControlMenu(2);
           Serial.println (FanSpeedPWM);
           break;
       }
     case 68: //D
        { 
           v = -10;
           ProcessHorFanMenu(0);   
           break;
        }
     
     case 100: //d
        { 
          v = -3;
          ProcessHorFanMenu(1);
          break;
        }
      case 105: //i
        { 
          v = 3;
          ProcessHorFanMenu(2);
          break;
        }
      case 73: //I
        { 
          v = 10;
          ProcessHorFanMenu(3);
          break;
        } 
       default:{
         
       }
     }
        if (v != 0)
         {
          Serial.print("Fan ");
          Serial.print(v);
          Serial.print (" from " );
          Serial.print(b );
          Serial.print (" to ");
          Serial.println (FanSpeedPWM);
          
         }
     break;
    } 
    case 80: //P
    { 
      Serial.print("Profile before:");
       for (int x = 0; x < SetPointCount; x++) {
        Serial.print(x);
        Serial.print("-");
        Serial.print( MySetPoints[x].Minutes);
        Serial.print("-");
        Serial.print (MySetPoints[x].Temperature);
        Serial.print(":");
       }
       Serial.println("");

     switch (incomingByte2){
     case 68://D
        { Serial.println("Profile Decrease 5 ");
          moveamount = -5;          MoveLast3Point();
          saveChangedSetpoints();
          break;
        }
        case 100:  //d
        { Serial.println("Profile Decrease 3 ");
          moveamount = -3;
          MoveLast3Point();
          saveChangedSetpoints();
          
          break;
        }
      case 105:  //i
        { Serial.println("Profile Increase 3 ");
          moveamount = 3;
          MoveLast3Point();
          saveChangedSetpoints();
          
          break;
        }
      case 73: //I
        { Serial.println("Profile Increase 5 ");
          moveamount = 5;
          MoveLast3Point();
          saveChangedSetpoints();
          
       
          break;
        }
     }
      Serial.print("Profile  after:");
       for (int x = 0; x < SetPointCount; x++) {
        Serial.print(x);
        Serial.print("-");
        Serial.print( MySetPoints[x].Minutes);
       Serial.print("-");
        Serial.print (MySetPoints[x].Temperature);
        Serial.print(":");
       }
       Serial.println("");

      break;  
      
     } 
    case 82: //R
    { 
       switch (incomingByte2){
       case 83: //S
       {
          Serial.println("Start Roast");
          ProcessHorControlMenu(0);
          break;
  
       }
      case 65: //A
      { Serial.println("Add a minute");
        ProcessBaseVMenu(7);  
        break;
      }
      case 82: //R
      { Serial.println("Remove a minute");
        ProcessBaseVMenu(6);  
        break;
      }
      
      }
      break;
    }
    case 101: //e
    {
      
    }
    case 69: //E
    { 
      Serial.println("End Roast");
      ProcessHorControlMenu(1);
      break;
    }
    case 115: //s
    {  
        switch (incomingByte2){
       case 49: //1
       {
          serialOutPutStatusBySecond = !serialOutPutStatusBySecond;
          Serial.print ("1secondStatus:");Serial.println(serialOutPutStatusBySecond);
          
          serialOutPutStatusBy3Seconds = false;
          break;
  
       }
       case 51: //3
       {
          serialOutPutStatusBy3Seconds = !serialOutPutStatusBy3Seconds;
          Serial.print ("3secondStatus:");Serial.println(serialOutPutStatusBySecond);

          serialOutPutTempsBySecond = false;
          break;
  
       }
       case 111: //0
       {
         serialOutPutStatusBySecond = false;
         serialOutPutStatusBy3Seconds = false;
         Serial.println("Status by seconds off");
         break;
       }
       default: 
       {
          SerialOutputStatus() ;      
        break;
    
       }     
     
      }
    break;
    }  
    case 99: //c
     {  
  
          Serial.print("Profile");
          for (int x = 0; x < SetPointCount; x++) {
          Serial.print(x);
          Serial.print("-");
          Serial.print( MySetPoints[x].Minutes);
          Serial.print("-");
          Serial.print (MySetPoints[x].Temperature);
          Serial.print(":");
         }
         Serial.println("");
  
        Serial.print ("G:"); Serial.print(Gain); Serial.print ("I:"); Serial.println(Integral);
        Serial.print ("FanDecrease DelayMin:"); Serial.print(FanSpeedPWNDelayDecreaseByMinutes); Serial.print ("Decrease:"); Serial.print(FanSpeedPWMAutoDecrease);Serial.print ("DecreaseMinutes"); Serial.println(FanSpeedPWNDecreaseByMinutes);
  
     
        
        break;
       
      }
       case 116: //t
      {  
       switch (incomingByte2){
       case 49: //1
       {
          serialOutPutTempsBySecond = !serialOutPutTempsBySecond;
          Serial.print ("1secondtemps:");Serial.println(serialOutPutTempsBySecond);
          
          serialOutPutTempsBy3Seconds = false;
          break;
  
       }
       case 51: //3
       {
          serialOutPutTempsBy3Seconds = !serialOutPutTempsBy3Seconds;
          Serial.print ("3secondtemps:");Serial.println(serialOutPutTempsBySecond);

          serialOutPutTempsBySecond = false;
          break;
  
       }
       case 111: //0
       {
         serialOutPutTempsBySecond = false;
         serialOutPutTempsBy3Seconds = false;
         Serial.println("Temps by seconds off");
         break;
       }
       default: 
       {
         serialOutPutTempsBySecond = false;
         serialOutPutTempsBy3Seconds = false;         
         Serial.print("Bean1:");   
         Serial.print(TBean1);   
         Serial.print(" Bean2:");   
         Serial.print(TBean2);
         Serial.print(" Avg:");   
         Serial.print(TBeanAvgRoll.mean());
         Serial.print(" Coil");   
         Serial.print(TCoil);        
         Serial.print(" Avg");   
         Serial.print(TCoilRoll.mean());        
         Serial.print(" Skipped:");   
         Serial.print(Readingskipped[1]);
         Serial.print("/");
         Serial.print(Readingskipped[2]);
         Serial.print("/");
         Serial.print(Readingskipped[0]);
         Serial.println();
       }
       
       break;
     
    }
     default:
    {
    
      Serial.print("I received : ");
      Serial.println(incomingByte, DEC);
   
    }

    
   }
  incomingByte = 0;
  }
   }
}
void SerialOutputStatus()
{

       Serial.print("Time:");   
       Serial.print(RoastMinutes);
       Serial.print("/");   
       Serial.print(MySetPoints[SetPointCount-1].Minutes);   
       Serial.print(" Bean Temp:");   
       Serial.print(TBeanAvgRoll.mean(),1);
       Serial.print("/");   
       Serial.print(MySetPoints[SetPointCount-1].Temperature);        
       Serial.print( " Delta:-"); 
       Serial.print(Err,1);   
       Serial.print(" Duty:");   
       Serial.print(Duty,2);
       Serial.print(" Coil Temp:");   
       Serial.print(TCoilRoll.mean(),1);
       Serial.print(" FanPWM:");   
       Serial.print(FanSpeedPWM);
       Serial.print("/254");

       Serial.println();

}
void SerialOutputTempsForPlotting()
{
       Serial.print("Bean1:");   
       Serial.print(TBean1);   
       Serial.print(",Bean2:");   
       Serial.print(TBean2);
       Serial.print(",BeanAvg:");   
       Serial.print(TBeanAvgRoll.mean());
       Serial.print(",Coil:");   
       Serial.print(TCoil);        
       Serial.print(",CoilAvg");   
       Serial.print(TCoilRoll.mean());        
       Serial.println();
}

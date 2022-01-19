

//----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------


int  CalcFanPWMForATime(double minutes){
     //Serial.println("starting CalcFanPWMForATime");
     //Serial.print("minutes:");Serial.println(minutes);
     //Serial.print("deviation:");Serial.println(FanDeviation);
     int calcedFanSpeed;
     int newFanSpeed;
     if (minutes == double(0.00)){
            //Serial.println("zeir minutes dedtected");
            //Serial.println(minutes);
            calcedFanSpeed = FanSpeedPWMStart;
     }
     else if (minutes <= FanSpeedPWNDelayDecreaseByMinutes){
            calcedFanSpeed = FanSpeedPWMStart;
        }
     else if (minutes < FanSpeedPWNDecreaseByMinutes){
            
            float ratio = (float)(minutes-FanSpeedPWNDelayDecreaseByMinutes)/(float)(FanSpeedPWNDecreaseByMinutes - FanSpeedPWNDelayDecreaseByMinutes);
            //Serial.print("ratio:");Serial.println(ratio);
            calcedFanSpeed =   (FanSpeedPWMStart - (ratio * FanSpeedPWMAutoDecrease)) ; 
        }
     else
       {
           //Serial.println( FanSpeedPWMStart);
           //Serial.println( FanSpeedPWMAutoDecrease);
           
           calcedFanSpeed =  ( FanSpeedPWMStart - FanSpeedPWMAutoDecrease);
       }

      newFanSpeed = calcedFanSpeed + FanDeviation;
      if (FanSpeedPWMStart > 254){
          if (FanDeviation > 0)
          {
              FanDeviation = 254 - calcedFanSpeed;
              if (FanDeviation > 0){FanDeviation = 0;} 
          }
       
       }
          
       if (FanSpeedPWMStart < 10){
          if (FanDeviation < 0)
          {
              FanDeviation =  calcedFanSpeed - 10;
              if (FanDeviation < 0){FanDeviation = 0;} 
          }
        
        FanSpeedPWMStart = 10;}

       //Serial.print("return:");Serial.println(newFanSpeed);
       return newFanSpeed;
}
int  YforAFan(int fanpwm) {
       //Serial.print("starting YforAFan");Serial.println(fanpwm);

      if (fanpwm <= FanGraphMinPWM)
      {
         //Serial.print("return min:"); Serial.println(FanGraphYBottom);
         return FanGraphBottom;
      }
      else
      {
          double ratio = (fanpwm - FanGraphMinPWM) / float(FanGraphMaxPWM -FanGraphMinPWM); 
          //Serial.print("ratio:");Serial.println(ratio);
          if (ratio > 1) {ratio =1;}
          return   (FanGraphBottom - (ratio * FanGraphHeight));
      }  

}


void  SetAndSendFanPWMForATime(double minutes) {
      //Serial.println("Call From SetFan");
     //Serial.println(minutes);
           
    FanSpeedPWM = CalcFanPWMForATime(minutes);
    sendFanPWM_Wire();
}

void  StopAndSendFanPWM() {
    FanSpeedPWM = 0 ;
    sendFanPWM_Wire();
}

void  sendFanPWM_Wire() {

     float q = 4092;
     float x = FanSpeedPWM;
     float j = q * x ;
     int i= j/254;
     
  //Serial.print("Setting Fan pwm:");Serial.println(FanSpeedPWM);
  //Serial.println(MCP4725_ADDR);
     //Serial.print("Setting Fan 10 bit :");Serial.println(i);
     //interrupts();
      Wire.beginTransmission(MCP4725_ADDR);
      Wire.write(64);                     // cmd to update the DAC
      Wire.write(i >> 4);        // the 8 most significant bits...
      Wire.write((i & 15) << 4); // the 4 least significant bits...
      Wire.endTransmission();
     // dac.setVoltage(i, false,100000);
     //Serial.print("Setting Fan pwm B:");Serial.println(FanSpeedPWM);
     
     
 
}

  
void ReturnSetPoints(Stream &port) {
  for (int x = 0; x < SetPointCount; x++) {
    if (x > 0) {
      port.print ("!");
    }
    port.print (x);
    port.print (":");
    port.print (MySetPoints[x].Minutes);
    port.print (":");
    port.print (MySetPoints[x].Temperature);
  }
  port.println("");
 
}

void Serial_println_rect(struct rect& rect) {
   Serial.print("x:");Serial.print(rect.x);Serial.print(" y:");Serial.print(rect.y);Serial.print(" xmax:");Serial.print(rect.xmax);Serial.print(" ymax:");Serial.println(rect.ymax);
}

int getCleanTemp(double temperature, int myID) {
  if (isnan(temperature)) {
    Readingskipped[myID]++;
    //Serial.print (myID);Serial.print ("nan temp:");Serial.println(temperature);
    return -1;
  }
  else if (temperature > 1000) {
    Readingskipped[myID]++;
   //Serial.print (myID);Serial.print ("too high temp:");Serial.println(temperature);
    return -1;
  }
  else if (temperature < 40) {
    Readingskipped[myID]++;
 //Serial.print (myID);Serial.print ("too low temp:");Serial.println(temperature);
    return -1;
  }
  else {
    int r = temperature;
    //Serial.print (myID);Serial.print ("clean temp returned:");Serial.println(r);
    return r;
  }
}

double getBeanAvgTemp(double t1, double t2) {
  if (t1 != -1 && t2 != -1) {
    return (t1 + t2) / 2;
  }
  else {
    if     (t1 != -1) {
      return t1;
    }
    else {
      return t2;
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ReadIntEprom(int loc, int min, int max, int def) {
  int t;
  t = EEPROM.read(loc);
  if (t >= 0 && t < max) {
    return t ;
  }  else {
    return def ;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------


double SetpointforATime(double roastminutes) {
  int setpoint;
  double r = (roastminutes - (int)roastminutes);
  setpoint = MyMinuteTemperature[(int)roastminutes] + ((double)(MyMinuteTemperature[int(roastminutes) + 1] - MyMinuteTemperature[(int)roastminutes]) * r);
  if (setpoint > 0) {
    return setpoint;
  }

  else
  {
    return 0;
  }
}

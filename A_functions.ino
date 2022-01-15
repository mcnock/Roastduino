

//----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------


int  CalcFanPWMForATime(double minutes){
     //Serial.println(minutes);
     if (minutes == double(0.00)){
            //Serial.println("zeir minutes dedtected");
            //Serial.println(minutes);
            return FanSpeedPWMStart;
     }
     else if (minutes <= FanSpeedPWNDelayDecreaseByMinutes){
            return FanSpeedPWMStart;
        }
     else if (minutes < FanSpeedPWNDecreaseByMinutes){
            return  (FanSpeedPWMStart - (FanSpeedPWMAutoDecrease/(FanSpeedPWNDecreaseByMinutes - FanSpeedPWNDelayDecreaseByMinutes))* (minutes-FanSpeedPWNDelayDecreaseByMinutes)) ; 
        }
       else
       {
           return ( FanSpeedPWMStart - FanSpeedPWMAutoDecrease);
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
     
  //   Serial.print("Setting Fan pwm:");Serial.println(FanSpeedPWM);
  //   Serial.println(MCP4725_ADDR);
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

void SerialprintRect(struct rect * rect) {
   Serial.print("x:");Serial.print(rect->x);Serial.print(" y:");Serial.print(rect->y);Serial.print(" xmax:");Serial.print(rect->xmax);Serial.print(" ymax:");Serial.println(rect->ymax);
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

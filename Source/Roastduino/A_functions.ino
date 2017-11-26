
//----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------

void SerialprintRect(rect &rect){
    Serial.print("xmin:"); Serial.print(rect.xmin); Serial.print(" ymin:"); Serial.print(rect.ymin); Serial.print(" xmax:");Serial.print(rect.xmax);Serial.print(" ymax:");Serial.println(rect.ymax);
}
void SerialprintPoint(point &point){
    Serial.print("x:"); Serial.print(point.x); Serial.print(" y:"); Serial.print(point.y); 
}
// Get the temperature
int getCleanTemp(double temperature, int myID) {
  if (isnan(temperature)) {
    Readingskipped++;
    //Serial.print (myID);Serial.print ("nan temp:");Serial.println(temperature);
    return -1;
  }
  else if (temperature > 1000) {
    Readingskipped++;
  //Serial.print (myID);Serial.print ("too high temp:");Serial.println(temperature);
    return -1;
  }
  else if (temperature < 40) {
    Readingskipped++;
  //Serial.print (myID);Serial.print ("too low temp:");Serial.println(temperature);
    return -1;
  }
  else {
    int r = temperature;
    //Serial.print (myID);Serial.print ("clean temp returned:");Serial.println(r);
    return r;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ReadIntEprom(int loc, int min, int max, int def) {
  int t;
  t = EEPROM.read(loc);
  if (t >= 0 && t < 100) {
    return t ;
  }  else {
    return def ;
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int ReadTempEprom(int loc, int def) {
  int t;
  t = EEPROM.read(loc) + 254;
  if (t > 254 && t < 495) {
    return ( t);
  }  else {
    return def ;
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SaveTempEprom(int loc, int temp) {
  EEPROM.update(loc, (temp - 254));
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
double calcSetpoint(double roastminutes) {
  int setpoint;
  double r = (roastminutes - (int)roastminutes);
  setpoint = MyMinuteTempuratureSetpoints[(int)roastminutes] + ((double)(MyMinuteTempuratureSetpoints[int(roastminutes) + 1] - MyMinuteTempuratureSetpoints[(int)roastminutes]) * r);
  if (setpoint > 0){
     return setpoint;}
  else
  {
    return 0;
  }
}

int Y(int temp){
  if (temp <= TempYSplit) {  
     return (240 - ((double)temp/TempPerPixL));
  }
  else if (temp <= TempYSplit2){
      return ((240 - PixelYSplit) - ((double)(temp-TempYSplit)/TempPerPixM));
      //460 x 460 x 460 /240  = 379.687
      //return (240 -  ((double)temp * (double)temp * double(temp)) / IYscale);    
  } else {
    
      return ((240 -  PixelYSplit2) - ((double)(temp-TempYSplit2)/TempPerPixH));
  }
  
}


//----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------
// Get the temperature
int getCleanTemp(double temperature, int myID) {
  if (isnan(temperature)) {
    Readingskipped++;
    //Serial.print (myID); Serial.print ("nan temp:"); Serial.println(temperature);
    return -1;
  }
  else if (temperature > 1000) {
    Readingskipped++;
    Serial.print (myID); Serial.print ("too high temp:"); Serial.println(temperature);
    return -1;
  }
  else if (temperature < 40) {
    Readingskipped++;
    Serial.print (myID); Serial.print ("too low temp:"); Serial.println(temperature);
    return -1;
  }
  else {
    int r = temperature;
    //  Serial.print (myID);Serial.print ("clean temp returned:");Serial.println(r);
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
  setpoint = MyMinuteSetpoints[(int)roastminutes] + ((MyMinuteSetpoints[int(roastminutes) + 1] - MyMinuteSetpoints[(int)roastminutes]) * r);
  return setpoint;
}

void LoadORSaveToHistory(boolean Load) {
  int number = 1;
  //300 to 306
  int address = number * 100;
  if (!Load) {
    unsigned long timestamp = millis();
    EEPROM.put(address, timestamp);
    address = address + 4;
    EEPROM.write(address, TempLastEndOfRoast);
    address++;
    EEPROM.write(address, (TimeLastEndOfRoast * 10));
  }
  //312 - 318 & 319 to 325
  address = (number * 100) + 12;
  if (!Load) {
    for (int X = 0; X < SetPointCount; X++) {
      address++;
      EEPROM.write(address, MySpanMinutes[X]);
      SaveTempEprom(address + SetPointCount, MyBaseSetpoints[X]);
    }
  }
  else {
    for (int X = 0; X < SetPointCount; X++) {
      address++;
      MySpanMinutes[X] = EEPROM.read(address);
      MyBaseSetpoints[X] = ReadTempEprom(address + SetPointCount , 0);
    }
  }
  //330 to 394 (320 pixes wide/5 = 64)
  address = (number * 100) + 30;
  int avgwidth = 5;
  if (!Load) {
    int total; int count;
    for (int X = 0; X < 320; X++) {
      total = total + myLastGraph[X];
      count++;
      if (count >= avgwidth) {
        address++;
        int avg = total / avgwidth;
        EEPROM.write(address, avg);
      }
    }
  }
  else {
    int Xlast = 0;
    int Ylast = EEPROM.read(address);
    address++;
    for (int X = 1; X < 64; X++) {
      int Ynext =  EEPROM.read(address);
      double Ydelta = (double)(Ynext - Ylast) / (double)avgwidth;
      for (int k = 1;  k <= avgwidth; X++) {
        myLastGraph[(X - 1) + k] = Ylast + (Ydelta * k) ;
      }
      address++;
    }
  }
  if (Load) {

  }
}


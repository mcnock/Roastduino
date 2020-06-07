

//----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS          FUNCTIONS
//----------------------------------------------------------------------------------------------------------------------------------------------------
void ReadSerial(Stream &port, Chrono &SerialInputTimer ) {
  //StepA discard serial that does not begin with +
  if (port.available() == 0) {
    return;
  }
  while (port.peek() != 43) {
    byte todiscard = port.read();
    port.print ("Discarding ascii:"  );
    port.print (todiscard);
    port.println (" did not start with +");

    if (port.available() == 0) {
      return;
    }
  }

  //StepB wait 2 seconds for 4  or discard
  if (port.available() < 4) {

    if (SerialInputTimer.isRunning() == false) {
      SerialInputTimer.restart();
    }
    else if (SerialInputTimer.elapsed() < 2000) {
    }
    else {
      SerialInputTimer.stop();
      port.println ("Discarding - did not get 4 char in 2 seconds");
      byte b = port.read();
      while (port.available() > 0) {
        if (port.peek() == 43) {
          port.print ("C found another +");
          break;
        }
        byte c = port.read();
      }
    }

    return;
  }

  //Step 3. We have 4 characters read them an look at them
  SerialInputTimer.stop();
  char cmd0 = port.read();
  char cmd1 = port.read();
  char cmd2 = port.read();
  char cmd3 = port.read();
  if (byte(cmd2) == 10 or byte(cmd2) == 13 or byte(cmd3) == 10 or byte(cmd3) == 13) {
    port.print (" Discarding found CR/LF in 3 or 4 character");
    return;
  }
  Serial.println ("Passes");
  Serial.print(cmd0) + Serial.print(cmd1) + Serial.print(cmd2) + Serial.println(cmd3);



  if (1 == 1) {
    switch (cmd1) {
      case 'G': case 'g':
        switch (cmd2) {
          case 'a':
            port.print ("State-0");
            port.print (":");
            port.print ("EndSetPoint-1");
            port.print (":");
            port.print ("RoastMinutes-2");
            port.print (":");
            port.print ("TBeanAvgRoll.mean()-3");
            port.print (":");
            port.print ("T4Bean1-4");
            port.print (":");
            port.print ("TBean2-5");
            port.print (":");
            port.print ("TCoil-6");
            port.print (":");
            port.print ("CurrentFan-7");
            port.print (":");
            port.print ("CurrentHeat1-8");
            port.print (":");
            port.print ("CurrentHeat2-9");
            port.print (":");
            port.print ("err-10");
            port.print (":");
            port.print ("errI-11");
            port.print (":");
            port.println ("Duty-12");
            break;
          case 'A':
            Serial.println("here2");
            port.print ("GA");
            port.print (StateName);
            port.print (":");
            port.print (EndingSetPoint);
            port.print (":");
            port.print (RoastMinutes);
            port.print (":");
            port.print (TBeanAvgRoll.mean());
            port.print (":");
            port.print (TBean1);
            port.print (":");
            port.print (TBean2);
            port.print (":");
            port.print (TCoil);
            port.print (":");
            port.print (CurrentFan);
            port.print (":");
            port.print (CurrentHeat1);
            port.print (":");
            port.print (CurrentHeat2);
            port.print (":");
            port.print (Err);
            port.print (":");
            port.print (ErrI);
            port.print (":");
            port.println (Duty);
            break;
          case 'c':
            port.print ("Gc");
            port.print ("ActiveRoastHistory");
            port.print ("minutes:temp!");
            break;
          case 'C':
            port.print ("GC");
            int count;
            for (int X = 0; X < 320; X = X+3) {
              if (myLastGraphTemps[X] > 0 ) {
                if (count > 0) {
                  port.print ("!");
                } 
                count++;
                double minutes = double(X)/PixelsPerMin;
                port.print (minutes);
                port.print (":");
                port.print (myLastGraphTemps[X]);
              }
              else
              {
                
              }
            }
            port.println("");
            
            break;
          case 'b':
            port.print ("Gb");
            port.println ("Setpoint:AccumMinutes:Tempurature");
            break;
          case 'B':
            Serial.print("here");
            port.print ("GB");
            ReturnSetPoints(port);
            break;
          case 'd':
            port.print ("Gd");
            port.println ("Integral:Gain:LoopsPerSecond:Readingskipped:Err:ErrI:CurrentSetPointTemp");
            break;
          case 'D':
            port.print ("GD");
            for (int x = 0; x < SetPointCount; x++) {
              if (x > 0) {
                port.print ("!");
              }
              port.print (x);
              port.print (":");
              port.print (MySetPoints[x].Minutes);
              port.print (":");
              port.print (MySetPoints[x].Temperature);
              port.println("");
            }
            break;

          default:
            port.println ("Try +GA,GB,GC");
            break;
        }
        break;
      case 'A': case 'a':
        switch (cmd2) {
          case 'c':
            port.println ("ClearIntegralSum");
            break;
          case 'C':
            ClearIntergalSum();
            break;
          case 'r':
            port.println ("RefreshAction");
            break;
          case 'R':
            RefreshAction();
            port.println ("AROK");
            break;
          case 'f':
            port.println ("AfFanAction");
            break;
          case 'F':
            FanAction();
            port.println ("AFOK");
            break;
          case 's':
            port.println ("AsStartAction");
            break;
          case 'S':
            StartAction();
            port.println ("ASOK");
            break;
          case 'e':
            port.println ("AeEndAction");
            break;
          case 'E':
            EndAction();
            port.println ("AEOK");
            break;
          case 't':
            port.println ("AtEndRoastToggleAction");
            break;
          case 'T':
            EndRoastToggleAction();
            port.println ("ATOK");
            break;
          default:
            port.println ("Action Try +AC,AR,AF,AS,AE,AT");
            break;
        }
        break;
      case 'i': case'd':
        port.println ("Increase/Decrease.  Try +[I,D][An,1n,2n,3n,4n,5n,In,Dn,Tn]");
        break;
      case 'I': case'D':
        //3rd char must be numeric
        if (!isDigit(cmd3)) {
          port.print ("3rd digit for I,D commands must be numeric.  You sent:");
          port.println(cmd3);
          return;
        }
        int cmd3AsSignedInt = (int)cmd3 - 48;;
        if (cmd1 == 'D')  {
          cmd3AsSignedInt = -cmd3AsSignedInt;
        }
        Serial.println(cmd2);
        switch (cmd2) {
          case 'a':
            port.print("MoveLast4Points by ");
            port.println(cmd3AsSignedInt);
            break;
          case 'A':
            //MoveLast4Point(cmd3AsSignedInt);
            drawprofileline();
            port.print(cmd1); port.print(cmd2); port.print(cmd3);
            ReturnSetPoints(port);
            break;
          case 'i':
            port.print("MoveIntegral by ");
            port.println(float(cmd3AsSignedInt) / 10);
            break;
          case 'I':
            MoveIntegral(cmd3AsSignedInt);
            port.print(cmd1); port.print(cmd2); port.print(cmd3); port.print( "OK");
            break;
          case 's':
            port.print("MoveIntegralSum by ");
            port.println(float(cmd3AsSignedInt) / 10);
            break;
          case 'S':
            MoveIntegralSum(cmd3AsSignedInt);
            port.print(cmd1); port.print(cmd2); port.print(cmd3); port.print( "OK");
            break;

          case 'g':
            port.print("MoveGain by ");
            port.println(cmd3AsSignedInt);
            break;
          case 'G':
            MoveGain(cmd3AsSignedInt) ;
            port.print(cmd1); port.print(cmd2); port.print(cmd3); port.print( "OK");
            break;
          case 't':
            port.print("MoveTime for setpoint");
            port.print(abs(cmd3AsSignedInt));
            port.print(" by ");
            if (cmd3AsSignedInt > 0) {
              port.println(" 1");
            }
            else {
              port.println(" -1");
            }
            break;
          case 'T':
            MoveTime(cmd3AsSignedInt) ;
            port.print(cmd1); port.print(cmd2); port.print(cmd3);
            ReturnSetPoints(port);
            break;

          case '1': case '2': case '3': case '4': case '5':
            int cmd2AsInt = (int)cmd2 - 48;
        //    MoveAPoint(cmd2AsInt, cmd3AsSignedInt);
            port.print(cmd1); port.print(cmd2); port.print(cmd3);
            ReturnSetPoints(port);
            drawprofileline();

          default:

            break;


            Serial.println("not supported");
        }
        break;
    }
  }

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
  Serial.print("xmin:"); Serial.print(rect->xmin); Serial.print(" ymin:"); Serial.print(rect->ymin); Serial.print(" xmax:"); Serial.print(rect->xmax); Serial.print(" ymax:"); Serial.println(rect->ymax);
}
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
int ReadTempEprom(int loc, int def) {
  int t;
  t = EEPROM.read(loc) + 254;
  if (t > 254 && t < 600) {
    return ( t);
  }  else {
    //Serial.print(loc)
    //Serial.print(" using def temp EEPROM t was"); Serial.println(t);
    return def ;
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SaveTempEprom(int loc, int temp) {

  if (temp >= 254) {
    EEPROM.update(loc, (temp - 254));
  }
  else {
    EEPROM.update(loc, (0));
  }


}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

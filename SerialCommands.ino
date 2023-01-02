
void processASerial(Stream &Serial) {
}
void processSerial(Stream &Serial) {
  if (Serial.available() > 0) {
    // read the 1st incoming byte:
    int incomingByte1 = Serial.read();
    int incomingByte2 = 0;
    //int incomingByte3 = 0;
    delay(100);
    if (Serial.available() > 0) {
      incomingByte2 = Serial.read();
    }

    // say what you got:
    switch (incomingByte1) {
      case 63:  //?
        {
          //Serial.println(F("E:End  s:GetStatus c:GetConfiguration t:temperatures  t[1,2,o]:1secondtemps,3secondtemps,off"));
          //Serial.println(F("F:Fan[S,i,I,d,D]:Start,Incr,Decr   R:Roast[S,A,R]:Start,AddMinute,RemMinute   P[i,I,d,D]:ProfileIncrDec]"));
          //Serial.println(F("GXX:SetGain   IXX:SetIntegral"));
          Serial.println("Sx-set by code x. S? shows option and current values.");
          break;
        }
      case 83:  //S
        {
          if (incomingByte2 == 63) {
            Serial.print(F("Saxxx TimeSubSecondDuration:"));  //a
            Serial.println(TimeSubSecondDuration);
            Serial.print(F("Sbxxx PIDIntegralUdateTimeValueTemp:"));  //b
            Serial.println(PIDIntegralUdateTimeValueTemp);
            Serial.print(F("Scxxx PIDIntegralUdateTimeValueFlow:"));  //c
            Serial.println(PIDIntegralUdateTimeValueFlow);
            Serial.print(F("Sdxxx PIDDutyWindow register  OCR1A:"));  //d
            Serial.println(OCR1A);
            Serial.print(F("Sexxx deltaYflow_avg._size:"));  //e
            Serial.print(deltaYflow_avg._size);
            Serial.print(" of ");
            Serial.println(deltaYflow_avg._sizemax);
            Serial.print(F("Sfxxx TimeReadThermoDuration:"));  //f
            Serial.println(TimeReadThermoDuration);
            Serial.print(F("Sgxxx PercentChangeFlow:"));  //g
            Serial.println(PercentChangeFlow);
            Serial.print(F("Shxxx TBeanAvgRoll._size:"));  //h
            Serial.print(TBeanAvgRoll._size);
            Serial.print(" of ");
            Serial.println(TBeanAvgRoll._sizemax);
            Serial.print(F("Sixxx _debugbyte (1 to 4):"));  //i
            Serial.println(_debugbyte);
            Serial.print(F("jxxx FanCoolingBoostPercent:"));  //j
            Serial.println(FanCoolingBoostPercent);
          } else {
            delay(100);
            if (Serial.available() > 0) {
              float n = Serial.parseFloat();
              switch (incomingByte2) {
                case 97:  //a TimeSubSecondDuration
                  {
                    Serial.print(F("Set TimeSubSecondDuration from"));
                    Serial.print(TimeSubSecondDuration);  //Serial.println();
                    TimeSubSecondDuration = n;
                    Serial.print(F(" to:"));
                    Serial.println(TimeSubSecondDuration);
                    break;
                  }
                case 98:  //b PIDIntegralUdateTimeValueTemp
                  {
                    Serial.print(F("Set PIDIntegralUdateTimeValueTemp from"));
                    Serial.print(PIDIntegralUdateTimeValueTemp);  //Serial.println();
                    PIDIntegralUdateTimeValueTemp = n;
                    Serial.print(F(" to:"));
                    Serial.println(PIDIntegralUdateTimeValueTemp);
                    break;
                  }
                case 99:  //c PIDIntegralUdateTimeValueFlow
                  {
                    Serial.print(F("Set PIDIntegralUdateTimeValueFlow from"));
                    Serial.print(PIDIntegralUdateTimeValueFlow);  //Serial.println();
                    PIDIntegralUdateTimeValueFlow = n;
                    Serial.print(F(" to:"));
                    Serial.println(PIDIntegralUdateTimeValueFlow);  //Serial.println();
                    break;
                  }

                case 100:  //d PIDDutyWindow register -  OCR1A
                  {
                    Serial.print(F("Set OCR1A from"));
                    Serial.print(OCR1A);  //Serial.println();
                    PIDDutyWindowSizeTemp = n;
                    Serial.print(F(" to:"));
                    Serial.println(OCR1A);  //Serial.println();
                    break;
                  }
                case 101:  //e deltaYflow_avg._size
                  {
                    Serial.print(F("Set flowAveraging._size from"));
                    Serial.print(deltaYflow_avg._size);  //Serial.println();
                    if (n > deltaYflow_avg._sizemax) { n = deltaYflow_avg._sizemax; }
                    deltaYflow_avg._size = n;
                    Serial.print(F(" to:"));
                    Serial.println(deltaYflow_avg._size);  //Serial.println();
                    break;
                  }
                case 102:  //f TimeReadThermoDuration
                  {
                    Serial.print(F("Set TimeReadThermoDuration from"));
                    Serial.print(TimeReadThermoDuration);  //Serial.println();
                    TimeReadThermoDuration = n;
                    Serial.print(F(" to:"));
                    Serial.println(TimeReadThermoDuration);  //Serial.println();
                    break;
                  }
                case 103:  //g PercentChangeFlow
                  {
                    Serial.print(F("Set PercentChangeFlow from"));
                    Serial.print(PercentChangeFlow);  //Serial.println();
                    PercentChangeFlow = n;
                    Serial.print(F(" to:"));
                    Serial.println(PercentChangeFlow);  //Serial.println();
                    break;
                  }
                case 104:  //h TBeanAvgRoll._size
                  {
                    Serial.print(F("Set TBeanAvgRoll from"));
                    Serial.print(TBeanAvgRoll._size);  //Serial.println();
                    if (n > TBeanAvgRoll._sizemax) { n = TBeanAvgRoll._sizemax; }
                    TBeanAvgRoll._size = n;
                    Serial.print(F(" to:"));
                    Serial.println(TBeanAvgRoll._size);  //Serial.println();
                    break;
                  }
                case 105:  //i _debugbyte
                  {
                    Serial.print(F("Set _debugbyte from"));
                    Serial.print(_debugbyte);  //Serial.println();
                    _debugbyte = n;
                    Serial.print(F(" to:"));
                    Serial.println(_debugbyte);  //Serial.println();
                    break;
                  }
                case 106:  // FanCoolingBoostPercent
                  {
                    Serial.print(F("Set FanCoolingBoostPercent from"));
                    Serial.print(FanCoolingBoostPercent);  //Serial.println();
                    FanCoolingBoostPercent = n;
                    Serial.print(F(" to:"));
                    Serial.println(FanCoolingBoostPercent);  //Serial.println();
                    break;
                  }
              }
            } else {
              Serial.println("No value found following the Sx string you typed");
            }
          }
          break;
        }
      case 1070:  //F
        {
          int v = 0;
          int b = FanSpeed254PWM;
          ;
          switch (incomingByte2) {
            case 83:  //S
              {
                Serial.print(F("Fan On "));
                ProcessHorControlMenu(2);
                Serial.println(FanSpeed254PWM);
                break;
              }
            case 68:  //D
              {
                v = -10;
                ProcessHorFanMenu(0);
                break;
              }

            case 100:  //d
              {
                v = -3;
                ProcessHorFanMenu(1);
                break;
              }
            case 105:  //i
              {
                v = 3;
                ProcessHorFanMenu(2);
                break;
              }
            case 73:  //I
              {
                v = 10;
                ProcessHorFanMenu(3);
                break;
              }
            default:
              {
              }
          }
          if (v != 0) {
            Serial.print(F("Fan "));
            Serial.print(v);
            Serial.print(F(" from "));
            Serial.print(b);
            Serial.print(F(" to "));
            Serial.println(FanSpeed254PWM);
          }
          break;
        }
      case 1080:  //P
        {
          Serial.print(F("Profile before:"));
          for (int x = 0; x < SetPointCount; x++) {
            Serial.print(x);
            Serial.print(F("-"));
            Serial.print(MySetPoints[x].Minutes);
            Serial.print(F("-"));
            Serial.print(MySetPoints[x].Temperature);
            Serial.print(F(":"));
          }
          Serial.println("");

          switch (incomingByte2) {
            case 68:  //D
              {
                Serial.println(F("Profile Decrease 5 "));
                ActiveAdjustment.moveamount = -5;
                //MoveLast3Point();
                saveChangedSetpoints();
                break;
              }
            case 100:  //d
              {
                Serial.println(F("Profile Decrease 3 "));
                ActiveAdjustment.moveamount = -3;
                // MoveLast3Point();
                saveChangedSetpoints();
                break;
              }
            case 105:  //i
              {
                Serial.println(F("Profile Increase 3 "));
                ActiveAdjustment.moveamount = 3;
                // MoveLast3Point();
                saveChangedSetpoints();

                break;
              }
            case 73:  //I
              {
                Serial.println(F("Profile Increase 5 "));
                ActiveAdjustment.moveamount = 5;
                // MoveLast3Point();
                saveChangedSetpoints();


                break;
              }
          }
          Serial.print(F("Profile  after:"));
          for (int x = 0; x < SetPointCount; x++) {
            Serial.print(x);
            Serial.print(F("-"));
            Serial.print(MySetPoints[x].Minutes);
            Serial.print("-");
            Serial.print(MySetPoints[x].Temperature);
            Serial.print(":");
          }
          Serial.println("");

          break;
        }
      case 1082:  //R
        {
          switch (incomingByte2) {
            case 83:  //S
              {
                Serial.println(F("Start Roast"));
                ProcessHorControlMenu(0);
                break;
              }
            case 65:  //A
              {
                Serial.println(F("Add a minute"));
                //ProcessBaseVMenu(7);
                break;
              }
            case 82:  //R
              {
                Serial.println(F("Remove a minute"));
                //ProcessBaseVMenu(6);
                break;
              }
          }
          break;
        }
      case 10100:  //d
        {
          _debug = incomingByte2;
          Serial.print("debug char recieved:");
          Serial.println(_debug);
          break;
        }
      case 1069:  //E
        {
          Serial.println(F("End Roast"));
          ProcessHorControlMenu(1);
          break;
        }
      case 10115:  //s
        {
          switch (incomingByte2) {
            case 49:  //1
              {
                serialOutPutStatusBySecond = !serialOutPutStatusBySecond;
                Serial.print(F("1secondStatus:"));
                Serial.println(serialOutPutStatusBySecond);

                serialOutPutStatusBy3Seconds = false;
                break;
              }
            case 51:  //3
              {
                serialOutPutStatusBy3Seconds = !serialOutPutStatusBy3Seconds;
                Serial.print(F("3secondStatus:"));
                Serial.println(serialOutPutStatusBySecond);

                serialOutPutTempsBySecond = false;
                break;
              }
            case 111:  //0
              {
                serialOutPutStatusBySecond = false;
                serialOutPutStatusBy3Seconds = false;
                Serial.println(F("Status by seconds off"));
                break;
              }
            default:
              {
                SerialOutputStatus();
                break;
              }
          }
          break;
        }
      case 1099:  //c
        {

          Serial.print(F("Profile"));
          for (int x = 0; x < SetPointCount; x++) {
            Serial.print(x);
            Serial.print("-");
            Serial.print(MySetPoints[x].Minutes);
            Serial.print("-");
            Serial.print(MySetPoints[x].Temperature);
            Serial.print(":");
          }
          Serial.println("");

          Serial.print(F("GTemp:"));
          Serial.print(GainTemp);
          Serial.print(F("Itemp:"));
          Serial.println(IntegralTemp);
          //Serial.print (F("FanDecrease DelayMin:"));Serial.println(FanSpeedPWNMinutesToA);
          //Serial.print (F("Decrease:"));Serial.print(FanSpeed254PWMAutoDecrease);Serial.print (F("DecreaseMinutes"));Serial.println(FanSpeedPWNDecreaseByMinutes);



          break;
        }
      case 10116:  //t
        {
          switch (incomingByte2) {
            case 49:  //1
              {
                serialOutPutTempsBySecond = !serialOutPutTempsBySecond;
                Serial.print(F("1secondtemps:"));
                Serial.println(serialOutPutTempsBySecond);

                serialOutPutTempsBy3Seconds = false;
                break;
              }
            case 51:  //3
              {
                serialOutPutTempsBy3Seconds = !serialOutPutTempsBy3Seconds;
                Serial.print(F("3secondtemps:"));
                Serial.println(serialOutPutTempsBySecond);

                serialOutPutTempsBySecond = false;
                break;
              }
            case 111:  //0
              {
                serialOutPutTempsBySecond = false;
                serialOutPutTempsBy3Seconds = false;
                Serial.println(F("Temps by seconds off"));
                break;
              }
            default:
              {
                serialOutPutTempsBySecond = false;
                serialOutPutTempsBy3Seconds = false;
                Serial.print(F("Bean1:"));
                Serial.print(TBean1);
                Serial.print(F(" Bean2:"));
                Serial.print(TBean2);
                Serial.print(F(" Avg:"));
                Serial.print(TBeanAvgRoll.mean());
                Serial.print(F(" Coil"));
                Serial.print(TCoil);
                Serial.print(F(" Avg"));
                Serial.print(TCoilRoll.mean());
                Serial.print(F(" Skipped:"));
                Serial.print(TempReadingskipped[1]);
                Serial.print(F("/"));
                Serial.print(TempReadingskipped[2]);
                Serial.print(F("/"));
                Serial.print(TempReadingskipped[0]);
                Serial.println();
              }

              break;
          }
          default:
            {

              Serial.print(F("I received : "));
              Serial.println(incomingByte1, DEC);
            }
        }
        incomingByte1 = 0;
    }
  }
}
void SerialOutputStatus() {
  Serial.print(F("Time:"));
  Serial.print(RoastMinutes);
  Serial.print(F("/"));
  Serial.print(MySetPoints[SetPointCount - 1].Minutes);
  Serial.print(F(" Bean Temp:"));
  Serial.print(TBeanAvgRoll.mean(), 1);
  Serial.print(F("/"));
  Serial.print(MySetPoints[SetPointCount - 1].Temperature);
  Serial.print(F(" Delta:-"));
  Serial.print(ErrTemp, 1);
  Serial.print(F(" Duty:"));
  Serial.print(DutyTemp, 2);
  Serial.print(F(" Coil Temp:"));
  Serial.print(TCoilRoll.mean(), 1);
  Serial.print(F(" FanPWM:"));
  Serial.print(FanSpeed254PWM);
  Serial.print(F("/254"));
  Serial.println();
}
void SerialOutputTempsForPlotting() {
  Serial.print(F("Bean1:"));
  Serial.print(TBean1);
  Serial.print(F(",Bean2:"));
  Serial.print(TBean2);
  Serial.print(F(",BeanAvg:"));
  Serial.print(TBeanAvgRoll.mean());
  Serial.print(F(",Coil:"));
  Serial.print(TCoil);
  Serial.print(F(",CoilAvg"));
  Serial.print(TCoilRoll.mean());
  Serial.println();
}

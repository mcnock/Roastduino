
void processASerial(Stream &Serial) {
}
void processSerial(Stream &Serial) {
  if (Serial.available() > 0) {
    // read the 1st incoming byte:
    int incomingByte1 = Serial.read();
    int incomingByte2 = 0;
    //int incomingByte3 = 0;
    delay(100);
    //if (Serial.available() > 0) {
    //   incomingByte2 = Serial.read();
    //}
    // say what you got:
    switch (incomingByte1) {
      case 63:  //? first char
        {
          //Serial.println(F("E:End  s:GetStatus c:GetConfiguration t:temperatures  t[1,2,o]:1secondtemps,3secondtemps,off"));
          //Serial.println(F("F:Fan[S,i,I,d,D]:Start,Incr,Decr   R:Roast[S,A,R]:Start,AddMinute,RemMinute   P[i,I,d,D]:ProfileIncrDec]"));
          //Serial.println(F("GXX:SetGain   IXX:SetIntegral"));
          Serial.println(F("Dx-send debug info by serial. D?"));
          Serial.println(F("Sx-set by code x. S? shows option and current values."));
          break;
        }
      case 83:  //S first char for Setting values
        {
          incomingByte2 = Serial.read();
          if (incomingByte2 == 63) {
            Serial.print(F("Saxxx TimeOpticalFlowDuration:"));  //a
            Serial.println(TimeOpticalFlowDuration);
            Serial.print(F("Sbxxx PIDIntegralUdateTimeValueTemp:"));  //b
            Serial.println(PIDIntegralUdateTimeValueTemp);
            Serial.print(F("Scxxx PIDIntegralUdateTimeValueFlow:"));  //c
            Serial.println(PIDIntegralUdateTimeValueFlow);
            Serial.print(F("Sdxxx PIDDutyWindow register  OCR1A:"));  //d
            Serial.println(OCR1A);
            Serial.print(F("Sexxx BeanYflow_avg._size:"));  //e
            Serial.print(BeanYflow_avg._size);
            Serial.print(F(" of "));
            Serial.println(BeanYflow_avg._sizemax);
            Serial.print(F("Sfxxx TimeReadThermoDuration:"));  //f
            Serial.println(TimeReadThermoDuration);
            Serial.print(F("Shxxx TBeanAvgRoll._size:"));  //h
            Serial.print(TBeanAvgRoll._size);
            Serial.print(F(" of "));
            Serial.println(TBeanAvgRoll._sizemax);
            Serial.print(F("Sixxx Debugbyte (1 to 4):"));  //i
            Serial.println(Debugbyte);
            Serial.print(F("Sjxxx FlowSensorMode (0,1,2):"));  //j
            Serial.println(FlowSensorMode);
          } else {
            delay(100);
            if (Serial.available() > 0) {
              float n = Serial.parseFloat();
              switch (incomingByte2) {
                case 97:  //a TimeOpticalFlowDuration
                  {
                    Serial.print(F("Set TimeOpticalFlowDuration from"));
                    Serial.print(TimeOpticalFlowDuration);  //Serial.println();
                    TimeOpticalFlowDuration = n;
                    Serial.print(F(" to:"));
                    Serial.println(TimeOpticalFlowDuration);
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
                case 101:  //e BeanYflow_avg._size
                  {
                    Serial.print(F("Set flowAveraging._size from"));
                    Serial.print(BeanYflow_avg._size);  //Serial.println();
                    if (n > BeanYflow_avg._sizemax) { n = BeanYflow_avg._sizemax; }
                    BeanYflow_avg._size = n;
                    Serial.print(F(" to:"));
                    Serial.println(BeanYflow_avg._size);  //Serial.println();
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
                case 103:  //g                   {
                  {
                    //Serial.print(F("Set MaxPercentChangePerSecondFlow from"));
                    //Serial.print(MaxPercentChangePerSecondFlow);  //Serial.println();
                    //MaxPercentChangePerSecondFlow = n;
                    //Serial.print(F(" to:"));
                    //Serial.println(MaxPercentChangePerSecondFlow);  //Serial.println();
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
                case 105:  //i Debugbyte
                  {
                    Serial.print(F("Set Debugbyte from"));
                    Serial.print(Debugbyte);  //Serial.println();
                    Debugbyte = n;
                    Serial.print(F(" to:"));
                    Serial.println(Debugbyte);  //Serial.println();
                    break;
                  }
                case (byte('j')):  //j FlowSensorMode
                  {
                    Serial.print(F("Set FlowSensorMode (0,1,2) from"));
                    Serial.print(FlowSensorMode);  //Serial.println();
                    FanCoolingBoostPercent = n;
                    Serial.print(F(" to:"));
                    Serial.println(FlowSensorMode);  //Serial.println();
                    break;
                  }
              }
            } else {
              Serial.println(F("No value found following the Sx string you typed"));
            }
          }
          break;
        }
      case 68:  //D first char
        {
          if (Serial.peek() == 63) {
            incomingByte2 = Serial.read();
          } else {
            incomingByte2 = Serial.parseInt();
          }

          switch (incomingByte2) {
            case 63:  // ?
              {
                Serial.println(F("FLOWPIDINFO_10 - show flow pid information per pid calculation"));
                Serial.println(F("FLOWSENSORDATARAW_11 - show flow data real time per reading"));
                Serial.println(F("FLOWSENSORDATASQRT_12 - show sqrt data real time per reading"));
                Serial.println(F("DRAWBOXESINFO_20 - show box redrawing information during a move"));
                Serial.println(F("TEMPPIDINFO_30 - show temp pid information per pid calculation"));
                Serial.println(F("TEMPDATARAW_31 - show temp data real time per reading"));
                Serial.println(F("CURRENTDATA_40 - show amperage data real time per reading"));
                break;
              }
            case 10:
            case 11:
            case 12:
            case 20:
            case 30:
            case 31:
            case 40:
              {
                if (Debugbyte != incomingByte2) {
                  Debugbyte = incomingByte2;
                } else {
                  Debugbyte = 0;
                }
                break;
              }
            default:
              {
                Serial.print(F("Value after D not recognized:"));
                Serial.println(incomingByte2);
                break;
              }
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
                // saveChangedSetpoints();
                break;
              }
            case 100:  //d
              {
                Serial.println(F("Profile Decrease 3 "));
                ActiveAdjustment.moveamount = -3;
                // MoveLast3Point();
                //saveChangedSetpoints();
                break;
              }
            case 105:  //i
              {
                Serial.println(F("Profile Increase 3 "));
                ActiveAdjustment.moveamount = 3;
                // MoveLast3Point();
                //saveChangedSetpoints();

                break;
              }
            case 73:  //I
              {
                Serial.println(F("Profile Increase 5 "));
                ActiveAdjustment.moveamount = 5;
                // MoveLast3Point();
                //saveChangedSetpoints();


                break;
              }
          }
          Serial.print(F("Profile  after:"));
          for (int x = 0; x < SetPointCount; x++) {
            Serial.print(x);
            Serial.print(F("-"));
            Serial.print(MySetPoints[x].Minutes);
            Serial.print(F("-"));
            Serial.print(MySetPoints[x].Temperature);
            Serial.print(F(":"));
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
          Serial.print(F("debug char recieved:"));
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
                SerialOutPutStatusBySecond = !SerialOutPutStatusBySecond;
                Serial.print(F("1secondStatus:"));
                Serial.println(SerialOutPutStatusBySecond);

                SerialOutPutStatusBy3Seconds = false;
                break;
              }
            case 51:  //3
              {
                SerialOutPutStatusBy3Seconds = !SerialOutPutStatusBy3Seconds;
                Serial.print(F("3secondStatus:"));
                Serial.println(SerialOutPutStatusBySecond);

                SerialOutPutTempsBySecond = false;
                break;
              }
            case 111:  //0
              {
                SerialOutPutStatusBySecond = false;
                SerialOutPutStatusBy3Seconds = false;
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
            Serial.print(F("-"));
            Serial.print(MySetPoints[x].Minutes);
            Serial.print(F("-"));
            Serial.print(MySetPoints[x].Temperature);
            Serial.print(F(":"));
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
                SerialOutPutTempsBySecond = !SerialOutPutTempsBySecond;
                Serial.print(F("1secondtemps:"));
                Serial.println(SerialOutPutTempsBySecond);

                SerialOutPutTempsBy3Seconds = false;
                break;
              }
            case 51:  //3
              {
                SerialOutPutTempsBy3Seconds = !SerialOutPutTempsBy3Seconds;
                Serial.print(F("3secondtemps:"));
                Serial.println(SerialOutPutTempsBySecond);

                SerialOutPutTempsBySecond = false;
                break;
              }
            case 111:  //0
              {
                SerialOutPutTempsBySecond = false;
                SerialOutPutTempsBy3Seconds = false;
                Serial.println(F("Temps by seconds off"));
                break;
              }
            default:
              {
                SerialOutPutTempsBySecond = false;
                SerialOutPutTempsBy3Seconds = false;
                Serial.print(F("Bean1:"));
                Serial.print(TBean1);
                Serial.print(F(" Bean2:"));
                Serial.print(TBean2);
                Serial.print(F(" Avg:"));
                Serial.print(TBeanAvgRoll.mean());
                Serial.print(F(" Coil"));
                Serial.print(TCoil);
                Serial.print(F(" Avg"));
                Serial.print(TCoilAvgRoll.mean());
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
  Serial.print(TCoilAvgRoll.mean(), 1);
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
  Serial.print(TCoilAvgRoll.mean());
  Serial.println();
}

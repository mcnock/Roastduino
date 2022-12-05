void processASerial(Stream &Serial)
{
  
}
void processSerial(Stream &Serial)

{
  if (Serial.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial.read();
    int incomingByte2 = 0;
    //int incomingByte3 = 0;
    delay(100);
    if (Serial.available() > 0) {
      incomingByte2 = Serial.read();

    }
    delay(100);
    
    if (Serial.available() > 0) {
      //int incomingByte3 = Serial.read();
      Serial.read();
    }
    // say what you got:
    switch (incomingByte) {
      case 63: //?
        {
          Serial.println(F("E:End  s:GetStatus c:GetConfiguration t:temperatures  t[1,2,o]:1secondtemps,3secondtemps,off"));
          Serial.println(F("F:Fan[S,i,I,d,D]:Start,Incr,Decr   R:Roast[S,A,R]:Start,AddMinute,RemMinute   P[i,I,d,D]:ProfileIncrDec]"));
          Serial.println(F("GXX:SetGain   IXX:SetIntegral"));

          break;
        }
      case 70: //F
        {
          int v = 0;
          int b = FanSpeedPWM;;
          switch (incomingByte2) {
            case 83: //S
              {
                Serial.print (F("Fan On "));
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
            default: {

              }
          }
          if (v != 0)
          {
            Serial.print(F("Fan "));
            Serial.print(v);
            Serial.print (F(" from ") );
            Serial.print(b );
            Serial.print (F(" to "));
            Serial.println (FanSpeedPWM);

          }
          break;
        }
      case 80: //P
        {
          Serial.print(F("Profile before:"));
          for (int x = 0; x < SetPointCount; x++) {
            Serial.print(x);
            Serial.print(F("-"));
            Serial.print( MySetPoints[x].Minutes);
            Serial.print(F("-"));
            Serial.print (MySetPoints[x].Temperature);
            Serial.print(F(":"));
          }
          Serial.println("");

          switch (incomingByte2) {
            case 68://D
              { Serial.println(F("Profile Decrease 5 "));
                moveamount = -5;          MoveLast3Point();
                saveChangedSetpoints();
                break;
              }
            case 100:  //d
              { Serial.println(F("Profile Decrease 3 "));
                moveamount = -3;
                MoveLast3Point();
                saveChangedSetpoints();

                break;
              }
            case 105:  //i
              { Serial.println(F("Profile Increase 3 "));
                moveamount = 3;
                MoveLast3Point();
                saveChangedSetpoints();

                break;
              }
            case 73: //I
              { Serial.println(F("Profile Increase 5 "));
                moveamount = 5;
                MoveLast3Point();
                saveChangedSetpoints();


                break;
              }
          }
          Serial.print(F("Profile  after:"));
          for (int x = 0; x < SetPointCount; x++) {
            Serial.print(x);
            Serial.print(F("-"));
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
          switch (incomingByte2) {
            case 83: //S
              {
                Serial.println(F("Start Roast"));
                ProcessHorControlMenu(0);
                break;

              }
            case 65: //A
              { Serial.println(F("Add a minute"));
                ProcessBaseVMenu(7);
                break;
              }
            case 82: //R
              { Serial.println(F("Remove a minute"));
                ProcessBaseVMenu(6);
                break;
              }

          }
          break;
        }
      case 100: //d
        {
            _debug = incomingByte2;
            Serial.print("debug char recieved:");Serial.println(_debug);
            break;
        }
      case 69: //E
        {
          Serial.println(F("End Roast"));
          ProcessHorControlMenu(1);
          break;
        }
      case 115: //s
        {
          switch (incomingByte2) {
            case 49: //1
              {
                serialOutPutStatusBySecond = !serialOutPutStatusBySecond;
                Serial.print (F("1secondStatus:"));Serial.println(serialOutPutStatusBySecond);

                serialOutPutStatusBy3Seconds = false;
                break;

              }
            case 51: //3
              {
                serialOutPutStatusBy3Seconds = !serialOutPutStatusBy3Seconds;
                Serial.print (F("3secondStatus:"));Serial.println(serialOutPutStatusBySecond);

                serialOutPutTempsBySecond = false;
                break;

              }
            case 111: //0
              {
                serialOutPutStatusBySecond = false;
                serialOutPutStatusBy3Seconds = false;
                Serial.println(F("Status by seconds off"));
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

          Serial.print(F("Profile"));
          for (int x = 0; x < SetPointCount; x++) {
            Serial.print(x);
            Serial.print("-");
            Serial.print( MySetPoints[x].Minutes);
            Serial.print("-");
            Serial.print (MySetPoints[x].Temperature);
            Serial.print(":");
          }
          Serial.println("");

          Serial.print (F("GTemp:"));Serial.print(GainTemp);Serial.print (F("Itemp:"));Serial.println(IntegralTemp);
          //Serial.print (F("FanDecrease DelayMin:"));Serial.println(FanSpeedPWNMinutesToA);
          //Serial.print (F("Decrease:"));Serial.print(FanSpeedPWMAutoDecrease);Serial.print (F("DecreaseMinutes"));Serial.println(FanSpeedPWNDecreaseByMinutes);



          break;

        }
      case 116: //t
        {
          switch (incomingByte2) {
            case 49: //1
              {
                serialOutPutTempsBySecond = !serialOutPutTempsBySecond;
                Serial.print (F("1secondtemps:"));Serial.println(serialOutPutTempsBySecond);

                serialOutPutTempsBy3Seconds = false;
                break;

              }
            case 51: //3
              {
                serialOutPutTempsBy3Seconds = !serialOutPutTempsBy3Seconds;
                Serial.print (F("3secondtemps:"));Serial.println(serialOutPutTempsBySecond);

                serialOutPutTempsBySecond = false;
                break;

              }
            case 111: //0
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
            Serial.println(incomingByte, DEC);

          }


        }
        incomingByte = 0;
    }
  }
}
void SerialOutputStatus()
{

  Serial.print(F("Time:"));
  Serial.print(RoastMinutes);
  Serial.print(F("/"));
  Serial.print(MySetPoints[SetPointCount - 1].Minutes);
  Serial.print(F(" Bean Temp:"));
  Serial.print(TBeanAvgRoll.mean(), 1);
  Serial.print(F("/"));
  Serial.print(MySetPoints[SetPointCount - 1].Temperature);
  Serial.print( F(" Delta:-"));
  Serial.print(ErrTemp, 1);
  Serial.print(F(" Duty:"));
  Serial.print(DutyTemp, 2);
  Serial.print(F(" Coil Temp:"));
  Serial.print(TCoilRoll.mean(), 1);
  Serial.print(F(" FanPWM:"));
  Serial.print(FanSpeedPWM);
  Serial.print(F("/254"));

  Serial.println();

}
void SerialOutputTempsForPlotting()
{
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

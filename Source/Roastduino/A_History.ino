
void savestring(int address, char string[])
{

  for (int S = 1; S < 50; S++) {
    address = (S * 100) + 500; byte F;
    F =  EEPROM.read(address);
  //Serial.println(F);
    if (F != 1) {
      break;
    }
  }
}


void loadstring(int address, char name[])
{
  for (int S = 1; S < strlen(name); S++) {
    address = (address + 1); byte F;
    F =  EEPROM.read(address);
  //Serial.println(F);
    if (F != 1) {
      break;
    }
  }
}
void Load(int &number)
{
  LoadORSaveToHistory(true, number);
}
void Save(int &number)
{
 LoadORSaveToHistory(false, number) ; 
}
void LoadORSaveToHistory(boolean Load, int &number) {

  int address = (number + 2) * 100;
  //300 to 311
  if (!Load) {
    EEPROM.put(address, number);
    address = address + 1;
    //savestring(address, name);
  }
  else {
    RoastNumber = EEPROM.read(address);
    address = address + 1;
    //loadstring(address, name);
  }

  //312 - 318
  address = ((number * 2) * 100) + 12;
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



}


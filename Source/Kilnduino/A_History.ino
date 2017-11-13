
void HistoryDelete() {
  Serial.println ("Delete History");
  int istart = WebBuf.indexOf("HISTORYDELETE") + 14;
  while (WebBuf.indexOf(" ", istart) == -1) { //wait till we get 10 more
    Serial.println("looking history...");
    Serial.println(WebBuf);
    while (Serial2.available()) {
      char r = Serial2.read();
      WebBuf.concat(r);
    }
  }
  WebBuf.replace("%2C", ",");
  istart++;
  istart++;
  int iend = WebBuf.indexOf(" ", istart);

  //Serial.println(WebBuf.substring(istart));
  int address1;
  do {
    int iComma = WebBuf.indexOf(",", istart);
    byte b = WebBuf.substring(istart, iComma).toInt();
    if (b > 0)
    {
      //Serial.print("deleting:"); Serial.println(b);
      address1 = (b * 100) + 500;
      EEPROM.write(address1, 0);

    }

    istart = iComma + 1;
  } while (istart > 0);

  int address2;
  //shift up

  byte availableslots[50];
  for (byte x = 0; x < 50; x++) {
    availableslots[x] = 0;
  }

  for (byte S = 1; S < 50; S++) {
    address1 = (S * 100) + 500; byte f;
    f = EEPROM.read(address1);
    if (f != 1) {
      AddByte(availableslots, S);
    }
    else {
      if (availableslots[0] > 0)
      {
        byte S2 = GetNextByte(availableslots);
        address2 = S2 * 100 ;
        //Serial.print("moving"); Serial.print(S) ; Serial.print(" to "); Serial.println(S2);
        for (int k = 0; k < 100; k++) {

          EEPROM.write(address2, EEPROM.read(address1));
          if (k == 0) {
            EEPROM.write(address1, 0);

            EEPROM.write(address2, 1);
          }
          address1++;
          address2++;
        }
        AddByte(availableslots, S);
      }
      else
      {
        //Serial.println("OK");

      }
    }
  }
}

byte GetNextByte(byte *Array) {
  byte r;
  r = Array[1];
  //Serial.print ("Get"); Serial.print(" - "); Serial.println (r);
  Array[0]--;
  //shuffle down
  for (int i = 1; i < 49 ; i++ ) {
    Array[i] = Array[i + 1];
  }
  return r;
}

void AddByte(byte *Array, const byte value) {
  Array[0]++;
  for (int i = 1; i < 50 ; i++ ) {
    if (Array[i] == 0)
    {
      // Serial.print("adding"); Serial.print(i); Serial.print(" - "); Serial.println (value);
      Array[i] = value;
      return i;
    }
  }

}

void ProcessHistoryRequest(int connectionId) {

  if (WebBuf.indexOf("HISTORYD") > 0) {
    HistoryDelete();
  }
  if (WebBuf.indexOf("HISTORYS") > 0) {
    Serial.println("save history");
    HistorySave();
  }

  int address;

  //set up page
  strcpy(WebPage, HTMLHeader);
  strcat(WebPage, "<body><style>input {margin:3px}</style>");
  strcat(WebPage, "<div style=font-size:large>Nock Kiln - History</div><table border=1  style=\"border-collapse:collapse;\">");
  strcat(WebPage, "<form action=\"OPTIONS\"><input type=\"submit\" value=\"Back\"></form>");
  strcat(WebPage, "<tr><td>ID<td>StartTime<td>MaxTime<td>MaxTemp<td>Description</tr>" );
  for (int X = 1; X < 50; X++) {
    address = (X * 100) + 500; byte x;
    x =  EEPROM.read(address);
    if (x != 1) {
      Serial.print("skipping"); Serial.println(address);
      break;
    }
    address++;
    Serial.print("found"); Serial.println(address);
    //start a row

    sprintf(WebPage + strlen(WebPage), "<tr><td>%i", X);
    //strcat(WebPage, "<tr>");

    int i;
    i = 0;
    char History[100];
    //Serial.println(History);
    for (int z = address; z < (address + 99); z++) {
      char p = EEPROM.read(z);
      if (p != '/0'   )
      {
        //Serial.print(z);   Serial.print(p) ;Serial.println((byte)p);
        History[i] = p;
      }
      else
      {
        Serial.println("null found");
      }
      i++;
    }

    //end row
    Serial.println(History);
    strcat(WebPage, History);
    strcat(WebPage, "</tr>");
  }
  strcat(WebPage, "</table><br><form action=\"HISTORYDELETE\"> Enter numbers to delete.<br>Example: 1,4,5,6,7");
  strcat(WebPage, "<br><br><input type=\"text\" name=\"d\" value=\"\"><br><input type=\"submit\" value=\"Delete above numbers\"></form>");
  strcat(WebPage, "</body></html>");
  SendWebPage(connectionId, true);
}

void HistorySave() {
  int address;
  for (int S = 1; S < 50; S++) {
    address = (S * 100) + 500; byte F;
    F =  EEPROM.read(address);
    Serial.println(F);
    if (F != 1) {
      break;
    }
  }
  Serial.print("save to:"); Serial.println(address);
  EEPROM.write(address, 1);
  address++;
  char History[100];
  int i = sprintf(History, "<td>%s<td>%02i:%02i<td>%4i<td>%s", StartTime16, hour(TimeLMax), minute(TimeLMax), TempLMax, RunDescription);
  Serial.println(History);
  EEPROM.put(address, History);
}


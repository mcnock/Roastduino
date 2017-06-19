
#include "MAX31856.h"
#include "Time.h"
#include "TimeLib.h"
#include <EEPROM.h>

//#define MAXDO   2
//#define MAXCS   3
//#define MAXCLK  4
//#define MAXDO2   6
//#define MAXCS2   7
//#define MAXCLK2  8


#define SCK    3
#define CS0    8
#define CS1    7
#define CS2    6
#define CS3    5
#define SDI    4
#define SDO    9

#define CR0_INIT  (CR0_AUTOMATIC_CONVERSION + CR0_OPEN_CIRCUIT_FAULT_TYPE_K /* + CR0_NOISE_FILTER_50HZ */)
#define CR1_INIT  (CR1_AVERAGE_2_SAMPLES + CR1_THERMOCOUPLE_TYPE_K)
#define MASK_INIT (~(MASK_VOLTAGE_UNDER_OVER_FAULT + MASK_THERMOCOUPLE_OPEN_FAULT))

#define NUM_MAX31856   2

// Create the temperature object, defining the pins used for communication
MAX31856 *TemperatureSensor[NUM_MAX31856] = {
  new MAX31856(SDI, SDO, CS0, SCK),
  new MAX31856(SDI, SDO, CS1, SCK)
  //  new MAX31856(SDI, SDO, CS2, SCK),
  //  new MAX31856(SDI, SDO, CS3, SCK)
};


// initialize the Thermocouple
//Adafruit_MAX31855 therm1(MAXCLK, MAXCS, MAXDO);
//Adafruit_MAX31855 therm2(MAXCLK2, MAXCS2, MAXDO2);


//#include <SoftwareSerial.h>
//use mega Serial 2 for serial monitor; Serial 1 on pins 19 (RX) and 18 (TX);// Serial2 on pins 17 (RX) and 16 (TX), Serial3 on pins 15 (RX) and 14 (TX).

#define DEBUG true

int Mode = 0;
String WebBuf = "";

//<link rel="icon" href="data:;base64,=">

//const char HTMLHeader[] = "<html><head><link href=\"http://www.example.com/favicon.ico\"/><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
const char HTMLHeader[] = "<html><head><link rel=\"shortcut icon\" href=\"data:;base64,=\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>";
// const char HTMLHeader[] = "<html><head><link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"></head>"
unsigned int TempLMax = 0;
unsigned long TimeLMax;
boolean TempLMaxLogged;

unsigned int TempHMax = 0;
unsigned long TimeHMax;
boolean TempHMaxLogged;

unsigned long StartTime;
char StartTime16[17];
unsigned long RunTime;
unsigned int StartTemp = 0;
unsigned int TempsL[48];
unsigned int TempsH[48];
unsigned long Times[48];
int TempInterval = 20;
int LogCount = 48;

unsigned int TempL;
unsigned int TempH;

int lastloggedminute = -1;
int logindex = 0;
//String webpage = "";
String hwebpage = "";
char RunDescription[30];
char History[50];
char WebPage[2000];
char WebPage30[30];
boolean Stopped = true;
void setup() {
  Serial2.begin(115200);//9600 (mine), 57600, 115200
  Serial.begin(115200);

  // test();
  // delay(100000);
  connectWiFi();
  Mode = 0;
  StartTime = 0;

  // Initializing the MAX31855's registers
  for (int i = 0; i < NUM_MAX31856; i++) {
    //Serial.println(i);
    TemperatureSensor[i]->writeRegister(REGISTER_CR0, CR0_INIT);
    TemperatureSensor[i]->writeRegister(REGISTER_CR1, CR1_INIT);
    TemperatureSensor[i]->writeRegister(REGISTER_MASK, MASK_INIT);
  }

  delay(1000);

  CurrentRunRead();
}

void test() {
  for (int X = 0; X < LogCount; X++) {
    TempsL[X] = X * 1000;
    TempsH[X] = (X * 1000) + 1;
    Times[X] = 1000000 *   X ;
    Serial.println(Times[X]);
  }
  logindex = LogCount;
  ProcessKilnWebRequest(0);
}

void loop() {
  delay(1000);
  if (StartTime > 0) {
    RunTime = now() - StartTime;
  }

  TempL = TemperatureSensor[0]->readThermocouple(FAHRENHEIT);
  TempH = TemperatureSensor[1]->readThermocouple(FAHRENHEIT);

  //error is 10001 - but really anything over 2500 (the limit for K) is an error

  if (Stopped == false) {

    if (TempL < 3000 && TempL > TempLMax) {
      TempLMax = TempL;
      TimeLMax = RunTime;
      TempLMaxLogged == false;
    }
    else if (TempL < TempLMax) {
      if (TempLMaxLogged == false) {
        Serial.println("Log time 1 max");
        EEPROM.put(8, TimeLMax);
        EEPROM.put(16, TempLMax);
        TempLMaxLogged = true;
      }
    }

    if (TempH < 3000 && TempH > TempHMax) {
      TempHMax = TempH;
      TimeHMax = RunTime;
      TempHMaxLogged = false;
    }
    else if (TempH < TempHMax ) {
      if (TempHMaxLogged == false) {
        Serial.println("Log time 2 max");
        EEPROM.put(12, TimeHMax);
        EEPROM.put(18, TempHMax);
        TempHMaxLogged = true;
      }
    }
    
    
    boolean logintervalfound = false;
    //4 x 8 hours = 32
    if (logindex < 32) {
      if (minute(RunTime) % TempInterval == 0) {
          logintervalfound = true;
      }
    }
    else if (logindex < LogCount){
       if (minute(RunTime) % 60 == 0) {
          logintervalfound = true;
      }
    }


   // if (minute(RunTime) % TempInterval == 0)
    if (logintervalfound){
      if (lastloggedminute != minute(RunTime)) {
        TempsL[logindex] = TempL;
        TempsH[logindex] = TempH;
        Times[logindex] = (RunTime);
        int mem = 22 + (logindex * 8);
        lastloggedminute = minute(RunTime);

        EEPROM.put(1, logindex);
        EEPROM.put(mem, TempsL[logindex]);
        EEPROM.put(mem + 2, TempsH[logindex]);
        EEPROM.put(mem + 4, Times[logindex]);
        EEPROM.put(20, lastloggedminute);
        logindex++;
        if (logindex == LogCount)
        {
          Stopped = true;
        }
      }
    }

  }

  sendSerial1ToSerial2();

  if (Mode == 0) { //this is looking for only for kiln web requests from esp8266
    if (Serial2.find("+")) {
      while (Serial2.available() < 4) { } //wait till we get 4 more
      if (Serial2.find("IPD,")) {
        unsigned long startwebrequest = millis();
        WebBuf = "";
        while (Serial2.available() < 30) { } //wait till we get 10 more
        int connectionId = Serial2.read() - 48; // subtract 48 because the read() function returns
        while (Serial2.available()) {
          char r = Serial2.read();
          WebBuf.concat(r);
        }
        Serial.print("found +IPD: "); Serial.print(connectionId); Serial.println(WebBuf);

        if (WebBuf.indexOf("OPTIONS") > 0) {
          ProcessOptionsRequest(connectionId);
        }
        else if (WebBuf.indexOf("HISTORY") > 0) {
          ProcessHistoryRequest(connectionId);
        }
        else if (WebBuf.indexOf("KILN") > 0) {
          ProcessKilnWebRequest(connectionId);
        }
        else  {
          Serial.println("other..");
          //sprintf(WebPage30, "AT+CIPCLOSE=%i", connectionId);
          //sendData(WebPage30);
        }
        unsigned long endwebrequest = millis();
        Serial.print("Web request mill seconds:"); Serial.println ((endwebrequest - startwebrequest));
      }


    }

  }

  if (Mode == 9) { //output anything
    while (Serial2.available()) {
      Serial.write(Serial2.read());
    }
  }
}

void CurrentRunRead() {
  EEPROM.get(0, Stopped);
  //if (Stopped == false) {
  EEPROM.get(1, logindex);
  Serial.print("reading cur:"); Serial.println(logindex);

  //3 available
  EEPROM.get(4, StartTime);
  Serial.println(StartTime);
  FormatDateTime16(StartTime, StartTime16);
  Serial.println(StartTime16);
  EEPROM.get(8, TimeLMax);
  EEPROM.get(12, TimeHMax);
  EEPROM.get(16, TempLMax);
  //Serial.println(Time1Max);
  //Serial.println(Time2Max);
  EEPROM.get(18, TempHMax);
  EEPROM.get(20, lastloggedminute);
  for (int S = 0  ; S < logindex; S++) {
    int mem = 22 + (S * 8);
    EEPROM.get(mem, TempsL[S]);
    EEPROM.get(mem + 2, TempsH[S]);
    EEPROM.get(mem + 4, Times[S]);
  }
  //22 + 48 x 8
  //406

  EEPROM.get(410, RunDescription);

  //ends 450 - say 500
  //}
}

void CurrentRunSaveClearedValues() {
  Serial.print("Saving..");
  EEPROM.put(0, Stopped);
  EEPROM.put(1, logindex);
  EEPROM.put(4, StartTime);
  Serial.println(StartTime);
  EEPROM.put(8, TimeLMax);
  EEPROM.put(12, TimeHMax);
  EEPROM.put(16, TempLMax);
  EEPROM.put(18, TempHMax);
  EEPROM.put(20, lastloggedminute);

}

void SendWebPage(int connectionId, boolean close) {
  strcat(WebPage, "\r\n");
  Serial.println(strlen(WebPage));
  sprintf(WebPage30, "AT+CIPSEND=%i,%i", connectionId, strlen(WebPage));
  //sprintf(WebPage30, "AT+CIPSEND=%i", connectionId, strlen(WebPage));
  sendData2(WebPage30);
  //Serial.println(WebPage);

  sendData2(WebPage);
  //strcpy(WebPage,'\0');
  if (close) {
    //delay(1000);

    //sprintf(WebPage30, "AT+CIPCLOSE=%i", connectionId);
    //sprintf(WebPage30, "AT+CIPCLOSE");
    // sendData2(WebPage30);

  }

}

void KilnReset() {
  Serial.println ("RESTART");
  Stopped = false;
  StartTime = now();
  FormatDateTime16(StartTime, StartTime16);
  for (int X = 0; X < LogCount; X++) {
    Times[X] = 0;
    TempsL[X] = 0;
    TempsH[X] = 0;
  }
  TempLMax = 0;
  TimeLMax = 0;
  TempHMax = 0;
  TimeHMax = 0;
  TempsL[0] = TempL;
  TempsH[0] = TempH;
  Times[0] = 0;
  RunTime = 0;
  logindex = 0;
  CurrentRunSaveClearedValues() ;
}

void SetKilnDesc() {
  Serial.println ("setdesc");
  int istart = WebBuf.indexOf("KILNSETDESC") + 14;
  unsigned long start = millis();
  while ((millis() - start) < 5000 && WebBuf.indexOf(" ", istart) == -1 && WebBuf.indexOf(",", istart) == -1 ) { //wait till we get 10 more
    Serial.println("looking kiln desc...");
    Serial.println(WebBuf);
    while (Serial2.available()) {
      char r = Serial2.read();
      WebBuf.concat(r);
    }
  }
  Serial.println(WebBuf);
  WebBuf.replace("%2C", ",");
  WebBuf.replace("%23", "#");
  int  iend = WebBuf.indexOf(" ", istart);
  if (istart > iend) {
    iend = WebBuf.length();
  }
  if (istart > iend) {
    iend = WebBuf.indexOf(",C", istart) - 1;

  }
  WebBuf.replace("+", " ");
  int ilen = (iend - istart + 1);
  if (ilen > 30) {
    ilen = 30;
  }
  Serial.println(ilen);
  WebBuf.substring(istart).toCharArray(RunDescription, ilen);
  EEPROM.put(410, RunDescription);
  Serial.println (RunDescription);


}

void ProcessOptionsRequest(int connectionId) {
  //set up page
  Serial.println("ProcessOptionsRequest");
  strcpy(WebPage, HTMLHeader);
  strcat(WebPage, "<body><style>input {margin:3px}</style>");
  strcat(WebPage, "<div style=font-size:large>Nock Kiln - Options</div>");
  strcat(WebPage, "<br>StartTime:"); strcat(WebPage, StartTime16);
  strcat(WebPage, "<br><form action=\"KILNSETDESC\"><input type=\"submit\" value=\"Submit Description\">");
  strcat(WebPage, "<br>Include cone, rate,  No spaces");
  strcat(WebPage, "<br><input type=\"text\" name=\"d\" value=\"");
  strcat(WebPage, RunDescription); strcat(WebPage, "\"><br></form>");
  strcat(WebPage, "<form action=\"KILNSTART\"><input type=\"submit\" value=\"Start logging\"></form>");
  strcat(WebPage, "<form action=\"KILNSTOP\"><input type=\"submit\" value=\"Stop logging\"></form>");
  strcat(WebPage, "<form action=\"HISTORYSAVEKILN\"><input type=\"submit\" value=\"Save\"></form>");
  strcat(WebPage, "<form action=\"HISTORY\"><input type=\"submit\" value=\"PriorRuns\"></form>");
  strcat(WebPage, "<form action=\"KILN\"><input type=\"submit\" value=\"Back\"></form>");
  strcat(WebPage, "<div style=\"clear:both;\"></div>");
  // strcat(WebPage,  Current Time:"); webpage.concat( month(now())); webpage.concat("/"); webpage.concat(day(now())); webpage.concat( " "); webpage.concat( hour(now())); webpage.concat( ":"); webpage.concat( minute(now()));
  strcat(WebPage, "<br>ConeRef: 06~1850;  05~1900;  04~1950;  4~2140;  5~2190");
  strcat(WebPage, "</body></html>");
  //Serial.println(WebPage);
  SendWebPage(connectionId, true);
}

bool ProcessKilnWebRequest(int connectionId) {
  Serial.println ("");
  Serial.print ("Kiln web request ");
  Serial.print("connectionId:"); Serial.println(connectionId);
  if (WebBuf.indexOf("KILNSTART") > 0) {
    KilnReset();
  }
  if (WebBuf.indexOf("KILNSETDESC") > 0) {
    SetKilnDesc();
  }
  if (WebBuf.indexOf("KILNSTOP") > 0) {
    Stopped = true;
  }
  strcpy(WebPage, HTMLHeader);
  strcat(WebPage, "<body><style>td {text-align:right} .l {text-align:left} input {float:left; margin:2px}</style>");
  if (Stopped == true) {
    strcat(WebPage, "Nock Kiln Temp  -- Stopped");
  }
  else {
    strcat(WebPage, "Nock Kiln Temp  -- Logging");

  }
  if (strlen(RunDescription) > 0) strcat(WebPage, "<br>");  strcat(WebPage, RunDescription);
  strcat(WebPage, "<div><form action=\"KILN\"><input type=\"submit\" value=\"Refresh\"></form>");
  strcat(WebPage, "<form action=\"OPTIONS\"><input type=\"submit\" value=\"Options\"></form>");
  //if (Stopped == true) {webpage.concat("<form action=\"KILNSTART\"><input type=\"submit\" value=\"Start logging\"></form>");}
  strcat(WebPage, "</div><div style=\"clear:both;\"></div>");
  strcat(WebPage,"Start Time:"); strcat(WebPage,StartTime16);

  strcat(WebPage, "<br><table style=font-size:large border=1>");
  strcat(WebPage, "<tr><td class='l'>Time<td>TempH F<td>TempL F<td>Note");
  boolean bMaxLAdded = false;
  boolean bMaxHAdded = false;

  //first row to table is always the current values
  addRow(-2, -2, RunTime, TempL, TempH);



  for (int X = 0; X < logindex; X ++) {
    if (X == 0 && logindex <= 3 ) {
      //addRow(0, 0, StartTime, TempsL[X], TempsH[X]);
      addRow(0, 0, Times[X], TempsL[X], TempsH[X]);
    }
    else if (X > 0) {
      if (bMaxLAdded == false && Times[X] > TimeLMax && TempsL[X] < TempLMax ) {
        addRow(-1, -1, TimeLMax, TempLMax, 0);
        bMaxLAdded = true;
      }
      if (bMaxHAdded == false && Times[X] > TimeHMax && TempsH[X] < TempHMax ) {
        addRow(-1, -1, TimeHMax, 0, TempHMax);
        bMaxHAdded = true;
      }
      //always log the last 3
      if (((logindex - X) <= 3) || minute(Times[X]) == 0) {
        addRow(X, 1, Times[X], TempsL[X], TempsH[X]);
      }
    }
  }
  strcat(WebPage, "</table></body></html>");
  // strcat(WebPage, "size");
  double len = strlen(WebPage);
  //Serial.println(len);
  SendWebPage(connectionId, true);
}

boolean addRow(int logindex, int flag, unsigned long Time, unsigned int TempL, unsigned int TempH) {
  //Serial.print("add row"); Serial.print(logindex); Serial.print("  "); Serial.println(strlen(WebPage));
  sprintf(WebPage + strlen(WebPage), "<tr><td class='l'>%02i:%02i", hour(Time), minute(Time));
  //-1 = Max, -2  = now, 0 = Start, 1 = normal-check for rate
  if (flag == -1 or flag == -2) {
    sprintf(WebPage + strlen(WebPage), ":%02i", second(Time));
  }

  sprintf(WebPage + strlen(WebPage), "</td><td>%i<td>%i<td>", TempL, TempH);

  switch (flag) {
    case -1:
      strcat(WebPage, " Max");
      break;
    case -2:
      strcat(WebPage, " Now");
      break;
    case 0:
      strcat(WebPage, " Start");
      break;
    default:
      int rate = Rate(logindex);
      sprintf(WebPage + strlen(WebPage), "%i F/hr", rate);
      break;
  }

}
int AvgTemps(int X) {
  int t;
  if (TempsH[X] < 3000) {
    if (TempsL[X] < 3000) {
      return (TempsL[X] + TempsH[X]) / 2;
    }
    else {
      return TempsH[X];
    }
  }
  else if (TempsL[X] < 3000) {
    return TempsL[X];
  }
  return 0;
}

int  Rate(int logindex) {
  if (logindex < 1) {
    return 0;
  }
  double avgX = AvgTemps(logindex);
  double avgXPrev = AvgTemps(logindex - 1);
  if (avgX > 0 and avgXPrev > 0) {
    int r = (int)((avgX - avgXPrev) / TempInterval) * 60;
    return r;
  }
  else {
    return 0;
  }


}

boolean connectWiFi() {
  sendDataWaitForResponse("ATE0", "OK");
  sendDataWaitForResponse("AT+CWMODE=3", "OK") ;
  sendDataWaitForResponse("AT+CWJAP=\"NETGEARMcnock\",\"19581958\"", "OK");
  //sendDataWaitForResponse("AT+CWJAP=\"G3 ajnock\",\"19581958\"", "OK");

  Serial.println("  Connected to WiFi!!");
  delay(3000);
  sendDataWaitForResponse("AT+CIFSR", "OK");
  sendDataWaitForResponse("AT+CIPMUX=1", "OK");//0 is single 1 is mulitple
  sendDataWaitForResponse("AT+CIPSERVER=1,80", "OK");
  sendDataWaitForResponse("AT+CIPSTART=0,\"TCP\",\"example.com\",80", "OK");

  sendDataWaitForResponse("AT+CIPSEND=0,20", "OK");


  sendDataWaitForResponse("GET / HTTP/1.1 \\r\\n\\r\\n", "</html>");

  //parse date
  int iStart = WebBuf.indexOf("Date:");
  if (iStart > 0) {
    iStart = WebBuf.indexOf(",", iStart) + 2;
    int iEnd = WebBuf.indexOf(" ", iStart);
    String dy = WebBuf.substring(iStart, iEnd);
    iStart = iEnd + 1;
    iEnd = WebBuf.indexOf(" ", iStart);
    String mh = WebBuf.substring(iStart, iEnd);
    iStart = iEnd + 1;
    iEnd = WebBuf.indexOf(" ", iStart);
    String yr = WebBuf.substring(iStart, iEnd);
    iStart = iEnd + 1;
    iEnd = WebBuf.indexOf(":", iStart);
    String hr = WebBuf.substring(iStart, iEnd);
    iStart = iEnd + 1;
    iEnd = WebBuf.indexOf(":", iStart);
    String mn = WebBuf.substring(iStart, iEnd);
    Serial.print(mh);    Serial.print("-"); Serial.print(dy); Serial.print("-"); Serial.print(yr); Serial.print(" "); Serial.print(hr); Serial.print(":"); Serial.println(mn);
    setTime(hr.toInt(), mn.toInt(), 0, dy.toInt(), MonthToInt(mh), yr.toInt());
    unsigned long secperhour = 60 * 60 * 8;
    adjustTime( - (secperhour) );
    Serial.println("adjusted:");
    Serial.print(month()); Serial.print("-");  Serial.print(day()); Serial.print("-"); Serial.print(year()); Serial.print (" "); Serial.print(hour()); Serial.print(":"); Serial.println(minute());
  }
  sendDataWaitForResponse("AT+CIPSTO=3", "OK");

  Serial.println(now());
  Serial.println("Connect WIFI done!");
  return true;
}

void sendSerial1ToSerial2() {
  while (Serial.available()) {
    char r = Serial.read();
    char x = ';';
    //Serial.println(r);
    //Serial.println(x);
    if (x == r) {
      Serial.println("new command");

      Serial2.println();
      delay(2000);
    }
    else
    {
      //    Serial.println("send");
      Serial2.print(r);
    }
  }
}

boolean sendDataWaitForResponse(const String& command, const String& response) {
  if (DEBUG) {
    Serial.print ("sending:") ; Serial.println(command);
    Serial.print ("looking for"); Serial.println(response);
  }
  Serial2.println(command); // send the read character to the esp8266
  WebBuf = "";
  unsigned long time = millis() + 10000;
  while (millis() < time) {
    while (Serial2.available()) {
      char r = Serial2.read();
      WebBuf.concat(r);
      if (WebBuf.indexOf(response) > 0) {
        if (DEBUG) {
          Serial.print("Found in:"); Serial.println(WebBuf);
        }
        return true;
      }
    }
  }
  if (DEBUG) {
    Serial.print("Not Found in:"); Serial.println(WebBuf);
  }
  return false;
}

void sendData2(const char *command) {
  if (DEBUG) {
    Serial.print ("sending2: ") ; Serial.println(command);
  }
  Serial2.println(command); // send the read character to the esp8266
}

void sendData(const String& command) {
  if (DEBUG) {
    Serial.print ("sending: ") ; Serial.println(command);
  }
  Serial2.println(command); // send the read character to the esp8266
}

int MonthToInt(const String& Month) {
  if (Month == "Jan") {
    return 1;
  }
  else if (Month == "Feb") {
    return 2;
  }
  else if (Month == "Mar") {
    return 3;
  }
  else if (Month == "Apr") {
    return 4;
  }
  else if (Month == "May") {
    return 5;
  }
  else if (Month == "Jun") {
    return 6;
  }
  else if (Month == "Jul") {
    return 7;
  }
  else if (Month == "Aug") {
    return 8;
  }
  else if (Month == "Sep") {
    return 9;
  }
  else if (Month == "Oct") {
    return 10;
  }
  else if (Month == "Nov") {
    return 11;
  }
  else if (Month == "Dec") {
    return 12;
  }
}
// Print the temperature, or the type of fault


void FormatDateTime16(const unsigned long& mydate, char *out) {
  int i = sprintf(out, "%02i/%02i/%4i %02i:%02i", month(mydate), day(mydate), year(mydate), hour(mydate), minute(mydate));
  //Serial.println(out);
  //Serial.println(i);
}

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




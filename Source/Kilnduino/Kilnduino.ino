
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

unsigned int Temp1Max = 0;
unsigned long Time1Max;
unsigned int Temp2Max = 0;
unsigned long Time2Max;

unsigned long StartTime;
unsigned long RunTime;
unsigned int StartTemp = 0;
unsigned int Temps1[32];
unsigned int Temps2[32];
unsigned long Times[32];
int Hour[32];
int Minute[32];
int TempInterval = 15;
int TempCount = 32;

unsigned int Temp1;
unsigned int Temp2;

int lastloggedminute = -1;
int logindex = 0;
String webpage = "";
String hwebpage = "";
char RunDescription[30];
char History[50];
char WebPage[2000];
char cipSend[30];

void setup()
{
  Serial2.begin(115200);//9600 (mine), 57600, 115200
  Serial.begin(115200);

  delay(2000);
  connectWiFi();
  Mode = 0;
  StartTime = now();
  // Initializing the MAX31855's registers
  for (int i = 0; i < NUM_MAX31856; i++) {
    Serial.println(i);
    TemperatureSensor[i]->writeRegister(REGISTER_CR0, CR0_INIT);
    TemperatureSensor[i]->writeRegister(REGISTER_CR1, CR1_INIT);
    TemperatureSensor[i]->writeRegister(REGISTER_MASK, MASK_INIT);
  }

}


void loop()
{

  delay(1000);
  RunTime = now() - StartTime;
  //   Serial.print("Internal Temp = ");
  //   Serial.println(therm1.readInternal());

  Temp1 = TemperatureSensor[0]->readThermocouple(FAHRENHEIT);
  Temp2 = TemperatureSensor[1]->readThermocouple(FAHRENHEIT);
  if (Temp2 > 3000) {
    Temp2 = 0;
  }


  //Serial.println(Temp2);


  if (Temp1 > Temp1Max)
  {
    //    Serial.println("max1found");
    //    Serial.println(Temp1);

    Temp1Max = Temp1;
    Time1Max = RunTime;
  }
  if (Temp2 > Temp2Max)
  {
    //  Serial.println("max2found");
    //    Serial.println(Temp2);

    Temp2Max = Temp2;
    Time2Max = RunTime;
  }

  //Serial.println(minute(RunTime) % TempInterval);
  if (minute(RunTime) % TempInterval == 0) {
    if (lastloggedminute != minute(RunTime) && logindex < 32) {

      Serial.print("logged at minute"); Serial.println(minute(RunTime));
      Temps1[logindex] = Temp1;
      Temps2[logindex] = Temp2;
      Times[logindex] = (RunTime);
      logindex++;
      lastloggedminute = minute(RunTime);
    }
  }

  sendSerial1ToSerial2();
  //


  if (Mode == 0) { //this is looking for only for kiln web requests from esp8266
    if (Serial2.find("+")) {
      while (Serial2.available() < 4) { } //wait till we get 4 more
      if (Serial2.find("IPD,")) {
        WebBuf = "";
        while (Serial2.available() < 30) { } //wait till we get 10 more
        int connectionId = Serial2.read() - 48; // subtract 48 because the read() function returns
        while (Serial2.available()) {
          char r = Serial2.read();
          WebBuf.concat(r);
        }
        Serial.print("found +IPD: ");  Serial.println(WebBuf);

        if (WebBuf.indexOf("ENTERDESC") > 0) {
          ProcessEnterDesc(connectionId);
        }
        else if (WebBuf.indexOf("HISTORY") > 0) {
          ProcessHistoryRequest(connectionId);
        }
        else if (WebBuf.indexOf("KILN") > 0) {
          ProcessKilnWebRequest(connectionId);
        }
        else
        {
          Serial.println("other..");

        }
      }


    }

  }

  if (Mode == 9) { //output anything
    while (Serial2.available()) {
      Serial.write(Serial2.read());
    }
  }
}




bool ProcessEnterDesc(int connectionId)
{
  Serial.println ("");
  Serial.print ("Kiln enter description ");
  Serial.print("connectionId:"); Serial.println(connectionId);
  strcpy(WebPage, "<html><body><style></style>");
  strcat(WebPage, "<div style=font-size:large>Nock Kiln - Set Run Description</div>");
  strcat(WebPage, "<br><form action=\"KILNSETDESC\"> Enter purpose, cone#, FXSpeed, StartWheel#<br>Example:Bisque,Cone5,Slowest,0");
  strcat(WebPage, "<br><br><input type=\"text\" name=\"d\" value=\"");
  strcat(WebPage, RunDescription);
  strcat(WebPage, "\"><br><input type=\"submit\" value=\"Submit Description\"></form>");
  strcat(WebPage, "<form action=\"KILN\"><input type=\"submit\" value=\"Cancel\"></form>");
  strcat(WebPage, "</body></html>");

  SendWebPage(connectionId);

}
void SendWebPage(int connectionId)
{
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += strlen(WebPage);
  cipSend += "\r\n";
  sendData(cipSend);
  sendData(WebPage);
  delay(2000);
  String closeCommand = "AT+CIPCLOSE=";
  closeCommand += connectionId; // append connection id
  //closeCommand += "\r\n";
  sendData(closeCommand);

}

void KilnReset()
{
  Serial.println ("RESTART");
  StartTime = now();
  for (int X = 1; X < TempCount; X++) {
    Times[X] = 0;
    Temps1[X] = 0;
    Temps2[X] = 0;
  }
  Temp1Max = 0;
  Time1Max = 0;
  Temp2Max = 0;
  Time2Max = 0;
  Temps1[0] = Temp1;
  Temps2[0] = Temp2;
  Times[0] = 0;
  RunTime = 0;
  logindex = 1;
  strcpy(RunDescription, "Please Enter");
}

void SetKilnDesc()
{
  Serial.println ("setdesc");
  int istart = WebBuf.indexOf("KILNSETDESC") + 14;
  while (WebBuf.indexOf(" ", istart) == -1) { //wait till we get 10 more
    Serial.println("looking...");
    while (Serial2.available()) {
      char r = Serial2.read();
      WebBuf.concat(r);
    }
  }
  WebBuf.replace("%2C", ",");
  WebBuf.replace("%23", "#");
  int iend = WebBuf.indexOf(" ", istart);
  WebBuf.replace("+", " ");
  int ilen = (iend - istart + 1);
  if (ilen > 30) {
    ilen = 30;
  }
  WebBuf.substring(istart).toCharArray(RunDescription, ilen);
  Serial.println (RunDescription);


}

bool ProcessKilnWebRequestB(int connectionId)
{
  Serial.println ("");
  Serial.print ("Kiln web request ");
  Serial.print("connectionId:"); Serial.println(connectionId);

  if (WebBuf.indexOf("KILNSTART") > 0) {
    KilnReset();
  }
  if (WebBuf.indexOf("KILNSETDESC") > 0) {
    SetKilnDesc();
  }
  if (WebBuf.indexOf("KILNSAVE") > 0) {
    Serial.println ("kiln save");
    SaveToHistory();
  }

  strcpy(WebPage, "<html><body><style>td {text-align:right} .l {text-align:left} input {float:left; margin:3px}</style>");
  strcat(WebPage, "<span style=font-size:large>Nock Kiln Temp</span>");
  strcat(WebPage, "<div><form action=\"KILN\"><input type=\"submit\" value=\"Refresh\"></form>");
  strcat(WebPage, "<form action=\"ENTERDESC\"><input type=\"submit\" value=\"Set Descr\"></form>");
  strcat(WebPage, "<form action=\"KILNSTART\"><input type=\"submit\" value=\"ReStart\"></form>");
  strcat(WebPage, "<form action=\"KILNSAVE\"><input type=\"submit\" value=\"Save\"></form>");
  strcat(WebPage, "<form action=\"HISTORY\"><input type=\"submit\" value=\"PriorRuns\"></form></div>");
  strcat(WebPage, "<div style=\"clear:both;\"></div>");
  strcat(WebPage, "  Current Time:"); strcat(WebPage, month(now())); strcat(WebPage, "/"); strcat(WebPage, day(now())); strcat(WebPage,  " "); strcat(WebPage, hour(now())); strcat(WebPage, ":"); strcat(WebPage, minute(now()));
  strcat(WebPage,"<br>ConeRef: 06~1850;  05~1900;  04~1950;  4~2140;  5~2190");
  strcat(WebPage,"<br>");
  strcat(WebPage,"<br>Start Time:"); strcat(WebPage, month(StartTime)); strcat(WebPage,"/"); strcat(WebPage,day(StartTime)); strcat(WebPage, " "); strcat(WebPage, hour(StartTime)); strcat(WebPage, ":"); strcat(WebPage, minute(StartTime));
  strcat(WebPage,"<br>Description:");  strcat(WebPage,RunDescription);
  strcat(WebPage,"<br><br><div>Runtime: "); strcat(WebPage, hour(RunTime));  strcat(WebPage, "hrs "); strcat(WebPage,minute(RunTime)) ; strcat(WebPage,"mins "); strcat(WebPage,second(RunTime)); strcat(WebPage,"secs");
  //webpage.concat("<br>Temp1 "); webpage.concat(Temp1); webpage.concat("F Max "); webpage.concat( hour(Time1Max)); webpage.concat( ":"); webpage.concat(minute(Time1Max)); webpage.concat(":"); webpage.concat(second(Time1Max)); webpage.concat(" "); webpage.concat(Temp1Max); webpage.concat("F ");
  //webpage.concat("<br>Temp2 "); webpage.concat(Temp2); webpage.concat("F Max "); webpage.concat( hour(Time2Max)); webpage.concat( ":"); webpage.concat(minute(Time2Max)); webpage.concat(":"); webpage.concat(second(Time2Max)); webpage.concat(" "); webpage.concat(Temp2Max); webpage.concat("F ");
  strcat(WebPage,"<br><br><table style=font-size:large border=1>");
  strcat(WebPage,"<tr><td class='l'>Time<td>Temp1<td>Temp2<td>Rate</tr>");
  boolean bMaxAdded = false;
  addRow(RunTime, Temp1, Temp2, -2);
  for (int X = 0; X < logindex; X++) {
    if (X > 0) {
      if (Times[X] > Time1Max && Temps1[X] <= Temp1Max &&  bMaxAdded == false) {
        addRow(Time1Max, Temp1Max, Temp1Max, -1);
        addRow(Times[X], Temps1[X], Temps2[X], Temps1[X - 1]);
        bMaxAdded = true;
      }
      else {
        addRow(Times[X], Temps1[X], Temps2[X], Temps1[X - 1]);
      }
    }
    else {
      addRow(Times[X], Temps1[X], Temps2[X], 0);
    }
  }

  strcat(WebPage, "</table></div></body></html>");
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += strlen(WebPage);
 // Serial.println(webpage);
  cipSend += "\r\n";
  sendData(cipSend);
  sendData(WebPage);
  delay(2000);
  String closeCommand = "AT+CIPCLOSE=";
  closeCommand += connectionId; // append connection id
  //closeCommand += "\r\n";
  sendData(closeCommand);

}


bool ProcessKilnWebRequest(int connectionId)
{
  Serial.println ("");
  Serial.print ("Kiln web request ");
  Serial.print("connectionId:"); Serial.println(connectionId);

  if (WebBuf.indexOf("KILNSTART") > 0) {
    KilnReset();
  }
  if (WebBuf.indexOf("KILNSETDESC") > 0) {
    SetKilnDesc();
  }
  if (WebBuf.indexOf("KILNSAVE") > 0) {
    Serial.println ("kiln save");
    SaveToHistory();
  }

  webpage = "<html><body><style>td {text-align:right} .l {text-align:left} input {float:left; margin:3px}</style>";
  webpage.concat("<span style=font-size:large>Nock Kiln Temp</span>");
  webpage.concat("<div><form action=\"KILN\"><input type=\"submit\" value=\"Refresh\"></form>");
  webpage.concat("<form action=\"ENTERDESC\"><input type=\"submit\" value=\"Set Descr\"></form>");
  webpage.concat("<form action=\"KILNSTART\"><input type=\"submit\" value=\"ReStart\"></form>");
  webpage.concat("<form action=\"KILNSAVE\"><input type=\"submit\" value=\"Save\"></form>");
  webpage.concat("<form action=\"HISTORY\"><input type=\"submit\" value=\"PriorRuns\"></form></div>");
  webpage.concat("<div style=\"clear:both;\"></div>");
  webpage.concat("  Current Time:"); webpage.concat( month(now())); webpage.concat("/"); webpage.concat(day(now())); webpage.concat( " "); webpage.concat( hour(now())); webpage.concat( ":"); webpage.concat( minute(now()));
  webpage.concat("<br>ConeRef: 06~1850;  05~1900;  04~1950;  4~2140;  5~2190");
  webpage.concat("<br>");
  webpage.concat("<br>Start Time:"); webpage.concat( month(StartTime)); webpage.concat("/"); webpage.concat(day(StartTime)); webpage.concat( " "); webpage.concat( hour(StartTime)); webpage.concat( ":"); webpage.concat( minute(StartTime));
  webpage.concat("<br>Description:");  webpage.concat(RunDescription);
  webpage.concat("<br><br><div>Runtime: "); webpage.concat( hour(RunTime));  webpage.concat( "hrs "); webpage.concat(minute(RunTime)) ; webpage.concat("mins "); webpage.concat(second(RunTime)); webpage.concat("secs");
  //webpage.concat("<br>Temp1 "); webpage.concat(Temp1); webpage.concat("F Max "); webpage.concat( hour(Time1Max)); webpage.concat( ":"); webpage.concat(minute(Time1Max)); webpage.concat(":"); webpage.concat(second(Time1Max)); webpage.concat(" "); webpage.concat(Temp1Max); webpage.concat("F ");
  //webpage.concat("<br>Temp2 "); webpage.concat(Temp2); webpage.concat("F Max "); webpage.concat( hour(Time2Max)); webpage.concat( ":"); webpage.concat(minute(Time2Max)); webpage.concat(":"); webpage.concat(second(Time2Max)); webpage.concat(" "); webpage.concat(Temp2Max); webpage.concat("F ");
  webpage.concat("<br><br><table style=font-size:large border=1>");
  webpage.concat( "<tr><td class='l'>Time<td>Temp1<td>Temp2<td>Rate</tr>");
  boolean bMaxAdded = false;
  addRow(RunTime, Temp1, Temp2, -2);
  for (int X = 0; X < logindex; X++) {
    if (X > 0) {
      if (Times[X] > Time1Max && Temps1[X] <= Temp1Max &&  bMaxAdded == false) {
        addRow(Time1Max, Temp1Max, Temp1Max, -1);
        addRow(Times[X], Temps1[X], Temps2[X], Temps1[X - 1]);
        bMaxAdded = true;
      }
      else {
        addRow(Times[X], Temps1[X], Temps2[X], Temps1[X - 1]);
      }
    }
    else {
      addRow(Times[X], Temps1[X], Temps2[X], 0);
    }
  }

  webpage.concat( "</table></div></body></html>");
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += webpage.length();
  Serial.println(webpage);
  cipSend += "\r\n";
  sendData(cipSend);
  sendData(webpage);
  delay(2000);
  String closeCommand = "AT+CIPCLOSE=";
  closeCommand += connectionId; // append connection id
  //closeCommand += "\r\n";
  sendData(closeCommand);

}



boolean addRow(unsigned long Time, unsigned int Temp1, unsigned int Temp2 , unsigned int Tempp) {
  webpage.concat( "<tr><td class='l'>");
  webpage.concat( hour(Time));
  webpage.concat( ":");
  webpage.concat( minute(Time));
  if (Tempp == -1 or Tempp == -2) {
    webpage.concat( ":");
    webpage.concat( second(Time));
  }
  webpage.concat( "<td>");
  webpage.concat( Temp1);
  webpage.concat( "F");
  webpage.concat( "<td>");
  webpage.concat( Temp2);
  webpage.concat( "F");
  webpage.concat( "<td>");

  switch (Tempp) {
    case -1:
      webpage.concat( " Max");
      break;
    case -2:
      webpage.concat( " Now");
      break;
    case 0:
      //do something when var equals 2
      break;
    default:
      webpage.concat( (int)(((double)Temp1 - (double)Tempp) / TempInterval) * 60);
      webpage.concat( " F/hr");
      break;
      break;
  }

}

boolean connectWiFi()
{
  sendDataWaitForResponse("AT+CWMODE=3", "OK") ;
  sendDataWaitForResponse("AT+CWJAP=\"NETGEARMcnock\",\"19581958\"", "OK");
  Serial.println("  Connected to WiFi!!");
  sendDataWaitForResponse("AT+CIFSR", "OK");
  sendDataWaitForResponse("AT+CIPMUX=1", "OK");
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
  return true;
  Serial.println(now());
  Serial.println("Setup done!");
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
  unsigned long time = millis() + 5000;
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
void printTemperature(double temperature) {
  switch ((int) temperature) {
    case FAULT_OPEN:
      Serial.print("FAULT_OPEN");
      break;
    case FAULT_VOLTAGE:
      Serial.print("FAULT_VOLTAGE");
      break;
    case NO_MAX31856:
      Serial.print("NO_MAX31856");
      break;
    default:
      Serial.print(temperature);
      break;
  }
  Serial.print(" ");
}
double getAvgTemp(double t1, double t2) {
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

void webpage_concatDateTime(unsigned long timestamp) {
  //webpage.concat( month(timestamp)); webpage.concat("/"); webpage.concat(day(timestamp)); webpage.concat( " "); webpage.concat( hour(timestamp)); webpage.concat( ":"); webpage.concat( minute(timestamp));
}

void ProcessHistoryRequest(int connectionId) {
  int address;

  hwebpage = "<html><body><style></style>";
  hwebpage.concat("<div style=font-size:large>Nock Kiln - History</div><table>");

  for (int X = 1; X < 50; X++) {
    address = X * 50;
    char x;
    x =  EEPROM.read(address);
    if (x != 'C') {
      Serial.print("skipping"); Serial.println(address);
      break;
    }
    address++;
    Serial.print("found"); Serial.println(address);

    webpage.concat("<tr><td>xxx</td><td>");
    for (int z = address; z < (address + 49); z++) {
      char p = EEPROM.read(z);
      webpage.concat(p);
    }
    webpage.concat("</td></tr>");
  }
  hwebpage.concat("</table><form action=\"KILN\"><input type=\"submit\" value=\"Cancel\"></form>");
  hwebpage.concat( "</body></html>");
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += hwebpage.length();
  cipSend += "\r\n";
  sendData(cipSend);
  sendData(webpage);
  delay(2000);
  String closeCommand = "AT+CIPCLOSE=";
  closeCommand += connectionId; // append connection id
  //closeCommand += "\r\n";
  sendData(closeCommand);
}

void SaveToHistory() {
  int address;
  for (int X = 1; X < 50; X++) {
    address = X * 50;
    char x;
    x =  EEPROM.read(address);
    Serial.println(x);
    if (x != 'C') {
      Serial.print("save to:"); Serial.println(address);
      break;
    }
  }
  EEPROM.write(address, 'C');
  address++;
  webpage = "";
  webpage.concat("<td>");
  webpage_concatDateTime(StartTime);
  webpage.concat("<td>");
  webpage_concatDateTime(Time1Max);
  webpage.concat("<td>");
  webpage.concat(Temp1Max);
  webpage.concat("<td>");
  webpage.concat(RunDescription);
  Serial.println(webpage);
  int i;
  for (int X = address; X < (address + 49); X++) {
    Serial.print(i);
    char e = webpage[i];
    Serial.print(e);
    EEPROM.write(X, e);
    i++;
  }
  Serial.println("");
}



#include "Time.h"
#include "TimeLib.h"

// NTP Servers:
// IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov


//const int timeZone = 1;     // Central European Time
//const int timeZone = -5;  // Eastern Standard Time (USA)
//const int timeZone = -4;  // Eastern Daylight Time (USA)
const int timeZone = -8;  // Pacific Standard Time (USA)
//const int timeZone = -7;  // Pacific Daylight Time (USA)


#define NTP_PACKET_SIZE 48 // NTP time is in the first 48 bytes of message

unsigned int localPort = 8888;  // local port to listen for UDP packets


//#include <SoftwareSerial.h>
//use mega Serial 2 for serial monitor; Serial 1 on pins 19 (RX) and 18 (TX);// Serial2 on pins 17 (RX) and 16 (TX), Serial3 on pins 15 (RX) and 14 (TX).

#define DEBUG true



int StartupStage = 0;
String WebBuf = "";

unsigned int TempCurrent = 77;
unsigned long StartTime;
unsigned int Temps[320];
unsigned long Times[320];
int TempInterval;
int TempCount = 16;

void setup()
{
  Serial2.begin(115200);//9600 (mine), 57600, 115200


  //serial 0 is to usb
  Serial.begin(115200);
  StartTime = millis;
  Serial.println(StartTime);

  Serial.println(hour(StartTime));


}
void loop()
{

  if (StartupStage < 100)
  {
    connectWiFi();
  }
  else
  {

    if (Serial2.available() > 2)
    {
      Serial.print("Recieved:");
      //Serial.println(Serial2.available());
      //String WebNew = WebBuf + Serial2.readString();
      //Serial.println(WebNew);
      //if (WebNew.indexOf("+IPD,") > 0 )
      if (Serial2.find("+IPD,"))
      {
        WebBuf = "";
        Serial.println ("Kiln web request");
        delay(1000);
        int connectionId = Serial2.read() - 48; // subtract 48 because the read() function returns

        Serial.print("connectionid"); Serial.println(connectionId);
        // the ASCII decimal value and 0 (the first decimal number) starts at 48
        String webpage = "<h1>Kiln Temp</h1>Started: ";

        webpage.concat( month(StartTime));
        webpage.concat( " ");
        webpage.concat( day(StartTime));
        webpage.concat( "   ");
        webpage.concat( hour(StartTime));
        webpage.concat( ":");
        webpage.concat( minute(StartTime));
        webpage.concat("<br><br>Current:");

        webpage.concat( hour());
        webpage.concat( ":");
        webpage.concat( minute());
        webpage.concat("   ");
        webpage.concat(TempCurrent);
        webpage.concat("F");
        webpage.concat("<br><table style=font-size:x-large border=1>");


        for (int X = 1; X < TempCount; X++) {
          webpage.concat( "<tr><td>");
          webpage.concat( hour(Times[X]));
          webpage.concat( ":");
          webpage.concat( minute(Times[X]));
          webpage.concat( "<td>");
          webpage.concat( Temps[X]);
          webpage.concat( "F");
        }

        webpage.concat( "</table>");
        String cipSend = "AT+CIPSEND=";
        cipSend += connectionId;
        cipSend += ",";
        cipSend += webpage.length();
        cipSend += "\r\n";
        sendData(cipSend, 1000, DEBUG);
        sendData(webpage, 5000, DEBUG);

        //webpage = "<button>LED2</button>";
        //cipSend = "AT+CIPSEND=";
        //cipSend += connectionId;
        //cipSend += ",";
        // cipSend += webpage.length();
        //cipSend += "\r\n";
        //sendData(cipSend, 1000, DEBUG);
        //sendData(webpage, 1000, DEBUG);
        String closeCommand = "AT+CIPCLOSE=";
        closeCommand += connectionId; // append connection id
        closeCommand += "\r\n";
        sendData(closeCommand, 3000, DEBUG);
      }
      else
      {
        //WebBuf = WebNew.substring(WebNew.length() - 30);
        // Serial.println ("Non Web response:");
        // while (Serial2.available()) {
        //   Serial.write(Serial2.read());
        // }
      }

    }
    else
    {

    }
    while (Serial.available()) {
      Serial2.write(Serial.read());
    }
  }



}
boolean connectWiFi()
{

  if (StartupStage == 0) //need to set mode
  {
    Serial2.println("AT+CWMODE=3");
    Serial.println("Send AT+CWMODE=3");
    StartupStage = 1;
  }
  else if (StartupStage == 1) //need to connected to wifi
  {
    if (Serial2.find("OK")) {
      Serial2.println("AT+CWJAP=\"NETGEARMcnock\",\"19581958\"");
      Serial.println("Send AT+CWJAP=\"NETGEARMcnock\",\"19581958\"");
      StartupStage = 2;
    }
  }
  else if (StartupStage == 2) //need to get ip adderess
  {
    if (Serial2.find("OK")) {
      Serial.println("  Connected to WiFi!!");
      Serial.println("Send AT+CIFSR"); //get local ip address
      Serial2.println("AT+CIFSR");
      StartupStage = 3;
    }
  }
  else if (StartupStage == 3) //need to set number of connections
  {
    if (Serial2.find("STAIP")) {
      delay(1000);
      Serial.write("STAIP");
      while (Serial2.available()) {
        Serial.write(Serial2.read());
      }
      Serial2.println("AT+CIPMUX=1");
      Serial.println("send AT+CIPMUX=1"); // connections 1 = multiple to 4, 0 single
      StartupStage = 4;
    }
  }
  //AT+CIPSTART=0,"UDP","132.163.4.101",123,4445,2   //start udp
  //AT+SIPSEND=0,7,"132.163.4.101",1445,123
  //REQUEST
  //AT+CIPCLOSE=0
  else if (StartupStage == 4)//need to set up server
  {
    if (Serial2.find("OK")) {
      StartupStage = 5;
      Serial2.println("AT+CIPSERVER=1,80"); //set up as server 1 is start, 0 stop, 80 is port
      Serial.println("AT+CIPSERVER=1,80");
    }
  }
  else if (StartupStage == 5) //need to get time
  {
    if (Serial2.find("OK")) {

      Serial2.println("AT+CIPSTART=0,UDP,132.163.4.101,123"); //set up as server 1 is start, 0 stop, 80 is port
      Serial2.println("AT+CIPSEND=0,7");
      sendNTSPacket();
      StartupStage = 6;
    }

  }

  else if (StartupStage == 6) //waiting for time
  {
    if (Serial2.available() >= NTP_PACKET_SIZE) {
      setTime(readNTSPacket());
      Serial.println(now());
      Serial.println("Setup done!");
      StartupStage = 100;
    }

  }

}

String sendData(String command, const int timeout, boolean debug) {
  String response = "";

  if (debug) {
    Serial.print ("sending:") ;
    Serial.println(command);
  }
  Serial2.print(command); // send the read character to the esp8266

  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (Serial2.available())
    {


      // The esp has data so display its output to the serial window
      char c = Serial2.read(); // read the next character.
      response += c;
    }
  }

  if (debug)
  {
    Serial.print(response);
  }

  return response;
}

/*-------- NTP code ----------*/


unsigned long readNTSPacket()
{
  Serial.println("Receive NTP Response");
  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
  Serial2.readBytes(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
  unsigned long secsSince1900;
  // convert four bytes starting at location 40 to a long integer
  secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
  secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
  secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
  secsSince1900 |= (unsigned long)packetBuffer[43];
  return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
}

// send an NTP request to the time server at the given address
bool sendNTSPacket()
{
  //String address = "132.163.4.101";
  // set all bytes in the buffer to 0
  byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  //Udp.beginPacket(address, 123); //NTP requests are to port 123
  //Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Udp.endPacket();
  Serial2.write(packetBuffer, NTP_PACKET_SIZE);
  //AT+CIPSTART=0,"UDP","132.163.4.101",123,4445,2   //start udp connection
  //AT+SIPSEND=0,7,"132.163.4.101",1445,123
  //REQUEST
  //AT+CIPCLOSE=0

}

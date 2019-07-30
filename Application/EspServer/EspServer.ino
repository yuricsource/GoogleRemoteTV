#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "remote_control.txt.h"
#include "RemoteCommands.h"

#ifndef STASSID
#define STASSID "Yuri_Duda"
#define STAPSK  "Australia2us"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

#define IRLEDpin  14
#define BITtime   562

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  String remotePage((char*)remote_control_txt);
  server.send(200, "", remotePage);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
  digitalWrite(led, 0);
}

void IrCommandHandler()
{
  IRsendCode(IrCommand[server.arg(1).toInt() - 1]);
}

void setup(void) {
    pinMode(IRLEDpin, OUTPUT);
  digitalWrite(IRLEDpin, HIGH);    //turn off IR LED to start
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  
  server.on("/command",IrCommandHandler);
  
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}




// Ouput the 38KHz carrier frequency for the required time in microseconds
// This is timing critial and just do-able on an Arduino using the standard I/O functions.
// If you are using interrupts, ensure they disabled for the duration.
void IRcarrier(unsigned int IRtimemicroseconds)
{
  for (int i = 0; i < (IRtimemicroseconds / 26); i++)
  {
    digitalWrite(IRLEDpin, HIGH);   //turn on the IR LED
    //NOTE: digitalWrite takes about 3.5us to execute, so we need to factor that into the timing.
    delayMicroseconds(9);          //delay for 13us (9us + digitalWrite), half the carrier frequnecy
    digitalWrite(IRLEDpin, LOW);    //turn off the IR LED
    delayMicroseconds(9);          //delay for 13us (9us + digitalWrite), half the carrier frequnecy
  }
}

//Sends the IR code in 4 byte NEC format
void IRsendCode(uint64_t code)
{
  //send the leading pulse
  IRcarrier(9000);            //9ms of carrier
  delayMicroseconds(4500);    //4.5ms of silence
  Serial.println("start bit:");
  //send the user defined 4 byte/32bit code
  for (int i = 0; i < 32; i++)        //send all 4 bytes or 32 bits
  {
    IRcarrier(BITtime);               //turn on the carrier for one bit time
    if (code & 0x80000000)            //get the current bit by masking all but the MSB
    { 
      delayMicroseconds(3 * BITtime); //a HIGH is 3 bit time periods
    }
    else
    {
      delayMicroseconds(BITtime);     //a LOW is only 1 bit time period
    }
      
    code <<= 1;                      //shift to the next bit for this byte
  }
  IRcarrier(BITtime);                 //send a single STOP bit.
  Serial.println("");
  Serial.println("End");
}



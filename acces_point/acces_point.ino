//ESP BOARD MANAGER V.2.7.4

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


#ifndef APSSID
#define APSSID "ESP-01"
#define APPSK  "azertyuiop1234567890^$"
#endif
#ifndef PIN
#define PIN 2
#endif
#ifndef REL
#define REL 0
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long currentMillis;
bool isConnectedToInternet = false;
bool dbtPrgm;
int cptNbError = 0;

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);





void handleRoot() {
  if(WiFi.status()==WL_CONNECTED)
  {
    server.send(200, "text/html", "<h1>Interface ESP-01</h1>"
    "</br>"
    "<form action=\"\" method=\"POST\">"
    
    "<input type=\"text\" name=\"SSID\" placeholder=\"[SSID_Wifi]\"></br>"
    "<input type=\"password\" name=\"password\" placeholder=\"[Password_Wifi]\"></br>"
    "<input type=\"submit\" value=\"Login\">"
    "</form></br>You're connected to "+(String)WiFi.SSID());
  }
  else
  {
    server.send(200, "text/html", "<h1>ESP-01 de Lolo</h1>"
    "</br>"
    "<form action=\"\" method=\"POST\">"
    
    "<input type=\"text\" name=\"SSID\" placeholder=\"[SSID_Wifi]\"></br>"
    "<input type=\"password\" name=\"password\" placeholder=\"[Password_Wifi]\"></br>"
    "<input type=\"submit\" value=\"Login\">"
    "</form></br>You're NOT connected to any wifi point");
  }
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void handleLogin() {                         // If a POST request is made to URI /login
  if( ! server.hasArg("SSID") || ! server.hasArg("password") 
      || server.arg("SSID") == NULL || server.arg("password") == NULL) // If the POST request doesn't have username and password data
  {
    server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }
  else
  {
    WiFi.begin(server.arg("SSID"), server.arg("password"));
    server.send(200, "text/html", "<script type=\"text/JavaScript\">"
    "setTimeout(\"location.href = \'\';\",8000);"
    "</script><h1>Trying to connect (please, wait 8s)...</h1>");
    delay(50);
  }
}

void blinkLed()
{
  for(int i=0; i<17; i++)
  {
    delay(130);
    digitalWrite(PIN, LOW);
    delay(40);
    digitalWrite(PIN, HIGH);
  }
}

void restartBox()
{
    digitalWrite(REL, LOW);
    delay(500);
    digitalWrite(REL, HIGH);
}

void checkInternet()
{
    HTTPClient http;
    //WiFiClient wifiClient;
    String url = "http://google.fr";

    http.begin(url);
    
    int httpCode = http.GET();
    if (httpCode > 0)
    { // Request has been made
        String payload = http.getString();
        Serial.println(payload);
        isConnectedToInternet = true;
        cptNbError = 0;
    }
    else
    {
        Serial.println("not resolving google.com");
        isConnectedToInternet = false;
        cptNbError++;
    }
  
    http.end();
}






void setup()
{
  pinMode(PIN, OUTPUT);
  pinMode(REL, OUTPUT);
  digitalWrite(REL, HIGH);
  WiFi.disconnect();
  delay(50);
  
  Serial.begin(115200);
  Serial.println("\n\n\n");
  Serial.println("Setting up access point ... ");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");


  Serial.println("\nStarting HTTP server");
  server.on("/",HTTP_GET, handleRoot);  // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/", HTTP_POST, handleLogin);  // Call the 'handleLogin' function when a POST request is made to URI "/"
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  server.begin();
  Serial.println("Ready\n");
  isConnectedToInternet = false;
  dbtPrgm = true;
  cptNbError = 0;
  for(int i=0; i<10; i++)
  {
    delay(500);
    digitalWrite(PIN, LOW);
    delay(100);
    digitalWrite(PIN, HIGH);
  }
}










void loop()
{
  server.handleClient();
  currentMillis = millis();
  if (currentMillis - previousMillis >= 1000*5) // each 5s
  {
    previousMillis = currentMillis;
    checkInternet();
    if(isConnectedToInternet==false && cptNbError==3)
    {
      if(dbtPrgm==true)
        dbtPrgm=false;
      else
        restartBox();
        Serial.println("redémarrage box");
    }
    if(WiFi.status()==WL_CONNECTED) digitalWrite(PIN, LOW);
    else digitalWrite(PIN, HIGH);
  }
}

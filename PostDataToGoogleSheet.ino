#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"

//############################################################################
const char* ssid = "uuholovoG";                                             // WiFi SSID
const char* password = "<password>";                                        // WiFi Password
const char *GScriptId = "AKfycbwrnJmLCkpSSaajLaFP1LCfsShX1cV6t47iIGDq";     // GoogleDocs API ID
#define HEALTH_CHECK_PERIOD  3600000                                        // how often to write a health check (24 hrs = 86400000 12h=43200000 1hr = 3600000 1min = 60000 )
//############################################################################

const char* host = "script.google.com";
const int httpsPort = 443; //the https port is same

HTTPSRedirect* client = nullptr;

const char* fingerprint = "";
String logMsg; 
unsigned long lastHealthCheck=0;

void setup() {
  delay(1000);
  Serial.begin(9600);

  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
   
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  
  sendToHTTPSHost("name=Starting&value=" + String(ssid) + "[" +  WiFi. RSSI() + "]_" + IpAddress2String(WiFi.localIP()) + "_(" + String((millis()/1000),DEC) + "s)");   
  lastHealthCheck=millis();

}

void loop () {
  char buffer [128];
  int cnt = 0;
  boolean eoln = false;
  
  while (eoln == false){
     if (Serial.available() > 0){
        char c = Serial.read();
        
        if (c!=10 and c!=13) {
          buffer[cnt] = c;
          cnt++;
      
        }
        if ((c == '\n') || (cnt == sizeof(buffer)-1)){
            buffer[cnt] = '\0';
            eoln = true;
        
       }
     }
    delay(1000);
    
    if ((millis()>HEALTH_CHECK_PERIOD and (millis()-HEALTH_CHECK_PERIOD)>lastHealthCheck) or lastHealthCheck > millis())  {
   
     sendToHTTPSHost("name=HealthCheck&value=" + String(ssid) + "[" +  WiFi. RSSI() + "]_" + IpAddress2String(WiFi.localIP()) + "_(" + String((millis()/1000),DEC) + "s)");
     
     lastHealthCheck=millis();
      }

     
    }

   // Serial.print("######\nReceived: ");
   // Serial.println(buffer);

   sendToHTTPSHost(buffer);
   eoln=false;
}


void sendToHTTPSHost(String pmessage) {
    
  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  Serial.print("Connecting to ");
  Serial.println(host);          //try to connect with "script.google.com"

  // Try to connect for a maximum of 5 times then exit
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      Serial.println("Connected!");
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }

  String uurl = String("/macros/s/") + GScriptId + "/exec?mac=" + String(WiFi.macAddress()) + "&" + pmessage;

  Serial.print("Calling ");
  Serial.println(uurl);
  client->GET(uurl, host);

  
  
  // delete HTTPSRedirect object
    
  delete client;
  client = nullptr;
 
}

String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}

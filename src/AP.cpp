#include <configAP.h>

IPAddress ip(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

AsynWebServer server(80);

void ConnectWiFi_AP(bool useStaticIP = false)
{ 
   Serial.println("");
   WiFi.mode(WIFI_AP);
   Serial.print("Conectando");
   while(!WiFi.softAP(ssid, password))
   {
     Serial.println(".");
     delay(100);
   }
   if(useStaticIP) WiFi.softAPConfig(ip, gateway, subnet);

   Serial.println("");
   Serial.print("Iniciando AP:\t");
   Serial.println(ssid);
   Serial.print("IP address:\t");
   Serial.println(WiFi.softAPIP());
}

void setup() 
{
  Serial.begin(115200);
  ConnectWiFi_AP();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)){
    request -> send (SPIFFS, "/index.html", String(), false)
  });
}

void loop() 
{ 
}
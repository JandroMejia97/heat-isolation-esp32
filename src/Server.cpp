#include "configServer.h"

AsyncWebServer server(80);

void sendResponse(DynamicJsonDocument doc, AsyncWebServerRequest *request){
    //crea la respuesta
    String response = "";
    serializeJson(doc, response);

    //enviar la respuesta
    request->send(200, "application/json", response);
}

//Funcion que recibe los datos del front y debe validar 
void handleFormRoot(AsyncWebServerRequest *request){
    DynamicJsonDocument reporte(1024);
    
    //obtengo los datos en formato String
    String desiredTempStr = request->arg("desiredTemperature");
    String desiredFreqStr = request->arg("desiredFrequency");

    //conversion a entero de los datos
    int dF = desiredFreqStr.toInt();
    int dT = desiredTempStr.toInt();

    /*deberia leer los datos actuales para compararlos con los que obtuve del front
       por el momento lo hare de forma estatica, se deberia leer los sensores
    int roomTemp = leerDHTExterno();
    int internalTemp = leerDHT11Interno();
    */

    int roomTemp = 23, internalTemp = 24;

    int condition;
    if (dT <= roomTemp) {
        condition = 1;
    } else if (internalTemp > dT) {
        condition = 2;
    } else {
        condition = 3;
    }

    switch(condition) {
        case 1:
            reporte["errorAmb"]="La temperatura ingresada no supera la temperatura ambiente";
            sendResponse(reporte, request);
            break;
        case 2:
            reporte["errorInt"]="La temperatura ingresada no supera la temperatura interna del recipiente";
            sendResponse(reporte, request);
            break;
        case 3:
            reporte["exito"]="La temperatura ingresada cumple las condiciones";
            sendResponse(reporte, request);
            break;
    }
}

//Funcion que recibe el formulario para "/on"
void handleFormOn(AsyncWebServerRequest *request){
    /*deberia volver a leer los datos actuales para actualizar la temperatura y el estado (COOLING, etc)
       por el momento lo hare de forma estatica, se deberia leer los sensores
    
    //sensar datos de temperatura externa, interna y estado del prototipo
    int roomTemp = leerDHTExterno();
    int intTemp = leerDHT11Interno();
    String state = leerEstado();
    */

    DynamicJsonDocument values(1024);
    int roomTemp = 23, intTemp = 27; //la temperatura interna deberia ir aumentando
    String state = "WARMING_UP", response = "";

    //llenar un JSON con los datos
    values["externalTemp"] = roomTemp;
    values["internalTemp"] = intTemp;
    values["prototypeState"] = state;
    serializeJson(values, response);

     //Incluir los datos del JSON y el archivo "on.html"
    request->send(200, "application/json", response);
}

void InitServer(){
    //Ruteo para "/"
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "index.html");
    });

    //Ruteo para "/" donde se modifican los valores
    server.on("/", HTTP_POST, handleFormRoot);

    //Ruteo para "/on", aqui se utilizaran los datos leidos iniciales
    server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "on.html");
    });

    //cuando quiera actualizar los datos se debe solicitar al /on/update
    server.on("/on/update", HTTP_GET, handleFormOn);
    
    //Ruteo para "/informe"
    server.on("/informe", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "informe.html");
    });
    
    //en caso que no encuentre la pagina
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(400, "text/plain", "Not found");
    });

    //Iniciar servidor
    server.begin();
    Serial.println("HTTP server started");
}

//Inicio del ESP en modo Access Point
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
  InitServer();
}

void loop(){
    //en el loop no debemos tener nada del servidor (server.handleClient()) ya que es asincrono
}
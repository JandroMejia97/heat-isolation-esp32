#include "http_server.h"

#if defined AP_SSID_SECRETS 
  const char* AP_SSID = AP_SSID_SECRETS;
#else
  #error "AP_SSID not defined. Please define it before uploading to your board."
#endif

#if defined AP_PASSWORD_SECRETS
  const char* AP_PASSWORD = AP_PASSWORD_SECRETS;
#else
  #error "AP_PASSWORD not defined. Please define it before uploading to your board."
#endif

IPAddress ip(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);;

AsyncWebServer server(80);

void sendResponse(DynamicJsonDocument doc, AsyncWebServerRequest *request){
    //crea la respuesta
    String response = "";
    serializeJson(doc, response);

    //enviar la respuesta
    if (doc.containsKey("error")) {
        request->send(400, "application/json", response);
    } else {
        request->send(200, "application/json", response);
    }
}

//Funcion que recibe los datos del front y debe validar 
void handleOnForm(AsyncWebServerRequest *request){
    DynamicJsonDocument reporte(1024);

    // Obtengo los datos en formato String
    String desiredTempStr = request->arg("desiredTemperature");
    String samplingFrequencyStr = request->arg("samplingFrequency");

    // Conversion a entero de los datos
    u_int8_t samplingFrequencyInSec = samplingFrequencyStr.toInt();
    u_int8_t desiredTemp = desiredTempStr.toInt();

    u_int8_t roomTemp = lroundf(getExternalTemp()), internalTemp = lroundf(getInternalTemp());

    if (desiredTemp <= roomTemp) {
        reporte["error"] = "La temperatura ingresada no supera la temperatura ambiente";
    } else if (internalTemp > desiredTemp) {
        reporte["error"] = "La temperatura ingresada no supera la temperatura interna del recipiente";
    } else {
        setDevStatus(ON);
        setDesiredTemp(desiredTemp);
        setCoolingDownTimeInMS(samplingFrequencyInSec * 1000);
        reporte["success"] = "La temperatura ingresada cumple las condiciones";
    }

    sendResponse(reporte, request);
}

void handleGetResults(AsyncWebServerRequest *request){
    QueueHandle_t results = getResults();

    if (results == NULL) {
        DynamicJsonDocument error(512);
        error["error"] = "No se pudo obtener la cola de resultados";
        sendResponse(error, request);
        return;
    }

    DynamicJsonDocument dataToSend(4096);
    dataToSend["data"] = JsonArray();
    do {
        DataToSend data;
        xQueueReceive(results, &data, 0);
        JsonObject obj = dataToSend["data"].createNestedObject();
        obj["internalTemp"] = data.internalTemp;
        obj["externalTemp"] = data.externalTemp;
    } while (uxQueueMessagesWaiting(results) > 0);
    
    sendResponse(dataToSend, request);
}

//Funcion que recibe el formulario para "/on"
void handleGetStatus(AsyncWebServerRequest *request){
    DynamicJsonDocument values(1024);
    String response = "";

    //llenar un JSON con los datos
    values["externalTemp"] = roundf(getExternalTemp());
    values["internalTemp"] = roundf(getInternalTemp());
    values["desiredTemp"] = getDesiredTemp();
    values["status"] = getDevStatusString();
    serializeJson(values, response);

     //Incluir los datos del JSON y el archivo "on.html"
    request->send(200, "application/json", response);
}

void initServer() {
    // Main page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        setDevStatus(OFF);
        request->send(SPIFFS, "/index.html");
    });

    // Routes for static files
    server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.js", "text/javascript");
    });

    server.on("/styles.css", HTTP_GET,[] (AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/styles.css", "text/css");
    });

    // Path to submit form data through an AJAX request
    server.on("/", HTTP_POST, handleOnForm);

    // On page, to turn on the device
    server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/on.html");
    });

    // Path to get the device status through an AJAX request
    server.on("/status", HTTP_GET, handleGetStatus);

    // Path to get the results through an AJAX request
    server.on("/results", HTTP_GET, handleGetResults);
    
    // Path to get the report page
    server.on("/report", HTTP_GET, [](AsyncWebServerRequest *request) {
        setDevStatus(OFF);
        request->send(SPIFFS, "/informe.html");
    });
    
    // Handle not found
    server.onNotFound([](AsyncWebServerRequest *request) {
        if (request->method() == HTTP_OPTIONS) {
            request->send(200);
        } else {
            request->send(404, "text/plain", "Not found");
        }
    });

    // Headers para CORS
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    //Iniciar servidor
    server.begin();
    Serial.println("HTTP server started");
}

//Inicio del ESP en modo Access Point
void connectWiFiAP(bool useStaticIP = false) { 
    WiFi.mode(WIFI_AP);
    Serial.println("Conectando");
    while(!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
        Serial.print(".");
        delay(100);
    }
    if(useStaticIP) {
        WiFi.softAPConfig(ip, gateway, subnet);
    }
    Serial.print("Iniciando AP:\t");
    Serial.println(AP_SSID);
    Serial.print("IP address:\t");
    Serial.println(WiFi.softAPIP());
}

void initialize() {
    connectWiFiAP();
    initServer();
    initHandler();
    if(!SPIFFS.begin(true)){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
}

void serverLoop() {
    loopHandler();
}

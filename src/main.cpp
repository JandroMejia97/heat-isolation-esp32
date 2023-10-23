 #include <WiFi.h>
 #include <Adafruit_MLX90614.h>


//------------------Servidor Web en puerto 80---------------------

WiFiServer server(80);

//---------------------Credenciales de WiFi-----------------------

const char* ssid     = "NADIA";
const char* password = "40393705";

//---------------------VARIABLES GLOBALES-------------------------
int contconexion = 0;

String header; // Variable para guardar el HTTP request

String estadoSalida = "off";

const int salida = 2;

//------------------------CODIGO HTML------------------------------
String pagina = "<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='utf-8' />"
"<meta name='viewport' content='width=device-width, initial-scale=1'>"
"<title>Servidor Web ESP32</title>"
"<style>"
"body {font-family: Arial, sans-serif; background-color: #f0f0f0; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; margin: 0; padding: 0;}"
"h1 {color: #000000;}"
"h2 {color: #50a8a0; font-size: 14px;}"
"table {width: 100%;}"
"td {text-align: center;}"
"button.on {background-color: #4CAF50; border: none; color: white; padding: 15px 32px;"
"text-decoration: none; display: inline-block; font-size: 16px; margin: auto; cursor: pointer; border-radius: 12px;}"
"button.off {background-color: #f44336; border: none; color: white; padding: 15px 32px;"
"text-decoration: none; display: inline-block; font-size: 16px; margin: auto; cursor: pointer; border-radius: 12px;}"
"</style>"
"</head>"
"<body>"
"<h1>Servidor Web ESP32</h1>"
"<h2>Control de Alimentación</h2>"
"<table>"
"<tr><td><a href='/on'><button class='on'>ON</button></a></td></tr>"
"<tr><td><a href='/off'><button class='off'>OFF</button></a></td></tr>"
"</table>"
"<h2>Control DHT11 Interno</h2>"
"<table>"
"<tr><td><a href='/dht11Inton'><button class='on'>DHT11 ON</button></a></td></tr>"
"<tr><td><a href='/dht11Intoff'><button class='off'>DHT11 OFF</button></a></td></tr>"
"</table>"
"<h2>Control DHT11 Externo</h2>"
"<table>"
"<tr><td><a href='/dht11Exton'><button class='on'>DHT11 ON</button></a></td></tr>"
"<tr><td><a href='/dht11Extoff'><button class='off'>DHT11 OFF</button></a></td></tr>"
"</table>"
"<h2>Control de Sensor Infrarrojo</h2>"
"<table>"
"<tr><td><a href='/SIon'><button class='on'>ON</button></a></td></tr>"
"<tr><td><a href='/SIoff'><button class='off'>OFF</button></a></td></tr>"
"</table>"
"</body>"
"</html>";


//---------------------------SETUP--------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("");
  
  pinMode(salida, OUTPUT); 
  digitalWrite(salida, LOW);

  // Conexión WIFI
  WiFi.begin(ssid, password);
  //Cuenta hasta 50 si no se puede conectar lo cancela
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { 
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion <50) {
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
      server.begin(); // iniciamos el servidor
  }
  else { 
      Serial.println("");
      Serial.println("Error de conexion");
  }
}

//----------------------------LOOP----------------------------------

void loop(){
  WiFiClient client = server.available();   // Escucha a los clientes entrantes

  if (client) {                             // Si se conecta un nuevo cliente
    Serial.println("Nuevo Cliente.");          // 
    String currentLine = "";                //
    while (client.connected()) {            // loop mientras el cliente está conectado
      if (client.available()) {             // si hay bytes para leer desde el cliente
        char c = client.read();             // lee un byte
        Serial.write(c);                    // imprime ese byte en el monitor serial
        header += c;
        if (c == '\n') {                    // si el byte es un caracter de salto de linea
          // si la nueva linea está en blanco significa que es el fin del 
          // HTTP request del cliente, entonces respondemos:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // enciende y apaga el GPIO
            if (header.indexOf("GET /on") >= 0) {
              Serial.println("GPIO on");
              estadoSalida = "on";
              digitalWrite(salida, HIGH);
            } else if (header.indexOf("GET /off") >= 0) {
              Serial.println("GPIO off");
              estadoSalida = "off";
              digitalWrite(salida, LOW);
            }
            
            // Muestra la página web
            client.println(pagina);
            
            // la respuesta HTTP temina con una linea en blanco
            client.println();
            break;
          } else { // si tenemos una nueva linea limpiamos currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // si C es distinto al caracter de retorno de carro
          currentLine += c;      // lo agrega al final de currentLine
        }
      }
    }
    // Limpiamos la variable header
    header = "";
    // Cerramos la conexión
    client.stop();
    Serial.println("Cliente desconectado.");
    Serial.println("");
  }
}
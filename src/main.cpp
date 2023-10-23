 #include <Adafruit_MLX90614.h>
 #include <WebServer.h>

/*

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };
}

void loop() {
  Serial.print("Ambient temperature = "); 
  Serial.print(mlx.readAmbientTempC());
  Serial.print("°C");      
  Serial.print("   ");
  Serial.print("Object temperature = "); 
  Serial.print(mlx.readObjectTempC()); 
  Serial.println("°C");
  
  Serial.print("Ambient temperature = ");
  Serial.print(mlx.readAmbientTempF());
  Serial.print("°F");      
  Serial.print("   ");
  Serial.print("Object temperature = "); 
  Serial.print(mlx.readObjectTempF()); 
  Serial.println("°F");

  Serial.println("-----------------------------------------------------------------");
  delay(1000);
} */

#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include <Ticker.h>

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif

/*
* Servidor web con ESP32
* Librerias necesarias para conectarnos a un entorno Wifi y poder configurar  
* un servidor WEB
*/
#include <WiFi.h>
#include <WebServer.h>

/* Añade tu SSID & Clave para acceder a tu Wifi */
const char* ssid = "Telecentro-23b0";  // Tu SSID
const char* password = "Torres54321";  //Tu Clave

/*
    Declaramos un objeto de la libreria WebServer para poder acceder a sus funciones
    Y como parametro 80, que es el puerto estandar de todos los servicios WEB HTTP
*/
WebServer server(80);

/*
   Declaramos el estado inicial de los LEDs del ESP32
*/
uint8_t LED1pin = 4;
bool LED1Estado = LOW;
uint8_t LED2pin = 5;
bool LED2Estado = LOW;

/*
   Todos las siguientes funciones ejecutan tres tareas:
   1 Cambian de estado las variables  de los LED
   2 Muestran por el monitor Serial de Arduino IDE, informacion relevante al estado de los LED
   3 Actualizan la vista de la pagina del servidor WEB con ESP32, envia al navegador un codigo 200 indican el exito
   de la conexion y llama a otra funcion SendHTML con dos parametros que modificaran la pagina 
   del servidor WEB con Arduino.
*/
void handle_OnConnect() {
  LED1Estado = LOW; // 1
  LED2Estado = LOW; // 1
  Serial.println("GPIO4 Estado: OFF | GPIO5 Estado: OFF"); // 2
  //server.send(200, "text/html", SendHTML(LED1Estado, LED2Estado)); // 3
}

void handle_led1on() {
  LED1Estado = HIGH; //1
  Serial.println("GPIO4 Estado: ON"); // 2
  //server.send(200, "text/html", SendHTML(true, LED2Estado)); //3
}

void handle_led1off() {
  LED1Estado = LOW;
  Serial.println("GPIO4 Estado: OFF");
  //server.send(200, "text/html", SendHTML(false, LED2Estado));
}

void handle_led2on() {
  LED2Estado = HIGH;
  Serial.println("GPIO5 Estado: ON");
  //server.send(200, "text/html", SendHTML(LED1Estado, true));
}

void handle_led2off() {
  LED2Estado = LOW;
  Serial.println("GPIO5 Estado: OFF");
  //server.send(200, "text/html", SendHTML(LED1Estado, false));
}

void handle_NotFound() {
  server.send(404, "text/plain", "La pagina no existe");
}

/*
   Aqui esta definido todo el HTML y el CSS del servidor WEB con ESP32
*/
String SendHTML(uint8_t led1stat, uint8_t led2stat) {
  // Cabecera de todas las paginas WEB
  String ptr = "<!DOCTYPE html> <html>\n";
  
  // <meta> viewport. Para que la pagina se vea correctamente en cualquier dispositivo
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Control LED</title>\n";
  
/*
 * El estilo de la pagina WEB, tipo de letra, tamaño, colores, 
 * El estilos de los botones (las clases en CSS) y la definicion de como van a cambiar dependiendo de como
 * cambien los estado de los LEDs, color fondo etc
 */
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #3498db;}\n";
  ptr += ".button-on:active {background-color: #2980b9;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  /*
   * Encabezados de la pagina
   */
  ptr += "<h1>ESP32 Servidor WEB</h1>\n";
  ptr += "<h3>Usando Modo Estacion</h3>\n";
/*
 * Aqui esta la inteligencia del servidor WEB con ESP32, dependiento de los parametros de la funcion SendHTML
 * modificara la vista de la pagina WEB,  llamaran a las clases "button-on y button-off" que cambian como
 * se muestran los datos en la pagina WEB 
 */
  if (led1stat)
  {
    ptr += "<p>LED1 Estado: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";
  }
  else
  {
    ptr += "<p>LED1 Estado: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";
  }

  if (led2stat)
  {
    ptr += "<p>LED2 Estado: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";
  }
  else
  {
    ptr += "<p>LED2 Estado: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";
  }

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

void setup() {
  /*
   * Declaracion de la velocidad de comunicacion entre Arduino IDE y ESP32
   * Configura el comportamiento de los pines
   */
  Serial.begin(115200);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);
/*
 * Configuracion de la conexion a la Wifi de tu casa
 */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado a ");
  Serial.println(ssid);
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());
/*
 * Para procesar las solicitudes HTTP necesitamos definir el codigo que debe de ejecutar en
 * cada estado. Para ello utilizamos el metodo "on" de la libreria WebServer que hemos 
 * habilitador en la linea 13 de este codigo
 * 1 El primero se ejecuta cuando te conectas al Servidor WEB con ESP32 http://la_ip_del_esp32/
 * 2 Los 4 siguientes procesan los 2 estados que puede tener cada LED ON/OFF
 * 3 El ultimo gestiona los errores por ejemplo si pones http://la_ip_del_esp32/holaquetal
 * esta pagina no existe, por lo tanto actualizara la pagina WEB con un mensaje de error
 */
  server.on("/", handle_OnConnect); // 1
  server.on("/led1on", handle_led1on); // 2
  server.on("/led1off", handle_led1off); // 2
  server.on("/led2on", handle_led2on); // 2
  server.on("/led2off", handle_led2off); // 2
  server.onNotFound(handle_NotFound); // 3
/*
 * Arrancamos el Servicio WEB
 */
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}
/*
 * Para gestionar las la peticiones HTTP es necesario llamar al metodo "handleClient"
 * de la libreria WebServer que se encarga de recibir las peticiones y lanzar las fuciones
 * de callback asociadas tipo "handle_led1on()" "handle_led2on()" etc
 * Tambien ejecutan el cambio de estado de los pines y por lo tanto hacen que los 
 * LEDs se enciendan o apaguen
 */
void loop() {
  server.handleClient();
}
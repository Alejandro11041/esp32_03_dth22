#include <Arduino.h>
#include "libwifi.h"
#include <WiFi.h>
#include "DHT.h"

#define DHTPIN 4  //Pin de datos del sensor DHT11/22

#define DHTTYPE DHT11  //Especificamos el tipo de sensor DHT11 DHT22 DHT21

DHT dht(DHTPIN, DHTTYPE);  //Creamos el objeto que representa al sensor

const char * ssid = "virus";
const char * password = "a1b2c3d4";
const char * host = "dweet.io";
const int puerto = 80;
String url = "/dweet/for/uceva00?temperatura=";

//@author Alejandro
/**
 * @brief Esta es la funcion de configuracion del dispositivo
 * 
 */

void setup() {
  Serial.begin(9600);
  Serial.println("Ensayo del DHT22!");
  dht.begin();
  pinMode(2, OUTPUT);  //Coloco el pin 2 como salida
  Serial.begin(115200);
  Serial.println("Inicializando dispositivo");
  conectarWifi(ssid, password);
}

/**
 * @brief Esta funcion es un bucle infinito que corresponde al main()
 * 
 */
void loop() {
  WiFiClient cliente; //Creamos un cliente TCP por wifi

   //Bloque del DHT22
   delay(2000);
   float humedad = dht.readHumidity(); //Leemos la humedad en % de humedad relativa (0 al 100%)
   float temperatura = dht.readTemperature(); //Leemos la temperatura en °C
   float tempFarenheit = dht.readTemperature(true); //Leemos la temperatura en grados farenheit
   
   //Verificamos las lecturas del sensor
   if(isnan(humedad) || isnan(temperatura) || isnan(tempFarenheit)){
     Serial.println("Fallo la lectura del sensor");
     return;
   }

   //Calculamos la sensacion termica o indice de calor
   float indice = dht.computeHeatIndex(tempFarenheit, humedad);

   //Calculamos la sensacion termica o indice de calor en celsius
   float indiceCelsius = dht.computeHeatIndex(temperatura, humedad, false);

   Serial.print(F("Humedad: ")); //La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
   Serial.print(humedad);
   Serial.print(F("%  Temperatura: ")); //La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
   Serial.print(temperatura);
   Serial.print(F("°C  Indice de calor: ")); //La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
   Serial.print(indice);
   Serial.print(F("°F  Indice de calor: ")); //La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
   Serial.print(indiceCelsius);
   Serial.println(F("°C")); //La F() siginifica que lo que esta dentro del parentesis se escribe en la FLASH
  //TERMINA EL BLOQUE DEL DHT22 


  //Bloque que realiza la conexion al servidor
  if(!cliente.connect(host, puerto)){
    Serial.println("Error conexion al host fallida");
    delay(2000);
    return;
  }

  //Peticion (request) GET al servidor http
  cliente.print("GET "+url+temperatura+" HTTP/1.1\r\nHost: "+String(host)+"\r\n"+"Connection: close\r\n\r\n");

  //Debemos darle un tiempo al servidor a que responda (response) la peticion
  //delay(5000); //No funciona aqui
  unsigned long milisegundos = millis(); //Hora de inicio
  while(cliente.available()==0){  //Preguntamos si no hay datos recibidos disponibles
      if(millis()-milisegundos > 5000){
         Serial.println("Se expiro el tiempo de la conexion");
         cliente.stop();
      }
  }

  while(cliente.available()){
    String linea = cliente.readStringUntil('\r');
    Serial.println(linea);
  }

  Serial.println("Fin de la conexion");
  cliente.stop();

  delay(2000);  //Espero 2s
}
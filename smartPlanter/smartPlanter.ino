

//********************************************************************************************
//********************************** Smart Oddish Planter ************************************
//*                                                                                          *
//*                                 by Ruben Martin Garcia                                   *
//*                                     January, 2019                                        *
//*                                                                                          *
//********************************************************************************************



//********************************************************************************************
//************************************ Librerias *********************************************
//********************************************************************************************
#include <Wire.h>  
#include <BME280I2C.h>          

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//********************************************************************************************
//**************************** WiFi Access Point *********************************************
//********************************************************************************************


const char* ssid = "TP-LINK_058CDE";
const char* password =  "45007200";


WiFiClient espClient;

//********************************************************************************************
//********************************** MQTT Setup **********************************************
//********************************************************************************************

const char* mqtt_server = "m20.cloudmqtt.com";
const int mqtt_port= 17161;
const char* mqtt_user = "yjqmjetm";
const char* mqtt_password = "gZlSODxPzTie";


PubSubClient clientMQTT(espClient);

//********************************************************************************************
//*************************************  Pinout **********************************************
//********************************************************************************************


//Sensor de humedad en tierra
const int lightPin = D3;

//Sensor de humedad en tierra
const int soilPin = A0;

//Sensor BME280 de temperatura,humedad y presion
BME280I2C bme;  

//Unidades de medicion para el sensor BME
BME280::TempUnit tempUnit(BME280::TempUnit_Celsius); //Celsius
BME280::PresUnit presUnit(BME280::PresUnit_hPa); //hPascales


//********************************************************************************************
//*************************************  Variables *******************************************
//********************************************************************************************

//ID de la planta ( se genera automaticamente)
int oddishId= 0;

//Variable para guardar el valor del sensor de tierra
int soil = 0;


//Variables para almacenar las mediciones del sensor BME
float hum=0;
float temp=0;
float presion=0;

//Variable para guardar el estado de la luz
int lightState= 0;

//MQTT
String subtopic="";
String msg="";

//Delay
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;       
const long interval = 5000; 





//********************************************************************************************
//************************************  Setup  ***********************************************
//********************************************************************************************

void setup()
{
  //OddishID
  //Obtenemos el identificador del chip
  oddishId = ESP.getChipId();
  
  //Abrir puerto serie
  Serial.begin(115200);
  Wire.begin();

  //Inicializacion luz
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(lightPin, OUTPUT);

  //Inicializacion sensores
  pinMode(soilPin, INPUT);
  bme.begin();
  
  //Nos conectamos a la red Wifi
  WifiReconnect();

  //MQTT setup
  clientMQTT.setServer(mqtt_server, mqtt_port);
  clientMQTT.setCallback(callback);
  
  if ( WiFi.status() == WL_CONNECTED )
  {
    MQTTreconnect();
    delay(500);
    MQTTsubscribeAll();
  }

  //Mensajes de inicio
  Serial.println("Ok");
  Serial.println();
  Serial.print("Id Oddish: ");
  Serial.println(oddishId);
  Serial.println();
  

}


//********************************************************************************************
//************************************  Loop  ************************************************
//********************************************************************************************
void loop()
{
    //Nos reconectamos a la wifi si hemos perdido la conexion
    if (WiFi.status() != WL_CONNECTED) {
      WifiReconnect();
    }
    
    //Reconexion a MQTT si la hemos perdido
    if ( WiFi.status() == WL_CONNECTED && !clientMQTT.connected() ) 
    {
      MQTTreconnect();
    }
    else
    {
      //Bucle para lectura de mensajes entrantes
      clientMQTT.loop();
    }


    //Muestreo de sensores
    //Se realiza cada 5000 milisegundos 0 5 segundos
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) 
    {
      previousMillis = currentMillis;

      //Medicion de sensores
      //Soil
      soil=readSoil();//mode filter
      
      //Temperatura,humedad y presion
      bme.read(presion, temp, hum, tempUnit, presUnit); 


      //Mostrar informacion
      Serial.println();
      Serial.print("Soil: ");Serial.println(soil);
      Serial.print("Presion: ");Serial.println(presion);
      Serial.print("Temperatura: ");Serial.println(temp);
      Serial.print("Humedad: ");Serial.println(hum);    
      Serial.println();
      

      //enviar información
      if ( WiFi.status() == WL_CONNECTED )
      {
        sendStateMQTT();
      }
  }

}



//********************************************************************************************
//*********************************  Functions  **********************************************
//********************************************************************************************

//Esta funcion se encarga de leer el sensor de suelo
int readSoil()
{
  int m = analogRead(soilPin); 
  //Serial.print("Analog: ");Serial.println(m);

  //Ajustamos el porcentaje de salida de 0 a 100 telniendo en cuenta la calibracion
  m = map(m, 115,215, 100, 0);//3.3v

  //Ajustamos los limites
  if( m < 0 )   m = 0;
  if( m > 100 ) m = 100;

  //Devolvemos la medida
  return m;
}

//Esta funcion se encarga de mandar a traves de internet los datos de los sensores
void sendStateMQTT()
{
  //Datos a enviar
  MQTTsend("soil", (String)soil);

  MQTTsend("temp", (String)temp);
  MQTTsend("hum", (String)hum);
  MQTTsend("presion", (String)presion);


}

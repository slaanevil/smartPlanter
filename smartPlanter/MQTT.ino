

//************************************************************************************
//************************************************************************************

//                                         MQTT

//************************************************************************************
//************************************************************************************

/****************************** Serial variables ***************************************/

#define SIZEBUFFERS 64

char topicBuffer[SIZEBUFFERS]="";
char msgBuffer[SIZEBUFFERS]="";
char subTopicBuffer[SIZEBUFFERS]="";

//int inIndex=0;

#define MAXSUBS 20
int subs=0;

char subTopic[MAXSUBS][SIZEBUFFERS];

#define TIMEOUT 500 //ms
unsigned long timeLast = 0;

unsigned int MQTTintentos = 0;


/** void callback(char* topic, byte* payload, unsigned int length) 
 * 
 *  Esta funcion es llamada cuando se recibe un mensaje
 *  
 *  Es la encargada de enviar el mensaje por el puerto serie
 *  
 *  Parametros:
 *    char* topic: topic del que se ha recibido el mensaje
 *    byte* payload: mensaje recibido
 *    unsigned int length: tamaño del mensaje recibido
 */
void callback(char* topic, byte* payload, unsigned int length) 
{
  //digitalWrite(LED_BUILTIN, LOW); //Activamos led

  String subtopic=(String)topic;
  String msg="";

  
  //Write topic
  Serial.print("Mensaje recibido: ");
  Serial.print(topic);
  Serial.print(",");
  
  
  //Write msg in serial port
  for (int i = 0; i < length; i++) {
    msg=msg+(char)payload[i];
    //Serial.print((char)payload[i]);
  }
  Serial.print(msg);
  Serial.print(";");
  Serial.println();

  
  subtopic=subtopic.substring(subtopic.lastIndexOf('/')+1);

  checkCommand(subtopic,msg);
  

  //digitalWrite(LED_BUILTIN, HIGH); //Desactivamos led

}


/** void reconnect()
 *  
 *  Funcion que se encarga de conectar con el servidor MQTT
 *  Si se ha desconectado, vuelve a realizar la conexion
 *  
 *  Vuelve a subscribirse a todos los topic
 * 
 */
void MQTTreconnect() {
  Serial.println("Connecting MQTT");
  // Loop until we're reconnected

  MQTTintentos=0;
  while (!clientMQTT.connected() && MQTTintentos <= 10) 
  {
    MQTTintentos++;
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    bool MQTTstatus;

    
    if ( clientMQTT.connect(clientId.c_str(),mqtt_user,mqtt_password) ) 
    {
      //resubscribe
      for(int i =0; i< subs; i++)
      {
        clientMQTT.subscribe(subTopic[i]);
        Serial.print("Subscribe ");
        Serial.println(subTopic[i]);
      }
    } else 
    {
      delay(500);
      Serial.print(".");
    }
  }

  if( clientMQTT.connected() )
  {
    Serial.println("");
    Serial.println("MQTT conectado");
    Serial.println("");
  }
  else
  {
    Serial.println("");
    Serial.println("MQTT NO conectado");
    Serial.println("");
  }
}


/** void addSubscription(char subsTopic[])
 * 
 *  Añade un topic a la lista de subscripciones
 *  
 *  Parametros:
 *    char subsTopic[] : topic al que se va a subscribir
 */
void addSubscription(char subsTopic[])
{
  
  for(int i =0 ; i < subs ; i++)
  {
    if(strcmp(subTopic[i], subsTopic)  == 0){ return; }//Ya existe el elemento y no se añade a la lista
  }
  
  //Asignamos el mensaje al topic a subscribirse
  strcpy(subTopic[subs],subsTopic);
  subs++;

  //Si esta conectado nos subscribimos en este momento, si no
  //esperamos a conectar
  if(clientMQTT.connected())
  {
    Serial.print("Subcribe to: ");
    Serial.println(subsTopic);
    clientMQTT.subscribe(subsTopic);
  }
}


void MQTTsubscribeAll()
{
  //Subscribe MQTT
  char aux[20];
  sprintf(aux,"/SEG/%d/c/#",oddishId);
  addSubscription(aux);
}


void checkCommand(String subtopic,String msg)
{
   Serial.print("Cmd Recibido: ");Serial.print(subtopic);Serial.print(",");Serial.println(msg);
    if(subtopic == "lightSt")
    {
      lightState=msg.toInt();
    }
}


void MQTTsend(String subtopic, String msg)
{
  //Traducimos String a char
  subtopic.toCharArray(subTopicBuffer, SIZEBUFFERS);
  msg.toCharArray(msgBuffer, SIZEBUFFERS);

  //Formateamos el topic
  sprintf(topicBuffer, "/Oddish/%d/s/%s", oddishId,subTopicBuffer);

  //Enviamos el mensaje
  clientMQTT.publish(topicBuffer,msgBuffer);

  //Mostramos por serie el mensaje enviado
  Serial.print("Enviando:");
  Serial.print(topicBuffer);
  Serial.print(",");
  Serial.print(msgBuffer);
  Serial.println(";");
}


void MQTTsubscribe(String subtopic)
{
  //Subscribe MQTT
  //serialMqtt.print("subs,/bonsai");
  Serial.print("subs,/SEG/");
  Serial.print(oddishId);
  Serial.print("/c/");
  Serial.print(subtopic);
  Serial.print(";");
}




//********************************************************************************************
//*************************************** MQTT ***********************************************
//********************************************************************************************


//Numero de caracteres maximos por buffer
#define SIZEBUFFERS 64

//Buffers de mensajes
char topicBuffer[SIZEBUFFERS]="";
char msgBuffer[SIZEBUFFERS]="";
char subTopicBuffer[SIZEBUFFERS]="";

//Numero maximo de subscripciones
#define MAXSUBS 20
int subs=0;

//Lista de todas las subscripciones
char subTopic[MAXSUBS][SIZEBUFFERS];

//Tiempo de timeout en conexion
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
  //Activamos led de aviso
  digitalWrite(LED_BUILTIN, LOW); 

  //Traducimos el mensaje recibido en String
  String subtopic=(String)topic;
  String msg="";

  //Mostramos el topic del mensaje recibido
  Serial.print("Mensaje recibido: ");
  Serial.print(topic);
  Serial.print(",");
  
  
  //Traducimos el mensaje a un String
  for (int i = 0; i < length; i++) {
    msg=msg+(char)payload[i];
    //Serial.print((char)payload[i]);
  }

  //Mostramos el mensaje
  Serial.print(msg);
  Serial.print(";");
  Serial.println();

  //Obtenemos el subtopic dentro del topic
  subtopic=subtopic.substring(subtopic.lastIndexOf('/')+1);

  //Pasamos el subtopic y el mensaje a la siguiente funcion para ejecutar la
  //tarea correspondiente
  checkCommand(subtopic,msg);
  
  //Apagamos el led
  digitalWrite(LED_BUILTIN, HIGH); 

}


/** void reconnect()
 *  
 *  Funcion que se encarga de conectar con el servidor MQTT
 *  Si se ha desconectado, vuelve a realizar la conexion
 *  
 *  Vuelve a subscribirse a todos los topic
 * 
 */
void MQTTreconnect() 
{
  
  Serial.println("Conectando al servidor MQTT...");

  //Bucle que se repite mientras no haya una conexion
  MQTTintentos=0;
  while (!clientMQTT.connected() && MQTTintentos <= 10) 
  {
    MQTTintentos++;
    
    // Creamos el Id de cliente
    String clientId = "Oddish_";
    clientId += oddishId;
    
    // Variable para guardar el estado de la conexion
    bool MQTTstatus;

    //Conexion
    if ( clientMQTT.connect(clientId.c_str(),mqtt_user,mqtt_password) ) 
    {
      //resubscribir a todos los topics
      for(int i =0; i< subs; i++)
      {
        clientMQTT.subscribe(subTopic[i]);
        Serial.print("Subscribe ");
        Serial.println(subTopic[i]);
      }
    } 
    else 
    {
      //Reconexion en 500 milisegundos
      delay(500);
      Serial.print(".");
    }
  }


  //Mostramos mensaje pasados todos los intentos
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


/** void MQTTsend(String subtopic, String msg)
 * 
 *  Envia un mensaje al servidor MQTT, dado un topic y un mensaje
 *  
 */
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


/** void addSubscription(char subsTopic[])
 * 
 *  Añade un topic a la lista de subscripciones
 *  
 *  Parametros:
 *    char subsTopic[] : topic al que se va a subscribir
 */
void addSubscription(char subsTopic[])
{
  //COmprobamos si ya existe el topic
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

/** void MQTTsubscribeAll()
 * 
 *  Se subscribe al patron generico para recibir todos los mensajes
 *  
 */
void MQTTsubscribeAll()
{
  //Subscribe MQTT
  char aux[20];
  sprintf(aux,"/Oddish/%d/c/#",oddishId);
  addSubscription(aux);
}



/** void checkCommand(String subtopic,String msg)
 * 
 *  Realiza la tarea correspondiente al mensaje recibido
 *  
 */
void checkCommand(String subtopic,String msg)
{
    //Opcion para encender la luz
    if(subtopic == "lightSt")
    {
      //Cambiamos el estado
      lightState=msg.toInt();

      //Encendemos o apagamos
      if(lightState)
      {
        digitalWrite(lightPin,HIGH);
      }
      else
      {
        digitalWrite(lightPin,LOW);
      }
    }
}

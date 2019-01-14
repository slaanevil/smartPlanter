

//********************************************************************************************
//*************************************** Wifi ***********************************************
//********************************************************************************************


unsigned int Wifiintentos = 0;



/** void reconnect()
 *  
 *  Funcion que se encarga de conectar el modulo a la wifi
 * 
 */
void WifiReconnect() 
{
  Serial.println();
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  //Inicializamos
  WiFi.begin(ssid, password);

  //Conectamos
  Wifiintentos = 0;
  while (WiFi.status() != WL_CONNECTED && Wifiintentos < 10) 
  {
    Wifiintentos++;
    delay(500);
    Serial.print(".");
  }

  //Mensaje de conexion
  if( WiFi.status() == WL_CONNECTED )
  {
    Serial.println("");
    Serial.println("WiFi conectada");
    Serial.println("IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("");
    Serial.println("WiFi no conectada");
  }
  
}

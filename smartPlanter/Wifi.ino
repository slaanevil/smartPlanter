
unsigned int Wifiintentos = 0;

void WifiReconnect() 
{
  Serial.println();
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  Serial.print("*");
  
  Wifiintentos = 0;
  while (WiFi.status() != WL_CONNECTED && Wifiintentos < 10) 
  {
    Wifiintentos++;
    delay(500);
    Serial.print(".");
  }

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

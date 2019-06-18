void Menssagem(float pressao, float oxigenio) {

  sensor.requestTemperatures();
  temp = sensor.getTempCByIndex(0);

    Serial.print("bpm: ");
  Serial.println(pressao);
  Serial.print("oxigenio: ");
  Serial.println(oxigenio);
  Serial.print("temperatura: ");
  Serial.println(temp);

  HTTPClient http;    //Declare object of class HTTPClient

  StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["chaveDispositivo"] = local_mac;
  JSONencoder["pressao"] = pressao;
  JSONencoder["oxgenio"] = oxigenio;
  JSONencoder["temperatura"] = temp;


  char JSONmessageBuffer[300];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  //Serial.println(JSONmessageBuffer);

  http.begin("http://10.20.30.25:8080/eventos");
  http.addHeader("Content-Type", "application/json"); //Specify content-type header

  int httpCode = http.POST(JSONmessageBuffer);

  //Serial.println(httpCode);
  http.end();
  //if(httpCode == 201)
  exit(0);
}

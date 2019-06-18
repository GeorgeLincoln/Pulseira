#include <ArduinoJson.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <MAX30100_PulseOximeter.h>

ESP8266WebServer server(8080);

#define REPORTING_PERIOD_MS     2000

String local_mac;

PulseOximeter pox;

float pressao, oxigenio, anterior_P = 0, anterior_O = 0;
uint32_t tsLastReport = 0;

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
  //Serial.println("Beat!");
}

void setup()
{
  Serial.begin(115200);                            //Serial connection

  pox.setOnBeatDetectedCallback(onBeatDetected);

  WiFi.begin("LAR_2_BW", "lar2-ifce");   //WiFi connection

  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion

    delay(500);
    Serial.println("Waiting for connection");

  }
  local_mac = WiFi.macAddress();

  Serial.print("Initializing pulse oximeter..");

  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else Serial.println("SUCCESS");
}
int i = 0;
//float anterior = 0, pressao = 0;
void loop() {

  // Make sure to call update as fast as possible
  //Sensor();
  //anterior = pox.getHeartRate();

  pox.update();

  if (pox.getHeartRate() != anterior_P && pox.getSpO2() != anterior_P){
    i++;
    pox.update();
  }
  

  if (i > 300) {
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

      Serial.println(pox.getHeartRate());
      Serial.println(pox.getSpO2());
      pressao = pox.getHeartRate();
      oxigenio = pox.getSpO2();
      
      anterior_P = pressao;
      anterior_O = oxigenio;
      
      //Serial.println(oxigenio);

      tsLastReport = millis();
      //Serial.println(tsLastReport);
      i = 0;
      Menssagem(pressao, oxigenio);
    }
  }
}

void Menssagem(float pressao, float oxigenio) {

  HTTPClient http;    //Declare object of class HTTPClient

  StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["chaveDispositivo"] = local_mac;
  JSONencoder["pressao"] = pressao;
  JSONencoder["temperatura"] = oxigenio;


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

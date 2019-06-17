#include <ArduinoJson.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <MAX30100_PulseOximeter.h>

ESP8266WebServer server(8080);

#define REPORTING_PERIOD_MS     2000

String mac;

PulseOximeter pox;

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

  WiFi.begin("Jeferson Oliveira", "printfcomandoseguro");   //WiFi connection

  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion

    delay(500);
    Serial.println("Waiting for connection");

  }
  mac = WiFi.macAddress();

  Serial.print("Initializing pulse oximeter..");

  // Initialize the PulseOximeter instance
  // Failures are generally due to an improper I2C wiring, missing power supply
  // or wrong target chip
  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;);
  } else Serial.println("SUCCESS");
}

//float anterior = 0, pressao = 0;
void loop() {
  // Make sure to call update as fast as possible
  //Sensor();
  //anterior = pox.getHeartRate();
  pox.update();

  //Serial.println(pox.getHeartRate());
  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

    pressao = pox.getHeartRate();
    Sensor(pressao);

    //Serial.println(oxigenio);

    tsLastReport = millis();
    //Serial.println(tsLastReport);

  }
}

void Sensor(float pressao) {

  HTTPClient http;    //Declare object of class HTTPClient

  StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
  JsonObject& JSONencoder = JSONbuffer.createObject();

  JSONencoder["valorAuferido"] = pressao;
  JSONencoder["tipoValor"] = "pressao";
  JSONencoder["chaveDispositivo"] = mac;

  char JSONmessageBuffer[1000];
  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);

  http.begin("http://10.70.80.107:8080/eventos");
  http.addHeader("Content-Type", "application/json"); //Specify content-type header

  int httpCode = http.POST(JSONmessageBuffer);

  //Serial.println(httpCode);
  http.end();
  //if(httpCode == 201)
  exit(0);
  //else  Sensor();
}

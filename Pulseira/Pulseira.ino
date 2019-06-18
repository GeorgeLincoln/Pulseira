#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <MAX30100_PulseOximeter.h>

ESP8266WebServer server(8080);

#define REPORTING_PERIOD_MS     2000

String local_mac;


OneWire barramento(D4);
DallasTemperature sensor(&barramento);

PulseOximeter pox;

int i = 0;
float pressao, oxigenio;
float temp = 0, anterior_P = 0, anterior_O = 0;
uint32_t tsLastReport = 0;

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
  //Serial.println("Beat!");
}

void setup()
{
  Serial.begin(115200);                            //Serial connection

  pox.setIRLedCurrent(MAX30100_LED_CURR_14_2MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);

  WiFi.begin("LAR_2_BW", "lar2-ifce");   //WiFi connection

  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion

    delay(500);
    Serial.println("Waiting for connection");

  }
  local_mac = WiFi.macAddress();
  sensor.begin(); //temperatura

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
  pox.update();
   // temperatura

  Serial.print("bpm: ");
  Serial.println(pox.getHeartRate());
  Serial.print("oxigenio: ");
  Serial.println(pox.getSpO2());
  Serial.print("temperatura: ");
  Serial.println(sensor.getTempCByIndex(0));
  if (pox.getHeartRate() != anterior_P && pox.getSpO2() != anterior_P) {
    i++;
    pox.update();
  }


  if (i > 300) {
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

      
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

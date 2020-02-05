#include <Arduino.h>

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>
#include <Arduino_JSON.h>

// Replace with your network credentials
const char* ssid = "Boszorkany";
const char* password = "Kucko725B";

// Set LED GPIO
const int LED_PIN = 2;
// LED connection is inverted
const int LED_ON = 0;
const int LED_OFF = 1;

// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String getTemperature() {
  float temperature = random(100);
  Serial.println(temperature);
  return String(temperature);
}

String getHumidity() {
  float humidity = random(100);
  Serial.println(humidity);
  return String(humidity);
}

String getPressure() {
  float pressure = random(100);
  Serial.println(pressure);
  return String(pressure);
}

String getDataJSON() {
  JSONVar myObject;

  myObject["ledState"] = !digitalRead(LED_PIN);  // invertált bekötés
  myObject["time"] = millis() / 1000;
  myObject["value"] = random(100);

  String retVal = JSON.stringify(myObject);

  Serial.println(retVal);
  return String(retVal);
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.onNotFound([](AsyncWebServerRequest * request) {
    Serial.println("** NOT FOUND **");
//    request->redirect("/");
      request->send(SPIFFS, "/index.html", "text/html");
  });








  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/styles.css", "text/css");
  });

  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/main.js", "text/javascript");
  });

  server.on("/polyfills.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/polyfills.js", "text/javascript");
  });

  server.on("/runtime.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/runtime.js", "text/javascript");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.ico", "image/vnd.microsoft.icon");
  });






  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(LED_PIN, LED_ON);
    request->send_P(200, "text/plain", getDataJSON().c_str());
  });

  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(LED_PIN, LED_OFF);
    request->send_P(200, "text/plain", getDataJSON().c_str());
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getTemperature().c_str());
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getHumidity().c_str());
  });

  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getPressure().c_str());
  });

  server.on("/rest", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getDataJSON().c_str());
  });





  // Start server
  server.begin();
}

void loop() {

}

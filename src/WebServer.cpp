#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>
#include <Arduino_JSON.h>
#include <Servo.h>

// Replace with your network credentials
const char* ssid = "Boszorkany";
const char* password = "Kucko725B";

// Set LED GPIO
const int LED_PIN = 2;
const int SERVO_PIN = 0;

// LED connection is inverted
const int LED_ON = 0;
const int LED_OFF = 1;

// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create Servo object
Servo servo;
int servoAngle = 0;

String getMonitorData() {
  JSONVar myObject;

  myObject["time"] = millis() / 1000;
  myObject["value"] = random(100);

  String retVal = JSON.stringify(myObject);

  Serial.println(retVal);
  return String(retVal);
}

String getControllerData() {
  JSONVar myObject;

  myObject["ledState"] = !digitalRead(LED_PIN);  // high on zero
  myObject["servoAngle"] = servoAngle;

  String retVal = JSON.stringify(myObject);

  Serial.println(retVal);
  return String(retVal);
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  servo.attach(SERVO_PIN);

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
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
    request->send_P(200, "text/plain", getMonitorData().c_str());
  });

  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(LED_PIN, LED_OFF);
    request->send_P(200, "text/plain", getMonitorData().c_str());
  });

  server.on("/rest", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getMonitorData().c_str());
  });

  // SERVO
  server.on("/servo", HTTP_GET, [](AsyncWebServerRequest * request) {
    int paramsNr = request->params();
    if(paramsNr > 0) {
      AsyncWebParameter* p = request->getParam(0);
      servoAngle = (p -> value()).toInt();
      request->send_P(200, "text/plain", getControllerData().c_str());
    } else {
      request->send_P(400, "text/plain", ((String)("Missing parameter")).c_str());    // todo check
    }
  });

  // Start server
  server.begin();
}

void loop() {

}

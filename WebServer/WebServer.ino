// Created by David Szegedi - 2020
// Feel free to use it for any project.

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <Wire.h>
#include <Arduino_JSON.h>
#include <Servo.h>

// Replace with your network credentials
// const char* ssid = "Boszorkany";
// const char* password = "Kucko725B";
const char* ssid = "SzeDavid";
const char* password = "19841209";

// Set LED GPIO
const int LED_PIN = LED_BUILTIN;
const int SERVO_MAX = 180;
const int SERVO_PIN = D0;
const int POTMETER_PIN = A0;

// LED connection is inverted
const int LED_ON = 0;
const int LED_OFF = 1;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create Servo object
Servo servo;

String getMonitorData() {
  JSONVar myObject;

  myObject["time"] = millis() / 1000;
  int raw = analogRead(POTMETER_PIN);
  int tmp = map(raw, 0, 1023, 0, 330);  // produces integer...
  double voltage = tmp / 100.0;     // ...but we need double
  myObject["value"] = voltage;

  String retVal = JSON.stringify(myObject);

  Serial.println(retVal);
  return String(retVal);
}

String getControllerData() {
  JSONVar myObject;

  myObject["ledState"] = !digitalRead(LED_PIN);  // high on zero
  myObject["servoAngle"] = 180-servo.read();  // servo is inverted (left -> right)

  String retVal = JSON.stringify(myObject);

  Serial.println(retVal);
  return String(retVal);
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);

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

  digitalWrite(LED_PIN, LED_ON);

  // Print Local IP Address
  Serial.println(WiFi.localIP());

  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
  }


  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  //   This is necessary for Angular routing if called with ang. router param. .../controller
    server.onNotFound([](AsyncWebServerRequest * request) {
      Serial.println("** NOT FOUND **");
      AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.html.gz", "text/html");
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });

  //  --- QUERY ---
  server.on("/v1/monitor", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getMonitorData().c_str());
  });

  server.on("/v1/controller", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getControllerData().c_str());
  });


  //  --- SET ---
  // LED
  server.on("/v1/led", HTTP_POST, [](AsyncWebServerRequest * request) {
    const char* PARAM = "state";
    if (request->hasParam(PARAM)) {
      AsyncWebParameter* p = request->getParam(PARAM);
      digitalWrite(LED_PIN, (p->value() == "true" ? LED_ON : LED_OFF));
      request->send_P(200, "text/plain", getControllerData().c_str());
    } else {
      request->send_P(400, "text/plain", ((String)("Missing parameter")).c_str());
    }
  });

  // SERVO
  server.on("/v1/servo", HTTP_POST, [](AsyncWebServerRequest * request) {
    const char* PARAM = "angle";
    if (request->hasParam(PARAM)) {
      AsyncWebParameter* p = request->getParam(PARAM);
      servo.write(SERVO_MAX-(p->value().toInt()));  // servo is inverted (left -> right)
      request->send_P(200, "text/plain", getControllerData().c_str());
    } else {
      request->send_P(400, "text/plain", ((String)("Missing parameter")).c_str());
    }
  });

  // POTMETER
  server.on("/v1/potmeter", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getMonitorData().c_str());
  });


  // Start server
  server.begin();
}

void loop() {

}

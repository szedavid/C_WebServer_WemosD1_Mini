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
  myObject["servoAngle"] = servo.read();

  String retVal = JSON.stringify(myObject);

  Serial.println(retVal);
  return String(retVal);
}

void sendJS(AsyncWebServerRequest * request, char* file) {
  AsyncWebServerResponse *response = request->beginResponse(SPIFFS, file, "text/javascript");
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
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

  // ** ENDPOINTS **
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("- Root -");
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.html", "text/html");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  // Angular routing
  server.onNotFound([](AsyncWebServerRequest * request) {
    Serial.println("** NOT FOUND **");
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.html", "text/html");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/styles.css", "text/css");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    //    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/main.js", "text/javascript");
    //    response->addHeader("Content-Encoding", "gzip");
    //    request->send(response);

    sendJS(request, "/main.js");
  });

  server.on("/polyfills.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    //    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/polyfills.js", "text/javascript");
    //    response->addHeader("Content-Encoding", "gzip");
    //    request->send(response);

    sendJS(request, "/polyfills.js");
  });

  server.on("/runtime.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    //    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/runtime.js", "text/javascript");
    //    response->addHeader("Content-Encoding", "gzip");
    //    request->send(response);

    sendJS(request, "/runtime.js");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/favicon.ico", "image/vnd.microsoft.icon");
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
      servo.write(p->value().toInt());
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

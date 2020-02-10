# C_WebServer_WemosD1_Mini
Embedded Webserver for Wemos D1 Mini written in Arduino style C++

The C_WebServer_WemosD1_Mini repository contains the latest frontend files.

If you change the FE, rebuild the project
and copy the gzipped files (under "dist/gzipped") into "WebServer/data".
(See Angular_FE_Wemos_D1-s README.md.)

Uploading the data (frontend files) to Wemos D1 mini:
	- https://www.instructables.com/id/Using-ESP8266-SPIFFS/

The next step is required if you use this project for the first time,
or if you changed something in the backend file (WebServer.ino):
- Build and upload the "C_WebServer_WemosD1_Mini/WebServer.ino" script (Arduino C++ file)
with e.g. Arduino IDE to your Wemos D1 mini to transform it into a webserver
with the required REST endpoints.

If you are new to Arduino or to Wemos D1 mini, this page can be a good starting point:
	- https://www.instructables.com/id/Wemos-ESP8266-Getting-Started-Guide-Wemos-101/

The project requires the following non Arduino core libraries:
	- ESPAsyncWebServer - https://github.com/me-no-dev/ESPAsyncWebServer
	- ESPAsyncTCP - https://github.com/me-no-dev/ESPAsyncTCP
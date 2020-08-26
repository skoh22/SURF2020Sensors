/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * Modified this code to read data from sensors and publish to a server
 * To change this code for specific sensors, change the lines with comments off the side (sensor specific)
 * Can either have pressure sensor or magnetic field sensor on one board. (I2C)
*/

//things to include

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include <OneWire.h> //DS18B20
//#include <DallasTemperature.h> //DS18B20
//#include "DHT.h" //AM2302
#include <Wire.h> //BME280 and LIS3MDL, not both
#include "SparkFunBME280.h" //BME280
//#include <LIS3MDL.h> //LIS3MDL

//defining pins

#define ONE_WIRE_BUS 4 //DS18B20 pin D2 on ESP8266

#define DHTPIN 0     //AM2302 connected to D3
#define DHTTYPE DHT22   //AM2302 DHT 22 defines type of sensor as AM2302


#ifndef STASSID
#define STASSID NETWORKID //REPLACE with network ID
#define STAPSK NETWORKPASSWORD //REPLACE with network password
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

//OneWire oneWire(ONE_WIRE_BUS); //DS18B20
//DallasTemperature sensors(&oneWire); //DS18B20

//DHT dht(DHTPIN, DHTTYPE); //AM2302

BME280 bme; //BME280

//LIS3MDL mag; //LIS3MDL

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  char temp[400];
  int tmil = millis();
  int sec = tmil / 1000;
  int minute = sec / 60;
  int hr = minute / 60;
  
  //get values from sensors
  
  //sensors.requestTemperatures(); //DS18B20
  //float tempVal = sensors.getTempCByIndex(0); //DS18B20

  //float tempVal =dht.readTemperature(); //AM2302
  //float humVal = dht.readHumidity(); //AM2302

  float tempVal = bme.readTempC(); //BME280
  float humVal = bme.readFloatHumidity(); //BME280
  float pressureVal = bme.readFloatPressure(); //BME280 

  //mag.read(); //LIS3MDL
  //float mxVal = 1000.0*mag.m.x/6842; //LIS3MDL //including conversion to milligauss
  //float myVal = 1000.0*mag.m.y/6842; //LIS3MDL
  //float mzVal = 1000.0*mag.m.z/6842; //LIS3MDL
  /*
   * Below is the print command of the HTML code
   * Percent signs show where values will be inserted in order of arguments at the end (%% acts as a percent sign in the text)
   * In the body of the HTML, make sure the lines with sensor data match the format--pretty specific for parsing
   * Make sure Uptime is first
   * 
  */
  
  snprintf(temp, 400, 

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Data Reading</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>ESP8266 Data Readings</h1>\
    <p>Uptime: %02d:%02d:%02d.%02d</p>\
    <p>Temp (C): %10.2f</p>\
    <p>Humidity (%%): %10.2f</p>\ 
    <p>Pressure (Pa): % 010.2f</p>\
  </body>\
</html>",

           hr, minute % 60, sec % 60, tmil%1000, tempVal, humVal, pressureVal
          );
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  //sensors.begin(); //DS18B20
  //dht.begin(); //AM2302
  Wire.begin(); //BME280 and LIS3MDL
  bme.beginI2C(); //BME280
  //mag.init(); //LIS3MDL
  //mag.enableDefault(); //LIS3MDL
  
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  //server.on("/test.svg", drawGraph);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}

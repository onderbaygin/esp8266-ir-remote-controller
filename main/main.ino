#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";
 
ESP8266WebServer server(80);
IRsend irsend(2);

void indexPage() {
  String param = server.argName(0);
  if(param == "code") {
    irsend.sendNEC(strtoul(server.arg(0).c_str(), NULL, 16));
  }
  server.send(200, "text/html", "<!DOCTYPE html><html><head> <meta charset='UTF-8'> <meta content='width=device-width, height=device-height, initial-scale=1, maximum-scale=1.0, user-scalable=no' name='viewport'> <title>IR HEX Code Sender</title> <style type='text/css'> body{background-color: black; text-align: center; font-size: 20px;}h2{color: white;}input{width: 80%; height: 50px; font-size: 20px;}#send{cursor: pointer; border-radius: 20px; border: none;}#send:focus{outline: none;}#code{border: none; border-radius: 20px; text-align: center;}#code:focus{outline: none;}</style></head><body> <form action='send'> <h2>IR HEX Code Sender</h2> <hr> <br><input type='text' name='code' placeholder='Hex Code' id='code'> </br></br> <input type='submit' value='Send' id='send'> </form> <script type='text/javascript'> document.querySelector('#send').onclick=(e)=>{e.preventDefault(); fetch('/?code=' + document.querySelector('#code').value);}</script></body></html>");
}

void samsungPage() {
  server.send(200, "text/html", "<!DOCTYPE html><html><head><meta charset='UTF-8'> <meta content='width=device-width, height=device-height, initial-scale=1, maximum-scale=1.0, user-scalable=no' name='viewport'><title>Samsung TV Control</title><style type='text/css'>body{text-align: center;background-color: black;}button{width: 100%;height: 50px;border: none;outline: none;margin-top: 5px;font-weight: bold;font-size: 20px;}button:focus{border: none;outline: none;}.power{background-color: red;color: white;}.number{width: 49% !important;height: 20px;font-size: 10px;}.number-full{width: 100% !important;height: 20px;font-size: 10px;}.group1{width: 49% !important;}</style></head><body><center><button class='power' hex='0xE0E040BF'>ON/OFF</button><button class='power' hex='0xE0E0F00F'>Mute</button><button class='group1' hex='0xE0E0E01F'>Volume +</button><button class='group1' hex='0xE0E0D02F'>Volume -</button><button class='group1' hex='0xE0E048B7'>Channel +</button><button class='group1' hex='0xE0E008F7'>Channel -</button><button class='number-full' hex='0xE0E08877'>0</button><button class='number' hex='0xE0E020DF'>1</button><button class='number' hex='0xE0E0A05F'>2</button><button class='number' hex='0xE0E0609F'>3</button><button class='number' hex='0xE0E010EF'>4</button><button class='number' hex='0xE0E0906F'>5</button><button class='number' hex='0xE0E050AF'>6</button><button class='number' hex='0xE0E030CF'>7</button><button class='number' hex='0xE0E0B04F'>8</button><button class='number-full' hex='0xE0E0708F'>9</button></center><script>document.querySelectorAll('button').forEach((value, key)=>{value.onclick=(e)=>{fetch('/?code=' + value.getAttribute('hex'));}})</script></body></html>");
}

// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, indexPage);
    server.on("/samsung", HTTP_GET, samsungPage);
}
 
// Manage not found URL
void handleNotFound() {
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
}
 
void setup(void) {
  irsend.begin();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
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
 
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("ir")) {
    Serial.println("MDNS responder started");
  }
 
  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
  
}
 
void loop(void) {
  server.handleClient();
}

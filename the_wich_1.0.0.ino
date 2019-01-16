/*********
  Topaz Digital Solutions.
  WiFi Controlled Switch Ver1.0.0
  Soft by A. Kimani
  Hardware by Mike Ng'ash 
*********/


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

IPAddress    apIP(192, 168, 4, 1);  // Defining a static IP address: local & gateway
                                    // Default IP in AP mode is 192.168.4.1

const char *ssid = "The WiCH";
const char *password = "TopazDigi";

// Define a web server at port 80 for HTTP
ESP8266WebServer server(80);

const int ledPin = 2; // Connect the Output to GPIO2 on esp 01

bool ledState = false;

void handleRoot() {
  digitalWrite (ledPin, server.arg("switch").toInt());
  ledState = digitalRead(ledPin);

 /* Dynamically generate the LED toggle link, based on its current state (on or off)*/
  char ledText[80];
  
  if (ledState) {
    strcpy(ledText, "<h2>The Switch is ON</h2><a href=\"/?switch=0\"><button>Switch OFF</button></a>");
  }

  else {
    strcpy(ledText, "<h2>The Switch is OFF</h2><a href=\"/?switch=1\"><button>Switch ON</button></a>");
  }
 
  ledState = digitalRead(ledPin);

  char html[3000];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

// Build an HTML page to display on the web-server root address
  snprintf ( html, 3000,

"<html>\
  <head>\
    <meta charset=\"utf-8\">\
    <meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">\
    <meta http-equiv='refresh' content='10'/>\
    <title>The WiCH: Switch Control</title>\
    <style>\
            @media only screen and (max-width: 500px) {\
            body{background-color:PaleGoldenRod}\
            .buttonCss {background-color:orangered; border-radius:25px; font:22px Arial; height:70px; width:auto; color: white;}\
            .header{width:100%;margin:0px0px0px0px;font:28px Arial;color:white;background-color: orangered;}\
            .formed{width:auto;height:auto;margin:auto;border:5px solid orange;border-radius: 30px;background: orange;text-align: center;}\
            .footer{font: 16px; margin: auto;text-align: center;}\
            }\
    </style>\
  </head>\
  <body>\
    <header class=\"header\"><h1><center>THE WiCH&reg;</center></h1></header>\
    <div class=\"formed\">\
    <p>Uptime: %02d:%02d:%02d</p>\
    <center>%s</center>\
    <p>This page refreshes every 10 seconds. Click <a href=\"javascript:window.location.reload();\">here</a> to refresh the page now.</p>\
    </div>\
    <footer><center><p> &copy; Topaz Digital Solutions. All rights reserved.</p></center></footer>\
  </body>\
</html>",

    hr, min % 60, sec % 60,
    ledText
  );
  server.send ( 200, "text/html", html );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void setup() {
  pinMode ( ledPin, OUTPUT );
//  digitalWrite ( ledPin, 0 );/
  
  delay(1000);

  //set-up the custom IP address
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00  
  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
 
  server.on ( "/", handleRoot );
  server.on ( "/switch=1", handleRoot);
  server.on ( "/switch=0", handleRoot);
  server.on ( "/inline", []() {
    server.send ( 200, "text/plain", "this works as well" );
  } );
  server.onNotFound ( handleNotFound );
  
  server.begin();
}

void loop() {
  server.handleClient();
}

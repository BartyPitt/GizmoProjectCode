/*------------------------------------------------------------------------------
  07/01/2018
  Author: Makerbro
  Platforms: ESP8266
  Language: C++/Arduino
  File: webserver_html_js.ino
  ------------------------------------------------------------------------------
  Description: 
  Code for YouTube video demonstrating how to use JavaScript in HTML weppages
  that are served in a web server's response.
  https://youtu.be/ZJoBy2c1dPk
  Do you like my videos? You can support the channel:
  https://patreon.com/acrobotic
  https://paypal.me/acrobotic
  ------------------------------------------------------------------------------
  Please consider buying products from ACROBOTIC to help fund future
  Open-Source projects like this! We'll always put our best effort in every
  project, and release all our design files and code for you to use. 
  https://acrobotic.com/
  https://amazon.com/acrobotic
  ------------------------------------------------------------------------------
  License:
  Please see attached LICENSE.txt file for details.
------------------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h> 

ESP8266WebServer server(80);
uint8_t pin_led = 2;
char* ssid = "Gizmo Test";
char* password = "";

char webpage[] PROGMEM = R"=====(
  <html lang="en">
      <head>
          <meta charset="utf-8">
          <title>ESP8266 Web Page Text to Serial</title>
      </head>
  
      <body>
        <script>
        window.addEventListener('deviceorientation', deviceOrientationHandler, false);
        
        var myVar = setInterval(sanity, 300);

        function sanity() {
            var request = new XMLHttpRequest();
            request.open("GET", "Xml request being sent" + " ", false);
            request.send(null);
        }
        function deviceOrientationHandler(eventData)
        {
            var tiltLR = eventData.gamma;
            var tiltFB = eventData.beta;
            var dir    = eventData.alpha;
  
            document.getElementById("doTiltLR").innerHTML = Math.round(tiltLR);
            document.getElementById("doTiltFB").innerHTML = Math.round(tiltFB);
            document.getElementById("doDirection").innerHTML = Math.round(dir);
  
            var Roll  = tiltLR.toString();
            var Pitch = tiltFB.toString();
            var request = new XMLHttpRequest();
            request.open("GET", Roll + " " + Pitch + " ", true);
            request.send(null);
        }
        </script>
  
          <p><b>Taking an Accelemetor Reading:</b></p>
          <table class="table table-striped table-bordered">
            <tr>
              <td>Tilt Left/Right [gamma]</td>
              <td id="doTiltLR"></td>
            </tr>
            <tr>
              <td>Tilt Front/Back [beta]</td>
              <td id="doTiltFB"></td>
            </tr>
            <tr>
              <td>Direction [alpha]</td>
              <td id="doDirection"></td>
            </tr>
            </table>
      </body>
  </html>
  )=====";

void HandleClient() {
  /*
  
  */
  server.send(200, "text/html", webpage); // Send a response to the client asking for input
  Serial.print("here 3");
  if (server.args() > 0 ) { // Arguments were received
    Serial.print("here");
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.print(server.argName(i)); // Display the argument
      Serial.print(server.arg(i));
    }
  }
}

void setup()
{
  pinMode(pin_led, OUTPUT);
  WiFi.softAP(ssid,password);
  IPAddress myIP = WiFi.softAPIP();
  
  Serial.begin(115200);
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(myIP);

  server.on("/", HandleClient);
  //server.on("/ledstate",getLEDState);
  server.begin();
}

void loop()
{
  server.handleClient();
  if (server.args() > 0 ) { // Arguments were received
    Serial.print("here 2.0");
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.print(server.argName(i)); // Display the argument
      Serial.print(server.arg(i));
    }
  }  
}

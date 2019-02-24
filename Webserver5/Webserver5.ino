/*
 Barty Pitt
 Attempt 5 ish 
 Why does nothing work :(
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
        /*
        function sanity() {
            var request = new XMLHttpRequest();
            request.open("POST", "/test", false);
            request.send("ThisIsNotaTest");
        }
        */
        function deviceOrientationHandler(eventData)
        {
            var tiltLR = eventData.gamma;
            var tiltFB = eventData.beta;
            var dir    = eventData.alpha;
  
            document.getElementById("doTiltLR").innerHTML = Math.round(tiltLR);
            document.getElementById("doTiltFB").innerHTML = Math.round(tiltFB);
            document.getElementById("doDirection").innerHTML = Math.round(dir);
  
            var Roll  = Math.round(tiltLR);
            var Pitch = Math.round(tiltFB);
            var request = new XMLHttpRequest();
            request.open("POST","/Data", true);
            request.send(Roll.toString() + " " + Pitch.toString());
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
  server.on("/Data", Data);
  //server.on("/ledstate",getLEDState);
  server.begin();
}

void loop()
{
  server.handleClient();
  if (server.args() > 0 ) { // Arguments were received
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.println(server.arg(i));
    } 
}
}

void Data()
{
  Serial.println("inData");
}

/*
 Barty Pitt
 Attempt 5 ish 
 Why does nothing work :(
------------------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h> 
#include <AccelStepper.h>

ESP8266WebServer server(80);
uint8_t pin_led = 2;
char* ssid = "Gizmo Test";
char* password = "";

int Xpos = 0;
int Ypos = 0;
int Rotation = 0;
int Length = 0;
int Width = 100;
int Height = 100;

//stepper motor Prememptive
AccelStepper stepperLong(1,16,0);
AccelStepper stepperRound;

char webpage[] PROGMEM = R"=====(
  <html lang="en">
      <head>
          <meta charset="utf-8">
          <title>ESP8266 Web Page Text to Serial</title>
      </head>
  
      <body>
        <script>
        window.addEventListener('deviceorientation', deviceOrientationHandler, false);
        
        var myVar = setInterval(sanity, 500);
        var Roll = 0
        var Pitch = 0
        
        function sanity() {
            console.log(Count);
            var request = new XMLHttpRequest();
            request.open("POST","/Data", true);
            request.send(Count);
        }
        
        function deviceOrientationHandler(eventData)
        {
            var tiltLR = eventData.gamma;
            var tiltFB = eventData.beta;
            var dir    = eventData.alpha;
  
            document.getElementById("doTiltLR").innerHTML = Math.round(tiltLR);
            document.getElementById("doTiltFB").innerHTML = Math.round(tiltFB);
            document.getElementById("doDirection").innerHTML = Math.round(dir);
  
            Roll  = Math.round(tiltLR);
            Pitch = Math.round(tiltFB);
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

void MainPage() 
{ 
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

  server.on("/",MainPage); //listens on these two channels
  server.on("/Data", Data); //when you get a request for data put into the "data" function
  server.begin();

  //Now for the steppers
  stepperLong.setMaxSpeed(1000);
  stepperLong.setAcceleration(200);
  stepperLong.moveTo(5000);
  stepperRound.setMaxSpeed(1000);
  stepperRound.setAcceleration(200);
  stepperRound.moveTo(5000);
}

void loop()
{
  server.handleClient();
}

void Data()
{
  Serial.print("here");
  Serial.println("inData");
  if (server.args() > 0 ) { // Arguments were received
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.println(server.arg(i));
      server.send(200,"text/plain","");
      }
    }
  
}

int Rspeed(int xSpeed,int ySpeed)
{
 int targetX = Xpos + xSpeed;
 int targetY = Ypos + ySpeed;
 int targetR = atan(targetY/targetX);
 return targetR - Rotation;
}

int Lspeed(int xSpeed,int ySpeed)
{
 int targetX = Xpos + xSpeed;
 int targetY = Ypos + ySpeed;
 int targetL =  sqrt(pow(xSpeed,2) + pow(ySpeed,2));
 return targetL - Length;
}

bool Move(int xSpeed,int ySpeed)
{
  if (abs(Xpos + xSpeed) < Width && abs(Ypos + ySpeed) < Height)
  {
    stepperRound.setSpeed(Rspeed(xSpeed,ySpeed));
    stepperLong.setSpeed(Lspeed(xSpeed,ySpeed));
    return true;
  }
  else
  {
    return false;
  }
}

bool UpdateXY()
{
  Xpos = Length * sin(Rotation);
  Ypos = Length * cos(Rotation);
}

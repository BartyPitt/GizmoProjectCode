/*
 Barty Pitt
 Attempt 5 ish 
 Why does nothing work :(
------------------------------------------------------------------------------*/
//Libaries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h> 
#include <AccelStepper.h>

//Webserver Important Constants
ESP8266WebServer server(80);
uint8_t pin_led = 2;
char* ssid = "Gizmo Barty";
char* password = "";

// Running Varibles
int Xpos = 0;
int Ypos = 0;
//float Rotation = 0;
//int Length = 0;
int RV = 0;
int LV = 0;


//Running Constants
#define Width 10000
#define Height 10000
#define TC 3
#define xOffset 0
#define yOffset 0
#define tpr 17152 //ticks per revolution


//stepper motor Prememptive
AccelStepper stepperLong(1,5,16);
AccelStepper stepperRound(1,0,4);


//kinda running Costants
#define Rotation stepperRound.currentPosition()
#define Length stepperLong.currentPosition()

char webpage[] PROGMEM = R"=====(
  <html lang="en">
      <head>
          <meta charset="utf-8">
          <title>ESP8266 Web Page Text to Serial</title>
      </head>
  
      <body>
        <script>
        window.addEventListener('deviceorientation', deviceOrientationHandler, false);
        
        var myVar = setInterval(sanity, 750);
        var Roll = 0
        var Pitch = 0
        
        function sanity() {
            var request = new XMLHttpRequest();
            request.open("POST","/Data", true);
            request.timeout = 500;
            request.send(Roll.toString() + "," + Pitch.toString());
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
            if (Roll == 0)
            {
              Roll = 1
            }
            if (Pitch == 0)
            {
              Pitch = 1
            }
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
  Serial.print("Client has connected probably");
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
  stepperLong.setSpeed(LV);
  stepperRound.setSpeed(RV);
  stepperLong.run();
  stepperRound.run();
  
}

void Data()
{
  if (server.args() > 0 ) // Arguments were received
  { 
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.println(server.arg(i));
      UpdateSpeeds(server.arg(i));
      server.send(200,"text/plain","");
      }
  }
}

int Rspeed(int xSpeed,int ySpeed)
{
 int targetX = Xpos + xSpeed;
 int targetY = Ypos + ySpeed;
 float targetR = atan(targetY/targetX);
 Serial.println("Target R :");
 Serial.println(targetR);
 Serial.println("TargetX");
 Serial.println(targetX);
 Serial.println("TargetY");
 Serial.println(targetY);
 return (targetR - Rotation)*tpr;
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
    RV = Rspeed(xSpeed,ySpeed);
    LV = Lspeed(xSpeed,ySpeed);
    Serial.println("Lv, Rv");
    Serial.println(String(LV));
    Serial.println(String(RV));
    return true;
  }
  else
  {
    return false;
  }
}

void UpdateXY()
{
  Xpos = Length * sin(Rotation);
  Ypos = Length * cos(Rotation);
}



void UpdateSpeeds(String input)
{
  String Roll = "";
  String Pitch = "";
  for (int i = 0; i < input.length(); i++) 
  {
    if (input.substring(i, i+1) == ",") 
    {
      Roll = input.substring(0, i).toInt();
      Pitch = input.substring(i+1).toInt();
      break;
    }
  }
  UpdateXY();
  /*
  Serial.println("pitch");
  Serial.print(Pitch.toInt()*3);
  Serial.println("Roll");
  Serial.print(Roll.toInt()*3);
  */
  Move(Roll.toInt()*TC , Pitch.toInt()*TC );
}

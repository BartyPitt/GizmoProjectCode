

#include <AccelStepper.h>
String readString; //String captured from serial port
AccelStepper stepperLong(1,5,16);
int n; //value to write to servo

void setup() {
  Serial.begin(9600);
  stepperLong.setMaxSpeed(16000);
  stepperLong.setAcceleration(500);
  Serial.println("Running Stepper Code"); // so I can keep track of what is loaded
  Serial.println();
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();  //gets one byte from serial buffer
    readString += c; //makes the string readString
    delay(2);  //slow looping to allow buffer to fill with next character
  }

  if (readString.length() >0) {
    Serial.println(readString);  //so you can see the captured string     

    n = readString.toInt();  //convert readString into a number
    stepperLong.moveTo(n);
    Serial.flush();
    // auto select appropriate value
    readString = "";
  }
  stepperLong.run();

}

#include <Servo.h>

const int servoPinPan = 9;       // first servo
const int servoPinTilt = 10;     // second servo

Servo panServo;  // create servo object to control a servo
Servo tiltServo;  // create servo object to control a servo

const int maxPan = 150;
const int minPan = 30;
const int maxTilt = 150;
const int initialTiltCorrection = 30;
int panIncrement = 2;
int tiltIncrement = 5;

int pan = 90;
int tilt = 0;
bool stop = false;

void setup() {
  // Servo  
  panServo.attach(servoPinPan);  // attaches the servo
  tiltServo.attach(servoPinTilt);  // attaches the servo

  // Inizialize Serial
  Serial.begin(9600);

  tiltServo.write(tilt);
  panServo.write(pan);
  delay(2000);
}


void loop(){
    if (stop == true) {
      delay(1000);
    }
    else {
      pan += panIncrement;
      if (pan > maxPan) {
        pan = maxPan;
        panIncrement = 0-panIncrement;
        tilt += tiltIncrement;
      }
      if (pan < minPan) {
        pan = minPan;
        panIncrement = 0-panIncrement;
        tilt += tiltIncrement;        
      }
      if (tilt >= (maxTilt-initialTiltCorrection) && pan == maxPan) {
        stop = true;
      }

      tiltServo.write(tilt);
      panServo.write(pan);
      Serial.print("pan increment: ");
      Serial.print(panIncrement);
      Serial.print("Pan: ");
      Serial.print(pan);
      Serial.print(" , Tilt: ");
      Serial.println(tilt);
      delay(150);
    }
}

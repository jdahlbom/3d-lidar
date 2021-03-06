#include <Servo.h>
#include <Wire.h>
#include <LIDARLite.h>

const int servoPinPan = 9;       // first servo
const int servoPinTilt = 10;     // second servo

Servo panServo;  // create servo object to control a servo
Servo tiltServo;  // create servo object to control a servo

const int maxPan = 150;
const int minPan = 30;
const int maxTilt = 120;
const int initialTiltCorrection = 30;
int panIncrement = 1;
int tiltIncrement = 1;
const int stopDelay = 100;
const int lidarDelay = 10;

int loopCount = 0;
int pan;
int tilt;
int runId;
bool blockLooping = true;

LIDARLite lidarLite;
void setup()
{
  Serial.begin(9600); // Initialize serial connection to display distance readings

  lidarLite.begin(0, true); // Set configuration to default and I2C to 400 kHz
  lidarLite.configure(0); // Change this number to try out alternate configurations

  // Servo  
  panServo.attach(servoPinPan);  // attaches the servo
  tiltServo.attach(servoPinTilt);  // attaches the servo

}

/**
 * Prompt for scan set identifier
 * Returns integer value to use as result set ID.
 */
int prompt() {
  resetInitialValues();
  Serial.println("#==========================#");
  Serial.println("#  Pan-tilt Lidar scanner  #");
  Serial.println("#  Scan prompt             #");
  Serial.println("#==========================#");
  Serial.println("#");
  while(true) {
    Serial.println("#> Send integer value to use as scan result identifier");
    while(!Serial.available()) {
      delay(5);
    }
    int input = Serial.parseInt();
    if (input != 0) {
      Serial.print("# Starting scan run ");
      Serial.println(input);
      Serial.println("# CSV header:");
      Serial.println("runId,distance,pan,tilt");
      blockLooping = false;
      return input;
    } else {
      Serial.print("#! Invalid input, please input integer value");
    }
  }
}

void sendScanResults(int runId, int distance, int intensity, int pan, int tilt) {
  Serial.print(runId);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(",");
  Serial.print(intensity);
  Serial.print(",");
  Serial.print(pan);
  Serial.print(",");
  Serial.println(tilt);
}

void resetInitialValues() {
  pan = minPan;
  tilt = 0;

  tiltServo.write(tilt);
  panServo.write(pan);
  delay(2000);
}

/**
 * Servo bracket right now uses "0" tilt as maximum upwards tilt it goes to.
 * It is in reality somewhere around 60 tilt from horizontal plane (90 - initialTiltCorrection)
 */
int mapServoTiltToRealTilt(int tilt) {
  return 90 - tilt - initialTiltCorrection;
}

/**
 * Pan servo is at middle initial position when given value "90". Middle position should be
 * 0 pan.
 */
int mapServoPanToForwardZeroPan(int pan) {
  return pan - 90;
}

int lidarIntensity() {
  byte isBusy = 1;
  // Poll busy bit in status register until device is idle
  while(isBusy)
  {
  // Read status register
    Wire.beginTransmission(LIDARLITE_ADDR_DEFAULT);
    Wire.write(0x01);
    Wire.endTransmission();
    Wire.requestFrom(LIDARLITE_ADDR_DEFAULT, 1);
    int statusByte = Wire.read();
    isBusy = bitRead(statusByte,0); // Take LSB of status register, busy bit
  }
  // read register 0x0e which is signal strength
  Wire.beginTransmission(LIDARLITE_ADDR_DEFAULT);
  Wire.write(0x0e);
  Wire.endTransmission();
  Wire.requestFrom(LIDARLITE_ADDR_DEFAULT, 1);
  return Wire.read();
}

void loop(){
    if (blockLooping == true) {
      runId = prompt();
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
        blockLooping = true;
        return;
      }
    }
    
    tiltServo.write(tilt);
    panServo.write(pan);
    delay(stopDelay);
    int dist;
    if (loopCount == 100) {
      dist = lidarLite.distance();
      loopCount = 0;
    } else {
      dist = lidarLite.distance(false);
      loopCount++;
    }
    int intensity = lidarIntensity();
    delay(lidarDelay);
    sendScanResults(runId, dist, intensity, mapServoPanToForwardZeroPan(pan), mapServoTiltToRealTilt(tilt));
}

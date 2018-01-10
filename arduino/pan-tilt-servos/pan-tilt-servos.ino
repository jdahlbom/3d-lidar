/* 
 Pan / Tilt PWM

 The analogWrite() function uses PWM, so if
 you want to change the pin you're using, be
 sure to use another PWM capable pin. On most
 Arduino, the PWM pins are identified with 
 a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.

 This example code is in the public domain.
 */

#include <Servo.h>
#include <I2C.h>

int panPin = 9;    // the PWM pin pan servo is attached to
int tiltPin = 10;   // the PWM pin tilt servo is attached to

Servo panServo;
Servo tiltServo;

const int INCREASING = 1;
const int DECREASING = -1;
int pan_direction = INCREASING;
int pan_target_angle = 90;
const int MAX_PAN = 180;
const int MIN_PAN = 0;

int next_tilt_state = DECREASING;
int tilt_target_angle = 90;
const int TILT_UP=-2;
const int MIN_TILT = 45;
const int MAX_TILT = 135;

const int LIDAR_ADDR = 0x62;
const int SIGNAL_STRENGTH_REGISTER = 0x0e;


bool is_tilt_at_top() {
  int tilt_angle = tiltServo.read();
  return tilt_angle < MIN_TILT;
}

bool is_tilt_at_bottom() {
  return tiltServo.read() >= MAX_TILT;
}

boolean init_servos() {
  if (is_tilt_at_bottom() && panServo.read() <= MIN_PAN) {
    return true;
  }
  tiltServo.write(MAX_TILT); // Max angle is lowest point.
  panServo.write(MIN_PAN);
  delay(10);
  return false;
}


void read_distance(uint8_t* result) {
  I2c.begin();
  int error_code = I2c.read(LIDAR_ADDR, SIGNAL_STRENGTH_REGISTER, 3, result);
  I2c.end();
}


uint8_t* lidar_result;

// the setup routine runs once when you press reset:
void setup() {
  lidar_result = (uint8_t*) malloc(3 * sizeof(uint8_t));
  panServo.attach(panPin);
  tiltServo.attach(tiltPin);
  while(!init_servos()) {}
}

// the loop routine runs over and over again forever:
void loop() {
  int current_tilt = tiltServo.read();
  int current_pan = panServo.read();

  if (is_tilt_at_top() && current_pan >= MAX_PAN) {
    delay(10);
    return;
  }
  
  if(current_pan >= MAX_PAN)
    pan_direction = DECREASING;
    if(next_tilt_state == DECREASING) {
      tilt_target_angle = current_tilt+TILT_UP;
      next_tilt_state = INCREASING;
    }
  if(current_pan <= MIN_PAN) {
    pan_direction = INCREASING;
    if (next_tilt_state == INCREASING) {
      tilt_target_angle = current_tilt+TILT_UP;
      next_tilt_state = DECREASING;
    }
  }

  read_distance(lidar_result);

  tiltServo.write(tilt_target_angle);
  pan_target_angle = current_pan + pan_direction;  
  panServo.write(pan_target_angle);

  delay(10);
}

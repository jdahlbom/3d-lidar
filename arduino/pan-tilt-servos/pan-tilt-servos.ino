/* 
 Pan / Tilt PWM

 The analogWrite() function uses PWM, so if
 you want to change the pin you're using, be
 sure to use another PWM capable pin. On most
 Arduino, the PWM pins are identified with 
 a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.

 This example code is in the public domain.
 */
#ifndef DISABLE_SERVOS
#define DISABLE_SERVOS 1
#endif

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

#define ACQ_COMMAND 0x00
#define ACQ_CONFIG_REQ 0x04

// Set most significant byte to 1 to enable continuous reading.
#define LIDAR_ADDR 0x62
#define SIGNAL_STRENGTH_REGISTER 0x8e
#define STATUS 0x01

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

void lidar_write_and_wait(int register_address, int value) {
  uint8_t nack_ack = 100;
  while(nack_ack != 0) {
    nack_ack = I2c.write(LIDAR_ADDR, register_address, value);
    delay(2);
  }
}

void lidar_read_and_wait(int register_address, int num_of_bytes, uint8_t* result) {
  uint8_t nack_ack = 100;
  while(nack_ack != 0) {
    nack_ack = I2c.read(LIDAR_ADDR, register_address, num_of_bytes, result);
    delay(2);
  }
}

void lidar_read_distance(uint8_t* result) {
  const int READ_WAIT_ATTEMPTS = 10;

  lidar_write_and_wait(ACQ_COMMAND, ACQ_CONFIG_REQ);
  lidar_read_and_wait(SIGNAL_STRENGTH_REGISTER, 3, result);
}

uint8_t* lidar_result;

struct Lidar {
  uint8_t signal_strength;
  uint16_t full_distance;
};

struct Lidar convert_bytes_to_result(uint8_t* bytes) {
  struct Lidar lidar;
  lidar.signal_strength = bytes[0];
  lidar.full_distance = bytes[1]<<8 + bytes[2];
  return lidar;
}

// the setup routine runs once when you press reset:
void setup() {
  I2c.begin();
  lidar_result = (uint8_t*) malloc(3 * sizeof(uint8_t));
  
  #ifndef DISABLE_SERVOS
  panServo.attach(panPin);
  tiltServo.attach(tiltPin);
  while(!init_servos()) {}
  #endif

  Serial.begin(9600);
  delay(100);
}

void update_servos(int current_tilt, int current_pan) {
  if (is_tilt_at_top() && current_pan >= MAX_PAN) {
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

  tiltServo.write(tilt_target_angle);
  pan_target_angle = current_pan + pan_direction;  
  panServo.write(pan_target_angle);
}

// the loop routine runs over and over again forever:
void loop() {
  Serial.println("Testing");
  lidar_read_distance(lidar_result);
  struct Lidar lidar = convert_bytes_to_result(lidar_result);
  
  #ifndef DISABLE_SERVOS
  int current_tilt = tiltServo.read();
  int current_pan = panServo.read();
  update_servos(current_tilt, current_pan);
  #else
  int current_tilt = 0;
  int current_pan = 0;
  #endif

  Serial.print(current_pan);
  Serial.print(",");
  Serial.print(current_tilt);
  Serial.print(",");
  Serial.print(lidar.signal_strength);
  Serial.print(",");
  Serial.println(lidar.full_distance);
  delay(100);
}

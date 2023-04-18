#include <serialize.h>
#include <stdarg.h>
#include "packet.h"
#include "constants.h"
#include <Wire.h>
#include <SparkFunLSM9DS1_test.h>
#include <math.h>

// IMU STUFF
LSM9DS1 imu;
#define DECLINATION +0.08 // Declination (degrees) in Singapore 
float gyroZ;
float currentTime, elapsedTime, previousTime;
float gyroErrorZ;
float degree;

// Motor control pins. You need to adjust these till Alex moves in the correct direction
#define LF                  5   // Left forward pin
#define LR                  6   // Left reverse pin
#define RF                  11  // Right forward pin
#define RR                  10  // Right reverse pin

#define S0 13
#define S1 9
#define S2 8
#define S3 7
#define sensorOut 12

// Motor proportional control
float motor_speed = 60; // 60% speed
float proportional = 0.1;
float derivative = 0.01;
float previousdegree = 0;

int val_1;
int pd_counter = 0;

// Ultrasonic
const int trigPin = 3;
const int echoPin = 2;
long duration;
float distance;

// Colour Sensor Stuff
float redFrequency = 0;
float greenFrequency = 0;
float blueFrequency = 0;
double rgb[3] = {0, 0, 0};
double hue;

// Set up Alex's motors. Right now this is empty, but later you will replace it with code to set up the PWMs to drive the motors.
// Convert percentages to PWM values
int pwmVal(float speed)
{
  if (speed < 0.0) speed = 0;
  if (speed > 100.0) speed = 100.0;
  return (int) ((speed / 100.0) * 255.0);
}

void calcError() {
  int count = 0;
  while (count < 200) {
    if ( imu.gyroAvailable() )
    {
      // To read from the gyroscope,  first call the
      // readGyro() function. When it exits, it'll update the
      // gx, gy, and gz variables with the most current data.
      imu.readGyro();
      gyroErrorZ += imu.calcGyro(imu.gz);
      count ++;
    }
  }
  gyroErrorZ /= 200;
}

void proportional_control(TDirection dir)
{
  int deltaAngle = round(-degree);
  int targetGyroZ;

  targetGyroZ = (int)round(1.3 * deltaAngle);

  if (round(targetGyroZ - gyroZ) == 0) {
    ;
  } else if (targetGyroZ > gyroZ) {
    if (dir == FORWARD) {
      val_1 += 1;
      if (val_1 > 255) val_1 = 255;
      OCR2A = val_1;
    } else {
      val_1 -= 1;
      if (val_1 < 0) val_1 = 0;
      OCR1B = val_1;
    }
  } else {
    if (dir == FORWARD) {
      val_1 -= 1;
      if (val_1 < 0) val_1 = 0;
      OCR2A = val_1;
    } else {
      val_1 += 1;
      if (val_1 > 255) val_1 = 255;
      OCR1B = val_1;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  setupSerial();
  startSerial();
  Wire.begin();

  cli();
  setupMotors();
  setupColour();
  startMotors();
  
  // Ultrasonic Setup
  DDRD |= 0b00001000;
  DDRD &= ~(0b00000100);
  sei();

  calcError();
  currentTime = micros();

  if (imu.begin() == false)
  {
    while (1);
  }
}

void loop() {

  // TPACKET RECEIVE
  TPacket recvPacket; // This holds commands from the Pi
  TResult result = readPacket(&recvPacket);
  if (result == PACKET_OK) {
    handlePacket(&recvPacket);
  }
  else if (result == PACKET_BAD)
  {
    sendBadPacket();
  }
  else if (result == PACKET_CHECKSUM_BAD)
  {
    sendBadChecksum();
  }


  if ( imu.gyroAvailable() )
  {
    // To read from the gyroscope,  first call the
    // readGyro() function. When it exits, it'll update the
    // gx, gy, and gz variables with the most current data.
    imu.readGyro();
    gyroZ = imu.calcGyro(imu.gz);
  }
  previousTime = currentTime;
  currentTime = micros();
  elapsedTime = (currentTime - previousTime) / 1000000;
  gyroZ -= gyroErrorZ;
  degree += gyroZ * elapsedTime;

  // Execute proportional_control
  if (dir == FORWARD || dir == BACKWARD)
  {
    if (pd_counter = 1000)
    {
      proportional_control(dir);
      pd_counter = 0;
    } else
    {
      pd_counter++;
    }
  } 
}

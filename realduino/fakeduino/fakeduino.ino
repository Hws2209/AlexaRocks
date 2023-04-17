#include <serialize.h>
#include <stdarg.h>
#include "packet.h"
#include "constants.h"
#include <Wire.h>
#include <SparkFunLSM9DS1_test.h>
#include <math.h>

// Alex's configuration constants
// Number of ticks per revolution from the wheel encoder.
#define COUNTS_PER_REV      180
#define WHEEL_CIRC          20.4
#define ALEX_LENGTH 22.5
#define ALEX_BREADTH 12.5
float alexDiagonal = 0.0;
float alexCirc = 0.0;

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

volatile unsigned long leftForwardTicks, rightForwardTicks, leftReverseTicks, rightReverseTicks;
volatile unsigned long leftForwardTicksTurns, rightForwardTicksTurns, leftReverseTicksTurns, rightReverseTicksTurns;
volatile unsigned long leftRevs, rightRevs;
volatile unsigned long forwardDist, reverseDist;
unsigned long deltaDist, newDist;
unsigned long deltaTicks, targetTicks;

void enablePullups()
{
  // Use bare-metal to enable the pull-up resistors on pins
  // 2 and 3. These are pins PD2 and PD3 respectively.
  // We set bits 2 and 3 in DDRD to 0 to make them inputs.
  DDRD &= ~((1 << 2) | (1 << 3));
  PORTD |=  (1 << 2) | (1 << 3);
}

// Set up Alex's motors. Right now this is empty, but later you will replace it with code to set up the PWMs to drive the motors.
// Convert percentages to PWM values
int pwmVal(float speed)
{
  if (speed < 0.0) speed = 0;
  if (speed > 100.0) speed = 100.0;
  return (int) ((speed / 100.0) * 255.0);
}

void initializeState()
{
  clearCounters();
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
      analogWrite(RF, val_1);
    } else {
      val_1 -= 1;
      if (val_1 < 0) val_1 = 0;
      analogWrite(RR, val_1); //would increase GyroZ
    }
  } else {
    if (dir == FORWARD) {
      val_1 -= 1;
      if (val_1 < 0) val_1 = 0;
      analogWrite(RF, val_1);
    } else {
      val_1 += 1;
      if (val_1 > 255) val_1 = 255;
      analogWrite(RR, val_1); //would increase GyroZ
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  setupSerial();
  startSerial();
  Wire.begin();

  // IMU Setup
  alexDiagonal = sqrt((ALEX_LENGTH * ALEX_LENGTH) + (ALEX_BREADTH * ALEX_BREADTH));
  alexCirc = PI * ALEX_BREADTH;

  cli();
  setupEINT();
  setupMotors();
  setupColour();
  startMotors();
  // enablePullups();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  // initializeState();
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
  
  // OLD MOTOR CONTROLS
  /*
  if (deltaDist > 0 && ((dir == FORWARD && forwardDist > newDist) || (dir == BACKWARD && reverseDist > newDist) || dir == STOP))
  {
    deltaDist = 0;
    newDist = 0;
    stop();
  }

  if (deltaTicks > 0 && ((dir == LEFT && leftReverseTicksTurns >= targetTicks) || (dir == RIGHT && leftForwardTicksTurns >= targetTicks) || dir == STOP))
  {
    deltaTicks = 0;
    targetTicks = 0;
    stop();
  } 
  */
}

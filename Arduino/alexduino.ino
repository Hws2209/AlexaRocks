#include <serialize.h>
#include <stdarg.h>
#include "packet.h"
#include "constants.h"
#include <Wire.h>
#include <SparkFunLSM9DS1_test.h>
#include <math.h>
//Alex's configuration constants
// Number of ticks per revolution from the wheel encoder.
#define COUNTS_PER_REV      180
// Wheel circumference in cm.
// We will use this to calculate forward/backward distance traveled by taking revs * WHEEL_CIRC
#define WHEEL_CIRC          20.4
#define ALEX_LENGTH 17.5
#define ALEX_BREADTH 12.5
float alexDiagonal = 0.0;
float alexCirc = 0.0;
LSM9DS1 imu;
#define DECLINATION +0.08 // Declination (degrees) in Singapore 
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

float redFrequency = 0;
float greenFrequency = 0;
float blueFrequency = 0;

// Store the ticks from Alex's left and right encoders.
volatile unsigned long leftForwardTicks, rightForwardTicks, leftReverseTicks, rightReverseTicks;
// Left and right encoder ticks for turning
volatile unsigned long leftForwardTicksTurns, rightForwardTicksTurns, leftReverseTicksTurns, rightReverseTicksTurns;
// Store the revolutions on Alex's left and right wheels
volatile unsigned long leftRevs, rightRevs;
// Forward and backward distance traveled
volatile unsigned long forwardDist, reverseDist;
unsigned long deltaDist, newDist;
//variables to keep track of our turning angle
unsigned long deltaTicks, targetTicks;
// Enable pull up resistors on pins 2 and 3
float getHeading(float ax, float ay, float az, float mx, float my, float mz)
{
//  float roll = atan2(ay, az);
//  float pitch = atan2(-ax, sqrt(ay * ay + az * az));
  float heading;
  if (my == 0)
    heading = (mx < 0) ? PI : 0;
  else
    heading = atan2(mx, my);

  heading -= DECLINATION * PI / 180;

  if (heading > PI) heading -= (2 * PI);
  else if (heading < -PI) heading += (2 * PI);

  // Convert everything from radians to degrees:
  return heading *= 180.0 / PI;
  //  pitch *= 180.0 / PI;
  //  roll  *= 180.0 / PI;

  //  Serial.print("Pitch, Roll: ");
  //  Serial.print(pitch, 2);
  //  Serial.print(", ");
  //  Serial.println(roll, 2);
  //  Serial.print("Heading: "); Serial.println(heading, 2);
}

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

void detectColour()
{
}

void setupColour()
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);  
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  
  pinMode(sensorOut, INPUT);
  
  // Setting frequency scaling to 20%
  digitalWrite(S0,HIGH);  
  digitalWrite(S1,LOW);
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  if (imu.begin() == false) // with no arguments, this uses default addresses (AG:0x6B, M:0x1E) and i2c port (Wire).
  {
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    Serial.println("Default settings in this sketch will " \
                   "work for an out of the box LSM9DS1 " \
                   "Breakout, but may need to be modified " \
                   "if the board jumpers are.");
    while (1);
  }
  alexDiagonal = sqrt((ALEX_LENGTH * ALEX_LENGTH) + (ALEX_BREADTH * ALEX_BREADTH));
  alexCirc = PI * alexDiagonal;
  cli();
  setupEINT();
  setupSerial();
  startSerial();
  setupMotors();
  startMotors();
  enablePullups();
  initializeState();
  sei();
}

void loop() {

  // put your main code here, to run repeatedly:
  TPacket recvPacket; // This holds commands from the Pi

  TResult result = readPacket(&recvPacket);

  if (result == PACKET_OK)handlePacket(&recvPacket);
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
  }
  if ( imu.accelAvailable() )
  {
    // To read from the accelerometer, first call the
    // readAccel() function. When it exits, it'll update the
    // ax, ay, and az variables with the most current data.
    imu.readAccel();
  }
  if ( imu.magAvailable() )
  {
    // To read from the magnetometer, first call the
    // readMag() function. When it exits, it'll update the
    // mx, my, and mz variables with the most current data.
    imu.readMag();
  }
  float heading = getHeading(imu.ax, imu.ay, imu.az,-imu.my, -imu.mx, imu.mz);


  if (deltaDist > 0 && ((dir == FORWARD && forwardDist > newDist) || (dir == BACKWARD && reverseDist > newDist) || dir == STOP))
  {
    deltaDist = 0;
    newDist = 0;
    stop();
  }

  if (deltaTicks > 0 && ((dir == LEFT && leftReverseTicksTurns >= targetTicks) || (dir == RIGHT && rightReverseTicksTurns >= targetTicks) || dir == STOP))
  {
    deltaTicks = 0;
    targetTicks = 0;
    stop();
  }
}

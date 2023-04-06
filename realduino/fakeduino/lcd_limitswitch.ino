#include <serialize.h>
#include <stdarg.h>
#include "packet.h"
#include "constants.h"
#include <Wire.h>
#include <SparkFunLSM9DS1_test.h>
#include <math.h>
#include <LiquidCrystal_I2C.h>

//I2C pins declaration
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

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
#define NO_PIN 1
#define sensorOut 12


// Motor proportional control
float motor_speed = 60; // 60% speed
float proportional = 0.4;

// Colour Sensor Stuff
float redFrequency = 0;
float greenFrequency = 0;
float blueFrequency = 0;

// Store the ticks from Alex's left and right encoders (NOT REALLY NEEDED)
volatile unsigned long leftForwardTicks, rightForwardTicks, leftReverseTicks, rightReverseTicks;
volatile unsigned long leftForwardTicksTurns, rightForwardTicksTurns, leftReverseTicksTurns, rightReverseTicksTurns;
volatile unsigned long leftRevs, rightRevs;
volatile unsigned long forwardDist, reverseDist;
unsigned long deltaDist, newDist;
unsigned long deltaTicks, targetTicks;

void printLCD(char* string) {
  lcd.setCursor(0,0);
  lcd.print(" ");
  lcd.print(string);
  lcd.print("  ");
  delay(8000);
}

void limitSwitch() {
  if (digitalRead(NO_PIN) == HIGH) {
    sendMessage("LIMIT SWITCH PRESSED\n");
  } 
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

char* detectColour()
{
  redFrequency = 0;
  greenFrequency = 0;
  blueFrequency = 0;

  float val = 0;

  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  for (int i = 0; i < 10; i++) {
    val = pulseIn(sensorOut, LOW);
    redFrequency += val / 10;
  }
  delay(100);

  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  for (int i = 0; i < 10; i++) {
    val = pulseIn(sensorOut, LOW);
    greenFrequency += val / 10;
  }
  delay(100);

  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  for (int i = 0; i < 10; i++) {
    val = pulseIn(sensorOut, LOW);
    blueFrequency += val / 10;
  }
  delay(100);

  if (redFrequency + greenFrequency + blueFrequency < 650) {
    sendMessage("WHITE");
    return "white";
  } else if (redFrequency + greenFrequency + blueFrequency > 1800) {
    sendMessage("EMPTY");
    return "empty";
  } else if (greenFrequency / redFrequency > 1.8) {
    sendMessage("RED");
    return "red";
  } else if (redFrequency - greenFrequency > 60) {
    sendMessage("GREEN");
    return "green";
  } else {
    sendMessage("???");
    return "?";
  }
}

void setupColour()
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(sensorOut, INPUT);

  // Setting frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  lcd.begin(16,2);
  lcd.backlight();
  // IMU Setup
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
  calcError();
  currentTime = micros();
  pinMode(NO_PIN, INPUT);
  // alexDiagonal = sqrt((ALEX_LENGTH * ALEX_LENGTH) + (ALEX_BREADTH * ALEX_BREADTH));
  alexCirc = PI * ALEX_BREADTH;
  cli();
  setupEINT();
  setupSerial();
  startSerial();
  setupMotors();
  setupColour();
  startMotors();
  enablePullups();
  initializeState();
  sei();
}

void loop() {

  // RECEIVE PACKETS
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

  //Limit Switch code
  limitswitch();
  
  // IMU SENSOR CODE
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
  elapsedTime = (currentTime - previousTime)/1000000; 
  gyroZ -= gyroErrorZ; 
  degree += gyroZ*elapsedTime;  

  // Execute proportional_control
  if (dir == FORWARD || dir == BACKWARD)
  {
    proportional_control(dir);
  }
  
  // OLD MOTOR CONTROLS (NOT REALLY NEEDED)
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

  char *colour = detectColour();
  if (colour == "red" || colour == ""green") {
    printLCD(colour);
  }
}

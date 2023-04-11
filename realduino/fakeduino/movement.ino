unsigned long computeDeltaTicks(float ang)
{
  unsigned long ticks = (unsigned long) ((ang * alexCirc * COUNTS_PER_REV) / (360.0 * WHEEL_CIRC));
  return ticks;
}
// Move Alex forward "dist" cm at speed "speed".
// "speed" is expressed as a percentage. E.g. 50 is
// move forward at half speed.
// Specifying a distance of 0 means Alex will
// continue moving forward indefinitely.
void forward(float dist, float speed)
{
  if (dist > 0)
    deltaDist = dist;
  else
    deltaDist = 9999999;

  newDist = forwardDist + deltaDist;

  dir = FORWARD;
  int val = pwmVal(speed);

  // For now we will ignore dist and move
  // forward indefinitely. We will fix this
  // in Week 9.

  // LF = Left forward pin, LR = Left reverse pin
  // RF = Right forward pin, RR = Right reverse pin
  // This will be replaced later with bare-metal code.

  analogWrite(LF, val);
  analogWrite(RF, val);
  analogWrite(LR, 0);
  analogWrite(RR, 0);
}

// Reverse Alex "dist" cm at speed "speed".
// "speed" is expressed as a percentage. E.g. 50 is
// reverse at half speed.
// Specifying a distance of 0 means Alex will
// continue reversing indefinitely.
void reverse(float dist, float speed)
{
  if (dist > 0)
    deltaDist = dist;
  else
    deltaDist = 9999999;

  newDist = reverseDist + deltaDist;

  dir = BACKWARD;
  int val = pwmVal(speed);

  // For now we will ignore dist and
  // reverse indefinitely. We will fix this
  // in Week 9.

  // LF = Left forward pin, LR = Left reverse pin
  // RF = Right forward pin, RR = Right reverse pin
  // This will be replaced later with bare-metal code.
  analogWrite(LR, val);
  analogWrite(RR, val);
  analogWrite(LF, 0);
  analogWrite(RF, 0);
}

// Turn Alex left "ang" degrees at speed "speed".
// "speed" is expressed as a percentage. E.g. 50 is
// turn left at half speed.
// Specifying an angle of 0 degrees will cause Alex to
// turn left indefinitely.
void left(float ang, float speed)
{
  if (ang == 0) {
    deltaTicks = 9999999;
  } else {
    deltaTicks = computeDeltaTicks(ang);
  }
  targetTicks = leftReverseTicksTurns + deltaTicks;
  dir = LEFT;
  int val = pwmVal(speed);

  // We will replace this code with bare-metal later.
  // To turn left we reverse the left wheel and move
  // the right wheel forward.
  analogWrite(LR, val);
  analogWrite(RF, val);
  analogWrite(LF, 0);
  analogWrite(RR, 0);
}

// Turn Alex right "ang" degrees at speed "speed".
// "speed" is expressed as a percentage. E.g. 50 is
// turn left at half speed.
// Specifying an angle of 0 degrees will cause Alex to
// turn right indefinitely.
void right(float ang, float speed)
{
  if (ang == 0) {
    deltaTicks = 9999999;
  } else {
    deltaTicks = computeDeltaTicks(ang);
  }
  targetTicks = rightReverseTicksTurns + deltaTicks;
  dir = RIGHT;
  int val = pwmVal(speed);

  // For now we will ignore ang. We will fix this in Week 9.
  // We will also replace this code with bare-metal later.
  // To turn right we reverse the right wheel and move
  // the left wheel forward.
  analogWrite(RR, val);
  analogWrite(LF, val);
  analogWrite(LR, 0);
  analogWrite(RF, 0);
}

/* void proportional_control(TDirection dir)
{
  float future = degree - previousdegree;
  int control = int(round(degree * proportional)); // future * derivative;

  if (dir == FORWARD)
  {
    if (degree > 0.3)
    {
      val_1 -= control;
    } else if (degree < -0.3) {
      val_1 += control;
    }

    if (val_1 < 0) val_1 = 0;
    if (val_1 > 255) val_1 = 255;
  
    analogWrite(RF, val_1);
    previousdegree = degree;
  }
  else if (dir == BACKWARD)
  {
    if (degree > 0.3)
    {
      val_1 -= control;
    } else if (degree < -0.3) {
      val_1 += control;
    }

    if (val_1 < 0) val_1 = 0;
    if (val_1 > 255) val_1 = 255;
  
    analogWrite(RR, val_1);
    previousdegree = degree;
  }
} */

void inch_forward()
{
  int val = pwmVal(motor_speed);
  analogWrite(LF,val);
  analogWrite(RF,val);
  analogWrite(LR, 0);
  analogWrite(RR, 0);
}

void inch_backward()
{
  int val = pwmVal(motor_speed);
  analogWrite(LF, 0);
  analogWrite(RF, 0);
  analogWrite(LR, val);
  analogWrite(RR, val);
}

void inch_left()
{
  int val = pwmVal(motor_speed);
  analogWrite(LF, 0);
  analogWrite(RF, val);
  analogWrite(LR, val);
  analogWrite(RR, 0);
}

void inch_right()
{
  int val = pwmVal(motor_speed);
  analogWrite(LF, val);
  analogWrite(RF, 0);
  analogWrite(LR, 0);
  analogWrite(RR, val);
}

// Stop Alex. To replace with bare-metal code later.
void stop()
{
  dir = STOP;
  analogWrite(LF, 0);
  analogWrite(LR, 0);
  analogWrite(RF, 0);
  analogWrite(RR, 0);
}

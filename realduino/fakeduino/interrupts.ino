// Functions to be called by INT0 and INT1 ISRs.
void leftISR()
{
  if (dir == FORWARD) {
    leftForwardTicks ++;
    forwardDist = (unsigned long)((float)leftForwardTicks / COUNTS_PER_REV * WHEEL_CIRC);
  }
  if (dir == BACKWARD) {
    leftReverseTicks++; reverseDist = (unsigned long)((float)leftReverseTicks / COUNTS_PER_REV * WHEEL_CIRC);
  }
  if (dir == LEFT) leftReverseTicksTurns++;
  if (dir == RIGHT) leftForwardTicksTurns++;
}

void rightISR()
{
  if (dir == FORWARD) rightForwardTicks ++;
  if (dir == BACKWARD) rightReverseTicks++;
  if (dir == LEFT) rightForwardTicksTurns++;
  if (dir == RIGHT) rightReverseTicksTurns++;
  // Serial.print("RIGHT: ");
  // Serial.println((double)rightTicks / COUNTS_PER_REV * WHEEL_CIRC);
}

// Set up the external interrupt pins INT0 and INT1
// for falling edge triggered. Use bare-metal.
void setupEINT()
{
  // Use bare-metal to configure pins 2 and 3 to be
  // falling edge triggered. Remember to enable
  // the INT0 and INT1 interrupts.
  EICRA = 0b1110;
  EIMSK |= 0b11;

}

// Implement the external interrupt ISRs below.
// INT0 ISR should call leftISR while INT1 ISR
// should call rightISR.

ISR(INT0_vect)
{
  leftISR();
}

ISR(INT1_vect)
{
  // rightISR();
  sendMessage("TOUCHED");
}

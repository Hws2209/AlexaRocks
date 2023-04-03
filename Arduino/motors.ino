void setupMotors()
{
  DDRD = (1 << 5) | (1 << 6);
  DDRB = (1 << 2) | (1 << 3);

  //  /* Our motor set up is:
  //        A1IN - Pin 5, PD5, OC0B
  //        A2IN - Pin 6, PD6, OC0A
  //        B1IN - Pin 10, PB2, OC1B
  //        B2In - pIN 11, PB3, OC2A
}

// Start the PWM for Alex's motors.
// We will implement this later. For now it is
// blank.
void startMotors()
{


}

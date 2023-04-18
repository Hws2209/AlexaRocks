void setupMotors()
{
  DDRD = (1 << 5) | (1 << 6);
  DDRB = (1 << 2) | (1 << 3);

  //  /* Our motor set up is:
  //        A1IN - Pin 5, PD5, OC0B
  //        A2IN - Pin 6, PD6, OC0A
  //        B1IN - Pin 10, PB2, OC1B
  //        B2IN - Pin 11, PB3, OC2A
}

// Start the PWM for Alex's motors.
// We will implement this later. For now it is
// blank.
void startMotors()
{
   TCCR1A |= (1 << COM1B1);  // Setup RR pins for PWM
   OCR1B = 0;

   TCCR2A |= (1 << COM2A1);  // Setup RF pins for PWM
   OCR2A = 0;

   TCCR0A |= 0b10100000;    // Setup LF & LR pins for PWM
   OCR0A = 0;
   OCR0B = 0;
}

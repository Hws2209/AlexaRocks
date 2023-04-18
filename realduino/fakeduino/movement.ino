void inch_forward()
{
  int val = pwmVal(motor_speed);
  OCR0A = 0;   //LR
  OCR0B = val;   //LF
  OCR2A = val;   //RF
  OCR1B = 0;   //RR
}

void inch_backward()
{
  int val = pwmVal(motor_speed);
  OCR0A = val;   //LR
  OCR0B = 0;   //LF
  OCR2A = 0;   //RF
  OCR1B = val;   //RR
}

void inch_left()
{
  int val = pwmVal(motor_speed);
  OCR0A = val;   //LR
  OCR0B = 0;   //LF
  OCR2A = val;   //RF
  OCR1B = 0;   //RR
}

void inch_right()
{
  int val = pwmVal(motor_speed);
  OCR0A = 0;   //LR
  OCR0B = val;   //LF
  OCR2A = 0;   //RF
  OCR1B = val;   //RR
}

// Stop Alex. To replace with bare-metal code later.
void stop()
{
  dir = STOP;
  OCR0A = 0;   //LR
  OCR0B = 0;   //LF
  OCR2A = 0;   //RF
  OCR1B = 0;   //RR
  
}

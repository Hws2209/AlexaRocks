void setupColour()
{
  DDRB |= 0b00100011;
  DDRB &= ~(0b00010000);
  DDRD |= 0b10000000;
  
  /*
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  */

  // Setting frequency scaling to 20%
  PORTB |= 0b00100000;
  PORTB &= ~(0b10);
  
  /*
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  */
}

double calcHue()
{
  double max = rgb[0];
  double min = rgb[0];
  for (int i = 1; i < 3; i++) 
  {
    if (max < rgb[i])
    {
      max = rgb[i];
    } else if (min > rgb[i]) 
    {
      min = rgb[i];
    }
  }

  if (max == rgb[0])
  {
    return ((rgb[1] - rgb[2]) / (max - min));
  }
  if (max == rgb[1])
  {
    return 2.0 + ((rgb[2] - rgb[0]) / (max - min));
  }
  if (max == rgb[2])
  {
    return 4.0 + ((rgb[0] - rgb[1]) / (max - min));
  }
}

void detectColour()
{
  redFrequency = 0;
  greenFrequency = 0;
  blueFrequency = 0;

  float val = 0;

  // Setting RED (R) filtered photodiodes to be read
  
  PORTB &= ~(0b00000001);
  PORTD &= ~(0b10000000);
  
  /*
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  */
  
  for (int i = 0; i < 10; i++) {
    val = pulseIn(sensorOut, LOW);
    redFrequency += val / 10;
  }
  delay(100);

  // Setting GREEN (G) filtered photodiodes to be read
  
  PORTB |= 0b00000001;
  PORTD |= 0b10000000;
  
  /*
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  */
  
  for (int i = 0; i < 10; i++) {
    val = pulseIn(sensorOut, LOW);
    greenFrequency += val / 10;
  }
  delay(100);

  // Setting BLUE (B) filtered photodiodes to be read
  
  PORTB &= ~(0b00000001);
  PORTD |= 0b10000000;
  
  /*
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  */
  
  for (int i = 0; i < 10; i++) {
    val = pulseIn(sensorOut, LOW);
    blueFrequency += val / 10;
  }
  delay(100);

  if (redFrequency + greenFrequency + blueFrequency < 1200) {
    sendMessage("WHITE");
  } else if (greenFrequency / redFrequency > 2.0 && greenFrequency > blueFrequency) {
    sendMessage("RED");
  } else if (greenFrequency < redFrequency && greenFrequency < blueFrequency) {
    sendMessage("GREEN");
  } else {
    sendMessage("???");
  }
  
  rgb[0] = map(redFrequency, 210, 1500, 255, 0);
  rgb[1] = map(greenFrequency, 250, 2400, 255, 0);
  rgb[2] = map(blueFrequency, 200, 2200, 255, 0);
  hue = calcHue() * 60;
}

void ultrasonic() {
  
  PORTD &= ~(0b1000);
  //digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  //digitalWrite(trigPin, HIGH);
  PORTD |= 0b1000;
  delayMicroseconds(10);

  //digitalWrite(trigPin, LOW);
  PORTD &= ~(0b1000);
  duration = pulseIn(echoPin, HIGH);

  distance = duration / 58.0;
}

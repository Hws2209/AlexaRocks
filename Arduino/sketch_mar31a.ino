#define S0 13
#define S1 9
#define S2 8
#define S3 7
#define sensorOut 12

// Stores frequency read by the photodiodes
float redFrequency = 0;
float greenFrequency = 0;
float blueFrequency = 0;

void setup() {
  // Setting the outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  
  // Setting the sensorOut as an input
  pinMode(sensorOut, INPUT);
  
  // Setting frequency scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  
   // Begins serial communication 
  Serial.begin(9600);
}

void getAvg()
{
  float val;
  
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);

  redFrequency = 0;
  for (int i = 0; i < 10; i++) { 
  // Reading the output frequency
    val = pulseIn(sensorOut, LOW);
    redFrequency += val/10;
  }
  delay(100);
  
  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  
  greenFrequency = 0;
  for (int i = 0; i < 10; i++) {
  // Reading the output frequency
    val = pulseIn(sensorOut, LOW);
    greenFrequency += val/10;
  }
  delay(100);
 
  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);

  blueFrequency = 0;
  for (int i = 0; i < 10; i++) {
  // Reading the output frequency
    val = pulseIn(sensorOut, LOW);
    blueFrequency += val/10;
  }
  delay(100); 

  Serial.print(redFrequency); 
  Serial.print(", ");
  Serial.print(greenFrequency); 
  Serial.print(", ");
  Serial.print(blueFrequency);
  Serial.print(", ");
}

void loop() {
  getAvg();
  if (redFrequency + greenFrequency + blueFrequency < 650) {
    Serial.println("WHITE");
  } else if (redFrequency + greenFrequency + blueFrequency >= 1800) {
    Serial.println("EMPTY");
  } else if (greenFrequency / redFrequency > 1.8) {
    Serial.println("RED");
  } else if (redFrequency - greenFrequency > 60) {
    Serial.println("GREEN");
  } else {
    Serial.println("WHAT'S THIS?");
  }
}

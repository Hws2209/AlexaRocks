// Set up the serial connection. 
void setupSerial()
{
  UBRR0H = 0;
  UBRR0L = 103;

  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
  UCSR0A = 0;
}

// Start the serial connection.
void startSerial()
{
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);  // Start Transmiter and receiver
}

// Read the serial port. Returns the read character in
// ch if available. Also returns TRUE if ch is valid.
int readSerial(char *buffer)
{
  int count = 0;
  while (UCSR0A & (1 << RXC0)) {
    buffer[count++] = UDR0;
  }
  return count;
}

// Write to the serial port. 
void writeSerial(const char *buffer, int len)
{
  for (int i = 0; i < len; i++) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = buffer[i];
  }
}

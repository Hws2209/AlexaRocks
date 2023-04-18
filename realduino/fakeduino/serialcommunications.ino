// Set up the serial connection. For now we are using
// Arduino Wiring, you will replace this later
// with bare-metal code.
void setupSerial()
{
  // To replace later with bare-metal.
  UBRR0H = 0;
  UBRR0L = 103;

  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
  UCSR0A = 0;
}

// Start the serial connection. For now we are using
// Arduino wiring and this function is empty. We will
// replace this later with bare-metal code.

void startSerial()
{
  // Empty for now. To be replaced with bare-metal code
  // later on.
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);  // Start Transmiter and receiver
}

// Read the serial port. Returns the read character in
// ch if available. Also returns TRUE if ch is valid.
// This will be replaced later with bare-metal code.

int readSerial(char *buffer)
{
  int count = 0;
  while (UCSR0A & (1 << RXC0)) {
    buffer[count++] = UDR0;
  }
  return count;
}

// Write to the serial port. Replaced later with
// bare-metal code

void writeSerial(const char *buffer, int len)
{
  for (int i = 0; i < len; i++) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = buffer[i];
  }
}

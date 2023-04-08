TResult readPacket(TPacket *packet)
{
  // Reads in data from the serial port and
  // deserializes it.Returns deserialized
  // data in "packet".

  char buffer[PACKET_SIZE];
  int len;

  len = readSerial(buffer);

  if (len == 0)
    return PACKET_INCOMPLETE;
  else
    return deserialize(buffer, len, packet);

}

void handleCommand(TPacket *command)
{
  switch (command->command)
  {
    // For movement commands, param[0] = distance, param[1] = speed.
    case COMMAND_FORWARD:
      sendOK();
      forward((float) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_REVERSE:
      sendOK();
      reverse((float) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_TURN_LEFT:
      sendOK();
      left((float) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_TURN_RIGHT:
      sendOK();
      right((float) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_STOP:
      sendOK();
      stop();
      break;
    case COMMAND_GET_STATS:
      sendStatus();
      break;
    case COMMAND_CLEAR_STATS:
      sendOK();
      clearOneCounter(command->params[0]);
      break;
    case COMMAND_DETECT_COLOUR:
      detectColour();
      sendColour();
      break;
    case COMMAND_F:
      dir = FORWARD;
      degree = 0;
      val_1 = pwmVal(motor_speed);
      val_2 = pwmVal(motor_speed);
      inch_forward();
      break;
    case COMMAND_B:
      dir = BACKWARD;
      degree = 0;
      val_1 = pwmVal(motor_speed);
      val_2 = pwmVal(motor_speed);
      inch_backward();
      break;
    case COMMAND_L:
      dir = LEFT;
      inch_left();
      break;
    case COMMAND_R:
      dir = RIGHT;
      inch_right();
      break;
    case COMMAND_DECREASE:
      if (motor_speed > 0) 
      {
        motor_speed -= 10.0;
      }
      break;
    case COMMAND_INCREASE:
      if (motor_speed < 100)
      {
        motor_speed += 10.0;
      }
      break;

    default:
      sendBadCommand();
  }
}

void handlePacket(TPacket *packet)
{
  switch (packet->packetType)
  {
    case PACKET_TYPE_COMMAND:
      handleCommand(packet);
      break;

    case PACKET_TYPE_RESPONSE:
      break;

    case PACKET_TYPE_ERROR:
      break;

    case PACKET_TYPE_MESSAGE:
      break;

    case PACKET_TYPE_HELLO:
      break;
  }
}

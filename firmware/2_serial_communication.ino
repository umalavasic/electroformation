// MARK: - Serial Communication Functions

void receiveProtocol() {
  static bool receiving = false;
  static unsigned index;
  char startMarker = '<', endMarker = '>', receivedByte;

  while (Serial.available() > 0 && !receivedProtocol) {
    receivedByte = Serial.read();

    if (receiving) {
      if (receivedByte != endMarker) {
        receivedMessage[index] = receivedByte;
        index++;
        if (index >= messageSize) {
          index = messageSize - 1;
        }
      } else {
        receivedMessage[index] = '\0';
        receiving = false;
        index = 0;
        receivedProtocol = true;
      }
    } else if (receivedByte == startMarker) {
      receiving = true;
    }
  }
}

void receivedProtocolMessage() {
  parseProtocol();
  exportProtocol();
  saveProtocol();
  setRGBColor(colors.blue);
  if (!BUTTON_IS_USED) {
    runProtocol();
  }
}

void parseProtocol() {
  char * token = receivedMessage;
  unsigned int stages = 0;
  for (int i = 0; token; i++) {
    token = (i == 0) ? strtok(receivedMessage, ",") : strtok(NULL, ",");
    if (!token) {
      // When the token == NULL, we've reached the end of data.
      continue;
    }
    unsigned long duration = atol(token);
    token = strtok(NULL, ",");
    float frequency = atof(token);
    token = strtok(NULL, ";");
    float amplitude = atof(token);

    protocol[i] = { duration, frequency, amplitude };
    stages++;
  }
  protocolStages = stages;
}

void exportProtocol() {
  Serial.print("<");
  for (int i = 0; i < protocolStages; i++) {
    unsigned long duration = protocol[i].duration;
    float frequency = protocol[i].frequency;
    float amplitude = protocol[i].amplitude;

    if (i != 0) {
      Serial.print(";");
    }
    Serial.print(duration);
    Serial.print(",");
    Serial.print(frequency);
    Serial.print(",");
    Serial.print(amplitude);
  }
  Serial.print(">");
}

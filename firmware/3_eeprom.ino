// MARK: - EEPROM Functions

void saveProtocol() {
  EEPROM.write(0, protocolStages); // Save number of stages to address 0

  for (int i = 0; i < protocolStages; i++) {
    Protocol stage = protocol[i];

    unsigned long duration = stage.duration;
    float frequency = stage.frequency;
    float amplitude = stage.amplitude;

    int durationAddress = (sizeof(duration) + sizeof(frequency) + sizeof(amplitude)) * i + 1;
    int frequencyAddress = durationAddress + sizeof(duration);
    int amplitudeAddress = frequencyAddress + sizeof(frequency);

    writeUnsignedLong(duration, durationAddress);
    writeFloat(frequency, frequencyAddress);
    writeFloat(amplitude, amplitudeAddress);
  }
}

void loadProtocol() {
  protocolStages = EEPROM.read(0);

  for (int i = 0; i < protocolStages; i++) {
    unsigned long duration;
    float frequency;
    float amplitude;

    int durationAddress = (sizeof(duration) + sizeof(frequency) + sizeof(amplitude)) * i + 1;
    int frequencyAddress = durationAddress + sizeof(duration);
    int amplitudeAddress = frequencyAddress + sizeof(frequency);

    duration = readUnsignedLong(durationAddress);
    frequency = readFloat(frequencyAddress);
    amplitude = readFloat(amplitudeAddress);

    protocol[i] = { duration, frequency, amplitude };
  }
}

void writeUnsignedLong(unsigned long value, int startAddress) {
  byte* bytes = (byte*) &value;
  for (int i = 0; i < sizeof(unsigned long); i++) {
    EEPROM.write(startAddress + i, bytes[i]);
  }
}

unsigned long readUnsignedLong(int startAddress) {
  const byte valueSize = sizeof(unsigned long);
  union {
    byte bytes[valueSize];
    unsigned long value;
  } valueUnion;
  for (int i = 0; i < valueSize; i++) {
    valueUnion.bytes[i] = EEPROM.read(startAddress + i);
  }
  return valueUnion.value;
}

void writeFloat(float value, int startAddress) {
  byte* bytes = (byte*) &value;
  for (int i = 0; i < sizeof(float); i++) {
    EEPROM.write(startAddress + i, bytes[i]);
  }
}

float readFloat(int startAddress) {
  const byte valueSize = sizeof(float);
  union {
    byte bytes[valueSize];
    float value;
  } valueUnion;
  for (int i = 0; i < valueSize; i++) {
    valueUnion.bytes[i] = EEPROM.read(startAddress + i);
  }
  return valueUnion.value;
}

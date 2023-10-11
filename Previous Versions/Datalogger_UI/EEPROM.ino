// Function to write a float value to the EEPROM
void writeFloatToEEPROM(int address, float value) {
  union {
    float f;
    byte b[sizeof(float)];
  } data;
  
  data.f = value;
  for (int i = 0; i < sizeof(float); i++) {
    EEPROM.write(address + i, data.b[i]);
  }
}

// Function to read a float value from the EEPROM
float readFloatFromEEPROM(int address) {
  union {
    float f;
    byte b[sizeof(float)];
  } data;
  
  for (int i = 0; i < sizeof(float); i++) {
    data.b[i] = EEPROM.read(address + i);
  }
  
  return data.f;
}
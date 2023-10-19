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


// Function to save a string to the EEPROM starting at address 4046
// Note that ";" will save as a tab delimeter
void saveStringToEEPROM(String data) {
  int address = startAddress;
  for (int i = 0; i < data.length(); i++) {
    char c = data[i];
    if (c == '\t') {
      EEPROM.write(address++, ';'); // Save the custom delimiter ';'
    } else {
      EEPROM.write(address++, c);
    }
  }
  // Write a null terminator to mark the end of the string in the EEPROM
  EEPROM.write(address, '\0');
}

// Function to read a string from the EEPROM starting at address 4046
String readStringFromEEPROM() {
  char buffer[maxStringLength];
  int address = startAddress;
  int i = 0;
  while (i < maxStringLength - 1) { // -1 to leave space for the null terminator
    char c = EEPROM.read(address++);
    if (c == '\0') {
      break; // Found the null terminator, stop reading
    } else if (c == ';') {
      buffer[i++] = '\t'; // Replace the custom delimiter ';' with '\t' (tab)
    } else {
      buffer[i++] = c;
    }
  }
  buffer[i] = '\0'; // Add null terminator manually to terminate the C-style string
  return String(buffer);
}
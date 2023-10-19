

void displayMenu() {
  Serial.println("\n\nMenu:");
  Serial.println("1. Display Date & Time");
  Serial.println("2. Change Date & Time");
  Serial.println("3. Display Log Time & Interval");
  Serial.println("4. Change Log Time & Interval");
  Serial.println("5. Check SD-Card");
  Serial.println("6. Setting OFFSET");
  Serial.println("7. Reset Arduino");
  Serial.println("8. Exit");
  //Serial.println("9. TEST");


  Serial.println("Enter your choice:\n");
  menuDisplayed = false;
}

void processUserInput() {
  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n');
    int choice = inputString.toInt();

    userInterface = processChoice(choice);  // Process the user's choice and get the menu continuation status

    if (!userInterface) {
      menuDisplayed = false;  // Reset the menu display flag
      userInput = "";  // Reset the user input string
    }
  }
  CurrentTime = Clock.read();
    // Check if it's time to exit the menu
  if (CurrentTime.Minute == LogTime && userInterface) {
    Serial.println("DATA LOGGING IN PROCESS... EXITING MENU");
    userInterface = false;
    menuDisplayed = false;
    userInput = "";
  }
}

bool processChoice(int choice) {
  bool continueMenu = true;  // Flag indicating whether to continue the menu or not
  switch (choice) {
  case 1:
    Serial.println("You selected Option 1: Display Date & Time");
    processOption1();
    Serial.println("Completed... Please select another option from the menu\n");
    break;
  case 2:
    Serial.println("You selected Option 2: Change Date");
    processOption2();
    Serial.println("Completed... Please select another option from the menu\n");
    break;
  case 3:
    Serial.println("You selected Option 3: Display Log Time & Interval");
    processOption3();
    Serial.println("Completed... Please select another option from the menu\n");
    break;
  case 4:
    Serial.println("You selected Option 4: Change Log Time & Interval");
    processOption4();
    Serial.println("Completed... Please select another option from the menu\n");
    break;
  case 5:
    Serial.println("You selected Option 5: Check SD-Card");
    processOption5();
    Serial.println("Completed... Please select another option from the menu\n");
    break;
  case 6:
    Serial.println("You selected Option 6: Changing OFFSET");
    processOption6();
    Serial.println("Completed... Please select another option from the menu\n");
    break;
  case 7:
    Serial.println("You selected Option 7: Reset Arduino");
    processOption7();
    Serial.println("Completed... Please select another option from the menu\n");
    break;
  case 8:
    Serial.println("You selected Option 8: Exit");
    processOption8();
    continueMenu = false;  // Break out of the menu
    break;
//  case 9:
//    Serial.println("You selected Option 9: TEST");
//    processOption9();
//    Serial.println("Completed... Please select another option from the menu\n");
//    break;
  default:
    Serial.println("Invalid choice. Please try again.");
    break;

  }
  
  return continueMenu;  // Return the menu continuation status
    
  
}

// Display the current date and time
void processOption1() {
  DateTime ClockTime = Clock.read();  // Taking a clock reading
  Serial.println("Date\t\tTime");
  Serial.println(Date_time(ClockTime));
}

// Change the date
void processOption2() {
  Serial.println("Please Enter Date (yy-mm-dd) *if single digit dont include extra 0*:");
  while (!Serial.available()) {
    // Wait for user input
  }
  String dateString = Serial.readStringUntil('\n');
  Serial.println("Date Entered:" + dateString);
  String delimiter = "-";

  String part1 = dateString.substring(0, dateString.indexOf(delimiter));
  dateString.remove(0, dateString.indexOf(delimiter) + delimiter.length());
  int year = part1.toInt();

  String part2 = dateString.substring(0, dateString.indexOf(delimiter));
  dateString.remove(0, dateString.indexOf(delimiter) + delimiter.length());
  int month = part2.toInt();

  int day = dateString.toInt();

  Serial.println("Please Enter Time (hh:mm:ss) *if single digit dont include extra 0*:");
  while (!Serial.available()) {
    // Wait for user input
  }
  String timeString = Serial.readStringUntil('\n');
  Serial.println("Time Entered:" + timeString);

  delimiter = ":";

  part1 = timeString.substring(0, timeString.indexOf(delimiter));
  timeString.remove(0, timeString.indexOf(delimiter) + delimiter.length());
  int hour = part1.toInt();

  part2 = timeString.substring(0, timeString.indexOf(delimiter));
  timeString.remove(0, timeString.indexOf(delimiter) + delimiter.length());
  int min = part2.toInt();

  int sec = timeString.toInt();

  DateTime MyTimestamp;

  MyTimestamp.Day    = day;
  MyTimestamp.Month  = month;
  MyTimestamp.Year   = year; 
  
  MyTimestamp.Hour   = hour;
  MyTimestamp.Minute = min;
  MyTimestamp.Second = sec;

  // Then write it to the clock
  Clock.write(MyTimestamp);
}

// Change the time
void processOption3() {
  LogTime = EEPROM.read(1);
  interval = EEPROM.read(0);
  Serial.println("Log Time:\t" +String(LogTime));
  Serial.println( "Log Interval:\t" + String(interval));
}

// Change the time
void processOption4() {
  Serial.println("Please Enter Date New Log Time & Interval (log,int) *if single digit dont include extra 0*:");
  while (!Serial.available()) {
    // Wait for user input
  }
  String logString = Serial.readStringUntil('\n');
  String delimiter = ",";

  String part1 = logString.substring(0, logString.indexOf(delimiter));
  logString.remove(0, logString.indexOf(delimiter) + delimiter.length());
  LogTime = part1.toInt();
  
  interval = logString.toInt();

  // Sending new interval and log time to EEPROM memory
  EEPROM.write(0, interval);
  EEPROM.write(1, LogTime);

  Serial.println("New Log Time:\t" + String(LogTime));
  Serial.println("New Log Interval:\t" + String(interval));

}

// Check to see if the SD card is operating
void processOption5() {
  if ((card.init(SPI_HALF_SPEED, CHIP_SELECT)) && (volume.init(card))) {
    Serial.print("SD Card Detected...\n");
    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);

    // Call the custom function to list files and redirect output to Serial
    listFiles("/",0);


  } else {
    Serial.print("**NO SD Card Detected**...\n");
  }
}

// Check to see if the SD card is operating
void processOption6() {
  OFFSET = readFloatFromEEPROM(2);
  Serial.println("Current OFFSET:\t" + String(OFFSET));
  Serial.println("Please Enter New OFFSET:");
  while (!Serial.available()) {
    // Wait for user input
  }
  String newOFFSET = Serial.readStringUntil('\n');
  OFFSET = newOFFSET.toFloat();
  writeFloatToEEPROM(2, OFFSET);
  OFFSET = readFloatFromEEPROM(2);
  Serial.println("\nNew OFFSET:\t" + String(OFFSET));
}

void processOption7() {
  Serial.println("Resetting Arduino...\n");
  delay(1000); // Optional delay for display purposes
  asm volatile ("jmp 0"); // Jump to address 0, which restarts the program

}

void processOption8() {
  Serial.println("Exiting menu");
}

//void processOption9() {
//  Serial.println("Enter test string:");
//  while (!Serial.available()) {
//    // Wait for user input
//  }
//  String testString = Serial.readStringUntil('\n');
//  saveStringToEEPROM(testString);
//  Serial.println("TEST STRING:\n");
//  String hello = readStringFromEEPROM();
//  Serial.println(hello);
//}

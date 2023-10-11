

void displayMenu() {
  Serial1.println("\n\nMenu:");
  Serial1.println("1. Display Date & Time");
  Serial1.println("2. Change Date & Time");
  Serial1.println("3. Display Log Time & Interval");
  Serial1.println("4. Change Log Time & Interval");
  Serial1.println("5. Check SD-Card");
  Serial1.println("6. Setting OFFSET");
  Serial1.println("7. Reset Arduino");
  Serial1.println("8. Exit");
  Serial1.println("Enter your choice:\n");
  menuDisplayed = false;
}

void processUserInput() {
  if (Serial1.available() > 0) {
    String inputString = Serial1.readStringUntil('\n');
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
    Serial1.println("DATA LOGGING IN PROCESS... EXITING MENU");
    userInterface = false;
    menuDisplayed = false;
    userInput = "";
  }
}

bool processChoice(int choice) {
  bool continueMenu = true;  // Flag indicating whether to continue the menu or not
  switch (choice) {
  case 1:
    Serial1.println("You selected Option 1: Display Date & Time");
    processOption1();
    Serial1.println("Completed... Please select another option from the menu\n");
    break;
  case 2:
    Serial1.println("You selected Option 2: Change Date");
    processOption2();
    Serial1.println("Completed... Please select another option from the menu\n");
    break;
  case 3:
    Serial1.println("You selected Option 3: Display Log Time & Interval");
    processOption3();
    Serial1.println("Completed... Please select another option from the menu\n");
    break;
  case 4:
    Serial1.println("You selected Option 4: Change Log Time & Interval");
    processOption4();
    Serial1.println("Completed... Please select another option from the menu\n");
    break;
  case 5:
    Serial1.println("You selected Option 5: Check SD-Card");
    processOption5();
    Serial1.println("Completed... Please select another option from the menu\n");
    break;
  case 6:
    Serial1.println("You selected Option 6: Changing OFFSET");
    processOption6();
    Serial1.println("Completed... Please select another option from the menu\n");
    break;
  case 7:
    Serial1.println("You selected Option 7: Reset Arduino");
    processOption7();
    Serial1.println("Completed... Please select another option from the menu\n");
    break;
  case 8:
    Serial1.println("You selected Option 8: Exit");
    processOption8();
    continueMenu = false;  // Break out of the menu
    break;
  
  default:
    Serial1.println("Invalid choice. Please try again.");
    break;

  }
  
  return continueMenu;  // Return the menu continuation status
    
  
}

// Display the current date and time
void processOption1() {
  DateTime ClockTime = Clock.read();  // Taking a clock reading
  Serial1.println("Date\t\tTime");
  Serial1.println(Date_time(ClockTime));
}

// Change the date
void processOption2() {
  Serial1.println("Please Enter Date (yy-mm-dd) *if single digit dont include extra 0*:");
  while (!Serial1.available()) {
    // Wait for user input
  }
  String dateString = Serial1.readStringUntil('\n');
  Serial1.println("Date Entered:" + dateString);
  String delimiter = "-";

  String part1 = dateString.substring(0, dateString.indexOf(delimiter));
  dateString.remove(0, dateString.indexOf(delimiter) + delimiter.length());
  int year = part1.toInt();

  String part2 = dateString.substring(0, dateString.indexOf(delimiter));
  dateString.remove(0, dateString.indexOf(delimiter) + delimiter.length());
  int month = part2.toInt();

  int day = dateString.toInt();

  Serial1.println("Please Enter Time (hh:mm:ss) *if single digit dont include extra 0*:");
  while (!Serial1.available()) {
    // Wait for user input
  }
  String timeString = Serial1.readStringUntil('\n');
  Serial1.println("Time Entered:" + timeString);

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
  Serial1.println("Log Time:\t" +String(LogTime));
  Serial1.println( "Log Interval:\t" + String(interval));
}

// Change the time
void processOption4() {
  Serial1.println("Please Enter Date New Log Time & Interval (log,int) *if single digit dont include extra 0*:");
  while (!Serial1.available()) {
    // Wait for user input
  }
  String logString = Serial1.readStringUntil('\n');
  String delimiter = ",";

  String part1 = logString.substring(0, logString.indexOf(delimiter));
  logString.remove(0, logString.indexOf(delimiter) + delimiter.length());
  LogTime = part1.toInt();
  
  interval = logString.toInt();

  // Sending new interval and log time to EEPROM memory
  EEPROM.write(0, interval);
  EEPROM.write(1, LogTime);

  Serial1.println("New Log Time:\t" + String(LogTime));
  Serial1.println("New Log Interval:\t" + String(interval));

}

// Check to see if the SD card is operating
void processOption5() {
  if ((card.init(SPI_HALF_SPEED, CHIP_SELECT)) && (volume.init(card))) {
    Serial1.print("SD Card Detected...\n");
    Serial1.println("\nFiles found on the card (name, date and size in bytes): ");
    root.openRoot(volume);

    // Call the custom function to list files and redirect output to Serial1
    listFiles("/",0);


  } else {
    Serial1.print("**NO SD Card Detected**...\n");
  }
}

// Check to see if the SD card is operating
void processOption6() {
  OFFSET = readFloatFromEEPROM(2);
  Serial1.println("Current OFFSET:\t" + String(OFFSET));
  Serial1.println("Please Enter New OFFSET:");
  while (!Serial1.available()) {
    // Wait for user input
  }
  String newOFFSET = Serial1.readStringUntil('\n');
  OFFSET = newOFFSET.toFloat();
  writeFloatToEEPROM(2, OFFSET);
  OFFSET = readFloatFromEEPROM(2);
  Serial1.println("\nNew OFFSET:\t" + String(OFFSET));
}

void processOption7() {
  Serial1.println("Resetting Arduino...\n");
  delay(1000); // Optional delay for display purposes
  asm volatile ("jmp 0"); // Jump to address 0, which restarts the program

}

void processOption8() {
  Serial1.println("Exiting menu");
}


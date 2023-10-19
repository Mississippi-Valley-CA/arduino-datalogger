//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------- USER INTERFACE ------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// Processing the user input for the interface, if the current time is the time for datalogging exit the user interface
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


//----------------------------------------------------- DISPLAYED MENU -----------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// The menu that is displayed when the command "menu" is used
// PLease add and edit to match your desired user interface (option number in menu should be the same as the case number and processOption#() functions)
void displayMenu() {
  Serial.println("\n\nMenu:");
  Serial.println("1. Display Date & Time");
  Serial.println("2. Change Date & Time");
  Serial.println("3. Display Log Time & Interval");
  Serial.println("4. Change Log Time & Interval");
  Serial.println("5. Check SD-Card");
  Serial.println("6. Reset Arduino");
  Serial.println("7. Exit");
  Serial.println("Enter your choice:\n");
  menuDisplayed = false;
}


//---------------------------------------------------- CHOICE SELECTION ----------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
  Processing the user inputs for each choice (1,2,3,...)
  To add a new case please copy the last item and change it to xae number proceeding the one copied
    Make sure all the numebrs have changed in the case (text and processOption#())
  Change the text to the desired output when the option is selected 
*/
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
    Serial.println("You selected Option 6: Reset Arduino");
    processOption6();
    Serial.println("Completed... Please select another option from the menu\n");
    break;
  case 7:
    Serial.println("You selected Option 7: Exit");
    processOption7();
    continueMenu = false;  // Break out of the menu
    break;
  default:
    Serial.println("Invalid choice. Please try again.");
    break;
  }
  return continueMenu;  // Return the menu continuation status 
}


//---------------------------------------------------- CHOICE FUNCTIONS ----------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------
// This is where all the option funcitons are defined (processOption#)
// These functions define what happens when each option are selected
// Please add and edit the functions to obtain the desired interface options

// ---------------------------------------------------------------- OPTION 1 ------------------------------------------------------------------
// Display the current date and time
void processOption1() {
  DateTime ClockTime = Clock.read();  // Taking a clock reading
  Serial.println("Date\t\tTime");
  Serial.println(Date_time(ClockTime));
}

// ---------------------------------------------------------------- OPTION 2 ------------------------------------------------------------------
// Change the date
void processOption2() {
  Serial.println("Use 'exit' to return to the main menu");
  Serial.println("Please Enter Date (yyyy-mm-dd):");
  while (!Serial.available()) {
    // Wait for user input
  }
  String dateString = Serial.readStringUntil('\n');
  
  // if user wants to exit then exit
  if (dateString == "exit") {
    Serial.println("Returning to main menu...");
  }
  // continue editing if not
  else{
    Serial.println("Date Entered:" + dateString);
    String delimiter = "-";

    String part1 = dateString.substring(0, dateString.indexOf(delimiter));
    dateString.remove(0, dateString.indexOf(delimiter) + delimiter.length());
    int year = part1.toInt();

    String part2 = dateString.substring(0, dateString.indexOf(delimiter));
    dateString.remove(0, dateString.indexOf(delimiter) + delimiter.length());
    int month = part2.toInt();

    int day = dateString.toInt();

    Serial.println("Please Enter Time (hh:mm:ss):");
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
}

// ---------------------------------------------------------------- OPTION 3 ------------------------------------------------------------------
// Change the time
void processOption3() {
  LogTime = EEPROM.read(1);
  interval = EEPROM.read(0);
  Serial.println("Log Time:\t" +String(LogTime));
  Serial.println( "Log Interval:\t" + String(interval));
}

// ---------------------------------------------------------------- OPTION 4 ------------------------------------------------------------------
// Change the time
void processOption4() {
  Serial.println("Use 'exit' to return to the main menu");
  Serial.println("Please Enter Date New Log Time & Interval (log,int):");
  while (!Serial.available()) {
    // Wait for user input
  }
  String logString = Serial.readStringUntil('\n');
  // if user wants to exit then exit
  if (logString == "exit") {
    Serial.println("Returning to main menu...");
  }
  // continue editing if not
  else{
    
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

}

// ---------------------------------------------------------------- OPTION 5 ------------------------------------------------------------------
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

// ---------------------------------------------------------------- OPTION 6 ------------------------------------------------------------------
// Reseting the Arduino
void processOption6() {
  Serial.println("Resetting Arduino...\n");
  delay(1000); // Optional delay for display purposes
  asm volatile ("jmp 0"); // Jump to address 0, which restarts the program

}

// ---------------------------------------------------------------- OPTION 7 ------------------------------------------------------------------
// Exiting the Menu
void processOption7() {
  Serial.println("Exiting menu");
}


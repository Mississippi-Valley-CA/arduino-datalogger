/*
 * This is the main program the Arduino datalogger will run off of
 * 
 * Please refer to the manual for more information on changes and how the functions work 
 * 
 * Adding a sensor's address to take measurements from:
 * #define SENSOR_NAME SENSOR_ADDRESS
 * 
 * Taking SDI-12 measurements:
 * float MEASUREMNET_NAME; -> define at begining of void loop{} as seen in sketch
 * MEASUREMNET_NAME = Measurement_Output(SENSOR_ADDRESS, MEASUREMENT_NUM, SLOPE, OFFSET);
 * 
 * Notes on Measurement_Output Function:
 *  SENSOR_ADDRESS => Address of sesnor that the measurement is taken from
 *  MEASUREMENT_NUM => The number of the measurement taken form the sensor (Ie: Pressure Transducer-> 1 = Water level, 2 = Water temp)
 *  SLOPE => Slope appplied to the measurement, will be defined in the sensor manual
 *  OFFSET => Offset applied to the measurement, will be defined by external factors such as location 
 * 
 * If you need to add any custom functions for sensors add to the Sensors_Func tab
 *  This is also where you will find functions to take readings from Air temperature Sensors and battery readings
 *
 */

//------------------------------------------------------------------------- INITIALIZATION -----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>            // Importin Arduino library
#include <EEPROM.h>             // Importing EEPROM memory library 
#include <SDI12.h>              // Importing SDI-12 library  
#include <DS3231_Simple.h>      // Importing the real time clock library

#define DATA_PIN 11             // SDI-12 bus data pin
#define SOIL_SENSOR_ADDRESS 1   // Defines SOIL sensor address
#define BATTERY A0              // Defining the battery analog pin
SDI12 busSDI12(DATA_PIN);       // Defining the SDI-12 bus 

#define Serial_BAUD 9600        // Serial COM rate

DS3231_Simple Clock;            // Initializing thew RTC

#include <SPI.h>                // Importing the SPI library (aids in SD card communication)
#include <SD.h>                 // Importing the SD card library 
Sd2Card card;                   // Initilizing the SD card
SdVolume volume;                // Initilizing the SD card 
SdFile root;                    // Initilizing the SD card

const int CHIP_SELECT = 53;              // The pin that the SD card pin CS is connected to
//  Note for the filename <name> has to be 7 characters or less 
String ORIGINALDATA = "test_O.txt";      // Defining the  where ALL the logged data will be saved to (only change <name>)
String DAILYDATA = "test_D.txt";         // Daily data file that is reset whenever the daily data function is called (only change <name>)  
String incomingByte;                     // Serial input variable (User input)
static boolean ALARM = false;            // Alarm variable -> true if alarm set off, false if alarm has not beenm set off

// Creating a variable to compare time with the set interval
DateTime CurrentTime;
int interval;  // Set alarm every [interval] minutes --> will be using EEPROM memory address 0
int LogTime;   // Set starting interval --> will be using EEPROM memory address 1

// User interface assignments
bool menuDisplayed = false;
String userInput = "";  // String to store user input
bool userInterface = false;


//----------------------------------------------------------------------------- SETUP LOOP -----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
This loop is only ran once every time the Arduino is booted up or rest
In this section we start the serial communication, setup the SDI bus, and clock

You can also reset the files that is going to be used on 
*/
void setup() {
  // Serial setup
  Serial.begin(Serial_BAUD);                   // Begin Serial communication
  Serial.print("\nVersion II Booting Up ...\n");  // Sending text to let user know the program has started

  // SDI-12 setup
  busSDI12.begin();  // Begining the SDI-12 databus to be able to take data
  delay(500);        // Sening 0.5 second delay to allow bus time to boot-up

  // THE CODE BELLOW WILL RESET THE ENTIRE FILE ONLY UNCOMMENT WHEN NEEDED
  // if (SD.exists(DAILYDATA)) {
  //   SD.remove(DAILYDATA);  // Resetting the daily data
  // }
  // file_write(DAILYDATA, "Date\tTime\tHG\tVB");  // Writing the headings to the file define DAILYDATA: (ADD PARAMETERS THAT WILL BE LOGGED)

  //   if(SD.exists(ORIGINALDATA)){
  //     SD.remove(ORIGINALDATA);     // Resetting the original data
  //   }
  //   file_write(ORIGINALDATA, "Date\tTime\tHG\tVB");   // Writing the headings to the file define ORIGINALDATA: (ADD PARAMETERS THAT WILL BE LOGGED)

  // END OF CODE THAT NEEDS TO BE COMMMENTED/UNCOMMENTED WHEN CREATING NEW FILES

  // SD card setup
  SD.begin(CHIP_SELECT);  // Begining the SD card communication on the pin CHIP_SELECT is defined as
  if ((card.init(SPI_HALF_SPEED, CHIP_SELECT)) && (volume.init(card))) {
    Serial.print("SD Card Detected...\n");
  } else {
    Serial.print("**NO SD Card Detected**...\n");
  }

  // Clock Setup
  Clock.begin();
  Clock.disableAlarms();  // Disable any existing alarms

  CurrentTime = Clock.read();
  String StartTime = "Datalogger Time:\t";
  StartTime += Date_time(CurrentTime) + "\n";
  Serial.print(StartTime);

  // Setting default interval(every 10 min) onto the EEPROM address 0  
  interval = EEPROM.read(0);
  interval = 10;
  EEPROM.write(0, interval);
  // Setting default log time onto the EEPROM address 1 
  // Default log time is at the next closest 10 min of the clock 
  LogTime = EEPROM.read(1);
  LogTime = (round(CurrentTime.Minute / 10) + 1) * 10;
  if (LogTime >= 59) {
    LogTime = 0;
  }
  EEPROM.write(1, LogTime);
}


//----------------------------------------------------------------------------- MAIN LOOP ------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
This loop is where the datalogging occurs 
It also is where the commands can be used
  **Please only use "live", "log", and "menu" the last command is for collecting data off of the datalogger
*/
void loop() {
  
  // This string will be sent to the SD card or the Serial
  String DataString;   // String that will be compiled of the date, time, and measured parameters. 
  
  interval = EEPROM.read(0);  // Reading the logging time interval off of EEPROM address 0
  LogTime = EEPROM.read(1);  // Reading the logging time off of EEPROM address 1

  // ADD THE VARIABLES BEING MEASURED AS FLOATS HERE:
  float VWC;    // Taken off of SOIL SENSOR
  float SLTMP;  // Taken off of SOIL SENSOR
  String VB;    // Battery voltage
  
  //----------------------------------------------------------------- DATALOGGING -------------------------------------------------------------------
  //Comparing the minute to the desired time of every 10 min (10, 20, 30...)
  CurrentTime = Clock.read(); // Taking a time measurement
  if (CurrentTime.Minute == LogTime) {

    DataString = Date_time(CurrentTime);  // Adding the date and time to the string that will be logged to the SD card

    // Add measurements here:
    VWC = Measurement_Output(SOIL_SENSOR_ADDRESS, 1, 1.0, 0.0);   // 1st measurement on PT12_SENSOR_ADDRESS
    SLTMP = Measurement_Output(SOIL_SENSOR_ADDRESS, 2, 1.0, 0.0);     // 2nd measurement on PT12_SENSOR_ADDRESS
    VB = BatteryVoltage(BATTERY);
      
    // Adding the parameters to thestring that will be logged to the SD card ( will need to add/subtract depending on parameters being logged )
    DataString += String(VWC) + "\t" + String(SLTMP) + "\t" + String(VB);

    file_write(DAILYDATA, DataString);     // Writing the string with all the parameters to the SD card
    file_write(ORIGINALDATA, DataString);  // Writing the string with all the parameters to the SD card
    delay(500);

    // Adding the interval to the comparison time
    LogTime = LogTime + interval;
    
    // Reseting the log time if we aer past 59 minutes
    if (LogTime >= 59) {
      LogTime = 0;
    }
    EEPROM.write(1, LogTime); // Writing the log time back to the EEPROM address 1

  } 
  //---------------------------------------------------------- END OF DATALOGGING -------------------------------------------------------------------
  

  //----------------------------------------------------------------- SERIAL INPUT ------------------------------------------------------------------ 
  // If there is a Serial input take the input into a integer ( will only take numbers -> floats become integers )   
  if (Serial.available() > 0) {
    incomingByte = Serial.readStringUntil('\n');
    incomingByte.trim();  // Remove leading/trailing whitespaces

    //---------------------------------- WILL TAKE A LIVE READING AND PRINT TO Serial MONITOR IF INPUT IS "live" ------------------------------
    if(incomingByte == "live"){
      
      DateTime MyTimestamp = Clock.read();  // Taking a clock reading
      DataString = Date_time(MyTimestamp);  // Adding the date and time to the string that will be printed to the Serial monitor
      
      // Add measurements here (should be same as logging to sd card):       
      VWC = Measurement_Output(SOIL_SENSOR_ADDRESS, 1, 1.0, 0.0);       // 1st measurement on PT12_SENSOR_ADDRESS
      SLTMP = Measurement_Output(SOIL_SENSOR_ADDRESS, 2, 1.0, 0.0);     // 2nd measurement on PT12_SENSOR_ADDRESS
      VB = BatteryVoltage(BATTERY);

     
      // Adding the parameters to thestring that will be printed to Serial( should be same as logging to sd card )
      DataString += String(VWC) + "\t" + String(SLTMP) + "\t" + String(VB);
      Serial.println("\nDate\t\tTime\t\tVWC\tSOILTMP\tVB");
      Serial.println(DataString);
     
    }
      
    //-------------------WILL PRINT ALL THE DATA FROM ORIGINALDATA IF INPUT IS "log" (Mainly used for complete data dump)--------------------
    else if (incomingByte == "log"){
      data_output(ORIGINALDATA); // Outputing the daat to Serial monitor       
    }

    // WILL ENTER THE USER INTERFACE IF THE INPUT IS "menu" (SEE User_Interface Tab)
    else if(incomingByte == "menu"){
      userInterface = true; // Turn on the user interface
      while(userInterface){ // Run the user interface until turned off by user or datalogging begins
        if (!menuDisplayed) {
          displayMenu();  // Display the menu options only once
          menuDisplayed = true;
        }
        processUserInput();  // Process user input continuously

      }

    }
    
    //-------------------WILL PRINT THE DAILY DATA FROM DAILYDATA IF INPUT IS "9999" (Mainly used for getting data into WISKI)----------------
    else if (incomingByte == "9999"){
      data_output(DAILYDATA);     // Outputing the daat to Serial monitor
      SD.remove(DAILYDATA);     // Reseting the daily data

      file_write(DAILYDATA, "Date\tTime\tVWC\tSOILTMP");  // Writing the headings to the file define DAILYDATA: (ADD PARAMETERS THAT WILL BE LOGGED)
    }

  } 
  //--------------------------------------------------------- END OF SERIAL INPUT ------------------------------------------------------------------

}




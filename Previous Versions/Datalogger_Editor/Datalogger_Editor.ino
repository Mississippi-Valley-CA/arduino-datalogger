/*
 * This is the program the Arduino datalogger will run off of
 * 
 * Please refer to the manual for in depth knowledge on changes and how the functions work 
 * 
 * Adding a sensor's address to take measurements from:
 * #define SENSOR_NAME SENSOR_ADDRESS
 * 
 * Taking measurements:
 * float MEASUREMNET_NAME; -> define at begining of void loop{} as seen in sketch
 * MEASUREMNET_NAME = Measurement_Output(SENSOR_ADDRESS, MEASUREMENT_NUM, SLOPE, OFFSET);
 * 
 * Notes on Measurement_Output Function:
 * SENSOR_ADDRESS => Address of sesnor that the measurement is taken from
 * MEASUREMENT_NUM => The number of the measurement taken form the sensor (Ie: Pressure Transducer-> 1 = Water level, 2 = Water temp)
 * SLOPE => Slope appplied to the measurement, will be defined in the sensor manual
 * OFFSET => Offset applied to the measurement, will be defined by external factors such as location
 * 
 */
 
//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// INITIALIZATION
#include <Arduino.h>
#include <EEPROM.h>
#include <SDI12.h>              // Importing the SDI12 library
#define DATA_PIN 11              // SDI-12 bus data pin
#define SOIL_SENSOR_ADDRESS 1   // Defines SOIL sensor address
float OFFSET;
#define BATTERY A0              // Defining the battery
SDI12 busSDI12(DATA_PIN);       // Defining the SDI-12 bus 

const int maxStringLength = 50; // Define the maximum length of the string you want to store
const int startAddress = 4046;  // Starting address in the EEPROM to store the string


#define Serial_BAUD 9600        // Serial COM rate

#include <DS3231_Simple.h>      // Importing the real time clock library
DS3231_Simple Clock;            // Initializing thew RTC

#include <SPI.h>                // Importing the SPI library (aids in SD card communication)
#include <SD.h>                 // Importing the SD card library 
Sd2Card card;
SdVolume volume; 
SdFile root; 

const int CHIP_SELECT = 53;              // The pin that the SD card pin CS is connected to
String ORIGINALDATA = "data_O.txt";      // Defining the  where ALL the logged data will be saved to (only change <name>)
String DAILYDATA = "data_D.txt";         // Daily data file that is reset whenever the daily data function is called (only change <name>)  
String incomingByte;                        // Serial input variable (User input)
static boolean ALARM = false;            // Alarm variable -> true if alarm set off, false if alarm has not beenm set off

//Note for the filename <name> has to be 7 characters or less 

// Creating a variable to compare time with the set interval
DateTime CurrentTime;
int interval;  // Set alarm every [interval] minutes
int LogTime;   // Set starting interval (round up to the nearest 10 min)

// User interface assignments
bool menuDisplayed = false;
String userInput = "";  // String to store user input
bool userInterface = false;


//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// SETUP LOOP (ran once)
void setup() {
  // Serial setup
  Serial.begin(Serial_BAUD);                   // Begin Serial communication
  Serial.print("\nDatalogger Testing Booting Up ...\n");  // Sending text to let user know the program has started

  // SDI-12 setup
  busSDI12.begin();  // Begining the SDI-12 databus to be able to take data
  delay(500);        // Sening 0.5 second delay to allow bus time to boot-up

  // SD card setup
  SD.begin(CHIP_SELECT);  // Begining the SD card communication on the pin CHIP_SELECT is defined as
  if ((card.init(SPI_HALF_SPEED, CHIP_SELECT)) && (volume.init(card))) {
    Serial.print("SD Card Detected...\n");
  } else {
    Serial.print("**NO SD Card Detected**...\n");
  }


// THE CODE BELLOW WILL RESET THE ENTIRE FILE ONLY UNCOMMENT WHEN NEEDED

//if (SD.exists(DAILYDATA)) {
//  SD.remove(DAILYDATA);  // Resetting the daily data
//}
//file_write(DAILYDATA, "Date\tTime\tHG\tTW\tVB");  // Writing the headings to the file define DAILYDATA: (ADD PARAMETERS THAT WILL BE LOGGED)
//
//if(SD.exists(ORIGINALDATA)){
//  SD.remove(ORIGINALDATA);     // Resetting the original data
//}
//file_write(ORIGINALDATA, "Date\tTime\tHG\tTW\tVB");   // Writing the headings to the file define ORIGINALDATA: (ADD PARAMETERS THAT WILL BE LOGGED)

// END OF CODE THAT NEEDS TO BE COMMMENTED/UNCOMMENTED WHEN CREATING NEW FILES

  // Clock Setup
  Clock.begin();
  Clock.disableAlarms();  // Disable any existing alarms

  // // Setting date and time using the Set_Date_Time function -> Set_Date_Time(int day, int month, int year, int hour, int min, int sec)
  // // Comment out if date and time is already set
  // int day = 7; int month = 6; int year = 23;  // Date variables
  // int hour = 9; int min = 29; int sec = 0;    // Time variables
  // Set_Date_Time(day, month, year, hour, min, sec);
  // // End setting date time (dont comment code bellow)

  CurrentTime = Clock.read();
  String StartTime = "Datalogger Time:\t";
  StartTime += Date_time(CurrentTime) + "\n";
  Serial.print(StartTime);

  // Setting default interval(every 10 min) onto the EEPROM address 0  
  interval = EEPROM.read(0);
  interval = 10;
  EEPROM.write(0, interval);
  // Setting default log time onto the EEPROM address 1 
  LogTime = EEPROM.read(1);
  LogTime = (round(CurrentTime.Minute / 10) + 1) * 10;
  if (LogTime >= 59) {
    LogTime = 0;
  }
  EEPROM.write(1, LogTime);
}



//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// MAIN LOOP (continuosly runnning)
void loop() {
// This string will be sent to the SD card or the Serial
  String DataString;   // String that will be compiled of the date, time, and measured parameters. 
  String HeaderString = readStringFromEEPROM();
  interval = EEPROM.read(0);
  LogTime = EEPROM.read(1);
  OFFSET = readFloatFromEEPROM(2);

// ADD THE VARIABLES BEING MEASURED AS FLOATS HERE:
  float VWC; // Taken off of SOIL SENSOR
  float SLTMP; // Taken off of SOIL SENSOR
  String VB;

  CurrentTime = Clock.read();
  //Comparing the minute to the desired time of every 10 min (10, 20, 30...)
  if (CurrentTime.Minute == LogTime) {

    DateTime MyTimestamp = Clock.read();  // Taking a time measurement
    DataString = Date_time(MyTimestamp);  // Adding the date and time to the string that will be logged to the SD card

  // Add measurements here:
    VWC = Measurement_Output(SOIL_SENSOR_ADDRESS, 1, 1.0, OFFSET);   // 1st measurement on PT12_SENSOR_ADDRESS
    SLTMP = Measurement_Output(SOIL_SENSOR_ADDRESS, 2, 1.0, 0.0);     // 2nd measurement on PT12_SENSOR_ADDRESS
    VB = BatteryVoltage(BATTERY);
      
  // Adding the parameters to thestring that will be logged to the SD card ( will need to add/subtract depending on parameters being logged )
    DataString += String(VWC) + "\t" + String(SLTMP) + "\t" + String(VB);
    

    file_write(DAILYDATA, DataString);     // Writing the string with all the parameters to the SD card
    file_write(ORIGINALDATA, DataString);  // Writing the string with all the parameters to the SD card
    delay(500);

    // Adding the interval to the comparison time
    LogTime = LogTime + interval;
    
    // Reseting the comparison
    if (LogTime >= 59) {
      LogTime = 0;
    }

    EEPROM.write(1, LogTime);
  }
  // End of alarm program


// If there is a Serial input take the input into a integer ( will only take numbers -> floats become integers )   
  if (Serial.available() > 0) {
    incomingByte = Serial.readStringUntil('\n');
    incomingByte.trim();  // Remove leading/trailing whitespaces

// WILL TAKE A LIVE READING AND PRINT TO Serial MONITOR IF INPUT IS 1100
    if(incomingByte == "live"){
      
      DateTime MyTimestamp = Clock.read();  // Taking a clock reading
      DataString = Date_time(MyTimestamp);  // Adding the date and time to the string that will be printed to the Serial monitor
      
// Add measurements here (should be same as logging to sd card):       
  VWC = Measurement_Output(SOIL_SENSOR_ADDRESS, 1, 1.0, OFFSET);   // 1st measurement on PT12_SENSOR_ADDRESS
  SLTMP = Measurement_Output(SOIL_SENSOR_ADDRESS, 2, 1.0, 0.0);     // 2nd measurement on PT12_SENSOR_ADDRESS
  VB = BatteryVoltage(BATTERY);

     
// Adding the parameters to thestring that will be printed to Serial( should be same as logging to sd card )
      DataString += String(VWC) + "\t" + String(SLTMP) + "\t" + String(VB);
      Serial.print(HeaderString);
      Serial.println();
      Serial.print(DataString);
     
      }
      
// WILL PRINT ALL THE DATA FROM THE DEFINED ORIGINALDATA FILE AND PRINT TO Serial MONITOR IF INPUT IS 1111 (Mainly used for complete data dump)
      else if (incomingByte == "log"){
        data_output(ORIGINALDATA); // Outputing the daat to Serial monitor       
      }

// WILL ENTER THE USER INTERFACE IF THE INPUT IS 333 (SEE User_Interface Tab)
    if(incomingByte == "menu"){
      userInterface = true;
      while(userInterface){
        if (!menuDisplayed) {
          displayMenu();  // Display the menu options only once
          menuDisplayed = true;
        }

        processUserInput();  // Process user input continuously

      }

    }
    
    
// WILL PRINT THE DAILY DATA FROM THE DEFINED DAILYDATA FILE AND PRINT TO Serial MONITOR IF INPUT IS 1110 (Mainly used for getting data into WISKI)
      else if (incomingByte == "9999"){
        data_output(DAILYDATA);     // Outputing the daat to Serial monitor
        SD.remove(DAILYDATA);     // Reseting the daily data

        file_write(DAILYDATA, "Date\tTime\tVWC\tSOILTMP");  // Writing the headings to the file define DAILYDATA: (ADD PARAMETERS THAT WILL BE LOGGED)
      }
  }
// End of Serial input program

}

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

#include <SDI12.h>              // Importing the SDI12 library
#define DATA_PIN 11              // SDI-12 bus data pin
#define SOIL_SENSOR_ADDRESS 1   // Defines SOIL sensor address
SDI12 busSDI12(DATA_PIN);       // Defining the SDI-12 bus 

#define SERIAL_BAUD 9600        // Serial COM rate

#include <DS3231_Simple.h>      // Importing the real time clock library
DS3231_Simple Clock;            // Initializing thew RTC

#include <SPI.h>                // Importing the SPI library (aids in SD card communication)
#include <SD.h>                 // Importing the SD card library   

const int CHIP_SELECT = 53;              // The pin that the SD card pin CS is connected to
Sd2Card card;
SdVolume volume;

// Note for the filename <name> has to be 7 characters or less 
String ORIGINALDATA = "test1_O.txt";      // Defining the  where ALL the logged data will be saved to (only change <name>)
String DAILYDATA = "test1_D.txt";         // Daily data file that is reset whenever the daily data function is called (only change <name>)  
String SENTDATA = "test1_S.txt";         // New troubleshooting file

int incomingByte;                        // Serial input variable (User input)
static boolean ALARM = false;            // Alarm variable -> true if alarm set off, false if alarm has not beenm set off

// Creating a variable to compare time with the set interval
DateTime CurrentTime;
int interval = 1;              // Set alarm every [interval] minutes
int LogTime = 10;                // Set starting interval (round up to the nearest 10 min)


// Setting up LCD screen
#include <LiquidCrystal.h>
//LCD pin to Arduino
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);
int count = 0;

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// SETUP LOOP (ran once)
void setup() {
// Serial setup
  Serial.begin(SERIAL_BAUD);            // Begin serial communication
  Serial.print("\nStarting Program...\n");  // Sending text to let user know the program has started

// SDI-12 setup
  busSDI12.begin();       // Begining the SDI-12 databus to be able to take data 
  delay(500);             // Sening 0.5 second delay to allow bus time to boot-up 

// SD card setup
  SD.begin(CHIP_SELECT);  // Begining the SD card communication on the pin CHIP_SELECT is defined as
  if ((card.init(SPI_HALF_SPEED, CHIP_SELECT)) && (volume.init(card)) ){
    Serial.print("SD Card Detected...\n");
  }
  else{
    Serial.print("**NO SD Card Detected**...\n");
  }


// THE CODE BELLOW WILL RESET THE ENTIRE FILE ONLY UNCOMMENT WHEN NEEDED
//   if(SD.exists(DAILYDATA)){
//     SD.remove(DAILYDATA);     // Resetting the daily data
//   }
//   if(SD.exists(ORIGINALDATA)){
//     SD.remove(ORIGINALDATA);     // Resetting the original data
//   }

// // New troubleshooting file
//   if(SD.exists(SENTDATA)){
//     SD.remove(SENTDATA);     // Resetting the original data
//   }


//   file_write(DAILYDATA, "Date\tTime\tVWC\tSOILTMP");      // Writing the headings to the file define DAILYDATA: (ADD PARAMETERS THAT WILL BE LOGGED)
//   file_write(ORIGINALDATA, "Date\tTime\tVWC\tSOILTMP");   // Writing the headings to the file define ORIGINALDATA: (ADD PARAMETERS THAT WILL BE LOGGED)
// END OF CODE THAT NEEDS TO BE COMMMENTED/UNCOMMENTED WHEN CREATING NEW FILES

// Clock Setup
  Clock.begin();    
  Clock.disableAlarms();      // Disable any existing alarms

  // // Setting date and time using the Set_Date_Time function -> Set_Date_Time(int day, int month, int year, int hour, int min, int sec)
  // // Comment out if date and time is already set
  int day = 25; int month = 12; int year = 23;  // Date variables
  int hour = 14; int min = 9; int sec = 20;    // Time variables
  Set_Date_Time(day, month, year, hour, min, sec);
  // // End setting date time (dont comment code bellow)

  CurrentTime = Clock.read();
  String StartTime = "Datalogger Time:\t";
  StartTime += Date_time(CurrentTime); 
  Serial.println(StartTime);

// LCD Setup
 lcd.begin(16, 2);
 pinMode(pin_BL, OUTPUT);
 digitalWrite(pin_BL, LOW);
 lcd.setCursor(0,0);


}


//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// MAIN LOOP (continuosly runnning)
void loop() {
// This string will be sent to the SD card or the Serial
  String DataString;   // String that will be compiled of the date, time, and measured parameters. 
  
// ADD THE VARIABLES BEING MEASURED AS FLOATS HERE:
  float VWC; // Taken off of SOIL SENSOR
  float SLTMP; // Taken off of SOIL SENSOR

  CurrentTime = Clock.read();
//Comparing the minute to the desired time of every 10 min (10, 20, 30...)
  if(CurrentTime.Minute == LogTime){
     
    DateTime MyTimestamp = Clock.read();   // Taking a time measurement 
    DataString = Date_time(MyTimestamp);   // Adding the date and time to the string that will be logged to the SD card
    
  // Add measurements here:
    VWC = Measurement_Output(SOIL_SENSOR_ADDRESS, 1, 1.0, 0.0);   // 1st measurement on PT12_SENSOR_ADDRESS
    SLTMP = Measurement_Output(SOIL_SENSOR_ADDRESS, 2, 1.0, 0.0);     // 2nd measurement on PT12_SENSOR_ADDRESS

      
  // Adding the parameters to thestring that will be logged to the SD card ( will need to add/subtract depending on parameters being logged )
    DataString += String(VWC) + "\t" + String(SLTMP);
    
    file_write(DAILYDATA,DataString);      // Writing the string with all the parameters to the SD card
    file_write(ORIGINALDATA,DataString);      // Writing the string with all the parameters to the SD card

  // Adding the interval to the comparison time
    LogTime = LogTime + interval;
  // Reseting the comparison
    if(LogTime >= 59){
      LogTime = 0;
    }

  } 
// End of alarm program


// If there is a serial input take the input into a integer ( will only take numbers -> floats become integers )   
  if (Serial.available() > 0) {
    incomingByte = Serial.parseInt();

// WILL TAKE A LIVE READING AND PRINT TO SERIAL MONITOR IF INPUT IS 111
    if(incomingByte == 111){
      
      DateTime MyTimestamp = Clock.read();  // Taking a clock reading
      DataString = Date_time(MyTimestamp);  // Adding the date and time to the string that will be printed to the Serial monitor
      
// Add measurements here (should be same as logging to sd card):       
  VWC = Measurement_Output(SOIL_SENSOR_ADDRESS, 1, 1.0, 0.0);   // 1st measurement on PT12_SENSOR_ADDRESS
  SLTMP = Measurement_Output(SOIL_SENSOR_ADDRESS, 2, 1.0, 0.0);     // 2nd measurement on PT12_SENSOR_ADDRESS
     
// Adding the parameters to thestring that will be printed to serial( should be same as logging to sd card )
      DataString += String(VWC) + "\t" + String(SLTMP);
      Serial.print("\nDate\t\tTime\t\tVWC\tSOILTMP");
      Serial.println();
      Serial.print(DataString);
     
      }
      
// WILL PRINT ALL THE DATA FROM THE DEFINED ORIGINALDATA FILE AND PRINT TO SERIAL MONITOR IF INPUT IS 222 (Mainly used for complete data dump)
      else if (incomingByte == 222){
        Serial.println();
        data_output(ORIGINALDATA); // Outputing the data to serial monitor       
      }

// NEW TROUBLESHOOTING FEATURE TO SEE THE DATA DUMPED FROM DAILYDATA FILE INTO WISKI AND THE TIME THE ARDUINO WAS ACCESSED BY THE WEBSITE
      else if (incomingByte == 333){
        Serial.println();
        data_output(SENTDATA); // Outputing the data to serial monitor       
      }
 
// WILL PRINT THE DAILY DATA FROM THE DEFINED DAILYDATA FILE AND PRINT TO SERIAL MONITOR IF INPUT IS 999 (Mainly used for getting data into WISKI)
      else if (incomingByte == 9999){
        data_output(DAILYDATA);     // Outputing the daat to serial monitor
        delay(500); //Delay to allow time for data download
        
        // New troubleshooting code -> Access the file that WISKI is acessing and copys the data into a new file as well as the time it was accessed
        String text_from_file;
        String AccessTime = "Time Accessed:\t";
        DateTime MyTimestamp = Clock.read();   // Taking a time measurement 
        AccessTime += Date_time(MyTimestamp);

        File data_file = SD.open(DAILYDATA, FILE_READ);
         while (data_file.available()) {
          text_from_file += char(data_file.read());
        }
        data_file.close();
        file_write(SENTDATA, AccessTime);
        file_write(SENTDATA, text_from_file);
        // END of new code -> Allows us to verify the data being sent to the scrapper and see why there is missing points in the DPC (all the data is being logged to the master dataset correctly)

        SD.remove(DAILYDATA);     // Reseting the daily data

        file_write(DAILYDATA, "Date\tTime\tVWC\tSOILTMP");  // Writing the headings to the file define DAILYDATA: (ADD PARAMETERS THAT WILL BE LOGGED)
      }
  }
// End of serial input program


// LCD display program -> displays date, time, and reading note that it will auto turn on and shut off
  int button = analogRead (0);

  if((button > 800) && (button < 1000)){
    CurrentTime = Clock.read();  // Taking a clock reading

    
    digitalWrite(pin_BL, HIGH); // Turning on the LCD backlight

    // Displaying the date and time
    lcd.setCursor(0,0);
    String line1 = ("Date: " + String(CurrentTime.Day) + "/" + String(CurrentTime.Month) + "/20" + String(CurrentTime.Year) );
    String line2 = ("Time: " + String(CurrentTime.Hour) + ":" + String(CurrentTime.Minute) + ":" + String(CurrentTime.Second) );
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);
    delay(5000);  // Wait for 5 seconds 
    lcd.clear();  // Clear the display

    // Displaying Measurements
    // If you have more than 2 sensors, copy until the lcd.clear(); line and edit the sensors to be the new sensors wanting to be displayed
    lcd.setCursor(0,0);
    VWC = Measurement_Output(SOIL_SENSOR_ADDRESS, 1, 1.0, 0.0);   // 1st measurement on PT12_SENSOR_ADDRESS
    SLTMP = Measurement_Output(SOIL_SENSOR_ADDRESS, 2, 1.0, 0.0);     // 2nd measurement on PT12_SENSOR_ADDRESS
    line1 = ("VWC: " + String(VWC));
    line2 = ("SLTMP: " + String(SLTMP));
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);
    delay(7500);  // Wait for 7.5 seconds 
    lcd.clear();  // Clear the display

    //turing the lcd screen off after 10 second
    digitalWrite(pin_BL, LOW);



  }

}
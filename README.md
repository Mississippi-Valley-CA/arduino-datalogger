# adl
Arduino Sketches
----------------
- Datalogger_V2 is the current version of the datalogger code that is implemented in the stations (edited to fit stations parameters)

- Previous versions contains all the previous versions of the sketches as well as some sketches that could be useful for adding new features to the datalogger
	1) "Datalogger" 	-> first ittertion of the datalogger code
	2) "Datalogger_DS1307" 	-> code used for a DS1307 clock instead of the DS3231 (was also the implementation of using time comparison instead of alarms)
	3) "Datalogger_LCD" 	-> unfinished version that started to implement an LCD screen into the datalogger's funciton
	4) "Datalogger_UI"	-> first version that incorperated a user interface into the datalogger

- Setup files contain sketches that can be used to help setup the Arduino datalogger
	* Note using the new version of the data logger code the only setup code that is needed is to setup SDI sensors ("SDI_SensorSetup" or "SDI_SensorTest")
	1) "SDCard_test"	-> will test to see if the SD card is readable and gives the storage used on the SD card
	2) "SDI_SensorSetup" 	-> test the SDI-12 sensor is connected, and allow to change the sensor's address
	3) "SDI_SensorTest" 	-> test the SDI-12 sensor is connected, and allow to change the sensor's address, and will take measurements to see if the sensor is working
	4) "SetDateTime"	-> determines if the clock is working, and allows the user to set the date and time

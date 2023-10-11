//------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- CAMPBELL SCIENTIFIC AIR TEMP SENSOR ---------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------
// This funciton is used for 
// Note that it is only this sensor as the others do not have manuals for setting up the functions
// Please see manual for the wiring details
float CampbellSciTemp(float AnalogSensor, float InputVoltage){
  double A = 1.129241 / 1000;
  double B = 2.341077 / 10000;
  double C = 8.775468 / 100000000;
  float ConV = 4.9;
  float Vx = InputVoltage * ConV;
  float Vs = AnalogSensor * ConV;

  double Rs = 24900 * ((Vx/Vs) - 1);

  float Temp = (1 / (A + B*log(Rs) + C*pow(log(Rs),3))) - 273.15;

  return Temp;
}


//-------------------------------------------------- BATTERY VOLTAGE READING -------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// Function that gives the battery reading based off of a voltage divider
// Voltage divider consists of a 2k resistor and a 1k resistors in series
// The 2k resistor should be connected the the + terminal of the battery with the 1k connected to GND (- terminal)
// Measuring after the 2k resistor so the voltage reading on the arduino should be 1/3 the voltage of the battery
String BatteryVoltage(int inputPin){
  float dividerConstant = 0.33;
  float analogReading = analogRead(inputPin);
  float analogVoltage = (analogReading* 5 / 1023);
  float BatteryVoltage = analogVoltage / dividerConstant;
  return roundNum(BatteryVoltage);
}

//-------------------------------------------------- SOLAR PANEL VOLTAGE READING ---------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// Function that gives the solar voltage reading based off of a voltage divider
// Note that the voltage divider will be custom thus you may have to change the dividerConstant value depending on your voltage divider
String SolarVoltage(int inputPin){
  float dividerConstant = 0.162;
  float analogReading = analogRead(inputPin);
  float analogVoltage = (analogReading* 5 / 1023);
  float SolarVoltage = analogVoltage / dividerConstant;
  return roundNum(SolarVoltage);
}

//--------------------------Function that rounds a float value to the nearest tenth decimal-----------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// For example: 0.17 -> 0.2 
// Has to be string as a float will add another 0 (0.2 -> 0.20)
String roundNum(float num){
  float roundedNumber = round(num * 10) / 10.0;  // Round to one decimal place
  char roundedString[6];  // Character array to store the rounded number
  dtostrf(roundedNumber, 4, 1, roundedString);  // Convert float to string with one decimal place
  return roundedString;
}


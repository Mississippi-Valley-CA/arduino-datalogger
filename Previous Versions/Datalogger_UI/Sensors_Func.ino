/*
 * 
 * This tab/file is to be used for new sensors other than SDI-12
 * 
 */

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// This funciton is used for Campbell Scienfic Air Temperature Sensors
// Noet that it is only this sensor as the others do not have manuals for setting up the functions
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


//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// Function that gives the battery reading based off of a voltage divider
// Voltage divider consists of a 2k resistor and a 1k resistors in series
// The 2k resistor should be connected the the + terminal of the battery with the 1k connected to GND (- terminal)
// Measuring after the 2k resistor so the voltage reading on the arduino should be 1/3 the voltage of the battery

String BatteryVoltage(int inputPin){
  float analogReading = analogRead(inputPin);
  float analogVoltage = (analogReading* 5 / 1023);
  float BatteryVoltage = analogVoltage / 0.33;
  return roundNum(BatteryVoltage);
}


String SolarVoltage(int inputPin){
  float analogReading = analogRead(inputPin);
  float analogVoltage = (analogReading* 5 / 1023);
  float SolarVoltage = analogVoltage / 0.162;
  return roundNum(SolarVoltage);
}


String roundNum(float num){
  float roundedNumber = round(num * 10) / 10.0;  // Round to one decimal place

  char roundedString[6];  // Character array to store the rounded number
  dtostrf(roundedNumber, 4, 1, roundedString);  // Convert float to string with one decimal place

  return roundedString;
  
}
//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

/***
This function is to be used for the Campbell Scientific Air Temperature Sensors
  (Small white tips with Campbell Scientific written on the wiring)

Connecting the sensor (Section 7-Installation of the manual):
  Purple & Clear Wire -> GND
  Black Wire -> 5V
  Red Wire -> Analog input pins (A0 to A5) 

This function takes the sensors pin and converts its reading to a temperature reading
  For reference on how this is done see the sensors manual (section 8-Operation)

NOTE: that this assumes that you are using the reference voltage of 5V
  
***/

float CampbellSciTemp(int SensorPin){
  double A = 1.129241 / 1000;
  double B = 2.341077 / 10000;
  double C = 8.775468 / 100000000;
  
  // Converting the sensors input to a voltage
  float SensorValue = analogRead(SensorPin);
  float SensorVoltage = (SensorValue* 5 / 1023);

  float RefVoltage = 5.01;

  // Using formula from manual convert voltage into resistance then to temperature
  double Rs = 24900 * ((RefVoltage / SensorVoltage) - 1);
  float Temp = (1 / (A + B*log(Rs) + C*pow(log(Rs),3))) - 273.15;

  return Temp;
}

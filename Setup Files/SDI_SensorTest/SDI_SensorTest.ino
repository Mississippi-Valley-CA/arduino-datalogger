/**
 * @file b_address_change.ino
 * @copyright (c) 2013-2020 Stroud Water Research Center (SWRC)
 *                          and the EnviroDIY Development Team
 *            This example is published under the BSD-3 license.
 * @author Kevin M.Smith <SDI12@ethosengineering.org>
 * @date August 2013
 *
 * @brief Example B: Changing the Address of your SDI-12 Sensor
 *
 * This is a simple demonstration of the SDI-12 library for arduino.
 * It discovers the address of the attached sensor and allows you to change it.
 */

#include <SDI12.h>

#define SERIAL_BAUD 9600   /*!< The baud rate for the output serial port */
#define DATA_PIN 11         /*!< The pin of the SDI-12 data bus */

/** Define the SDI-12 bus */
SDI12 mySDI12(DATA_PIN);

int newAdd;
String myCommand  = "";   // empty to start
char   oldAddress = '!';  // invalid address as placeholder


// this checks for activity at a particular address
// expects a char, '0'-'9', 'a'-'z', or 'A'-'Z'
boolean checkActive(byte i) {  // this checks for activity at a particular address
  Serial.print("Checking address ");
  Serial.print((char)i);
  Serial.print("...");
  myCommand = "";
  myCommand += (char)i;  // sends basic 'acknowledge' command [address][!]
  myCommand += "!";

  for (int j = 0; j < 3; j++) {  // goes through three rapid contact attempts
    mySDI12.sendCommand(myCommand);
    delay(30);
    if (mySDI12.available()) {  // If we here anything, assume we have an active sensor
      Serial.println("Occupied");
      mySDI12.clearBuffer();
      return true;
    } else {
      //Serial.println("Vacant");  // otherwise it is vacant.
      mySDI12.clearBuffer();
    }
  }
  return false;
}


void setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial)
    ;

  Serial.println("Opening SDI-12 bus...");
  mySDI12.begin();
  delay(500);  // allow things to settle

}

void loop() {
  Serial.println("Identification:");
  int sensorAdd = identification();
  sensorAdd = sensorAdd - 48;
  Serial.println("Sensor on adress:\t" + String(sensorAdd));
  float M1 = Measurement_Output(sensorAdd, 1, 1.0, 0.0);   // 1st measurement on PT12_SENSOR_ADDRESS
  float M2 = Measurement_Output(sensorAdd, 2, 1.0, 0.0);     // 2nd measurement on PT12_SENSOR_ADDRESS
  float M3 = Measurement_Output(sensorAdd, 3, 1.0, 0.0);     // 2nd measurement on PT12_SENSOR_ADDRESS

  Serial.println("Measurements:");

  Serial.println(String(M1) + "\t" + String(M2) + "\t" + String(M3)  );


}

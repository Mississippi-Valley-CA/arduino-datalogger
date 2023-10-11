 char identification(){
  bool identify = false;
  while(!identify){
    boolean found = false;  // have we identified the sensor yet?

    for (byte i = '0'; i <= '9'; i++) {  // scan address space 0-9
      if (found) break;
      if (checkActive(i)) {
        found      = true;
        oldAddress = i;
      }
    }

    for (byte i = 'a'; i <= 'z'; i++) {  // scan address space a-z
      if (found) break;
      if (checkActive(i)) {
        found      = true;
        oldAddress = i;
      }
    }

    for (byte i = 'A'; i <= 'Z'; i++) {  // scan address space A-Z
      if (found) break;
      if (checkActive(i)) {
        found      = true;
        oldAddress = i;
      }
    }

    if (!found) {
      Serial.println(
        "No sensor detected. Check physical connections.");  // couldn't find a sensor.
                                                            // check connections..
    } else {
      Serial.print("Sensor active at address ");  // found a sensor!
      Serial.print(oldAddress);
      Serial.println(".");

      Serial.println("Enter new address.");  // prompt for a new address
      while (!Serial.available())
        ;
      newAdd = Serial.read();

      // wait for valid response
      while (((newAdd < '0') || (newAdd > '9')) && ((newAdd < 'a') || (newAdd > 'z')) &&
            ((newAdd < 'A') || (newAdd > 'Z'))) {
        if (!(newAdd == '\n') || (newAdd == '\r') || (newAdd == ' ')) {
          Serial.println(
            "Not a valid address. Please enter '0'-'9', 'a'-'A', or 'z'-'Z'.");
        }
        while (!Serial.available())
          ;
        newAdd = Serial.read();
        
      }

      /* the syntax of the change address command is:
      [currentAddress]A[newAddress]! */

      Serial.println("Readdressing sensor.");
      myCommand = "";
      myCommand += (char)oldAddress;
      myCommand += "A";
      myCommand += (char)newAdd;
      myCommand += "!";
      mySDI12.sendCommand(myCommand);

      /* wait for the response then throw it away by
      clearing the buffer with clearBuffer()  */
      delay(300);
      mySDI12.clearBuffer();

      Serial.println("Success");
      identify = true;
      break;
      
    }
  } 
  return int((char)newAdd);
  
   
 }
 
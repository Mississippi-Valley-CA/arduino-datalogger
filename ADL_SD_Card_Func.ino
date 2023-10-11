//-------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------ Function that opens and writes a line to the SD card -------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------
void file_write(String filename, String text){
  
  File dataFile = SD.open(filename, FILE_WRITE);
  
  if(dataFile){
    dataFile.println(text);
  }
  dataFile.close(); 
  
}

//------------------------ Function that opens and reads data from the given filename and then outputs it to the Serial ---------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------
void data_output(String filename){
  File dataFile = SD.open(filename);
  while(dataFile.available()){
    Serial.write(dataFile.read());
  }
  dataFile.close();
  
  if(!dataFile.available()){
//    Serial.println("\t\tError opening file");
  }
  
}

//---------------------------------------- Function to list files and directories recursively ---------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void listFiles(const char* dirName, int numTabs = 0) {
  File root = SD.open(dirName);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }

  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      // no more files
      break;
    }

    for (int i = 0; i < numTabs; i++) {
      Serial.print('\t'); // add tabs for better formatting
    }

    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      listFiles(entry.name(), numTabs + 1); // Recursive call for subdirectories
    } else {
      Serial.print("\t\t");
      Serial.print(entry.size());
      Serial.println(" bytes");
    }
    entry.close();
  }

  root.close();
}

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// Function that opens and writes a line to the SD card

void file_write(String filename, String text){
  
  File dataFile = SD.open(filename, FILE_WRITE);
  
  if(dataFile){
    dataFile.println(text);
  }
  dataFile.close(); 
  
}

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
// Function that opens and reads data from the given filename and then outputs it to the Serial1

void data_output(String filename){
  File dataFile = SD.open(filename);
  while(dataFile.available()){
    Serial1.write(dataFile.read());
  }
  dataFile.close();
  
  if(!dataFile.available()){
//    Serial1.println("\t\terror opening file");
  }
  
}

// Function to list files and directories recursively
void listFiles(const char* dirName, int numTabs = 0) {
  File root = SD.open(dirName);
  if (!root) {
    Serial1.println("Failed to open directory");
    return;
  }

  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      // no more files
      break;
    }

    for (int i = 0; i < numTabs; i++) {
      Serial1.print('\t'); // add tabs for better formatting
    }

    Serial1.print(entry.name());
    if (entry.isDirectory()) {
      Serial1.println("/");
      listFiles(entry.name(), numTabs + 1); // Recursive call for subdirectories
    } else {
      Serial1.print("\t\t");
      Serial1.print(entry.size());
      Serial1.println(" bytes");
    }
    entry.close();
  }

  root.close();
}



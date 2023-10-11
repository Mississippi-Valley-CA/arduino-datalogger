void Set_Date_Time(int day, int month, int year, int hour, int min, int sec){
  DateTime MyTimestamp;
  
  // Load it with the date and time you want to set, for example
  // Example is set to August 4th 2022 at 13:07:30 
  //Please set parameters to current time +10 sec upload time
  MyTimestamp.Day    = day;
  MyTimestamp.Month  = month;
  MyTimestamp.Year   = year; 
  MyTimestamp.Hour   = hour;
  MyTimestamp.Minute = min;
  MyTimestamp.Second = sec;
  
  // Then write it to the clock
  Clock.write(MyTimestamp);

}
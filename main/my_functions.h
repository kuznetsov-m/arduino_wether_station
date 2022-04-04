#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

namespace ntp_helper {
  /**
  * https://github.com/taranais/NTPClient/blob/master/NTPClient.cpp
  * 
  * @return secs argument (or 0 for current date) formatted to ISO 8601
  * like `2004-02-12T15:19:21+00:00`
  */
  #define LEAP_YEAR(Y)     ( (Y>0) && !(Y%4) && ( (Y%100) || !(Y%400) ) )
  String getFormattedDate(const NTPClient& ntp_client) {
    unsigned long secs = 0;
    unsigned long rawTime = (secs ? secs : ntp_client.getEpochTime()) / 86400L;  // in days
    unsigned long days = 0, year = 1970;
    uint8_t month;
    static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
  
    while((days += (LEAP_YEAR(year) ? 366 : 365)) <= rawTime)
      year++;
    rawTime -= days - (LEAP_YEAR(year) ? 366 : 365); // now it is days in this year, starting at 0
    days=0;
    for (month=0; month<12; month++) {
      uint8_t monthLength;
      if (month==1) { // february
        monthLength = LEAP_YEAR(year) ? 29 : 28;
      } else {
        monthLength = monthDays[month];
      }
      if (rawTime < monthLength) break;
      rawTime -= monthLength;
    }
    String monthStr = ++month < 10 ? "0" + String(month) : String(month); // jan is month 1  
    String dayStr = ++rawTime < 10 ? "0" + String(rawTime) : String(rawTime); // day of month  
    return String(year) + "-" + monthStr + "-" + dayStr + "T" + ntp_client.getFormattedTime() + "Z";
  }
} // ntp_helper

//void read_write_example() {
//  // open the file. note that only one file can be open at a time,
//  // so you have to close this one before opening another.
//  myFile = SD.open("test.txt", FILE_WRITE);
//
//  // if the file opened okay, write to it:
//  if (myFile) {
//    Serial.print("Writing to test.txt...");
//    myFile.println("testing 1, 2, 3.");
//    // close the file:
//    myFile.close();
//    Serial.println("done.");
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
//
//  // re-open the file for reading:
//  myFile = SD.open("test.txt");
//  if (myFile) {
//    Serial.println("test.txt:");
//
//    // read from the file until there's nothing else in it:
//    while (myFile.available()) {
//      Serial.write(myFile.read());
//    }
//    // close the file:
//    myFile.close();
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
//}


#endif // MY_FUNCTIONS_H

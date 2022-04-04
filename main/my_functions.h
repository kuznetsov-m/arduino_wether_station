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

#endif // MY_FUNCTIONS_H

#include <ESP8266WiFi.h>
// NTP Client
#include <WiFiUdp.h>
#include <NTPClient.h>
// SD card
#include <SPI.h>
#include <SD.h>

#include "constants.h"
#include "my_functions.h"

const long utcOffsetInSeconds = 3600;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// SD card
File root;
bool sd_card_is_ready = false;

struct bme_data {
  float temperature;
  float pressure;
  float altitude;
  float humidity;
};

File bme_data_file;
File myFile;

void setup(){
  Serial.begin(115200);
  Serial.println("");

  // WiFi
  {
    WiFi.begin(wifi_config::ssid, wifi_config::password);
  
    Serial.print("Connecting to WiFi \"");
    Serial.print(wifi_config::ssid);
    Serial.print("\" ");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("connected.");
  }

  // NTP Client
  timeClient.begin();
  timeClient.update();

  // SD card
  Serial.print("Initializing SD card...");
  sd_card_is_ready = SD.begin(D8);
  Serial.println(sd_card_is_ready ? "done." : "failed!");

  if (sd_card_is_ready) {
    root = SD.open("/");
  }
  
  //
  Serial.println(ntp_helper::getFormattedDate(timeClient));
}

void loop() {
  String date_time_string(ntp_helper::getFormattedDate(timeClient));
  bme_data data = read_bme_data();

  Serial.print(date_time_string + "\t");
  Serial.printf("%.2f\t%.2f\t%.2f\t%.2f",
    date_time_string, data.temperature, data.pressure, data.altitude, data.humidity
  );
  Serial.println();

  if (sd_card_is_ready) {
    write_bme_data(date_time_string, data);
  }
  
  delay(1000);
}

bme_data read_bme_data() {
  return bme_data{26.5, 256.1, 1024.3, 11.22};
}

void write_bme_data(const String& date_time_string, const bme_data& data) {
  bme_data_file = SD.open(file_name, FILE_WRITE);
  if (bme_data_file) {
    Serial.print("Writing to file...");

    bme_data_file.print(date_time_string + "\t");
    bme_data_file.printf("%.2f\t%.2f\t%.2f\t%.2f", data.temperature, data.pressure, data.altitude, data.humidity);
    bme_data_file.println();
    bme_data_file.close();
    
    Serial.println("done.");
  } else {
    Serial.println("error opening file");
  }
}

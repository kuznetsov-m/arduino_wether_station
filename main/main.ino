#include <ESP8266WiFi.h>
// NTP Client
#include <WiFiUdp.h>
#include <NTPClient.h>
// SD card
#include <SPI.h>
#include <SD.h>
// BME200
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#define SEALEVELPRESSURE_HPA (1013.25)  // Задаем высоту
// HTTP
#include <ESP8266HTTPClient.h>

#include "constants.h"
#include "my_functions.h"

const long utcOffsetInSeconds = 3600;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient ntpClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// SD card
File root;
bool sd_card_is_ready = false;

// BME200
Adafruit_BME280 bme;

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
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // NTP Client
  ntpClient.begin();
  ntpClient.update();

  // SD card
  Serial.print("Initializing SD card...");
  sd_card_is_ready = SD.begin(D8);
  Serial.println(sd_card_is_ready ? "done." : "failed!");

  if (sd_card_is_ready) {
    root = SD.open("/");
  }

  // BME200
  Serial.print("Initializing BME200 sensor...");
  Serial.println(bme.begin(0x76) ? "done." : "Could not find a valid BME280 sensor!");
  
  //
  Serial.println(ntp_helper::getFormattedDate(ntpClient));
}

void loop() {
  String date_time_string(ntp_helper::getFormattedDate(ntpClient));
  bme_data data = read_bme_data();

  Serial.print(date_time_string + "\t");
  Serial.printf("%.2f\t%.2f\t%.2f\t%.2f",
    data.temperature, data.pressure, data.altitude, data.humidity
  );
  Serial.println();

  send_data(date_time_string, data);
  
  delay(1000);
}

bme_data read_bme_data() {
  return bme_data{
    bme.readTemperature(),
    bme.readPressure() / 100.0F,
    bme.readAltitude(SEALEVELPRESSURE_HPA),
    bme.readHumidity()
  };
}

void write_bme_data(const String& date_time_string, const bme_data& data) {
  bme_data_file = SD.open(file_name, FILE_WRITE);
  if (bme_data_file) {
    Serial.print("Writing to file...");

    bme_data_file.print(date_time_string);
    bme_data_file.print("\t");
    bme_data_file.printf("%.2f\t%.2f\t%.2f\t%.2f", data.temperature, data.pressure, data.altitude, data.humidity);
    bme_data_file.println();
    bme_data_file.close();
    
    Serial.println("done.");
  } else {
    Serial.println("error opening file");
  }
}

// Send data to server or write to file
void send_data(const String& date_time_string, const bme_data& data) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = serverName + "?"
      + "datetime=" + String(date_time_string)
      + "temperature=" + String(data.temperature, 2)
      + "pressure=" + String(data.pressure, 2)
      + "altitude=" + String(data.altitude, 2)
      + "humidity=" + String(data.humidity, 2);
    Serial.println("URL: " + url);

    http.begin(client, url.c_str());
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
  else if (sd_card_is_ready) {
    write_bme_data(date_time_string, data);
  }
}

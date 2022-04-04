#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace device_config {
  namespace wifi {
    const char* ssid = "";
    const char* password = "";
  }

  const char* server_name = "";
  
  const char* file_name = "bme_data.txt";

  // интервал опроса BME200 и записи данных (мс)
  const unsigned int write_data_interval = 1 * 1000;
}

#endif // CONSTANTS_H

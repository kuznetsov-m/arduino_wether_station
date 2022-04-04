/*
Тестировалось на Arduino IDE 1.8.5
Дата тестирования 03.10.2019г.
*/ 

#include <ESP8266WiFi.h>                                // Подключаем библиотеку ESP8266WiFi
#include <Wire.h>                                       // Подключаем библиотеку Wire
#include <Adafruit_BME280.h>                            // Подключаем библиотеку Adafruit_BME280
#include <Adafruit_Sensor.h>                            // Подключаем библиотеку Adafruit_Sensor

#include <ESP8266HTTPClient.h>

#define SEALEVELPRESSURE_HPA (1013.25)                  // Задаем высоту

Adafruit_BME280 bme;                                    // Установка связи по интерфейсу I2C

#include "my_variables.h"

WiFiServer server(80);                                  // Указываем порт Web-сервера
String header;

void setup() {
  Serial.begin(115200);                                 // Скорость передачи 115200
  bool status;
                                                       
  if (!bme.begin(0x76)) {                               // Проверка инициализации датчика
    Serial.println("Could not find a valid BME280 sensor, check wiring!"); // Печать, об ошибки инициализации.
    while (1);                                          // Зацикливаем
  }

  Serial.print("Connecting to ");                       // Отправка в Serial port 
  Serial.println(ssid);                                 // Отправка в Serial port 
  WiFi.begin(ssid, password);                           // Подключение к WiFi Сети
  while (WiFi.status() != WL_CONNECTED) {               // Проверка подключения к WiFi сети
    delay(500);                                         // Пауза
    Serial.print(".");                                  // Отправка в Serial port 
  }

  Serial.println("");                                   // Отправка в Serial port 
  Serial.println("WiFi connected.");                    // Отправка в Serial port 
  Serial.println("IP address: ");                       // Отправка в Serial port 
  Serial.println(WiFi.localIP());                       // Отправка в Serial port 
  server.begin();                                  
}

void loop(){
//  WiFiClient client = server.available();               // Получаем данные, посылаемые клиентом 
//
//  if (client) {                                         
//    Serial.println("New Client.");                      // Отправка "Новый клиент"
//    String currentLine = "";                            // Создаем строку для хранения входящих данных от клиента
//    while (client.connected()) {                        // Пока есть соединение с клиентом 
//      if (client.available()) {                         // Если клиент активен 
//        char c = client.read();                         // Считываем посылаемую информацию в переменную "с"
//        Serial.write(c);                                // Отправка в Serial port 
//        header += c;
//        if (c == '\n') {                                // Вывод HTML страницы 
//          if (currentLine.length() == 0) {
//            client.println("HTTP/1.1 200 OK");          // Стандартный заголовок HT
//            client.println("Content-type:text/html ");
//            client.println("Connection: close");        // Соединение будет закрыто после завершения ответа
//            client.println("Refresh: 2");              // Автоматическое обновление каждые 10 сек 
//            client.println();
//            
//            client.println("<!DOCTYPE html><html>");    // Веб-страница создается с использованием HTML
//            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
//            client.println("<meta charset='UTF-8'>");   // Делаем русскую кодировку
//            client.println("<link rel=\"icon\" href=\"data:,\">");
//                     
//            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
//            client.println("table { border-collapse: collapse; width:40%; margin-left:auto; margin-right:auto; }");
//            client.println("th { padding: 12px; background-color: #0043af; color: white; }");
//            client.println("tr { border: 1px solid #ddd; padding: 12px; }");
//            client.println("tr:hover { background-color: #bcbcbc; }");
//            client.println("td { border: none; padding: 12px; }");
//            client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }");
//            
//            client.println("</style></head><body><h1>Метеостанция на BME280 и ESP8266</h1>");
//            client.println("<table><tr><th>Параметр</th><th>Показания</th></tr>");
//            client.println("<tr><td>Температура</td><td><span class=\"sensor\">");
//            client.println(bme.readTemperature());
//            client.println(" *C</span></td></tr>");    
//            client.println("<tr><td>Давление</td><td><span class=\"sensor\">");
//            client.println(bme.readPressure() / 100.0F);
//            client.println(" hPa</span></td></tr>");
//            client.println("<tr><td>Приблизительная высота над уровнем моря</td><td><span class=\"sensor\">");
//            client.println(bme.readAltitude(SEALEVELPRESSURE_HPA));
//            client.println(" m</span></td></tr>"); 
//            client.println("<tr><td>Влажность</td><td><span class=\"sensor\">");
//            client.println(bme.readHumidity());
//            client.println(" %</span></td></tr>"); 
//            client.println("</body></html>");
//            
//            client.println();
//            break;
//          } else { 
//            currentLine = "";
//          }
//        } else if (c != '\r') {  
//          currentLine += c;      
//        }
//      }
//    }
//    header = "";
//    client.stop();
//    Serial.println("Client disconnected.");
//    Serial.println("");
//  }

  if (WiFi.status()== WL_CONNECTED) {
    Serial.println("WL_CONNECTED");

    WiFiClient client;
    HTTPClient http;
    
    String url = serverName + "?";
    url += String("temperature=") + bme.readTemperature() + "&";
    url += String("pressure=") + (bme.readPressure() / 100.0F) + "&";
    url += String("altitude=") + bme.readAltitude(SEALEVELPRESSURE_HPA) + "&";
    url += String("humidity=") + bme.readHumidity();

    Serial.println(url);
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, url.c_str());
      
    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  
  Serial.println("Loop");
  delay(1000);
}

#include <SPI.h>
#include <SD.h>

File root;

struct bme_data {
  float temperature;
  float pressure;
  float altitude;
  float humidity;
};

const char* file_name = "bme_data.txt";

File bme_data_file;
File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  Serial.print("Initializing SD card...");

  if (!SD.begin(D8)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  root = SD.open("/");

//  printDirectory(root, 0);

  bme_data data = read_bme_data();

//  read_write_example();
  write_bme_data(data);
  print_bme_data_from_file();

  Serial.println("done!");
}

void loop() {
  // nothing happens after setup finishes.
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.print(entry.size(), DEC);
      time_t cr = entry.getCreationTime();
      time_t lw = entry.getLastWrite();
      struct tm * tmstruct = localtime(&cr);
      Serial.printf("\tCREATION: %d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
      tmstruct = localtime(&lw);
      Serial.printf("\tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    }
    entry.close();
  }
}

bme_data read_bme_data() {
  return bme_data{26.5, 256.1, 1024.3, 11.22};
}

void write_bme_data(const bme_data data) {
  Serial.print("Data: ");
  Serial.println(data.temperature);

  bme_data_file = SD.open(file_name, FILE_WRITE);
  if (bme_data_file) {
    Serial.print("Writing to file...");
    
    bme_data_file.printf("%.2f\t%.2f\t%.2f\t%.2f", data.temperature, data.pressure, data.altitude, data.humidity);
    bme_data_file.println();
    bme_data_file.close();
    
    Serial.println("done.");
  } else {
    Serial.println("error opening file");
  }
}

void print_bme_data_from_file() {
  bme_data_file = SD.open(file_name);
  if (bme_data_file) {
    Serial.print(file_name);
    Serial.println(":");

    while (bme_data_file.available()) {
      Serial.write(bme_data_file.read());
    }

    bme_data_file.close();
  } else {
    Serial.println("error opening file");
  }
}

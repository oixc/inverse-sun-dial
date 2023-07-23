/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>
#include <TimeLib.h>
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t

#define SD_chip_select_PIN 4

File file;
float azimuth;
float altitude;
time_t target_time;
time_t date_time;
unsigned long pos_estimate = -1;
String filename;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  setSyncProvider(RTC.get);  // the function to get the time from the RTC
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");
  setSyncInterval(60 * 5);  // re-sync every 5 minutes

  SD_info();
  //target_time = 1673893589;  // 1703466000;
  filename = "/sundial/1/2023.csv";
  //setTime(target_time);
}

void loop() {
  target_time = now();
  read_data(filename, target_time, pos_estimate, date_time, azimuth, altitude);
  print_date_time(target_time);
  Serial.print(" date_time: ");
  print_date_time(date_time);
  Serial.print(" azimuth: ");
  Serial.print(azimuth);
  Serial.print(" altitude: ");
  Serial.println(altitude);
  delay(5000);
  //target_time = target_time + 60 * 60 * 0.4 + 17;
}

void SD_info() {
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_chip_select_PIN)) {
    Serial.println("initialization failed!");
    while (1)
      ;
  }
  Serial.println("initialization done.");

  // list all files in the card with date and size
  Serial.println("card content.");
  Sd2Card card;
  SdVolume volume;
  SdFile root;
  if (!card.init(SPI_HALF_SPEED, SD_chip_select_PIN)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1)
      ;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }
  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1)
      ;
  }
  root.openRoot(volume);
  root.ls(LS_R | LS_DATE | LS_SIZE);
}

void read_data(String filename, time_t target_time, unsigned long &pos_estimate, time_t &date_time, float &azimuth, float &altitude) {
  if (date_time >= target_time) {
    // last reading was already ahead of the target time. nothing to do now
    return;
  }

  if (!SD.begin(SD_chip_select_PIN)) {
    Serial.println("initialization failed!");
    while (1)
      ;
  }

  file = SD.open(filename);

  if (pos_estimate < 0) {
    uint8_t current_year = year(target_time);
    time_t current_year_start = makeTime({ 0, 0, 0, 0, 1, 1, current_year - 1970 });
    time_t next_year_start = makeTime({ 0, 0, 0, 0, 1, 1, current_year - 1970 + 1 });

    pos_estimate = file.size() * 1.0 * (target_time - current_year_start) / (next_year_start - current_year_start);
    file.seek(pos_estimate);  // seek to jump into the correct line
    file.find('\n');          // advance to the end of the line after seeking
    pos_estimate = file.position();
  }

  if (file) {
    file.seek(pos_estimate);  // seek to jump into the correct line
    while (file.available()) {
      digitalWrite(LED_BUILTIN, HIGH);
      date_time = file.parseInt();
      azimuth = file.parseFloat();
      altitude = file.parseFloat();
      if (date_time >= target_time) {
        digitalWrite(LED_BUILTIN, LOW);
        pos_estimate = file.position();
        break;
      }
    }
    // close the file:
    file.close();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening file: ");
    Serial.println(filename);
  }
}

void print_date_time(time_t date_time) {
  Serial.print(year(date_time));
  Serial.print("-");
  Serial.print(month(date_time));
  Serial.print("-");
  Serial.print(day(date_time));
  Serial.print(" ");
  Serial.print(hour(date_time));
  Serial.print(":");
  Serial.print(minute(date_time));
  Serial.print(":");
  Serial.print(second(date_time));
}
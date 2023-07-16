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

#define SD_chip_select_PIN 4

File myFile;
String data;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_chip_select_PIN)) {
    Serial.println("initialization failed!");
    while (1);
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
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }
  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }
  root.openRoot(volume);
  root.ls(LS_R | LS_DATE | LS_SIZE);

  // open the file for reading:
  String filename = "/sundial/1/2023.csv";

  if (SD.exists(filename)) {
    Serial.println(filename + " exists.");
  } else {
    Serial.println(filename + " doesn't exist.");
  }

  int idx;
  char c;
  myFile = SD.open(filename);
  if (myFile) {
    Serial.println(filename);

    // read from the file until there's nothing else in it:
    myFile.seek(279956); // seek to jump into the correct line
    while (myFile.available()) {
      data = myFile.readStringUntil('\n');
      idx = data.indexOf(",");
      //Serial.println(data.substring(0, idx));
      if (data.substring(0, idx).startsWith("2023-12-11")) {
        Serial.println(myFile.position());
        Serial.println(data);
      }
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
  }
}

void loop() {
  // nothing happens after setup
}



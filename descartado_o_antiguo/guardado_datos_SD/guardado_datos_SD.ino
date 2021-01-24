/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Libraries for SD card
#include "FS.h"
//#include "SD.h"
#include <SPI.h>

// Libraries to get time from NTP Server
#include <WiFi.h>
#include "time.h"

#define PIN_CS 13 // Pin usado para activar las comunicaciones SPI de la tarjeta SD


//Datos servidor ntp
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;   //si es gtm+1 3600, si es +2 7200 etc...
const int   daylightOffset_sec = 0; //3600;  //verano 3600, invierno 0.

// Replace with your network credentials
const char* ssid     = "MiFibra-8F9C";
const char* password = "645519569";

String dataMessage;

File myFile;

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}


void setup() {
  // Start serial communication for debugging purposes
  Serial.begin(9600);

  //SD init
  pinMode(PIN_CS,OUTPUT); // Es necesario configurar para salida de datos el pin conectado a CS/SS
  if(!SD.begin(PIN_CS)){
    Serial.println("esto no va");
    Serial.println(PIN_CS);
  }

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

//  //iniciamos comunicacion con la tarjeta SD
//   myFile = SD.open("test.txt", FILE_WRITE);
//    if(myFile)
//    {
//      Serial.println("Documento creado correctamente");
//      Serial.println("Guardando datos");
//      myFile.println("[00:42.00]Ese ritmo es tan sencillo");
//      myFile.println("[00:45.00]que yo lo toco tumbao");
//      //datos.flush(); // Vaciar el buffer es una manera de asegurar que los datos se han grabado
//      myFile.close();
//      Serial.println("Datos guardados");
//    }
//    else
//    {
//      Serial.println("No se puede crear el documento");
//    }
  



}

void loop() {
  // The ESP32 will be in deep sleep
  // it never reaches the loop()
}


//// Function to get date and time from NTPClient
//void getTimeStamp() {
//  while(!timeClient.update()) {
//    timeClient.forceUpdate();
//  }
//  // The formattedDate comes with the following format:
//  // 2018-05-28T16:00:13Z
//  // We need to extract date and time
//  formattedDate = timeClient.getFormattedDate();
//  Serial.println(formattedDate);
//
//  // Extract date
//  int splitT = formattedDate.indexOf("T");
//  dayStamp = formattedDate.substring(0, splitT);
//  Serial.println(dayStamp);
//  // Extract time
//  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
//  Serial.println(timeStamp);
//}
//
//// Write the sensor readings on the SD card
//void logSDCard() {
//  dataMessage = String(readingID) + "," + String(dayStamp) + "," + String(timeStamp) + "," + 
//                String(temperature) + "\r\n";
//  Serial.print("Save data: ");
//  Serial.println(dataMessage);
//  appendFile(SD, "/data.txt", dataMessage.c_str());
//}
//
//// Write to the SD card (DON'T MODIFY THIS FUNCTION)
//void writeFile(fs::FS &fs, const char * path, const char * message) {
//  Serial.printf("Writing file: %s\n", path);
//
//  File file = fs.open(path, FILE_WRITE);
//  if(!file) {
//    Serial.println("Failed to open file for writing");
//    return;
//  }
//  if(file.print(message)) {
//    Serial.println("File written");
//  } else {
//    Serial.println("Write failed");
//  }
//  file.close();
//}
//
//// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
//void appendFile(fs::FS &fs, const char * path, const char * message) {
//  Serial.printf("Appending to file: %s\n", path);
//
//  File file = fs.open(path, FILE_APPEND);
//  if(!file) {
//    Serial.println("Failed to open file for appending");
//    return;
//  }
//  if(file.print(message)) {
//    Serial.println("Message appended");
//  } else {
//    Serial.println("Append failed");
//  }
//  file.close();
//}


#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include <Wire.h>

#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G        0x00
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18



//////////////////////////////////////Configuracion sensor
//Funcion auxiliar lectura
void I2Cread(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data)
{
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.endTransmission();

  Wire.requestFrom(Address, Nbytes);
  uint8_t index = 0;
  while (Wire.available())
    Data[index++] = Wire.read();
}


// Funcion auxiliar de escritura
void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data)
{
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.write(Data);
  Wire.endTransmission();
}


//////////////////////////////configuracion SD

void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}



void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


String dataMedia = "1111111111";
String dataVarianza = "1111111";
String dataTotal = "1111111";
int writeFlag = 0;

//int16_t ax = 0;
//int16_t ay = 0;
//int16_t az = 0;
int cont1 = 0, cont2 = 15, f, c;
double datos1 [6][30], datos2 [6][30], datos3 [6][30], media[6], varianza[6];
double sum, med, medExp, sumExp, var, desvTip, med2;
int filas = 6, columnas = 30, filasm1 = 5, columnasm1 = 29;

void setup() {
  Wire.begin();
  Serial.begin(115200);


  //configuracion SD
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }


  // Configuracion tareas freetos

  xTaskCreate(Tarea1, "Tarea1", 10000, NULL, 1, NULL);
  xTaskCreate(Tarea2, "Tarea2", 10000, NULL, 1, NULL);

  // Configurar acelerometro
  I2CwriteByte(MPU9250_ADDRESS, 28, ACC_FULL_SCALE_16_G);

  // Configurar giroscopio
  I2CwriteByte(MPU9250_ADDRESS, 27, GYRO_FULL_SCALE_2000_DPS);


  deleteFile(SD, "/datos.txt");
  writeFile(SD, "/datos.txt", "");
}


void loop() {
  //delay(10);
}

void Tarea1( void * parameter ) {
  while (1) {
    // ---  Lectura acelerometro y giroscopio ---
    uint8_t Buf[14];
    I2Cread(MPU9250_ADDRESS, 0x3B, 14, Buf);

    // Convertir registros acelerometro
    int16_t ax = -(Buf[0] << 8 | Buf[1]);
    int16_t ay = -(Buf[2] << 8 | Buf[3]);
    int16_t az = Buf[4] << 8 | Buf[5];


    // Convertir registros giroscopio
    int16_t gx = -(Buf[8] << 8 | Buf[9]);
    int16_t gy = -(Buf[10] << 8 | Buf[11]);
    int16_t gz = Buf[12] << 8 | Buf[13];

    datos1[0][cont1] = double(ax) / 2000;
    datos1[1][cont1] = double(ay) / 2000;
    datos1[2][cont1] = double(az) / 2000;
    datos1[3][cont1] = double(gx) / 2000;
    datos1[4][cont1] = double(gy) / 2000;
    datos1[5][cont1] = double(gz) / 2000;

    datos2[0][cont2] = double(ax) / 2000;
    datos2[1][cont2] = double(ay) / 2000;
    datos2[2][cont2] = double(az) / 2000;
    datos2[3][cont2] = double(gx) / 2000;
    datos2[4][cont2] = double(gy) / 2000;
    datos2[5][cont2] = double(gz) / 2000;

   // Serial.println(datos2[5][cont2]);

    cont1++;
    cont2++;

    if (cont1 == columnasm1) {
      cont1 = 0;
      for (f = 0 ; f < filas; f++) {
        for (c = 0 ; c < columnas; c++) {
          datos3[f][c] = datos1[f][c];
          //Serial.println(datos3[f][c]);
        }
      }
      writeFlag = 1;

    }

    if (cont2 == columnasm1) {
      cont2 = 0;
      for (f = 0 ; f < filas; f++) {
        for (c = 0 ; c < columnas; c++) {
          datos3[f][c] = datos1[f][c];
        }
      }
      writeFlag = 1;
    }
    delay(10);

  }



}




void Tarea2( void * parameter) {
  while (1) {
    if (writeFlag == 1) {
      writeFlag = 0;
      sum = 0;
      sumExp = 0;
      //calculo de la media y la varianza
      for (f = 0 ; f < filas; f++) {
        sum = 0;
        sumExp = 0;
        for (c = 0 ; c < columnas; c++) {
          sum = sum + datos3[f][c];
          sumExp = sumExp + datos3[f][c] * datos3[f][c];
          //Serial.print(datos3[f][c]);
        }
        med = sum / columnas;
        medExp = sumExp / columnas;
        med2 = med * med;
        var = medExp - med2;
        media [f] = med;
        varianza [f] = var;

      }
      Serial.println("medidas");
      dataMedia = String(media[0]) + "," + String(media[1]) + "," + String(media[2])+ "," + String(media[3]) + "," + String(media[4]) + "," + String(media[5]) + "\r\n";
      dataVarianza = String(varianza[0]) + "," + String(varianza[1]) + "," + String(varianza[2])+ "," + String(varianza[3]) + "," + String(varianza[4]) + "," + String(varianza[5]) + "\r\n";
      dataTotal = String(media[0]) + "," + String(media[1]) + "," + String(media[2])+ "," + String(media[3]) + "," + String(media[4]) + "," + String(media[5]) + "," + String(varianza[0]) + "," + String(varianza[1]) + "," + String(varianza[2])+ "," + String(varianza[3]) + "," + String(varianza[4]) + "," + String(varianza[5]) + "\r\n";
      appendFile(SD, "/datos.txt", dataTotal.c_str() );

        Serial.println(dataMedia);
        Serial.println(dataVarianza);
    }
    delay(100);
  }

  Serial.println("Finalizando tarea 2");
  vTaskDelete( NULL );
}

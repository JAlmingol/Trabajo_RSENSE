
#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#include "time.h"

#include <Wire.h>
#include <WiFi.h>

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



/////////////////////////////////////////////declaracion timer
volatile int interruptCounter;
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}


/////////////////////////////////////////// Configuración WiFi
char ssid[] = "Jorge_Almin";
char pass[] = "iotdejorge";

int status = WL_IDLE_STATUS;
IPAddress server(192, 168, 43, 150);
WiFiClient client;


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
  //Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  //  if (file.print(message)) {
  //   // Serial.println("Message appended");
  //  } else {
  //    Serial.println("Append failed");
  //  }
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


String datosMedia = "1111111111";
String datosVarianza = "1111111";
String datosTotal = "1111111";
String movimiento = "1111111";
String movimientoAnterior = "1111111";
int estado = 1;
int writeFlag = 0;
int hayDato = 0;
int datoEnviado = 0;

//int16_t ax = 0;
//int16_t ay = 0;
//int16_t az = 0;
int filas = 6, columnas = 30, filasm1 = 5, columnasm1 = 29; //si se modifica modificar tamaño de arrays
int cont1 = 0, cont2 = columnas / 2, f, c;
double datos1 [6][30], datos2 [6][30], datos3 [6][30], media[6], varianza[6];
double sum, med, medExp, sumExp, var, desvTip, med2;
unsigned long tiempo1 = 0;
unsigned long tiempo2 = 0;
unsigned long diferenciaTiempo = 0;




void setup() {
        tiempo1 = micros();
  Wire.begin();
  Serial.begin(115200);

  //conecta a la red WiFi
  Serial.println();
  Serial.print("Conectando con ");
  Serial.println(ssid);

  //connect to WiFi
  WiFi.begin(ssid, pass);
  Serial.println("esto esta estando");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(WiFi.status());
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  //connect to server
  Serial.println("conectando con el servidor local");
  // if you get a connection, report back via serial:
  if (client.connect(server, 98)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("conectado");
    client.println();

  }


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

  //Configuracion timer
  timer = timerBegin (0, 80, true); //80MHz dividido entre 80, dejando 1MHZ de frecuencia y por tanto un timer de microsegundos
  timerAttachInterrupt(timer, &onTimer, true); //declara el timer de la interrupcion
  timerAlarmWrite(timer, 10000, true); //indica momento alarma del timer, (vector de inicializacion, momento de alarma y true para reiniciar)
  timerAlarmEnable(timer);


  // Configuracion tareas freetos

  xTaskCreate(Tarea1, "Tarea1", 10000, NULL, 1, NULL);
  xTaskCreate(Tarea2, "Tarea2", 10000, NULL, 1, NULL);
  xTaskCreate(Tarea3, "Tarea3", 10000, NULL, 1, NULL);

  // Configurar acelerometro
  I2CwriteByte(MPU9250_ADDRESS, 28, ACC_FULL_SCALE_16_G);

  // Configurar giroscopio
  I2CwriteByte(MPU9250_ADDRESS, 27, GYRO_FULL_SCALE_2000_DPS);


  deleteFile(SD, "/datos.txt");
  writeFile(SD, "/datos.txt", "");
      tiempo2 = micros();
    diferenciaTiempo = tiempo2 - tiempo1;
    Serial.print("El tiempo transcurrido entre el primer dato enviado y el ultimo es: ");
    Serial.println(diferenciaTiempo);
    //Serial.println(tiempo1);
    //Serial.println(tiempo2);
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

    datos1[0][cont1] = double(ax) / 17500;
    datos1[1][cont1] = double(ay) / 17500;
    datos1[2][cont1] = double(az) / 17500;
    datos1[3][cont1] = double(gx) / 2000;
    datos1[4][cont1] = double(gy) / 2000;
    datos1[5][cont1] = double(gz) / 2000;

    datos2[0][cont2] = double(ax) / 17500;
    datos2[1][cont2] = double(ay) / 17500;
    datos2[2][cont2] = double(az) / 17500;
    datos2[3][cont2] = double(gx) / 2000;
    datos2[4][cont2] = double(gy) / 2000;
    datos2[5][cont2] = double(gz) / 2000;

    //Serial.println(az);
    // Serial.println(datos2[2][cont2]);

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
          datos3[f][c] = datos2[f][c];
        }
      }
      writeFlag = 1;
    }
    //delay(10);


    vTaskDelay(pdMS_TO_TICKS(10));

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
      hayDato = 1;
      //  Serial.println("medidas");
      datosMedia = String(media[0]) + "," + String(media[1]) + "," + String(media[2]) + "," + String(media[3]) + "," + String(media[4]) + "," + String(media[5]) + "\r\n";
      datosVarianza = String(varianza[0]) + "," + String(varianza[1]) + "," + String(varianza[2]) + "," + String(varianza[3]) + "," + String(varianza[4]) + "," + String(varianza[5]) + "\r\n";
      datosTotal = String(media[0]) + "," + String(media[1]) + "," + String(media[2]) + "," + String(media[3]) + "," + String(media[4]) + "," + String(media[5]) + "," + String(varianza[0]) + "," + String(varianza[1]) + "," + String(varianza[2]) + "," + String(varianza[3]) + "," + String(varianza[4]) + "," + String(varianza[5]) + "\r\n";
      appendFile(SD, "/datos.txt", datosTotal.c_str() );

      // Serial.println(datosMedia);
      // Serial.println(datosVarianza);
    }
    //delay(100);

    


    
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  Serial.println("Finalizando tarea 2");
  vTaskDelete( NULL );
}



void Tarea3( void * parameter) {
  while (1) {

    switch (estado)
    {
      case 1: //sentencias cuando la variable = valor1
        if (hayDato == 1) {
          if (datoEnviado == 1) {
            datoEnviado = 0;
            estado = 1;
            hayDato = 0;
          }
          else {
            estado = 2;
            hayDato = 0;
          }

        }
        else {
          estado = 1;
        }

        break;
      case 2: //Comprueba si es un movimiento o son datos no significativos
        //si may negativo y hay varianza en los 3 giros es un golpe
        if (media [1] < 0 && varianza [3] > 1 && varianza [4] > 1 && varianza [5] > 1) {
          estado = 3;
          //Serial.println("movimiento");
        }
        else {
          estado = 5;
          movimiento = "nada";
          // Serial.println("no es movimiento");
        }
        break;
      case 3: //Determina cual de los movimientos anteriores es
        //si vgy es menor que 4 y maz es negativa es un directo
        if ( varianza [4] < 4 && media [2] < 0) {
          movimiento = "directo";
          estado = 4;
        } else {
          //si vgz pasa de 4, vaz mayor a 1,6 y maz menor que 0.6 es un crochet
          if (varianza [4] > 10 && varianza [5] > 1 && varianza [2] > 1.6 && media [2] < 0.6) {
            movimiento = "crochet";
            estado = 4;
          } else {
            //si maz es menor que 1 y vgx es menor que 5 hook
            if (varianza [4] > 10 && media [2] < 1 && varianza [3] < 5 ) {
              movimiento = "hook";
              estado = 4;
            } else {
              //si maz es mayor que 1 y vgx es mayor que 5 gancho
              if (varianza [4] > 10 && media [2] > 1 && varianza [3] > 5 ) {
                movimiento = "gancho";
                estado = 4;
              } else {      //Si no, se descarta --> Manda a reposo
                movimiento = "nada";
                estado = 5;

              }

            }
          }
        }


        break;
      case 4: //Envia el resultado al receptor WiFi
        if (movimiento == movimientoAnterior) {
          estado = 5;
        }
        else{
        //        client.println(movimiento);
        Serial.println(movimiento);
        client.println(movimiento);
        
        client.println(datosVarianza);
        
        client.println(datosMedia);
        datoEnviado = 1;
        //}
        estado = 5;
        }
        break;
      case 5: //Guarda el valor para compararlo con el siguiente y manda la maquina a estado de reposo
        movimientoAnterior = movimiento;
        estado = 1;

        break;
    }



    vTaskDelay(pdMS_TO_TICKS(10));



  }


}

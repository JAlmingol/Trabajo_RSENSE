#include <Wire.h>



int32_t media (int16_t datos[], float dimensiones) {
  //float valuesLengt = sizeof(datos);// / sizeof(datos[0]); //23 datos
  //Serial.println ("dimensiones dentro del array: " +  char(dimensiones));
  int32_t sum, med;
  for ( int i = 0 ; i < dimensiones ; i++) {
    sum = sum + datos[i];
    Serial.println(datos[i]);
  }
  Serial.println(sum);
  med = sum / dimensiones;
  return med;
}

int32_t varianza (int16_t datos[], float dimensiones) {
  //float valuesLengt = sizeof(datos);// / sizeof(datos[0]); //23 datos
  //Serial.println ("dimensiones dentro del array: " +  char(dimensiones));
  int32_t sum, med, medExp2, sumExp2, var, desvTip;
  for ( int i = 0 ; i < dimensiones ; i++) {
    sumExp2 = sumExp2 + datos[i]^2;
    sum = sum + datos[i];
   // Serial.println(datos[i]);
  }
  medExp2 = sumExp2 / dimensiones;
  
  med = sum / dimensiones;
  var = medExp2-med^2;
  desvTip = sqrt(var);
  return var; 
  
}


int16_t numeros[] = {1.0, 1.4, 2.1, 3.652, 0.841, 8.425, 3.200, 4.684, 4.269, 8.426, 8.418, 2.84, 8.4, 3.456, 3.42, 3.235, 3.545, 4.168, 3.51, 3.548, 9.843, 2.158, 6.84};



void setup() {
  Wire.begin();
  Serial.begin(115200);
  float dimensionArray = sizeof(numeros) / sizeof(numeros[0]);
  Serial.println(dimensionArray);
  Serial.println("va a calcular");
  float mediaCalculada = media(numeros, dimensionArray);
  float varianzaCalculada = varianza(numeros, dimensionArray);
  Serial.println("aqui va la media");
  Serial.println(mediaCalculada);
  Serial.println(varianzaCalculada);


}

void loop() {
  // put your main code here, to run repeatedly:

}

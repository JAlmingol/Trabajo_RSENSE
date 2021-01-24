#include <Wire.h>



double media (double datos[], double dimensiones) {
  //float valuesLengt = sizeof(datos);// / sizeof(datos[0]); //23 datos
  //Serial.println ("dimensiones dentro del array: " +  char(dimensiones));
  double sum, med;
  for ( int i = 0 ; i < dimensiones ; i++) {
    sum = sum + datos[i];  
  }
  
  //Serial.println(dimensiones);
  med = sum / dimensiones;
  Serial.println(med);
  return med;
}

double varianza (double datos[], double dimensiones) {
  //float valuesLengt = sizeof(datos);// / sizeof(datos[0]); //23 datos
  //Serial.println ("dimensiones dentro del array: " +  char(dimensiones));
  double sum, med, medExp2, sumExp2, var, desvTip, med2;
  for ( int i = 0 ; i < dimensiones ; i++) {
    sum = sum + datos[i];
    Serial.println(sum);
    sumExp2 = sumExp2 + datos[i]*datos[i];
   // Serial.println(datos[i]);
  }
  medExp2 = int(sumExp2) / dimensiones;
  Serial.println(sumExp2);
  Serial.println(medExp2);
  med = sum / dimensiones;
  med2 = med*med;
  Serial.println(med);
  var = medExp2-med2;
  //desvTip = sqrt(var);
  return var; 
  
}


double numeros[] = {1.0, 1.4, 2.1, 3.652, 0.841, 8.425, 3.200, 4.684, 4.269, 8.426, 8.418, 2.84, 8.4, 3.456, 3.42, 3.235, 3.545, 4.168, 3.51, 3.548, 9.843, 2.158, 6.84};



void setup() {
  Wire.begin();
  Serial.begin(115200);
  double dimensionArray = sizeof(numeros) / sizeof(numeros[0]);
  Serial.println(dimensionArray);
  Serial.println("va a calcular");
  double mediaCalculada = media(numeros, dimensionArray);
  double varianzaCalculada = varianza(numeros, dimensionArray);
  Serial.println("aqui va la media");
  Serial.println(mediaCalculada);
  Serial.println(varianzaCalculada);


}

void loop() {
  // put your main code here, to run repeatedly:

}

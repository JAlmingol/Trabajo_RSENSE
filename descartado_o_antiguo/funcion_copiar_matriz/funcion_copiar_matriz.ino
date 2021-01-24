#include <Wire.h>


double matrizCopiada ( int filas, int columnas, double matrizOriginal[][columnas]){
  double matrizCopi[filas][columnas];
  for ( int fila = 0 ; fila < filas ; fila++) {
    for ( int colum = 0 ; colum < columnas ; colum++) {
      matrizCopi[fila][colum] = matrizOriginal[fila][colum];
      
    }
  }
  return MatrizCopi;
}
  





double numeros[][] = {{3, 5, 4},{1,2,3},{4,5,6},{7,8,9}};
double numerosCopiados[4][3];


void setup() {
  Wire.begin();
  Serial.begin(115200);

  Serial.println(numeros);
  numerosCopiados = matrizCopiada(numeros, 4, 3);
  Serial.println(numerosCopiados);


}

void loop() {
  // put your main code here, to run repeatedly:

}

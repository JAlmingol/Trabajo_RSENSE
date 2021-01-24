

switch (estado)
{
case Reposo: //sentencias cuando la variable = valor1
      if(HayDato == 1){
      estado = EsMovimiento  
      }
  break;
case EsMovimiento: //Comprueba si es un movimiento o son datos no significativos
                   //si may negativo y hay varianza en los 3 giros es un golpe
                   //si vgz pasa de 4, vaz mayor a 1,6 y maz menor que 0.6 es un croche
                   //si maz es menor que 1 y vgx es menor que 5 hook
                   //si maz es menor que 1 y vgx es menor que 5 hook
                   //si maz es mayor que 1 y vgx es mayor que 5 gancho
                   //Si no, se descarta --> Manda a reposo
  break;
case QueMovimientoEs: //Determina cual de los movimientos anteriores es
                      //si vgy es menor que 10 y maz es negativa es un directo
  break;
case EnviarDatos: //Envia el resultado al receptor WiFi
  break;
case MandaAReposo: //Guarda el valor para compararlo con el siguiente y manda la maquina a estado de reposo

  break;
default: //sentencias cuando no se cumple ninguno de los casos anteriores
}




void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

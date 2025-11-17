#include <Encoder.h>
#include <analogWrite.h>

// Configuración inicial
Encoder encoder_der(5, 23);
#define Tiempo_muestreo 40

// Parámetros del PI
float Kp = 0.0044;
float Ki = 0.12;
float Kd = 0;

float referencia_cm_s = 10; // Valor deseado (referencia) 10 cm/s
int contador = 0; //contador para cambiar de referencia
float actuacion = 0;

void setup() {
    Serial.begin(115200);
}

void loop() {

  actuacion = funcion_PID(referencia_cm_s, Kp, Ki, Kd);

  //Se actua a la Planta
  motor(0,actuacion);

  contador = contador + 1;
  delay(Tiempo_muestreo); //40 ms
  
  if(contador > 450)
  {
    if(referencia_cm_s > 25)
    {
      referencia_cm_s = 10;
    }
    else if(referencia_cm_s < 12)
    {
      referencia_cm_s = 30;
    }
    contador = 0;
  }

}


/*
Este programa permite la lectura de distancia que mide el sensor VL53L0X, 
ubicado en la parte frontal del DDMR.
-Requisitos: Libreria Wire.h <https://github.com/espressif/arduino-esp32/tree/master/libraries/Wire>
             Libreria VL53L0X.h <https://github.com/pololu/vl53l0x-arduino##readme>
             Archivo sensores.ino
-Cofiguracion: Al visualizar por consola los valores entregados por el sensor, verificar si el valor minimo que se obtiene
               al poner un objeto justo en frente del DDMR corresponede a 0[cm] (de ser necesario ajustar "offset_sensor").
*/

#include <VL53L0X.h>
#include <Wire.h>

double        OFFSET_SENSOR_CM    = 4;  //ajusta el offset del sensor para que la medicion sea certera
const double  DISTANCIA_MAXIMA_CM = 80.0; //saturación en la medicion
const double  DISTANCIA_MINIMA_CM = 0.0;  // Distancia minima
const int     FILTRO_MUESTRAS     = 200;      // numero de repeticiones para filtrar muestras validas
#define MAX_CAMBIO_CM   30
#define N_MUESTRAS      5 



VL53L0X sensor;

double distanciaActual = 0;         //Variable que almacena la distancia medida por el sensor
int  contadorFiltro    = 0;         //Variable que cuenta las veces que se repite una medición para discriminar su validez
  //Valor de ajuste del sensor en [cm]

void setup()
{
  Serial.begin(115200);
  InicializarSensor(sensor); //Fucion que configura el sensor VL53L0X
}
void loop()
{
  leerOffsetSerial();
  distanciaActual = leerDistancia(sensor);   //Obtención de la distancia en centimetros
  Serial.println(distanciaActual);
}

void leerOffsetSerial() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    OFFSET_SENSOR_CM = input.toFloat();
  }
}

README - Modulo sensor_ToF (VL53L0X)

Este modulo tiene toda la informacion basica para la configuracion
 y lectura del sensor de distancia VL53L0X.  

Incluye: La definicion de constantes para la calibracion, la inicializacion 
del sensor, la lectura CONTINUA mediante la funcion nativa de la libreria del sensor
(En aplicaciones futuras se puede añadir en la inicializacion un ajuste para permitir elegir 
entre la lectura continua o la lectura via registro siesque se requiera.)


/////////////////////////////////////////////////////////////
Archivos del modulo. 
////////////////////////////////////////////////////////////

sensor_ToF_3.3.1.h -> declaraciones, constantes y documentacion de las funciones
sensor_ToF_3.3.1-ino -> Implementación de las funciones.
(3.3.1 hace referencia a la version del gestor de placa esp32 con la que esta libreria es compatible)


///////////////////////////////////////////////////////////
Dependencias necesarias
/////////////////////////////////////////////////////////////

Este modulo requiere las siguientes librerias para funcionar

1. Arduino Core
	#include <Arduino.h>

2. Wire (I2C)
	#include <Wire.h>
	(necesaria para la comunicacion entre el sensor y el micro, se usa en la 
	configuracion del sensor "configurarSensorToF()")


3. VL53L0x (Polulu)
	#include <VL53L0X.h>
	(es el controlador del sensor de distancia)

*Este modulo fue desarrollado utilizando la libreria VL53L0X de Pololu
y no la de ST Microelectronics

*Una vez instaladas estas dependencias, en el archivo principal solo debe incluirse el .h de la libreria
#include "sensor_ToF.h" ya que este archivo inclue las dependencias necesarias.



1. sensor_ToF.h
//////////////////////////////////////////////////////////////
Constantes de calibración:
/////////////////////////////////////////////////////////////

OFFSET_SENSOR_CM: Compensa el error sistemático del VL53L0X según
calibración experimental.
DISTANCIA_MAXIMA_CM: Límite máximo útil con saturación.
DISTANCIA_MINIMA_CM: Reservado para futura saturación inferior.


///////////////////////////////////////////////////////////
Funciones declaradas:
//////////////////////////////////////////////////////////


configurarSensorToF(VL53L0X &sensor):
Inicializa el sensor y configura I2C, timeout, rate limit, timing budget
y medición continua para la configuracion y medicion del sensor.

leerDistancia(VL53L0X &sensor):
Realiza la lectura continua del sensor y retorna distancia en
centímetros, aplicando conversión, offset y saturación.
(Eventualmente se puede modificar para añadir un filtro de muestras, o añadirlo y aplicar segun parametro
de entrada a la funcion.)




2. sensor_ToF.ino

/////////////////////////////////////////////////////
leerDistancia():
////////////////////////////////////////////////////

Lee distancia en mm, convierte a cm, ajusta offset y aplica saturación.

configurarSensorToF():
Inicializa I2C, configura parámetros del sensor y activa modo continuo.

Ejemplo de uso

#include “sensor_ToF.h”

void setup() { 
Serial.begin(115200); 
configurarSensorToF(sensor); 
}

void loop() { 
double d = leerDistancia(sensor); 
Serial.println(d);
delay(50); }

// inicializa y configura el sensor para medicion continua, almacena 
en la variable "d" el valor medido de la distancia en cm.















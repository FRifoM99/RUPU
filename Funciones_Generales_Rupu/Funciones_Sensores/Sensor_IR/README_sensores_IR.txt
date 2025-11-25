Readme - Modulo Sensores_IR 

Este documento describe el funcionamiento del modulo "Sensores_IR", 
todas las funciones disponibles, la calibracion y las dependencias necesarias.

El modulo sensores_IR implementa:

- control del mux analogico necesario para leer los 16 sensores IR (8 frontales y 8 posteriores)
- calibracion de maximos y minimos de cada sensor
- discrimina si la linea de trayectoria es blanca o negra
- calcula la posicion del robot respecto a la lineal
- funciones auxiliares
- calibracion de la posicion utilizando los sensores

Diseñada para rupu o cualquier robot Diferencial DDMR

////////////////////////////////////////////////////////////
Dependencias 
///////////////////////////////////////////////////////////

Este modulo requiere las siguientes librerias

1. Arduino Core
	#include <Arduino.h>

2. Encoder (para los motores y cinematica)
	#include <ESP32Encoder.h>


* Los pines del multiplexor estan definidor en el .ino, ya que son 
variables privadas que no se deberia poder modificar al menor al utilizar el mismo hardware.



////////////////////////////////////////////////////////
constantes fisicas (sensores_IR_3.3.1.h)
/////////////////////////////////////////////////////


RADIO_RUEDA: Radio de ruedas en cm
DIST_RUEDAS: Distancia entre las reuedas en cm
N_REDUCCION: Numero de reduccion de la caja reductora del motor
CPR: cuentas por revolucion de los encoders
D1: Distancia desde el centro de giro al centro del arreglo IR frontal en mm
D2: distancia desde el centro de giro al centro del arreglo IR posterior en mm
LINEA_NEGRA: valor logico 1 para indicar trayectoria blanca



/////////////////////////////////////////////////////
Funciones declaradas
////////////////////////////////////////////////////

1. void configurarMUX()
	configura los pines del multiplexor para permitir seleccionar los canales

2. void calibrarSensores()
	ejecuta la calibracion completa, girando el robot y midiendo todos los sensores
	IR para registrar máximos y minimos. Al final alineal el robot con la linea.

3. void setMaxMin(int i, float lectura)
	actualiza los valores maximos y minimos detectados para el sensor "i"

4. void seleccionarCanalMUX(byte canal)
	activa el canal correspondiente del mux para leer el sensor IR indicado

5. float getPosition(bool frontal)
	calcula la posicion horizontal de la linea respecto al centro del arreglo de sensores IR
	
6. float calcularNivelSensor(byte index, bool frontal)
	Normaliza la lectura de un sensor IR a un valor entre 0 y 100% para indicar que tan
	negra o blanca es la superficie

7. void Calculo_Velocidades()
	calcula las velocidades lineales de cada rueda usando los encoders

8. void Curvatura_Pista()
	determina si el robot va en recta o curva segun las distancias recorridas por cada rueda



**La calibracion requiere que los motores y encoders funcionen correctamente, ademas
en esta version de la libreria, se supone que las constantes y objetos relacionados 
con los encoders y motores, se encuentran en la funcion principal. 
(si estas constantes y objetos estan definidos en una libreria especifica, entonces se debe incluir
en la libreria sensores_IR_3.3.1.h, el otro .h de la libreria que se encarga de los motores y encoders)

 




















































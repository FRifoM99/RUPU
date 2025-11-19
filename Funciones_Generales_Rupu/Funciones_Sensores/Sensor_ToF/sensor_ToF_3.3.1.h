#pragma once //(aqui no use el tipico Include Guard, pragma once hace lo mismo)
#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

//esta libreria se encarga de las lecturas del sensor de distancia, su funcion  es solo configurar el sensor y entregar la muestra de la medicion
//cuando la funcion leerDistancia se llama

// ===============================
// Constantes de configuración
// ===============================
//(las saque del main y las puse aqui, ya que son parte del modulo de sensor ToF)

constexpr float  OFFSET_SENSOR_CM    = 4.0f;  //ajusta el offset del sensor para que la medicion sea certera (ajuste experimental)
constexpr float  DISTANCIA_MAXIMA_CM = 80.0f; //saturación en la medicion
constexpr float  DISTANCIA_MINIMA_CM = 0.0f;  // Distancia minima
// no se ocupan en este codigo, pero si en el codigo de pruebas del sensor ToF
//constexpr int     FILTRO_MUESTRAS    = 200;   // numero de repeticiones para filtrar muestras validas
//int               contadorFiltro     =  0;    //Variable que cuenta las veces que se repite una medición para discriminar su validez    
VL53L0X sensor;  //define el objeto sensor

// ===============================
//            Funciones
// ===============================

/**
 * @brief Configura el sensor de distancia VL53L0X.
 *
 * Inicializa la comunicación I2C, establece el tiempo máximo de espera,
 * los límites de tasa de señal, y configura el sensor para operar en modo
 * de medición continua.
 *
 * @param sensor Referencia al objeto VL53L0X que representa el sensor.
 * @note Debe llamarse una sola vez dentro del setup() antes de usar leerDistancia().
 */
void configurarSensorToF(VL53L0X &sensor);


/**
 * @brief Lee la distancia medida por el sensor VL53L0X.
 *
 * Obtiene la distancia actual medida por el sensor y la convierte a centímetros.
 * Si la distancia supera el límite máximo definido, se aplica saturación.
 *
 * @param sensor Referencia al objeto VL53L0X que representa el sensor.
 * @return Distancia medida en centímetros (double).
 */
double leerDistancia(VL53L0X &sensor);



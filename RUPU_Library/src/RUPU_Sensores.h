#ifndef RUPU_SENSORES_H
#define RUPU_SENSORES_H

#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <Encoder.h>
#include "RUPU_Config.h"
#include "RUPU_Globals.h"
#include "RUPU_Actuadores.h"

void inicializar_sensores();
void calibrarSensores();
void setMaxMin(int i, float aread);
float getposition(bool direccion);
void configuracion_sensor_d();
double distancia();
void velocidades();
void curvatura_pista();

#endif // RUPU_SENSORES_H

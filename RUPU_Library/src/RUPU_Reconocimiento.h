#ifndef RUPU_RECONOCIMIENTO_H
#define RUPU_RECONOCIMIENTO_H

#include <Arduino.h>
#include "RUPU_Globals.h"
#include "RUPU_Comunicacion.h"
#include "RUPU_Sensores.h"

void ciclo_reconocimiento();
void sendHELLO();
bool disparo_sensor_boton();

#endif // RUPU_RECONOCIMIENTO_H

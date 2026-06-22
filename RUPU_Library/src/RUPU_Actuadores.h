#ifndef RUPU_ACTUADORES_H
#define RUPU_ACTUADORES_H

#include <Arduino.h>
#include "RUPU_Config.h"
#include "RUPU_Globals.h"

void inicializar_motores();
void motor(int Velocidad_motor_izq, int Velocidad_motor_der);

#endif // RUPU_ACTUADORES_H

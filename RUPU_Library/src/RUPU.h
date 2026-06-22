#ifndef RUPU_H
#define RUPU_H

#include <Arduino.h>
#include "RUPU_Config.h"
#include "RUPU_Globals.h"
#include "RUPU_Actuadores.h"
#include "RUPU_Sensores.h"
#include "RUPU_Controladores.h"
#include "RUPU_Comunicacion.h"
#include "RUPU_Reconocimiento.h"

void ciclo_de_inicio();
void ciclo_de_control();

#endif // RUPU_H

#ifndef RUPU_CONTROLADORES_H
#define RUPU_CONTROLADORES_H

#include <Arduino.h>
#include "RUPU_Globals.h"

double calculoPID(double y,double ref,double &error_ant,double &error_integral,double kp,double ki,double kd,double limite, String MODO, double &out_manual, String direccion);
double calculoPIDd(double y,double ref,double &error_ant,double &error_integral,double kp,double ki,double kd,double limite, String MODO, double &out_manual, String direccion);

#endif // RUPU_CONTROLADORES_H

#ifndef RUPU_COMUNICACION_H
#define RUPU_COMUNICACION_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include "RUPU_Config.h"
#include "RUPU_Globals.h"

void setup_wifi();
void udp_recep();
void lectura_estado(int len);
String configuracion_remota(int len);
void estado_predecesor(int len);
void udp_transm();
void udp_check();
void udp_monitor();

#endif // RUPU_COMUNICACION_H

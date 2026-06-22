#ifndef RUPU_GLOBALS_H
#define RUPU_GLOBALS_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <VL53L0X.h>
#include <ESP32Encoder.h>
#include "RUPU_Config.h"

// Variables Globales (externs)
extern IPAddress IP_monitoreo;
extern IPAddress IP_sucesor;
extern String EtiquetaRobot;
extern unsigned long t_hold_start;
extern unsigned long t_last_hello;
extern const uint16_t REG_PORT;
extern bool ack_recibido_recon;

extern byte estado;
extern byte estado_siguiente;
extern byte calibrar;
extern String parar;

extern double Input_theta, Output_theta;
extern double theta_ref;
extern double Kp_theta, Ki_theta, Kd_theta;

extern double Input_vel, Output_vel;
extern double Output_vel_ant;
extern double vel_ref;
extern double sp_vel;
extern double vel_crucero;
extern double Kp_vel, Ki_vel, Kd_vel;

extern double Input_d, Output_d;
extern double d_ref;
extern double error_d;
extern float delta;
extern double Kp_d, Ki_d, Kd_d;

extern String PID_theta;
extern double sat_theta;
extern double error_ant_theta;
extern double integral_theta;

extern String PID_vel;
extern double sat_vel;
extern double error_ant_vel;
extern double integral_vel;

extern String PID_d;
extern double sat_d;
extern double error_ant_d;
extern double integral_d;

extern WiFiUDP udp;
extern String cadena;
extern char msg[128];
extern char paquete_entrante[64];
extern unsigned long t_com_predecesor, t_monitoreo;

extern unsigned long t_actual;
extern unsigned long t_controlador;

extern double ang_vel;
extern double v_r[5];
extern double v_l[5];
extern double a_sl, a_sr, b_sl, b_sr, c_sl, c_sr;
extern double a_s, b_s, c_s;
extern unsigned long t_arco;
extern byte recta;
extern double a_curvatura, b_curvatura, c_curvatura, curvatura;
extern double curvatura_predecesor;
extern unsigned long t_svel;
extern double cuenta;

extern VL53L0X sensor;
extern ESP32Encoder encoder_der;
extern ESP32Encoder encoder_izq;
extern byte control;

extern float sensoresMax[16];
extern float sensoresMin[16];

#endif // RUPU_GLOBALS_H

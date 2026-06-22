#include "RUPU_Globals.h"

IPAddress IP_monitoreo(0,0,0,0);
IPAddress IP_sucesor(172,20,10,100);
String EtiquetaRobot = "L";
unsigned long t_hold_start = 0;
unsigned long t_last_hello = 0;
const uint16_t REG_PORT = 7777;
bool ack_recibido_recon = false;

byte estado = 0;
byte estado_siguiente = 0;
byte calibrar = 0;
String parar = "si";

double Input_theta = 0, Output_theta = 0;
double theta_ref = 0;
double Kp_theta = 2000, Ki_theta = 4500, Kd_theta = 60;

double Input_vel = 0, Output_vel = 0;
double Output_vel_ant = 0;
double vel_ref = 0;
double sp_vel = 0;
double vel_crucero = 15;
double Kp_vel = 20.3, Ki_vel = 145.3, Kd_vel = 0;

double Input_d = 0, Output_d = 0;
double error_d = 0;
double d_ref = 10;
float delta = 0.1;
double Kp_d = 4, Ki_d = 3, Kd_d = 0;

String PID_theta="MANUAL";
double sat_theta = 1023;
double error_ant_theta = 0;
double integral_theta = 0;

String PID_vel="MANUAL";
double sat_vel = 800;
double error_ant_vel = 0;
double integral_vel = 0;

String PID_d="MANUAL";
double sat_d = 30;
double error_ant_d = 0;
double integral_d = 0;

WiFiUDP udp;
String cadena;
char msg[128];
char paquete_entrante[64];
unsigned long t_com_predecesor = 0, t_monitoreo = 0;

unsigned long t_actual = 0;
unsigned long t_controlador = 0;

double ang_vel = 0;
double v_r[5] = {0, 0, 0, 0, 0};
double v_l[5] = {0, 0, 0, 0, 0};
double a_sl=0, a_sr=0, b_sl=0, b_sr=0, c_sl=0, c_sr=0;
double a_s=0, b_s=0, c_s=0;
unsigned long t_arco = 0;
byte recta = 0;
double a_curvatura=0, b_curvatura=0, c_curvatura=0, curvatura=0;
double curvatura_predecesor=0;
unsigned long t_svel = 0;
double cuenta = 0;

VL53L0X sensor;
ESP32Encoder encoder_der;
ESP32Encoder encoder_izq;
byte control = 1;

float sensoresMax[16] = {0};
float sensoresMin[16] = {0};

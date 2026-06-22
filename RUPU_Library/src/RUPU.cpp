#include "RUPU.h"

void ciclo_de_inicio(){
  udp_recep();
  PID_theta="MANUAL";
  PID_vel="MANUAL";
  PID_d="MANUAL";
  error_ant_d=0;
  integral_d=0;
  error_ant_vel=0;
  integral_vel=0;
  error_ant_theta=0;
  integral_theta=0;
  Output_d=0;
  Output_vel=0;
  Output_theta=0;
  d_ref = distancia();
  sp_vel = 0;
  Input_vel=0;
  vel_ref=0;
  motor(0,0);
  encoder_der.clearCount();
  encoder_izq.clearCount();
  udp_transm();
}

void ciclo_de_control(){
  udp_recep();
  Input_d = distancia();
  if(EtiquetaRobot!="L"){
    error_d = Input_d - d_ref;
  }
  else{
    error_d=0;
  }
  velocidades();
  curvatura_pista();
  control=1;                                                              
  if(Output_vel<0){
    Input_theta=-getposition(0)/CONSTANTE_D2;
  }
  else{
    Input_theta=getposition(1)/CONSTANTE_D1;
  }
  if ((abs(error_d) >= delta + 1) || EtiquetaRobot=="L") {
    PID_theta="AUTO";
    PID_vel="AUTO";
    if(control){
      PID_d="AUTO";
    }
    else{
      PID_d="MANUAL";
      Output_d=vel_crucero;
      error_ant_d=0;
      integral_d=vel_crucero/Ki_d - error_d*0.04;
    }
    if(EtiquetaRobot=="L"){
      PID_d="MANUAL"; 
      Output_d = 0;
      error_ant_d=0;
      integral_d=0;
      error_d =0;
    }
  }
  if (abs(Input_vel) <= 6 && abs(Input_theta) <= 0.015 && ((abs(error_d) < delta) && (sp_vel==0 || EtiquetaRobot!="L"))) {
    PID_d="MANUAL";
    PID_vel="MANUAL";
    PID_theta="MANUAL";
    Output_d = 0;
    Output_vel = 0;
    Output_theta = 0;
    error_ant_d=0;
    integral_d=0;
    error_ant_vel=0;
    integral_vel=0;
    error_ant_theta=0;
    integral_theta=0;
  }
  udp_transm();                                                   
  t_actual=millis()-t_controlador;
  if(t_actual>=40){
    Output_d=calculoPIDd(Input_d, d_ref,error_ant_d, integral_d, Kp_d, Ki_d, Kd_d, sat_d, PID_d, Output_d, "INVERSO"); 
    vel_ref= Output_d *(EtiquetaRobot!="L") + (EtiquetaRobot=="L")*sp_vel;
    Output_vel=calculoPID(Input_vel, vel_ref, error_ant_vel, integral_vel, Kp_vel, Ki_vel, Kd_vel, sat_vel, PID_vel, Output_vel, "DIRECTO");
    Output_theta=calculoPID(Input_theta, theta_ref, error_ant_theta, integral_theta, Kp_theta, Ki_theta, Kd_theta, sat_theta, PID_theta, Output_theta, "DIRECTO");
    udp_monitor();
    t_controlador=millis();
  }
  motor(Output_vel - Output_theta, Output_vel + Output_theta);
}

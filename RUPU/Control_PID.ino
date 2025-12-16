#include "config.h"

// Variables de estado PID
static long ultima_posicion = 0;
static unsigned long ultimo_tiempo = 0;
static float error_0=0, error_1=0, error_2=0;
static float u_0=0, u_1=0;

void configurarEncoders() {
  encoder_der.attachHalfQuad(PIN_ENC_DER_A, PIN_ENC_DER_B);
  encoder_izq.attachHalfQuad(PIN_ENC_IZQ_A, PIN_ENC_IZQ_B);
  encoder_der.clearCount();
  encoder_izq.clearCount();
}

float funcion_PID(float referencia_cm_s, float Kp, float Ki, float Kd) {
  long posicion_actual = encoder_der.getCount();
  unsigned long tiempo_actual = millis();
  
  float delta_tiempo = (tiempo_actual - ultimo_tiempo)/1000.0;
  if(delta_tiempo == 0) delta_tiempo = TIEMPO_MUESTREO_PID/1000.0;

  float velocidad = (posicion_actual - ultima_posicion)/delta_tiempo;
  float velocidad_cm_s = (velocidad * 2 * PI * (RADIO_RUEDA/100.0) * 100) / (CPR * N_REDUCCION); 
  
  error_0 = referencia_cm_s - velocidad_cm_s;

  u_0 = u_1 + (Kp + (Kd/delta_tiempo))*error_0 + (-Kp + Ki*delta_tiempo - 2*(Kd/delta_tiempo))*error_1 + (Kd/delta_tiempo)*error_2;
  
  float actuacion = constrain(u_0, -PWM_RESOLUCION, PWM_RESOLUCION);

  ultimo_tiempo = tiempo_actual;
  ultima_posicion = posicion_actual;
  error_2 = error_1; error_1 = error_0; u_1 = u_0;

  return actuacion;
}
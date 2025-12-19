#include "config.h"

// --- INSTANCIAS DE ESTADO PID ---
// Al definirlas aquí, limpiamos el archivo principal
PID_State pid_izq;
PID_State pid_der;

// Variables internas de velocidad
float velocidad_izq_cm_s = 0;
float velocidad_der_cm_s = 0;
unsigned long t_ultimo_calculo = 0;

// --- NUEVA: GESTIÓN DE INICIO (Setup) ---
void iniciarPID() {
  // Inicializamos los tiempos para evitar el "salto" en el primer ciclo
  pid_izq.ultimo_tiempo = millis();
  pid_der.ultimo_tiempo = millis();
  
  // Reseteamos errores por si acaso
  pid_izq.error_1 = 0; pid_izq.error_2 = 0; pid_izq.u_1 = 0;
  pid_der.error_1 = 0; pid_der.error_2 = 0; pid_der.u_1 = 0;
}

// --- NUEVA: GESTIÓN DE TIEMPO (Loop) ---
void esperarCiclo() {
  // Aquí encapsulamos el delay para enseñar "Concepto de Muestreo"
  delay(TIEMPO_MUESTREO_PID);
}

// ... (MANTENER configurarEncoders Y actualizarVelocidades IGUAL QUE ANTES) ...

void configurarEncoders() {
  encoder_der.attachHalfQuad(PIN_ENC_DER_A, PIN_ENC_DER_B);
  encoder_izq.attachHalfQuad(PIN_ENC_IZQ_A, PIN_ENC_IZQ_B);
  encoder_der.clearCount();
  encoder_izq.clearCount();
}

void actualizarVelocidades() {
  unsigned long t_actual = millis();
  float delta_t = (t_actual - t_ultimo_calculo) / 1000.0;

  if (delta_t >= (TIEMPO_MUESTREO_PID / 1000.0)) {
    long cuenta_izq = encoder_izq.getCount();
    long cuenta_der = encoder_der.getCount();
    static long prev_izq = 0;
    static long prev_der = 0;

    long delta_izq = cuenta_izq - prev_izq;
    long delta_der = cuenta_der - prev_der;

    velocidad_izq_cm_s = (delta_izq / P_CONST) / delta_t;
    velocidad_der_cm_s = (delta_der / P_CONST) / delta_t;

    prev_izq = cuenta_izq;
    prev_der = cuenta_der;
    t_ultimo_calculo = t_actual;
  }
}

float getVelocidad(bool izquierda) {
  return izquierda ? velocidad_izq_cm_s : velocidad_der_cm_s;
}

int funcion_PID(float referencia_cm_s, float medicion_cm_s, PID_State &state, float Kp, float Ki, float Kd) {
  unsigned long tiempo_actual = millis();
  float delta_tiempo = (tiempo_actual - state.ultimo_tiempo) / 1000.0;
  
  if(delta_tiempo <= 0) delta_tiempo = TIEMPO_MUESTREO_PID / 1000.0;

  float referencia_pulsos = referencia_cm_s * P_CONST;
  float medicion_pulsos   = medicion_cm_s   * P_CONST;
  float error_0 = referencia_pulsos - medicion_pulsos;

  float term_P = (Kp + (Kd/delta_tiempo)) * error_0;
  float term_I = (-Kp + Ki*delta_tiempo - 2*(Kd/delta_tiempo)) * state.error_1;
  float term_D = (Kd/delta_tiempo) * state.error_2;
  
  float u_0 = state.u_1 + term_P + term_I + term_D;
  int actuacion = constrain((int)u_0, -PWM_RESOLUCION, PWM_RESOLUCION);

  state.ultimo_tiempo = tiempo_actual;
  state.error_2 = state.error_1; 
  state.error_1 = error_0;
  state.u_1 = u_0; 

  return actuacion;
}
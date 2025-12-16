#include "config.h"

void motor(int Velocidad_motor_izq, int Velocidad_motor_der) { 
  pinMode(PIN_MOTOR_DER_IN1, OUTPUT); 
  pinMode(PIN_MOTOR_DER_IN2, OUTPUT); 
  pinMode(PIN_MOTOR_IZQ_IN1, OUTPUT); 
  pinMode(PIN_MOTOR_IZQ_IN2, OUTPUT);

  if (Velocidad_motor_der > PWM_RESOLUCION) Velocidad_motor_der = PWM_RESOLUCION;
  else if(Velocidad_motor_der < -PWM_RESOLUCION) Velocidad_motor_der = -PWM_RESOLUCION;
  
  if (Velocidad_motor_izq > PWM_RESOLUCION) Velocidad_motor_izq = PWM_RESOLUCION;
  else if(Velocidad_motor_izq < -PWM_RESOLUCION) Velocidad_motor_izq = -PWM_RESOLUCION;
  
  // Motor Derecho
  if (Velocidad_motor_der > 0) {
    digitalWrite(PIN_MOTOR_DER_IN1, HIGH); digitalWrite(PIN_MOTOR_DER_IN2, LOW);
    analogWrite(PIN_MOTOR_DER_PWM, Velocidad_motor_der); 
  } else if (Velocidad_motor_der < 0) {
    digitalWrite(PIN_MOTOR_DER_IN1, LOW); digitalWrite(PIN_MOTOR_DER_IN2, HIGH);
    analogWrite(PIN_MOTOR_DER_PWM, -Velocidad_motor_der); 
  } else {
    digitalWrite(PIN_MOTOR_DER_IN1, LOW); digitalWrite(PIN_MOTOR_DER_IN2, LOW);
    analogWrite(PIN_MOTOR_DER_PWM, 0); 
  }

  // Motor Izquierdo
  if (Velocidad_motor_izq > 0) {
    digitalWrite(PIN_MOTOR_IZQ_IN1, HIGH); digitalWrite(PIN_MOTOR_IZQ_IN2, LOW);
    analogWrite(PIN_MOTOR_IZQ_PWM, Velocidad_motor_izq); 
  } else if (Velocidad_motor_izq < 0) {
    digitalWrite(PIN_MOTOR_IZQ_IN1, LOW); digitalWrite(PIN_MOTOR_IZQ_IN2, HIGH);
    analogWrite(PIN_MOTOR_IZQ_PWM, -Velocidad_motor_izq); 
  } else {
    digitalWrite(PIN_MOTOR_IZQ_IN1, LOW); digitalWrite(PIN_MOTOR_IZQ_IN2, LOW);
    analogWrite(PIN_MOTOR_IZQ_PWM, 0); 
  }
}
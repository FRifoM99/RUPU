#include "RUPU_Actuadores.h"

void inicializar_motores() {
  pinMode(AIN1, OUTPUT); 
  pinMode(AIN2, OUTPUT); 
  pinMode(BIN1, OUTPUT); 
  pinMode(BIN2, OUTPUT); 
  
  // ESP32 Core v3.x configuracion nativa
  analogWriteResolution(PWM_A, 10);
  analogWriteResolution(PWM_B, 10);
  analogWriteFrequency(PWM_A, 500);
  analogWriteFrequency(PWM_B, 500);
}

void motor(int Velocidad_motor_izq, int Velocidad_motor_der)
{ 
  if (Velocidad_motor_der >= RESOLUCION_PWM){
    Velocidad_motor_der=RESOLUCION_PWM;  
  }
  else if(Velocidad_motor_der <= -RESOLUCION_PWM){
    Velocidad_motor_der=-RESOLUCION_PWM;
  }
  if (Velocidad_motor_izq >= RESOLUCION_PWM){
    Velocidad_motor_izq=RESOLUCION_PWM;  
  }
  else if(Velocidad_motor_izq <= -RESOLUCION_PWM){
    Velocidad_motor_izq=-RESOLUCION_PWM;
  }

  if (Velocidad_motor_der > 0)
  {
    digitalWrite(AIN1, HIGH); 
    digitalWrite(AIN2, LOW);
    analogWrite(PWM_A, Velocidad_motor_der); 
  }
  else if (Velocidad_motor_der < 0)
  {
    digitalWrite(AIN1, LOW); 
    digitalWrite(AIN2, HIGH);
    analogWrite(PWM_A, -Velocidad_motor_der); 
  }
  else
  {
    digitalWrite(AIN1, LOW); 
    digitalWrite(AIN2, LOW);
    analogWrite(PWM_A, Velocidad_motor_der); 
  }

  if (Velocidad_motor_izq > 0)
  {
    digitalWrite(BIN1, HIGH); 
    digitalWrite(BIN2, LOW);
    analogWrite(PWM_B, Velocidad_motor_izq); 
  }
  else if (Velocidad_motor_izq < 0)
  {
    digitalWrite(BIN1, LOW); 
    digitalWrite(BIN2, HIGH);
    analogWrite(PWM_B, -Velocidad_motor_izq); 
  }
  else
  {
    digitalWrite(BIN1, LOW); 
    digitalWrite(BIN2, LOW);
    analogWrite(PWM_B, Velocidad_motor_izq); 
  }
}

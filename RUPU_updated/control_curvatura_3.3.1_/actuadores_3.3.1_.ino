/*
Funciones para el accionamiento de los motores a través del puente H TB6612FNG
Actualizado para ESP32 Core 3.3.1 usando LEDC en lugar de analogWrite
*/

void motor(int Velocidad_Motor_Izquierdo, int Velocidad_Motor_Derecho)
{

  // Limitar valores al rango [-RESOLUCION_PWM, RESOLUCION_PWM]
  if (Velocidad_Motor_Derecho > RESOLUCION_PWM) Velocidad_Motor_Derecho = RESOLUCION_PWM;
  if (Velocidad_Motor_Derecho < -RESOLUCION_PWM) Velocidad_Motor_Derecho = -RESOLUCION_PWM;
  if (Velocidad_Motor_Izquierdo > RESOLUCION_PWM) Velocidad_Motor_Izquierdo = RESOLUCION_PWM;
  if (Velocidad_Motor_Izquierdo < -RESOLUCION_PWM) Velocidad_Motor_Izquierdo = -RESOLUCION_PWM;

  // Motor derecho  
  if 
  (Velocidad_Motor_Derecho > 0) 
  {
    digitalWrite(PIN_CONTROL_MOTOR_DER_AIN1, HIGH);
    digitalWrite(PIN_CONTROL_MOTOR_DER_AIN2, LOW);
    ledcWrite(PIN_PWM_MOTOR_DER, Velocidad_Motor_Derecho);  // Canal 0 PWM derecho
  } 
  else if (Velocidad_Motor_Derecho < 0) 
  {
    digitalWrite(PIN_CONTROL_MOTOR_DER_AIN1, LOW);
    digitalWrite(PIN_CONTROL_MOTOR_DER_AIN2, HIGH);
    ledcWrite(PIN_PWM_MOTOR_DER, -Velocidad_Motor_Derecho);
  } 
  else 
  {
    digitalWrite(PIN_CONTROL_MOTOR_DER_AIN1, LOW);
    digitalWrite(PIN_CONTROL_MOTOR_DER_AIN2, LOW);
    ledcWrite(PIN_PWM_MOTOR_DER, 0);
  }

  // Motor izquierdo
  if (Velocidad_Motor_Izquierdo > 0) 
  {
    digitalWrite(PIN_CONTROL_MOTOR_IZQ_BIN1, HIGH);
    digitalWrite(PIN_CONTROL_MOTOR_IZQ_BIN2, LOW);
    ledcWrite(PIN_PWM_MOTOR_IZQ, Velocidad_Motor_Izquierdo);  // Canal 1 PWM izquierdo
  } 
  else if (Velocidad_Motor_Izquierdo < 0) 
  {
    digitalWrite(PIN_CONTROL_MOTOR_IZQ_BIN1, LOW);
    digitalWrite(PIN_CONTROL_MOTOR_IZQ_BIN2, HIGH);
    ledcWrite(PIN_PWM_MOTOR_IZQ, -Velocidad_Motor_Izquierdo);
  } 
  else 
  {
    digitalWrite(PIN_CONTROL_MOTOR_IZQ_BIN1, LOW);
    digitalWrite(PIN_CONTROL_MOTOR_IZQ_BIN2, LOW);
    ledcWrite(PIN_PWM_MOTOR_IZQ, 0);
  }
}  


void configurarEncoder() {

  ledcAttach(PIN_PWM_MOTOR_DER, FRECUENCIA_PWM, RESOLUCION_bits);  // Configurar PWM para motor derecho
  ledcAttach(PIN_PWM_MOTOR_IZQ, FRECUENCIA_PWM, RESOLUCION_bits);  // Configurar PWM para motor izquierdo
  ESP32Encoder::useInternalWeakPullResistors = puType::up;; // Configura resistencias de pull-up internas
  encoder_izq.attachFullQuad(5, 23); // Conecta los pines del encoder al ESP32
  encoder_der.attachFullQuad(18, 19);
  encoder_izq.clearCount(); // Inicializa el contador del encoder en 0
  encoder_der.clearCount();
 
}

void configurarDriver() {
  // Configuracion pines de control del driver para el motor
  pinMode(PIN_CONTROL_MOTOR_DER_AIN1, OUTPUT); 
  pinMode(PIN_CONTROL_MOTOR_DER_AIN2, OUTPUT); 
  pinMode(PIN_CONTROL_MOTOR_IZQ_BIN1, OUTPUT); 
  pinMode(PIN_CONTROL_MOTOR_IZQ_BIN2, OUTPUT); 
}
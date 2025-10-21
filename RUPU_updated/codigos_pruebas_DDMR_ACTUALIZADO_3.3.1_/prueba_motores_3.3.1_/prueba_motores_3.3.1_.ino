/*
Este programa permite el accionamiento de los motores a través del puente H TB6612FNG
Actualizado para ESP32 Core 3.3.1 usando LEDC PWM que es la libreria nativa 
Ademas utilizando la consola serial se ingresan los valores velocidad para el motor
izquierdo y derecho con el formato (Vel_IZQ, Vel_DER)
*/

#define PIN_CONTROL_MOTOR_DER_AIN1 16
#define PIN_CONTROL_MOTOR_DER_AIN2 27
#define PIN_PWM_MOTOR_DER          17

#define PIN_CONTROL_MOTOR_IZQ_BIN1 13
#define PIN_CONTROL_MOTOR_IZQ_BIN2 12
#define PIN_PWM_MOTOR_IZQ          14

#define RESOLUCION_PWM 1023  // Rango de PWM (10bits)

int Velocidad_Motor_Izquierdo = 0;
int Velocidad_Motor_Derecho   = 0;

void setup() {
  Serial.begin(115200);

  // Configurar PWM para ambos motores usando ledcAttach(pin, frec, bits)
  ledcAttach(PIN_PWM_MOTOR_DER, 5000, 10);  // Motor derecho, frecuencia 5 kHz, 10 bits
  ledcAttach(PIN_PWM_MOTOR_IZQ, 5000, 10);  // Motor izquierdo
  Serial.print("Ingrese VelmotorIZQ, VelmotorDER");
}

void loop() {
  lectura();
  motor(Velocidad_Motor_Izquierdo, Velocidad_Motor_Derecho);
  // motor(izq, der) en rango [-resolucionPWM, resolucionPWM]
}



void lectura(){
// Leer entrada Serial si hay datos
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    int comaIndex = input.indexOf(',');
    if (comaIndex > 0) {
      Velocidad_Motor_Izquierdo = input.substring(0, comaIndex).toInt();
      Velocidad_Motor_Derecho   = input.substring(comaIndex + 1).toInt();
    }
  }
}

  

#include "config.h"
#include "SimpleUDP.h" // Si usas telemetría, si no, puedes comentarlo

// --- OBJETOS GLOBALES ---
VL53L0X sensor;
ESP32Encoder encoder_der;
ESP32Encoder encoder_izq;

// --- PARÁMETROS PID DE VELOCIDAD ---
// Estos controlan que la rueda gire a los cm/s exactos que le pidas
float KP_VEL = 0.0044; 
float KI_VEL = 0.12;
float KD_VEL = 0.0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // 1. Configuración de Hardware
  configurarEncoders();      
  configurarMUX();           
  configurarSensorToF();    

  Serial.println("--- INICIO DE SISTEMA ---");
  Serial.println("Ponga el robot sobre la línea para calibrar.");
  delay(2000);

  // 2. Calibración de Sensores IR
  // El robot girará sobre su eje para leer Blanco y Negro.
  // Es OBLIGATORIO para que getPosition() funcione bien.
  Serial.println("Calibrando...");
  calibrarSensores(); 
  Serial.println("Calibración Lista.");
  
  // 3. Inicializar temporizadores del PID de velocidad
  iniciarPID(); 
}

void loop() {
  // --- PASO 1: LECTURA DE SENSORES ---
  
  // A) Velocidad Real (cm/s)
  actualizarVelocidades(); 
  float vel_real_izq = getVelocidad(true);
  float vel_real_der = getVelocidad(false);

  // B) Posición de Línea (Error transversal)
  // true = Sensores Frontales
  float error_linea = getPosition(true); 

  // C) Distancia (Obstáculos)
  double distancia = leerDistancia();


  // --- PASO 2: ESTRATEGIA (CONTROL DE MISIÓN) ---
  
  // Definimos la velocidad base a la que queremos viajar (cm/s)
  float velocidad_meta_izq = 10.0; 
  float velocidad_meta_der = 10.0;

  // Lógica de Parada: Si hay obstáculo cerca, frenar.
  if(distancia < 10.0) {
    velocidad_meta_izq = 0;
    velocidad_meta_der = 0;
  }
  
  // NOTA: Aquí podrías agregar un PID de Dirección para seguir la línea.
  // Por ejemplo: 
  // velocidad_meta_izq += error_linea * Kp_linea;
  // velocidad_meta_der -= error_linea * Kp_linea;
  

  // --- PASO 3: CONTROL PID DE MOTORES (Mantiene los cm/s deseados) ---
  
  int pwm_izq = funcion_PID(velocidad_meta_izq, vel_real_izq, pid_izq, KP_VEL, KI_VEL, KD_VEL);
  int pwm_der = funcion_PID(velocidad_meta_der, vel_real_der, pid_der, KP_VEL, KI_VEL, KD_VEL);


  // --- PASO 4: ACTUAR ---
  motor(pwm_izq, pwm_der);


  // --- PASO 5: DEBUG Y SINCRONIZACIÓN ---
  
  // Imprimimos para ver en el Serial Plotter
  // Orden: Posición_Línea, Vel_Izq, Vel_Der
  Serial.print("Linea:"); Serial.print(error_linea);
  Serial.print("\tV_L:"); Serial.print(vel_real_izq);
  Serial.print("\tV_R:"); Serial.println(vel_real_der);

  // Mantiene el bucle a 40ms exactos para que el PID y la integral funcionen bien
  esperarCiclo(); 
}
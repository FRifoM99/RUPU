#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <ESP32Encoder.h>

// --- PINES ---
constexpr int PIN_MUX_S0 = 25;
constexpr int PIN_MUX_S1 = 26;
constexpr int PIN_MUX_S2 = 2;
constexpr int PIN_MUX_S3 = 4;
constexpr int PIN_MUX_SIGNAL = 39;

constexpr int PIN_MOTOR_DER_IN1 = 16;
constexpr int PIN_MOTOR_DER_IN2 = 27;
constexpr int PIN_MOTOR_DER_PWM = 17;
constexpr int PIN_MOTOR_IZQ_IN1 = 13;
constexpr int PIN_MOTOR_IZQ_IN2 = 12;
constexpr int PIN_MOTOR_IZQ_PWM = 14;

constexpr int PIN_ENC_DER_A = 5;
constexpr int PIN_ENC_DER_B = 23;
constexpr int PIN_ENC_IZQ_A = 18; 
constexpr int PIN_ENC_IZQ_B = 19; 

// --- CONSTANTES ---
constexpr float RADIO_RUEDA = 2.15;   
constexpr float DIST_RUEDAS = 15.0;   
constexpr float N_REDUCCION = 100.0;  
constexpr float CPR = 28.0;           
constexpr int LINEA_NEGRA = 1;        
constexpr int UMBRAL_RUIDO_FRONTAL = 30;
constexpr int UMBRAL_RUIDO_POSTERIOR = 10;
constexpr float OFFSET_SENSOR_CM    = 4.0f;
constexpr float DISTANCIA_MAXIMA_CM = 80.0f;
constexpr int PWM_RESOLUCION = 1023;
#define TIEMPO_MUESTREO_PID 40       

// --- OBJETOS ---
extern VL53L0X sensor;         
extern ESP32Encoder encoder_der;
extern ESP32Encoder encoder_izq;

// --- FUNCIONES ---
void motor(int Velocidad_motor_izq, int Velocidad_motor_der);
void configurarEncoders();
float funcion_PID(float referencia_cm_s, float Kp, float Ki, float Kd);
float getVelocidadActualPID(); // <--- NUEVA: Para leer velocidad desde el PID

void configurarMUX();                    
void calibrarSensores();                 
void setMaxMin(int i, float lectura);    
void seleccionarCanalMUX(byte canal);    
float getPosition(bool frontal);         
float calcularNivelSensor(byte index, bool frontal);
// (Calculo_Velocidades y Curvatura ya no son necesarias en el main)

void configurarSensorToF();              
double leerDistancia();
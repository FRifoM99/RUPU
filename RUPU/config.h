#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <ESP32Encoder.h>

// ==========================================
// 1. MAPA DE PINES (HARDWARE)
// ==========================================
// MUX Sensores IR
constexpr int PIN_MUX_S0 = 25;
constexpr int PIN_MUX_S1 = 26;
constexpr int PIN_MUX_S2 = 2;
constexpr int PIN_MUX_S3 = 4;
constexpr int PIN_MUX_SIGNAL = 39;

// Motores (Driver TB6612FNG)
constexpr int PIN_MOTOR_DER_IN1 = 16;
constexpr int PIN_MOTOR_DER_IN2 = 27;
constexpr int PIN_MOTOR_DER_PWM = 17;
constexpr int PIN_MOTOR_IZQ_IN1 = 13;
constexpr int PIN_MOTOR_IZQ_IN2 = 12;
constexpr int PIN_MOTOR_IZQ_PWM = 14;

// Encoders
constexpr int PIN_ENC_DER_A = 5;
constexpr int PIN_ENC_DER_B = 23;
constexpr int PIN_ENC_IZQ_A = 18; 
constexpr int PIN_ENC_IZQ_B = 19; 

// ==========================================
// 2. CONSTANTES FÍSICAS Y CALIBRACIÓN
// ==========================================
// Física del Robot
constexpr float RADIO_RUEDA = 2.15;   
constexpr float DIST_RUEDAS = 15.0;   
constexpr float N_REDUCCION = 100.0;  
constexpr float CPR = 28.0;           
constexpr int LINEA_NEGRA = 1;        

// Parametros Sensor IR
constexpr int UMBRAL_RUIDO_FRONTAL = 30;
constexpr int UMBRAL_RUIDO_POSTERIOR = 10;

// Parametros Sensor ToF
constexpr float OFFSET_SENSOR_CM    = 4.0f;
constexpr float DISTANCIA_MAXIMA_CM = 80.0f;
constexpr float DISTANCIA_MINIMA_CM = 0.0f;

// Control
constexpr int PWM_RESOLUCION = 1023;
#define TIEMPO_MUESTREO_PID 40       

// ==========================================
// 3. OBJETOS GLOBALES (EXTERN)
// ==========================================
extern VL53L0X sensor;         
extern ESP32Encoder encoder_der;
extern ESP32Encoder encoder_izq;

// ==========================================
// 4. PROTOTIPOS DE FUNCIONES
// ==========================================

// --- Módulo Motores ---
void motor(int Velocidad_motor_izq, int Velocidad_motor_der);

// --- Módulo PID ---
void configurarEncoders();
float funcion_PID(float referencia_cm_s, float Kp, float Ki, float Kd);

// --- Módulo Sensores IR ---
void configurarMUX();                    
void calibrarSensores();                 
void setMaxMin(int i, float lectura);    
void seleccionarCanalMUX(byte canal);    
float getPosition(bool frontal);         
float calcularNivelSensor(byte index, bool frontal);
void Calculo_Velocidades();              
void Curvatura_Pista();           
float getVelocidadEnc(bool izquierda);       

// --- Módulo Sensor ToF ---
void configurarSensorToF();              
double leerDistancia();
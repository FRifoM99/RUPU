#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>
#include <ESP32Encoder.h>

// ==========================================
//               DEFINICIÓN DE PINES
// ==========================================

// --- MOTORES ---
constexpr int PIN_MOTOR_DER_IN1 = 16;
constexpr int PIN_MOTOR_DER_IN2 = 27;
constexpr int PIN_MOTOR_DER_PWM = 17;
constexpr int PIN_MOTOR_IZQ_IN1 = 13;
constexpr int PIN_MOTOR_IZQ_IN2 = 12;
constexpr int PIN_MOTOR_IZQ_PWM = 14;

// --- ENCODERS ---
constexpr int PIN_ENC_DER_A = 5;
constexpr int PIN_ENC_DER_B = 23;
constexpr int PIN_ENC_IZQ_A = 18; 
constexpr int PIN_ENC_IZQ_B = 19; 

// --- MUX / SENSORES IR ---
constexpr int PIN_MUX_S0 = 25;
constexpr int PIN_MUX_S1 = 26;
constexpr int PIN_MUX_S2 = 2;
constexpr int PIN_MUX_S3 = 4;
constexpr int PIN_MUX_SIGNAL = 39;

// ==========================================
//           CONSTANTES FÍSICAS
// ==========================================

constexpr float RADIO_RUEDA = 2.15;   // cm
constexpr float DIST_RUEDAS = 15.0;   // cm
constexpr float CPR = 28.0;           // Cuentas motor
constexpr float N_REDUCCION = 100.0;  // Reducción
constexpr int PWM_RESOLUCION = 1023;
constexpr int TIEMPO_MUESTREO_PID = 40; // ms

// Factor de conversión para HalfQuad (1400 pulsos/vuelta)
constexpr float PULSOS_POR_VUELTA = 1400.0; 
constexpr float P_CONST = PULSOS_POR_VUELTA / (2 * PI * RADIO_RUEDA); 

// ==========================================
//          CONSTANTES DE SENSORES
// ==========================================

constexpr int LINEA_NEGRA = 1;
constexpr int UMBRAL_RUIDO_FRONTAL = 30;    
constexpr int UMBRAL_RUIDO_POSTERIOR = 10;  
constexpr float OFFSET_SENSOR_CM    = 4.0f;
constexpr float DISTANCIA_MAXIMA_CM = 80.0f;

// ==========================================
//          ESTRUCTURAS Y OBJETOS
// ==========================================

// Estructura de memoria PID
struct PID_State {
    long ultima_posicion = 0;
    unsigned long ultimo_tiempo = 0;
    float error_1 = 0;
    float error_2 = 0;
    float u_1 = 0;
};

// Objetos Globales
extern VL53L0X sensor;         
extern ESP32Encoder encoder_der;
extern ESP32Encoder encoder_izq;

// Instancias PID (Declaradas extern para limpiar el Main)
extern PID_State pid_izq;
extern PID_State pid_der;

// ==========================================
//               PROTOTIPOS
// ==========================================

// Motores
void motor(int izq, int der);

// Configuración Hardware
void configurarEncoders();
void configurarMUX();                    
void calibrarSensores();                 
void configurarSensorToF();              
double leerDistancia();

// Funciones de Sensores IR (Usadas en Sensores_IR.ino)
void setMaxMin(int i, float lectura);    
void seleccionarCanalMUX(byte canal);    
float getPosition(bool frontal);         
float calcularNivelSensor(byte index, bool frontal);

// Funciones de Control (Usadas en Control_PID.ino)
void iniciarPID();    // Inicializa variables
void esperarCiclo();  // Control de tiempo del bucle
void actualizarVelocidades(); 
float getVelocidad(bool izquierda); 
int funcion_PID(float referencia_cm_s, float medicion_cm_s, PID_State &state, float Kp, float Ki, float Kd);
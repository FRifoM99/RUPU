#ifndef RUPU_CONFIG_H
#define RUPU_CONFIG_H

// --- Configuración RED ---
#define RUPU_SSID "Antenas B343"
#define RUPU_PASSWORD "Lab/Antenas"
#define PUERTO_MONITOREO 1234
#define PUERTO_SUCESOR 1111
#define PUERTO_LOCAL 1111

// --- Pines y Constantes Generales ---
#define PIN_LED 2
#define RADIO_RUEDA 2.15 // RADIO EN [CM]
#define DISTANCIA_L 5.6  // l corresponde a la distancia entre las ruedas en cm
#define REDUCCION_N 0.01 // Modulo de REDUCCION
#define CPR_ENCODER 28   // Cuentas por revolucion encoders
#define CONSTANTE_D1 52
#define CONSTANTE_D2 32

// --- Pines Motor 1 (Derecho) ---
#define AIN1  16
#define AIN2  27
#define PWM_A  17

// --- Pines Motor 2 (Izquierdo) ---
#define BIN1  13
#define BIN2  12
#define PWM_B  14

#define RESOLUCION_PWM 1023

// --- Configuración Sensores de Línea ---
#define VALOR_LINEA 1  // 1: pista blanca, 0: pista negra

// --- Máquina de Estados ---
#define ESTADO_INICIO 0
#define ESTADO_CALIBRACION 1
#define ESTADO_CONTROL_LOOP 2
#define ESTADO_RECONOCIMIENTO 3

#endif // RUPU_CONFIG_H

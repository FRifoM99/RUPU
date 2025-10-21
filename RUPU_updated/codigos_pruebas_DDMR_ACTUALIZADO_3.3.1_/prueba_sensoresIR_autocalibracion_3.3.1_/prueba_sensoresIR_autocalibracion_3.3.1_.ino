/*
Este programa permite la auto-calibracion de los arreglos de sensores IR, y permite la obtencion del angulo de orientacion entre el centro del DDMR y la trayectoria de navegacion
-Requisitos: Libreria Encoder.h <http://www.pjrc.com/teensy/td_libs_Encoder.html>
             Libreria analogWrite.h <https://github.com/ERROPiX/ESP32_AnalogWrite>
             Archivo actuadores.ino
             Archivo sensores.ino
-Cofiguracion: Antes de encender el DDMR desconectar el cable USB o cuidar que este no se enrede (el robot realizara dos vueltas sobre su eje de giro)
               En una zona recta de la trayectoria, posicionar el robot de forma ortogonal, es decir, con sus dos ruedas activas sobre la trayectoria, luego encender el DDMR
               El robot comenzará a calibrarse a medida que realiza los giros, luego se detendra alineado con la trayectoria, finalmente comenzara a imprimir por consola el valor
               del angulo de orientacion medido con el arreglo de sensores frontal y posterior, por lo que es necesario abrir la consola y conectar el robot (USB) de ser necesario.
               Segun el color de la superficie y trayectoria de navegacion a utilzar es necesario modificar el parametro "linea".
*/
#include <ESP32Encoder.h>

// ========== PARAMETROS MOTORES ==============================
/*  Pines canal A driver motor derecho*/
constexpr int PIN_CONTROL_MOTOR_DER_AIN1 = 16;  //Invertir pines de control (ain1, ain2) en caso de que el sentido de giro no corresponda.
constexpr int PIN_CONTROL_MOTOR_DER_AIN2 = 27;
constexpr int PIN_PWM_MOTOR_DER          = 17;

/*  Pines canal B driver motor izquierdo*/
constexpr int PIN_CONTROL_MOTOR_IZQ_BIN1 = 13;  //Invertir pines de control (bin1, bin2) en caso de que el sentido de giro no corresponda.
constexpr int PIN_CONTROL_MOTOR_IZQ_BIN2 = 12;
constexpr int PIN_PWM_MOTOR_IZQ          = 14;


// ========== parametros del mux ====================================================

constexpr int MUX_PIN_S0     = 25;
constexpr int MUX_PIN_S1     = 26;
constexpr int MUX_PIN_S2     = 2;
constexpr int MUX_PIN_S3     = 4;
constexpr int MUX_PIN_SIGNAL = 39;

// =========== parametros fisicos =============================================

constexpr float RADIO_RUEDA = 2.15;   //Radio ruedas en [cm]
constexpr float DIST_RUEDAS  = 5.6;              //Distancia entre las ruedas en [cm]
constexpr float N_REDUCCION  = 0.01;             //Modulo de reduccion caja reductora
constexpr int CPR  = 28;              //Cuentas por revolucion encoders
constexpr float D1 = 52;              //Distancia desde el centro de giro del DDMR al centro del arreglo IR frontal en [mm]
constexpr float D2 = 32;             //Distancia desde el centro de giro del DDMR al centro del arreglo IR posterior en [mm]

constexpr int LINEA_NEGRA = 0;  //Valor logico 1 corresponde a una pista con trayectoria color blanco; Valor lógico 0 corresponde a una pista con trayectoria color negro. 

// ========= ENCODERS ==============================
ESP32Encoder encoder_der;    
ESP32Encoder encoder_izq;   
constexpr int FRECUENCIA_PWM  = 5000;
constexpr int RESOLUCION_bits = 10;  //cantidad de bits para el pwm
constexpr int RESOLUCION_PWM  = (1 << RESOLUCION_bits) - 1; //equivalente a 1023 valores de 10 bits

// ==================================================
String parar = "si"; //Variable que permite saber si el robot se salió de la pista

void setup() {
  Serial.begin(115200);
  configurarMUX();                    
  configurarDriver();
  configurarEncoder();
  calibrarSensores();      //Funcion que calibra los sensores IR

}

void loop() {
  float angulo_frontal   = getposition(1)/D1;
  float angulo_posterior = -getposition(0)/D2;  

  //Serial.print(angulo_frontal);    //Imprime en pantalla una aproximación del angulo de orientacion del DDMR medido con el arreglo frontal en [rad] : rango +- 0.54[rad]
  //Serial.print("   ---   ");
  //Serial.println(angulo_posterior); //Imprime en pantalla una aproximación del angulo de orientacion del DDMR medido con el arreglo posterior en [rad] : rango +- 0.88[rad]
}

void configurarMUX() {
//configura los pines de control del mux
  pinMode(MUX_PIN_S0, OUTPUT);     //PIN S0
  pinMode(MUX_PIN_S1, OUTPUT);     //PIN S1
  pinMode(MUX_PIN_S2, OUTPUT);      //PIN S2
  pinMode(MUX_PIN_S3, OUTPUT);      //PIN S3
}

void configurarDriver() {
// Configuracion pines de control del driver para el motor
  pinMode(PIN_CONTROL_MOTOR_DER_AIN1, OUTPUT); 
  pinMode(PIN_CONTROL_MOTOR_DER_AIN2, OUTPUT); 
  pinMode(PIN_CONTROL_MOTOR_IZQ_BIN1, OUTPUT); 
  pinMode(PIN_CONTROL_MOTOR_IZQ_BIN2, OUTPUT); 
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


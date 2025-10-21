#include <esp_wifi.h> //
#include <WiFi.h> //se mantiene y se actualiza
//#include <analogWrite.h> //ESP32 LEDC ahora es nativa, no se ocupa analogWrite
//#include <Encoder.h> //se reemplazada por la version compatible ESP32Encoder.h
#include <ESP32Encoder.h>
#include <Wire.h> //I2C se mantiene
#include <VL53L0X.h> // se mantiene


///     WIFI
///////////////////////////////////////
// ======================= Configuración de RED ==========================================

// Credenciales Wi-Fi
constexpr char WIFI_SSID[]      = "iPhone de Kevin";       // Nombre de la red Wi-Fi
constexpr char WIFI_PASSWORD[]  = "kevin123";      // Contraseña de la red

// Configuración del robot de monitoreo
constexpr char MONITOR_IP[]     = "192.168.137.1"; // IP del dispositivo que supervisa los datos
constexpr uint16_t MONITOR_PORT  = 1234;           // Puerto de envío UDP de monitoreo

// Configuración del robot sucesor
constexpr char SUCCESSOR_IP[]    = "192.168.1.6";  // IP del robot sucesor
constexpr uint16_t SUCCESSOR_PORT = 1111;          // Puerto del robot sucesor

// Configuración del puerto local del robot
constexpr uint16_t LOCAL_UDP_PORT = 1111;         // Puerto local para recibir paquetes UDP


// === PARAMETROS FISICOS DEL RUPU ==========================================
constexpr char EtiquetaRobot = 'L';  // "L": Robot0 (lider)   "R": robot1    "S": robot2    "T": Robot3    "O": Robot4 
constexpr float RADIO_RUEDA = 2.15;   //Radio ruedas en [cm]
constexpr float DIST_RUEDAS  = 5.6;   //Distancia entre las ruedas en [cm]
constexpr float N_REDUCCION  = 0.01;  //Modulo de reduccion caja reductora
constexpr uint8_t CPR  = 28;              //Cuentas por revolucion encoders
constexpr float D1 = 52;              //Distancia desde el centro de giro del DDMR al centro del arreglo IR frontal en [mm]
constexpr float D2 = 32;              //Distancia desde el centro de giro del DDMR al centro del arreglo IR posterior en [mm]
constexpr uint8_t LINEA_NEGRA = 0;       //Valor logico 1 corresponde a una pista con trayectoria color blanco; Valor lógico 0 corresponde a una pista con trayectoria color negro. 

// ========= ENCODERS ==============================
ESP32Encoder encoder_der;    
ESP32Encoder encoder_izq;   
constexpr int FRECUENCIA_PWM  = 5000;
constexpr uint8_t RESOLUCION_bits = 10;  //cantidad de bits para el pwm
constexpr int RESOLUCION_PWM  = (1 << RESOLUCION_bits) - 1; //equivalente a 1023 valores de 10 bits


// ========== PARAMETROS MOTORES ==============================
/*  Pines canal A driver motor derecho*/
constexpr uint8_t PIN_CONTROL_MOTOR_DER_AIN1 = 16;  //Invertir pines de control (ain1, ain2) en caso de que el sentido de giro no corresponda.
constexpr uint8_t PIN_CONTROL_MOTOR_DER_AIN2 = 27;
constexpr uint8_t PIN_PWM_MOTOR_DER          = 17;

/*  Pines canal B driver motor izquierdo*/
constexpr uint8_t PIN_CONTROL_MOTOR_IZQ_BIN1 = 13;  //Invertir pines de control (bin1, bin2) en caso de que el sentido de giro no corresponda.
constexpr uint8_t PIN_CONTROL_MOTOR_IZQ_BIN2 = 12;
constexpr uint8_t PIN_PWM_MOTOR_IZQ          = 14;

// ========== parametros del mux ====================================================

constexpr uint8_t MUX_PIN_S0     = 25;
constexpr uint8_t MUX_PIN_S1     = 26;
constexpr uint8_t MUX_PIN_S2     = 2;
constexpr uint8_t MUX_PIN_S3     = 4;
constexpr uint8_t MUX_PIN_SIGNAL = 39;

// ======= Sensor ToF =================

constexpr float  OFFSET_SENSOR_CM    = 4.0f;  //ajusta el offset del sensor para que la medicion sea certera
constexpr float  DISTANCIA_MAXIMA_CM = 80.0f; //saturación en la medicion
constexpr float  DISTANCIA_MINIMA_CM = 0.0f;  // Distancia minima
// no se ocupan en este codigo, pero si en el codigo de pruebas del sensor ToF
//constexpr int     FILTRO_MUESTRAS    = 200;   // numero de repeticiones para filtrar muestras validas
//int               contadorFiltro     =  0;    //Variable que cuenta las veces que se repite una medición para discriminar su validez    
VL53L0X sensor;

// ========= LECTURA ADC =====================
constexpr uint8_t ADC_PIN_BATERIA = 36;
constexpr float Rtop = 9920.0;       // Valor medido
constexpr float Rbot = 5080.0;       // Valor medido
constexpr float divider_ratio = Rbot / (Rtop + Rbot); // 0.339 aprox.

const unsigned long ADC_INTERVAL = 100; // ms entre lecturas
unsigned long t_adc = 0;
bool LOW_BATTERY = false;
constexpr float UMBRAL_BATERIA_BAJA = 7.4;

//=========== MAQUINA DE ESTADOS =================================
#define inicio 0
#define calibracion 1
#define controlLoop 2

byte estado=0;
byte estado_siguiente=0;
byte calibrar = 0;
String parar = "si"; //permite saber si el robot se salió de la pista
bool flag_parar = true;  // false = "no", true = "si"


/////////////////////////////////////////////////////////////// VARIABLES CONTROLADORES PID
//Define Variables we'll be connecting to PID de posicion
double Input_theta, Output_theta;
double theta_ref = 0;
//Define the aggressive and conservative Tuning Parameters
/*PID posición*/
double Kp_theta = 2000;//64.078;//147.657974619537;//60;
double Ki_theta = 4500;//145.3619;//555.734047342947;//150;
double Kd_theta = 60;//7.0616;//8.30454187876464;//1;

//Define Variables we'll be connecting to PID de Velocidad
double Input_vel, Output_vel;
double Output_vel_ant = 0;
double vel_ref = 0;
double sp_vel = 0;
double vel_crucero = 15;

//Define the aggressive and conservative Tuning Parameters
/*PID velocidad*/
double Kp_vel = 20.3;//100;//49.9757241214599;//130;
double Ki_vel = 145.3;//282.271150682254;//130;
double Kd_vel = 0;//0.197722771627088;//0;
 
//Define Variables we'll be connecting to PID de distancia
double Input_d, Output_d;
double d_ref = 10;
double error_d;
float delta = 0.1;

//Define the aggressive and conservative Tuning Parameters
/*PID velocidad*/
double Kp_d = 4;//1.5;//8.0013;
double Ki_d = 3;//11;//5.6025;
double Kd_d = 0;//0.1;//1.0077;
//Specify the links and initial tuning parameters
String PID_theta="MANUAL";
double sat_theta=1023;
double error_ant_theta=0;
double integral_theta=0;
String PID_vel="MANUAL";
double sat_vel=800;
double error_ant_vel=0;
double integral_vel=0;
String PID_d="MANUAL";
double sat_d=30;
double error_ant_d=0;
double integral_d=0;
/////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////// VARIABLES UDP
WiFiUDP udp;
String cadena;
char msg[128];
char paquete_entrante[64];  // buffer for incoming packets
unsigned long t_com_predecesor, t_monitoreo;
////////////////////////////////////////////////////////////

unsigned long t_actual;     //TIMER GENERAL
unsigned long t_controlador;
unsigned long t_led;

constexpr int LED_TIME = 500; //ms para el contador del led 
bool LED_STATE = false;


//////////SENSORES
double ang_vel;
double v_r[] = {0, 0, 0, 0, 0};
double v_l[] = {0, 0, 0, 0, 0};
double a_sl=0;
double a_sr=0;
double b_sl=0;
double b_sr=0;
double c_sl=0;
double c_sr=0;
double a_s=0;
double b_s=0;
double c_s=0;
unsigned long t_arco;
byte recta=0;
double a_curvatura=0;
double b_curvatura=0;
double c_curvatura=0;
double curvatura=0;
double curvatura_predecesor=0;
unsigned long t_svel;
double cuenta = 0;




byte control = 1;

void setup()
{
  Serial.begin(115200);
  Serial.println("setup!");
  RUPU_INIT_CONFIG();

  //se puede hacer una funcion CONFIGURAR_RUPU, donde se hagan todas estas configuraciones y limpiar visualmente el void setup
}

void loop(){
  Actualizar_Flag_bateria();
  Actualizar_Maquina_Estado();
  //Serial.print("ESTADO   :");
  //Serial.print(estado);
  //Serial.print(" calibrar   :");
  //Serial.print(calibrar);
  //Serial.print(" parar   :");
  //Serial.print(parar);

}

void Actualizar_Maquina_Estado() {
  switch (estado) { 
    //Serial.println(estado);
    case inicio:
      ciclo_de_inicio();
      break;

    case calibracion:
      calibrar=0;
      calibrarSensores();
      //Serial.println(" sali de calibrar");
      break;

    case controlLoop:
      Serial.println(" Ciclo de control");
      ciclo_de_control();
      break; 

    default:
      ciclo_de_inicio();
      break;
  }
  //========== transiciones =================
  //estado_siguiente = estado;
  //Serial.print(LOW_BATTERY);
//  if(LOW_BATTERY) {
//  estado_siguiente = inicio;
//  unsigned long now_time = millis();
//  if(now_time - t_led >= LED_TIME){
//    LED_STATE = !LED_STATE;
//    digitalWrite(MUX_PIN_S2, LED_STATE ? HIGH : LOW);
//    t_led = now_time;
//  }
//}

  if(estado==inicio && calibrar){
    estado_siguiente=calibracion;
    Serial.print(" calibracion1");
  }

  else if(estado==inicio && flag_parar){
    estado_siguiente=inicio;
    Serial.print(" inicio1");
  }
  else if(estado==inicio && !flag_parar){
    estado_siguiente=controlLoop;
    Serial.print(" control loop1");
  }
  else if(estado==calibracion){
    estado_siguiente=inicio;
    Serial.print(" inicio2");
  }
  else if(estado==controlLoop && !flag_parar){
    estado_siguiente=controlLoop;
    Serial.print(" control loop 2");
  }
  else if(estado==controlLoop && flag_parar){
    estado_siguiente=inicio;
    Serial.print(" inicio 3");
  }
  //estado = estado_siguiente;
  else {
    estado_siguiente = inicio; 
  }
  estado = estado_siguiente;

}


void ciclo_de_inicio(){
  udp_recep();
  PID_theta="MANUAL";
  PID_vel="MANUAL";
  PID_d="MANUAL";
  error_ant_d=0;
  integral_d=0;
  error_ant_vel=0;
  integral_vel=0;
  error_ant_theta=0;
  integral_theta=0;
  Output_d=0;
  Output_vel=0;
  Output_theta=0;
  d_ref = leerDistancia(sensor);
  sp_vel = 0;
  Input_vel=0;
  vel_ref=0;
  //actualizar
  motor(0,0);
  encoder_der.clearCount();
  encoder_izq.clearCount();
  udp_transm();
}

void ciclo_de_control(){
  udp_recep();
  Input_d = leerDistancia(sensor);
  if(EtiquetaRobot!='L'){
    error_d = Input_d - d_ref;
  }
  else{
    error_d=0;
  }
  Calculo_Velocidades();
  Curvatura_Pista();
  //control=((curvatura_predecesor <= 0.01) && (curvatura <= 0.01));      // HABILITA ALGORITMO DE SWITCHEO CURVATURA
  control=1;                                                              // DESAVILITA ALGORITMO CURVATURA
  if(Output_vel<0){
    Input_theta=-getposition(0)/D2;
  }
  else{
    Input_theta=getposition(1)/D1;
  }
  if ((abs(error_d) >= delta + 1) || EtiquetaRobot=='L') {
    PID_theta="AUTO";
    PID_vel="AUTO";
    if(control){
      PID_d="AUTO";
    }
    else{
      PID_d="MANUAL";
      Output_d=vel_crucero;
      error_ant_d=0;
      integral_d=vel_crucero/Ki_d - error_d*0.04;
    }
    if(EtiquetaRobot=='L'){
      PID_d="MANUAL"; 
      Output_d = 0;
      error_ant_d=0;
      integral_d=0;
      error_d =0;
    }
  }
  if (abs(Input_vel) <= 6 && abs(Input_theta) <= 0.015 && ((abs(error_d) < delta) && (sp_vel==0 || EtiquetaRobot!='L'))) {
    PID_d="MANUAL";
    PID_vel="MANUAL";
    PID_theta="MANUAL";
    Output_d = 0;
    Output_vel = 0;
    Output_theta = 0;
    error_ant_d=0;
    integral_d=0;
    error_ant_vel=0;
    integral_vel=0;
    error_ant_theta=0;
    integral_theta=0;
  }
  udp_transm();                                                   //HABILITA LA TRANSMSION DE DATOS ENTRE ROBOTS
  t_actual=millis()-t_controlador;
  if(t_actual>=40){
    //Output_d=calculoPID(Input_d, d_ref,error_ant_d, integral_d, Kp_d, Ki_d, Kd_d, sat_d, PID_d, Output_d, "INVERSO");     //HABILITAR PARA CONTROLADOR PID SIN COMUNICACION Y SWITCHEO CURVATURA
    Output_d=calculoPIDd(Input_d, d_ref,error_ant_d, integral_d, Kp_d, Ki_d, Kd_d, sat_d, PID_d, Output_d, "INVERSO"); //HABILITA CALCULO DE PID CON SATURACION PREDECESOR
    vel_ref= Output_d *(EtiquetaRobot!='L') + (EtiquetaRobot=='L')*sp_vel;
    Output_vel=calculoPID(Input_vel, vel_ref, error_ant_vel, integral_vel, Kp_vel, Ki_vel, Kd_vel, sat_vel, PID_vel, Output_vel, "DIRECTO");
    Serial.println(Output_vel);
    Output_theta=calculoPID(Input_theta, theta_ref, error_ant_theta, integral_theta, Kp_theta, Ki_theta, Kd_theta, sat_theta, PID_theta, Output_theta, "DIRECTO");
    udp_monitor();
    t_controlador=millis();
  }
  motor(Output_vel - Output_theta, Output_vel + Output_theta);
}

void RUPU_INIT_CONFIG(){
  WiFi.mode(WIFI_STA);
  esp_wifi_set_ps(WIFI_PS_NONE);
  setup_wifi(); // ver abajo
  configurarEncoder();
  configurarDriver();
  configurarMUX();
  configurarSensorToF(sensor);
  Configurar_ADC();
}





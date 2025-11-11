// Las librerías de WiFi ya no son necesarias aquí --- fueron movidas a la libreria ""ComunicacionRobot.h""
 #include <esp_wifi.h> 
// #include <WiFi.h>     
#include "ComunicacionRobot.h"
#include <ESP32Encoder.h>
#include <Wire.h> 
#include <VL53L0X.h>


///     WIFI
// ======================= Configuración de RED ==========================================

// Credenciales Wi-Fi
constexpr char WIFI_SSID[]      = "iPhone de Kevin"; 
constexpr char WIFI_PASSWORD[]  = "kevin123";        

// Configuración del robot de monitoreo
constexpr char MONITOR_IP[]     = "192.168.137.1";   
constexpr uint16_t MONITOR_PORT  = 1234;             

// Configuración del robot sucesor
constexpr char SUCCESSOR_IP[]    = "192.168.1.6";   
constexpr uint16_t SUCCESSOR_PORT = 1111;            

// Configuración del puerto local del robot
constexpr uint16_t LOCAL_UDP_PORT = 1111;           


// === PARAMETROS FISICOS DEL RUPU ==========================================

constexpr char EtiquetaRobot = 'L';   
constexpr float RADIO_RUEDA = 2.15;   
constexpr float DIST_RUEDAS  = 5.6; 
constexpr float N_REDUCCION  = 0.01;  
constexpr uint8_t CPR  = 28; 
constexpr float D1 = 52;              
constexpr float D2 = 32;              
constexpr uint8_t LINEA_NEGRA = 0;    

// ========= ENCODERS ==============================

ESP32Encoder encoder_der;    
ESP32Encoder encoder_izq;                                   
constexpr int FRECUENCIA_PWM  = 5000;                       
constexpr uint8_t RESOLUCION_bits = 10;                     
constexpr int RESOLUCION_PWM  = (1 << RESOLUCION_bits) - 1; 


// ========== PARAMETROS MOTORES ==============================

constexpr uint8_t PIN_CONTROL_MOTOR_DER_AIN1 = 16; 
constexpr uint8_t PIN_CONTROL_MOTOR_DER_AIN2 = 27; 
constexpr uint8_t PIN_PWM_MOTOR_DER          = 17; 
constexpr uint8_t PIN_CONTROL_MOTOR_IZQ_BIN1 = 13; 
constexpr uint8_t PIN_CONTROL_MOTOR_IZQ_BIN2 = 12; 
constexpr uint8_t PIN_PWM_MOTOR_IZQ          = 14; 

// ========== parametros del mux ====================================================

constexpr uint8_t MUX_PIN_S0     = 25; 
constexpr uint8_t MUX_PIN_S1     = 26; 
constexpr uint8_t MUX_PIN_S2     = 2;  
constexpr uint8_t MUX_PIN_S3     = 4;  
constexpr uint8_t MUX_PIN_SIGNAL = 39; 

// ======= Sensor ToF =================

constexpr float  OFFSET_SENSOR_CM    = 4.0f;    
constexpr float  DISTANCIA_MAXIMA_CM = 80.0f;   
constexpr float  DISTANCIA_MINIMA_CM = 0.0f;
VL53L0X sensor;                                 

// ========= LECTURA ADC =====================

constexpr uint8_t ADC_PIN_BATERIA = 36;               
constexpr float Rtop = 9920.0;                        
constexpr float Rbot = 5080.0;                        
constexpr float divider_ratio = Rbot / (Rtop + Rbot); 
const unsigned long ADC_INTERVAL = 100;               
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
String parar = "si"; 
bool flag_parar = true; 


/////////////////////////////////////////////////////////////// VARIABLES CONTROLADORES PID

double Input_theta, Output_theta;      
double theta_ref = 0;                  
double Kp_theta = 2000;
double Ki_theta = 4500;
double Kd_theta = 60;                  

double Input_vel, Output_vel;          
double Output_vel_ant = 0;             
double vel_ref = 0;                    
double sp_vel = 0;                     
double vel_crucero = 15;               
double Kp_vel = 20.3;
double Ki_vel = 145.3;
double Kd_vel = 0;                     

double Input_d, Output_d;              
double d_ref = 10;                     
double error_d;                        
float delta = 0.1;                     
double Kp_d = 4;
double Ki_d = 3;
double Kd_d = 0;                       

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

// --- CAMBIO: Crear el objeto de nuestra librería ---
ComunicacionRobot miRobot;

// --- CAMBIO: Eliminar las variables UDP antiguas ---
// WiFiUDP udp; (Movido a la librería)
// String cadena; (Movido a la librería)
// char msg[128]; (Movido a la librería)
// char paquete_entrante[64]; (Movido a la librería)


unsigned long t_com_predecesor; 
////////////////////////////////////////////////////////////

unsigned long t_actual;     //TIMER GENERAL 
unsigned long t_controlador; 
unsigned long t_led; 

constexpr int LED_TIME = 500; 
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
}

void loop(){
  Actualizar_Flag_bateria();    
  Actualizar_Maquina_Estado();  
}

void Actualizar_Maquina_Estado() {
  
  switch (estado) { 
    case inicio:
      ciclo_de_inicio(); 
      break;

    case calibracion:
      calibrar=0;
      calibrarSensores(); 
      break;

    case controlLoop:
      // Serial.println(" Ciclo de control"); 
      ciclo_de_control(); 
      break; 

    default:
      ciclo_de_inicio(); 
      break;
  }
  //========== transiciones =================
  if(estado==inicio && calibrar){ 
    estado_siguiente=calibracion;
  }
  else if(estado==inicio && flag_parar){ 
    estado_siguiente=inicio;
  }
  else if(estado==inicio && !flag_parar){ 
    estado_siguiente=controlLoop;
  }
  else if(estado==calibracion){
    estado_siguiente=inicio;
  }
  else if(estado==controlLoop && !flag_parar){ 
    estado_siguiente=controlLoop;
  }
  else if(estado==controlLoop && flag_parar){ 
    estado_siguiente=inicio;
  }
  else {
    estado_siguiente = inicio; 
  }
  estado = estado_siguiente; 
}


void ciclo_de_inicio(){
  // --- CAMBIO: Reemplazar udp_recep() con su equivalente ---
  miRobot.revisarMensajes();

  // --- CAMBIO: Añadir lógica para actuar según los mensajes ---
  if (miRobot.hayMensajeNuevoDelComputador()) {
    // Actualizamos las variables globales con los settings
    flag_parar = miRobot.getSetting_Parar();
    calibrar = (byte)miRobot.getSetting_Calibrar();
    sp_vel = miRobot.getSetting_VelocidadReferencia();
  }

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
  motor(0,0); 
  encoder_der.clearCount();
  encoder_izq.clearCount();

  // --- CAMBIO: Reemplazar udp_transm() con su lógica ---
  t_actual = millis() - t_com_predecesor;
  if (t_actual >= 100) {
    miRobot.enviarEstadoAlRobotSiguiente(flag_parar, vel_ref, curvatura);
    t_com_predecesor = millis();
  }
}

void ciclo_de_control(){
  // --- CAMBIO: Reemplazar udp_recep() con su equivalente ---
  miRobot.revisarMensajes(); 

  // --- CAMBIO: Revisar si llegaron datos nuevos ---
  if (miRobot.hayMensajeNuevoDeAdelante()) {
    // Actualizar variables con datos del robot predecesor
    curvatura_predecesor = miRobot.getCurvaturaDeAdelante(); 
    
    // Lógica de saturación que estaba en `estado_predecesor` 
    sat_d = miRobot.getVelocidadRefDeAdelante();
  }

  if (miRobot.hayMensajeNuevoDelComputador()) {
    // Actualizar todas las variables globales de los PID y control
    Kp_theta = miRobot.getSetting_Kp_angulo();
    Ki_theta = miRobot.getSetting_Ki_angulo();
    Kd_theta = miRobot.getSetting_Kd_angulo();
    Kp_vel = miRobot.getSetting_Kp_velocidad();
    Ki_vel = miRobot.getSetting_Ki_velocidad();
    Kd_vel = miRobot.getSetting_Kd_velocidad();
    sp_vel = miRobot.getSetting_VelocidadReferencia();
    Kp_d = miRobot.getSetting_Kp_distancia();
    Ki_d = miRobot.getSetting_Ki_distancia();
    Kd_d = miRobot.getSetting_Kd_distancia();
    delta = miRobot.getSetting_Distancia_Delta();
    d_ref = miRobot.getSetting_Distancia_Referencia();
    calibrar = (byte)miRobot.getSetting_Calibrar();
    flag_parar = miRobot.getSetting_Parar();
  }

  Input_d = leerDistancia(sensor); 
  if(EtiquetaRobot!='L'){
    error_d = Input_d - d_ref; 
  }
  else{
    error_d=0; 
  }
  Calculo_Velocidades(); 
  Curvatura_Pista(); 
  control=1; 
  
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

  // --- CAMBIO: Reemplazar udp_transm() con su lógica ---
  t_actual = millis() - t_com_predecesor;
  if (t_actual >= 100) {
    miRobot.enviarEstadoAlRobotSiguiente(flag_parar, vel_ref, curvatura);
    t_com_predecesor = millis();
  }
  
  t_actual=millis()-t_controlador; 
  if(t_actual>=40){ 
    Output_d=calculoPIDd(Input_d, d_ref,error_ant_d, integral_d, Kp_d, Ki_d, Kd_d, sat_d, PID_d, Output_d, "INVERSO"); 
    vel_ref= Output_d *(EtiquetaRobot!='L') + (EtiquetaRobot=='L')*sp_vel; 
    Output_vel=calculoPID(Input_vel, vel_ref, error_ant_vel, integral_vel, Kp_vel, Ki_vel, Kd_vel, sat_vel, PID_vel, Output_vel, "DIRECTO"); 
    Output_theta=calculoPID(Input_theta, theta_ref, error_ant_theta, integral_theta, Kp_theta, Ki_theta, Kd_theta, sat_theta, PID_theta, Output_theta, "DIRECTO"); 
    
    // --- CAMBIO: Reemplazar udp_monitor() con su equivalente ---
    miRobot.enviarTelemetria(EtiquetaRobot, t_actual, Input_d, d_ref, vel_ref, Input_vel, Input_theta, Output_d, Output_vel, Output_theta, curvatura, vel_crucero, curvatura_predecesor, control);
    
    t_controlador=millis(); 
  }
  motor(Output_vel - Output_theta, Output_vel + Output_theta); 
}

void RUPU_INIT_CONFIG(){
  WiFi.mode(WIFI_STA);
  esp_wifi_set_ps(WIFI_PS_NONE);

  // --- CAMBIO: Reemplazar setup_wifi() con la librería ---
  miRobot.iniciar(WIFI_SSID, WIFI_PASSWORD, LOCAL_UDP_PORT);
  miRobot.configurarDestinos(MONITOR_IP, MONITOR_PORT, SUCCESSOR_IP, SUCCESSOR_PORT);
  
  configurarEncoder();                    
  configurarDriver();                     
  configurarMUX();                        
  configurarSensorToF(sensor);            
  Configurar_ADC();                       
}
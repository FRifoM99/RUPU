#include "config.h"
#include "SimpleUDP.h" 
// --- CONFIGURACIÓN DE RED ---
const char* WIFI_SSID = "Galaxy";  
const char* WIFI_PASS = "fzlr8665";  
const int   MI_PUERTO = 5005;            

String IP_MONITOR = "223.48.169.85";       
const int PUERTO_DESTINO = 1234;        

// --- INSTANCIACIÓN DE OBJETOS ---
VL53L0X sensor;
ESP32Encoder encoder_der;
ESP32Encoder encoder_izq;

void setup() {
  Serial.begin(115200);
  Wire.begin(); 

  conectarWifi(WIFI_SSID, WIFI_PASS);
  iniciarUDP(MI_PUERTO);

  configurarEncoders();      
  configurarMUX();           
  configurarSensorToF();    

  Serial.println("Robot Iniciado. Calibrando...");
  delay(1000);
  
  // Calibración
  calibrarSensores();
  Serial.println("Calibración Lista.");
}

void loop() {
  String mensaje = recibirMensaje(IP_MONITOR);
  if (mensaje != "") {
    Serial.println("Orden recibida: " + mensaje);
  }

  // Lectura de Sensores
  float posicion = getPosition(true); 
  double distancia = leerDistancia();
  
  //  Control
  float control_pid = funcion_PID(15.0, 1, 0.1, 0.0);

  // Lógica
  if(distancia < 10.0) {
    motor(0, 0);a
  } else {
    motor((int)control_pid , -(int)control_pid);
  }
  
  // Odometría
  Calculo_Velocidades();
  Curvatura_Pista();
  
  static unsigned long t_envio = 0;
  if (millis() - t_envio > 100) {
    t_envio = millis();
    
    String reporte = String(distancia) + "," + String(getVelocidadEnc(false));
    
    enviarMensaje(IP_MONITOR, PUERTO_DESTINO, reporte);
  }

  delay(1);
}
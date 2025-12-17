#include "config.h"
#include "SimpleUDP.h"

// --- RED ---
const char* WIFI_SSID = "VTR-1335633";   
const char* WIFI_PASS = "nrxtqvWgh4gg";  
const int   MI_PUERTO = 5005;            
String IP_MONITOR     = "192.168.1.5";   
const int PUERTO_DESTINO = 1234;         

// --- OBJETOS ---
VL53L0X sensor;
ESP32Encoder encoder_der;
ESP32Encoder encoder_izq;

void setup() {
  Serial.begin(115200);
  Wire.begin(); 

  // 1. Comunicación
  conectarWifi(WIFI_SSID, WIFI_PASS);
  iniciarUDP(MI_PUERTO);

  // 2. Hardware
  configurarEncoders();      
  configurarMUX();           
  configurarSensorToF();    

  Serial.println("Robot Iniciado. Calibrando...");
  delay(1000);
  calibrarSensores();
  Serial.println("Calibración Lista.");
}

void loop() {
  // Comunicación RX
  String mensaje = recibirMensaje(IP_MONITOR);
  if (mensaje != "") Serial.println("RX: " + mensaje);

  // 1. Sensores
  float posicion = getPosition(true); 
  double distancia = leerDistancia();
  
  // 2. PID (Aquí se calcula la velocidad internamente)
  float control_pid = funcion_PID(15.0, 1, 0.1, 0.0);

  // 3. Motores
  if(distancia < 10.0) {
    motor(0, 0);
  } else {
    motor((int)control_pid , -(int)control_pid);
  }
  
  // 4. Envío de Telemetría (10 Hz)
  static unsigned long t_envio = 0;
  if (millis() - t_envio > 100) {
    t_envio = millis();
    
    // Leemos la velocidad DIRECTAMENTE del PID
    float velocidad_real = getVelocidadActualPID();

    String reporte = String(distancia) + "," + String(velocidad_real);
    enviarMensaje(IP_MONITOR, PUERTO_DESTINO, reporte);
  }

  delay(1);
}
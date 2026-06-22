#include <esp_wifi.h>
#include <RUPU.h>

void setup() {
  Serial.begin(115200);
  Serial.println("setup!");
  WiFi.mode(WIFI_STA);
  esp_wifi_set_ps(WIFI_PS_NONE);
  setup_wifi(); 
  inicializar_sensores();
  inicializar_motores();
}

void loop() {
  switch (estado) {
    case ESTADO_INICIO:
      ciclo_de_inicio();
      break;
    case ESTADO_CALIBRACION:
      calibrarSensores();
      calibrar = 0;
      break;
    case ESTADO_CONTROL_LOOP:
      ciclo_de_control();
      break;  
    case ESTADO_RECONOCIMIENTO:
      ciclo_reconocimiento();
      break;
    default:
      ciclo_de_inicio();
      break;
  }
  
  // transiciones
  if(estado==ESTADO_INICIO && calibrar){
    estado_siguiente=ESTADO_CALIBRACION;
  }
  else if (estado == ESTADO_INICIO && disparo_sensor_boton() && !ack_recibido_recon) {
    estado_siguiente = ESTADO_RECONOCIMIENTO;             
  }
  else if (estado == ESTADO_RECONOCIMIENTO && !ack_recibido_recon) {
    estado_siguiente = ESTADO_RECONOCIMIENTO;             
  }
  else if (estado == ESTADO_RECONOCIMIENTO && ack_recibido_recon) {
    estado_siguiente = ESTADO_INICIO;                     
  }
  else if(estado==ESTADO_INICIO && parar=="si"){
    estado_siguiente=ESTADO_INICIO;
  }
  else if(estado==ESTADO_INICIO && parar=="no"){
    estado_siguiente=ESTADO_CONTROL_LOOP;
  }
  else if(estado==ESTADO_CALIBRACION){
    estado_siguiente=ESTADO_INICIO;
  }
  else if(estado==ESTADO_CONTROL_LOOP && parar=="no"){
    estado_siguiente=ESTADO_CONTROL_LOOP;
  }
  else if(estado==ESTADO_CONTROL_LOOP && parar=="si"){
    estado_siguiente=ESTADO_INICIO;
  }
  else{
    estado_siguiente=ESTADO_INICIO;
  }
  estado=estado_siguiente;
}

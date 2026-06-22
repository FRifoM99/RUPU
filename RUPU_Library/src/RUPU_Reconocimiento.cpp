#include "RUPU_Reconocimiento.h"

void ciclo_reconocimiento()
{
  udp_recep(); 

  if (ack_recibido_recon) {
    digitalWrite(PIN_LED, LOW);
  }
  
  // Enviamos el broadcast y printeamos cada 1 segundo (1000ms) para evitar saturar el router (Spam Protection) y el Serial
  if (millis() - t_last_hello >= 1000) {
    t_last_hello = millis();
    
    Serial.println("Datos a actualizar mediante GUI:");
    Serial.println(IP_monitoreo);
    Serial.println(EtiquetaRobot);
    
    sendHELLO();   
  }
}

void sendHELLO() {
  // En redes modernas o de universidades, el broadcast global 255.255.255.255 suele ser bloqueado.
  // Es mucho más seguro usar el broadcast de la subred local (ej: 192.168.1.255)
  IPAddress bcast(255, 255, 255, 255);
  udp.beginPacket(bcast, REG_PORT);
  udp.print("RUPU/HELLO");
  udp.endPacket();
}

bool disparo_sensor_boton() {
  double d = distancia();  
  switch (estado) {
    case ESTADO_INICIO:
      if (d < 20) { 
        if (t_hold_start == 0) 
        {
          t_hold_start = millis();
        }
        if (millis() - t_hold_start >= 2000)
        {
          t_hold_start = 0;
          digitalWrite(PIN_LED, HIGH); 
          return true;  // disparo
        }
      } 
      else {
        t_hold_start = 0;  // cancelado
      }
      break;
    default:
      t_hold_start = 0;
      break;
  }
  return false;
}

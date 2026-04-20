//Funciones utilizadas en estado de reconocimiento 
void ciclo_reconocimiento()
{
  udp_recep();  // <- por si llega el ACK

  if (ack_recibido_recon) {
    digitalWrite(led, LOW);
  }
  Serial.println("Datos a actualizar mediante GUI:");
  Serial.println(IP_monitoreo);
  Serial.println(EtiquetaRobot);
  //Si no ha llegado ACK se reenvia
  if (millis() - t_last_hello >= 250) {
    t_last_hello = millis();
    sendHELLO();   // envía "RUPU/1 HELLO" por broadcast:7777
  }
}

void sendHELLO() {
  IPAddress bcast(255,255,255,255);
  udp.beginPacket(bcast, REG_PORT);
  udp.print("RUPU/HELLO");
  udp.endPacket();
}

// true cuando recién se detecta "presionado por 2s"
bool disparo_sensor_boton() {
  double d = distancia();  
  switch (estado) {
    case inicio:
      if (d < 20) { 
        if (t_hold_start == 0) 
        {
          t_hold_start = millis();
        }
        if (millis() - t_hold_start >= 2000)
        {
          t_hold_start = 0;
          digitalWrite(led, HIGH); 
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
//NUEVO

#ifndef SimpleUDP_h
#define SimpleUDP_h

#include <WiFi.h>
#include <WiFiUdp.h>

WiFiUDP _udp;
char _bufferRx[255];

void conectarWifi(const char* ssid, const char* pass) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("[WiFi] Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WiFi] Conectado! IP: " + WiFi.localIP().toString());
}

// 2. INICIAR PUERTO
void iniciarUDP(uint16_t puerto) {
  _udp.begin(puerto);
  Serial.printf("[UDP] Escuchando en puerto %d (Filtro activado)\n", puerto);
}

// 3. ENVIAR MENSAJE
void enviarMensaje(String ipDestino, uint16_t puertoDestino, String mensaje) {
  _udp.beginPacket(ipDestino.c_str(), puertoDestino);
  _udp.print(mensaje);
  _udp.endPacket();
}


String recibirMensaje(String ipPermitida) {
  int packetSize = _udp.parsePacket();
  
  if (packetSize > 0) {
    String ipRemota = _udp.remoteIP().toString();

    if (ipRemota == ipPermitida) {
      int len = _udp.read(_bufferRx, 254);
      if (len > 0) _bufferRx[len] = 0;
      return String(_bufferRx);
    } 
    else {
      _udp.read(_bufferRx, 254); 
      return ""; 
    }
  }
  return "";
}

#endif
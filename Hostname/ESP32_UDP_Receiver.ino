#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>

// ======= CONFIGURA AQUÍ =======
const char* SSID     = "VTR-1335633";
const char* PASS     = "nrxtqvWgh4gg";
const char* HOSTNAME = "esp32_lider";
const uint16_t UDP_LISTEN_PORT = 5005;

// Si quieres probar IP estática MÁS ADELANTE, descomenta y ajusta.
// OJO: que coincida con tu red real (p.ej., 192.168.0.x o 192.168.1.x)
// IPAddress local_IP(192,168,1,80);
// IPAddress gateway (192,168,1,1);
// IPAddress subnet  (255,255,255,0);
// IPAddress dns1    (192,168,1,1);

WiFiUDP udp;
const int LED = 2;

// --- Utilidad: traducir WiFi.status() a texto claro
const char* wifiStatusToStr(wl_status_t s) {
  switch (s) {
    case WL_IDLE_STATUS:     return "IDLE";
    case WL_NO_SSID_AVAIL:   return "NO_SSID";
    case WL_SCAN_COMPLETED:  return "SCAN_DONE";
    case WL_CONNECTED:       return "CONNECTED";
    case WL_CONNECT_FAILED:  return "CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "CONNECTION_LOST";
    case WL_DISCONNECTED:    return "DISCONNECTED";
    default:                 return "UNKNOWN";
  }
}

void connectWifi() {
  WiFi.mode(WIFI_STA);

  // PRIMERO validamos con DHCP. Si luego quieres IP fija, descomenta WiFi.config(...)
  // WiFi.config(local_IP, gateway, subnet, dns1);

  // Hostname ANTES de pedir DHCP
  WiFi.setHostname(HOSTNAME);

  Serial.printf("[WiFi] Conectando a SSID='%s'...\n", SSID);
  WiFi.begin(SSID, PASS);

  unsigned long t0 = millis();
  unsigned long lastPrint = 0;

  // LED parpadeo lento mientras intenta conectar
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);

    // Parpadeo lento
    digitalWrite(LED, !digitalRead(LED));

    // Log de estado cada ~1s
    if (millis() - lastPrint > 1000) {
      wl_status_t st = WiFi.status();
      Serial.printf("[WiFi] Estado: %s (%d)\n", wifiStatusToStr(st), (int)st);
      lastPrint = millis();
    }

    // Si tarda más de 20 s, reintenta
    if (millis() - t0 > 20000) {
      Serial.println("[WiFi] Timeout, reintentando...");
      WiFi.disconnect(true, true);
      delay(500);
      WiFi.begin(SSID, PASS);
      t0 = millis();
    }
  }

  // Conectado
  digitalWrite(LED, LOW);  // LED apagado = OK
  Serial.println("[WiFi] CONECTADO!");
  Serial.print("[WiFi] IP: "); Serial.println(WiFi.localIP());
  Serial.print("[WiFi] Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("[WiFi] Subnet: ");  Serial.println(WiFi.subnetMask());
  Serial.print("[WiFi] Hostname: ");Serial.println(WiFi.getHostname());
}

void setupMDNS() {
  if (MDNS.begin(HOSTNAME)) {
    Serial.printf("[mDNS] Anunciado como %s.local\n", HOSTNAME);
    // MDNS.addService("udp", "udp", UDP_LISTEN_PORT); // opcional
  } else {
    Serial.println("[mDNS] Falló MDNS.begin (no es crítico)");
  }
}

void setup() {
  Serial.begin(115200);         // <<<<<<<<<< IMPORTANTE
  delay(200);
  Serial.println("\n[BOOT] Arrancando...");

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  connectWifi();
  setupMDNS();

  // UDP
  if (udp.begin(UDP_LISTEN_PORT)) {
    Serial.printf("[UDP] Escuchando en puerto %u\n", UDP_LISTEN_PORT);
  } else {
    Serial.println("[UDP] ERROR al abrir el puerto");
  }

  // Señal OK: dos destellos rápidos
  for (int i = 0; i < 2; ++i) { digitalWrite(LED, HIGH); delay(120); digitalWrite(LED, LOW); delay(120); }
}

void loop() {
  int pkt = udp.parsePacket();
  if (pkt <= 0) return;

  char buf[256];
  int n = udp.read(buf, sizeof(buf)-1);
  if (n < 0) return;
  buf[n] = '\0';

  IPAddress rip = udp.remoteIP();
  uint16_t rpo = udp.remotePort();

  Serial.printf("[UDP] %d bytes desde %s:%u -> '%s'\n", n, rip.toString().c_str(), rpo, buf);

  if (String(buf) == "DISCOVER_ESP32") {
    String ip   = WiFi.localIP().toString();
    String name = String(HOSTNAME);
    String resp = String("{\"type\":\"ESP32\",\"hostname\":\"") + name + "\",\"ip\":\"" + ip + "\"}";
    udp.beginPacket(rip, rpo);
    udp.write((const uint8_t*)resp.c_str(), resp.length());
    udp.endPacket();
    Serial.println("[DISCOVERY] Respondido con IP/hostname");
  } else {
    // Parpadeo rápido = llegó mensaje normal
    for (int i = 0; i < 3; ++i) { digitalWrite(LED, HIGH); delay(80); digitalWrite(LED, LOW); delay(80); }
  }
}

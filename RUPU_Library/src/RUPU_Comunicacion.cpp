#include "RUPU_Comunicacion.h"

void udp_recep() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(paquete_entrante, 63);
    if (len > 0) {
      paquete_entrante[len] = 0;
    }
    Serial.print("UDP RECIBIDO: ");
    Serial.println(paquete_entrante);
    if (paquete_entrante[0] == 'L') {                //SOLICITUDE DE ESTADO
      lectura_estado(len);
    }
    else if (paquete_entrante[0] == 'E') {          //ESCRITURA DE PARAMETROS
      cadena = configuracion_remota(len);
      cadena.toCharArray(msg, cadena.length() + 1);
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.printf(msg);
      udp.endPacket();
    }
    else if (paquete_entrante[0] == 'R'){           //RECEPCION DATOS ESTADO RECONOCIMIENTO
      cadena = configuracion_remota(len);
      cadena.toCharArray(msg, cadena.length() + 1);
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.printf(msg);
      udp.endPacket();
    }
    else if (paquete_entrante[0] == 'V') {          //RECEPCION DE DATOS PREDECESOR
      estado_predecesor(len);
    }
  }
}

void lectura_estado(int len) {
  String mensaje;
  for (int i = 1; i < len; i++) {
    char nuevo = paquete_entrante[i];
    mensaje.concat(nuevo);
  }
  if (mensaje == "/estado_predecesor") {
    cadena = "V/" + parar + "/" + String(Input_vel) + "/" + String(vel_ref) + "/" + String(curvatura_predecesor);
  }
  else {
    cadena = "incorrecto";
  }
  cadena.toCharArray(msg, cadena.length() + 1);
  for (int i = 0; i < 3; i++) {
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.printf(msg);
    udp.endPacket();
  }
}

String configuracion_remota(int len) {
  String variable;
  int i = 1;
  while ((paquete_entrante[i] != '/' || i == 1) && i < len) {
    char nuevo = paquete_entrante[i];
    variable.concat(nuevo);
    i++;
  }
  String dato;
  int j = i + 1;
  while (i < len - 1) {
    char nuevo = paquete_entrante[i + 1];
    dato.concat(nuevo);
    i++;
  }
  
  // LIMPIEZA DE DATOS: Elimina basura como saltos de linea o retornos de carro (\r \n)
  dato.trim();
  variable.trim();
  
  if ((dato.toFloat() == 0 && (j < len && paquete_entrante[j] != '0')) && variable != "/parar" && variable != "/label") {
    return "datoIncorrecto";
  }
  if (variable == "/co_p") {
    Kp_theta = dato.toFloat();
  }
  else if (variable == "/co_i") {
    Ki_theta = dato.toFloat();
  }
  else if (variable == "/co_d") {
    Kd_theta = dato.toFloat();
  }
  else if (variable == "/cv_p") {
    Kp_vel = dato.toFloat();
  }
  else if (variable == "/cv_i") {
    Ki_vel = dato.toFloat();
  }
  else if (variable == "/cv_d") {
    Kd_vel = dato.toFloat();
  }
  else if (variable == "/cv_ref") {
    sp_vel = dato.toFloat();
  }
  else if (variable == "/cd_p") {
    Kp_d = dato.toFloat();
  }
  else if (variable == "/cd_i") {
    Ki_d = dato.toFloat();
  }
  else if (variable == "/cd_d") {
    Kd_d = dato.toFloat();
  }
  else if (variable == "/cd_delta") {
    delta = dato.toFloat();
  }
  else if (variable == "/cd_ref") {
    d_ref = dato.toFloat();
  }
  else if (variable == "/calibrar") {
    calibrar = dato.toFloat();
  }
  else if (variable == "/parar") {
    parar = dato;
  }
  else if (variable == "/sucesor") {
    IPAddress tmp;
    if (tmp.fromString(dato)) {   
      IP_sucesor = tmp;
    } else {
      Serial.print(F("IP sucesor invalida: ")); Serial.println(dato);
    }
  }
  else if (variable == "/label") {
    EtiquetaRobot = dato;                          
    IP_monitoreo = udp.remoteIP();      
    ack_recibido_recon = true;                     
  }
  else {
    return "incorrecto";
  }
  return "ok";
}

void estado_predecesor(int len) {
  String PARAR;
  int i = 2;
  while (paquete_entrante[i] != '/' && i < len) {
    char nuevo = paquete_entrante[i];
    PARAR.concat(nuevo);
    i++;
  }
  String VEL;
  int j = i + 1;
  i++;
  while (paquete_entrante[i] != '/' && i < len) {
    char nuevo = paquete_entrante[i];
    VEL.concat(nuevo);
    i++;
  }
  String refVel;
  int k = i + 1;
  i++;
  while (paquete_entrante[i] != '/' && i < len) {
    char nuevo = paquete_entrante[i];
    refVel.concat(nuevo);
    i++;
  }
  String CURVA_predecesor;
  int m = i + 1;
  while (i < len - 1) {
    char nuevo = paquete_entrante[i + 1];
    CURVA_predecesor.concat(nuevo);
    i++;
  }
  parar=PARAR;
  sat_d=refVel.toFloat();                            
  curvatura_predecesor=CURVA_predecesor.toFloat();
}

void udp_transm() {
  t_actual = millis() - t_com_predecesor;
  if (t_actual >= 100) {
    cadena = "V/" + parar + "/" + String(Input_vel) + "/" + String(vel_ref) + "/" + String(curvatura);
    cadena.toCharArray(msg, cadena.length() + 1);
    udp.beginPacket(IP_sucesor, PUERTO_SUCESOR);
    udp.printf(msg);
    udp.endPacket();
    t_com_predecesor = millis();
  }
}

void udp_check() {
  cadena = String(EtiquetaRobot) + "," + String(IP_sucesor) + ": Encendido";
  cadena.toCharArray(msg, cadena.length() + 1);
  udp.beginPacket(IP_monitoreo, PUERTO_MONITOREO);
  udp.printf(msg);      
  udp.endPacket();
}

void udp_monitor() {
  cadena = String(EtiquetaRobot) +"," + String(t_actual) +"," + String(Input_d) + "," + String(d_ref) + "," + String(vel_ref) + "," + String(Input_vel) + "," + String(Input_theta) + "," + String(Output_d) + "," + String(Output_vel) + "," + String(Output_theta)+ "," + String(curvatura)+ "," + String(vel_crucero)+ "," + String(curvatura_predecesor)+ "," + String(control);
  cadena.toCharArray(msg, cadena.length() + 1);
  udp.beginPacket(IP_monitoreo, PUERTO_MONITOREO);
  udp.printf(msg);      
  udp.endPacket();
}

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(RUPU_SSID);

  WiFi.begin(RUPU_SSID, RUPU_PASSWORD);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    WiFi.begin(RUPU_SSID, RUPU_PASSWORD);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  udp.begin(PUERTO_LOCAL);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), PUERTO_LOCAL);
}

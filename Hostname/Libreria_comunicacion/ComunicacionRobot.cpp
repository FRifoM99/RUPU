/*
  
  ComunicacionRobot.cpp - Implementación de la librería "ComunicacionRobot.h".
  
*/
#include "ComunicacionRobot.h"

// Constructor: Inicializa las variables
ComunicacionRobot::ComunicacionRobot() {
  _nuevoMensajeAdelante = false;
  _nuevoMensajeComputador = false;
  _velRefAdelante = 0.0;
  _curvaturaAdelante = 0.0;
  _pararAdelante = true;
  _setting_sp_vel = 0.0;
  _setting_flag_parar = true;
  _setting_Kp_theta = 0.0;
  _setting_Ki_theta = 0.0;
  _setting_Kd_theta = 0.0;
  _setting_Kp_vel = 0.0;
  _setting_Ki_vel = 0.0;
  _setting_Kd_vel = 0.0;
  _setting_Kp_d = 0.0;
  _setting_Ki_d = 0.0;
  _setting_Kd_d = 0.0;
  _setting_delta = 0.0;
  _setting_d_ref = 0.0;
  _setting_calibrar = 0.0;
}

// 1. Botón para conectar
bool ComunicacionRobot::iniciar(const char* nombreRed, const char* contrasena, uint16_t miPuerto) {
  // Lógica de `setup_wifi`
  Serial.print("Conectando a ");
  Serial.println(nombreRed);
  WiFi.begin(nombreRed, contrasena);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Falla de conexión WiFi");
    return false;
  }
  Serial.println("WiFi conectado!");
  Serial.println(WiFi.localIP());
  udp.begin(miPuerto);                                           // Inicia la "oreja" UDP
  Serial.printf("Escuchando en puerto UDP %d\n", miPuerto);
  return true;
}

// 2. Botón para configurar destinos
void ComunicacionRobot::configurarDestinos(const char* ipComputador, uint16_t puertoComputador, const char* ipRobotSiguiente, uint16_t puertoRobotSiguiente) {
  ipDelComputador.fromString(ipComputador);
  ipDelRobotSiguiente.fromString(ipRobotSiguiente);
  puertoDelComputador = puertoComputador;
  puertoDelRobotSiguiente = puertoRobotSiguiente;
}

// 3. Botón para "escuchar" mensajes
void ComunicacionRobot::revisarMensajes() {
  // Lógica de `udp_recep`
  int largoPaquete = udp.parsePacket();
  if (largoPaquete > 0) {
    _procesarMensaje(largoPaquete);
  }
}

// 4. Botón para enviar datos al robot siguiente
void ComunicacionRobot::enviarEstadoAlRobotSiguiente(bool parar, float velReferencia, float curvatura) {
  // Lógica de `udp_transm`
  String pararStr = parar ? "si" : "no";
  String cadena = "V/" + pararStr + "/" + String(pararStr) + "/" + String(velReferencia) + "/" + String(curvatura);
  cadena.toCharArray(msg, cadena.length() + 1);
  udp.beginPacket(ipDelRobotSiguiente, puertoDelRobotSiguiente);
  udp.printf(msg);
  udp.endPacket();
}

// 5. Botón para enviar datos al computador
void ComunicacionRobot::enviarTelemetria(char etiqueta, unsigned long t, float dist, float d_ref, float v_ref, float v_in, float ang, float out_d, float out_v, float out_th, float curv, float v_crucero, float curv_pred, int ctrl) {
  // Lógica de `udp_monitor`
  String cadena = String(etiqueta) +"," + String(t) +"," + String(dist) + "," + String(d_ref) + "," + String(v_ref) + "," + String(v_in) + "," + String(ang) + "," + String(out_d) + "," + String(out_v) + "," + String(out_th)+ "," + String(curv)+ "," + String(v_crucero)+ "," + String(curv_pred)+ "," + String(ctrl);
  cadena.toCharArray(msg, cadena.length() + 1);
  udp.beginPacket(ipDelComputador, puertoDelComputador);
  udp.printf(msg);
  udp.endPacket();
}

// --- Getters (simplemente devuelven el valor) ---
bool ComunicacionRobot::hayMensajeNuevoDeAdelante() {
  bool valor = _nuevoMensajeAdelante;
  _nuevoMensajeAdelante = false; // Resetea la bandera
  return valor;
}
bool ComunicacionRobot::hayMensajeNuevoDelComputador() {
  bool valor = _nuevoMensajeComputador;
  _nuevoMensajeComputador = false; // Resetea la bandera
  return valor;
}
float ComunicacionRobot::getVelocidadRefDeAdelante()  { return _velRefAdelante; }
float ComunicacionRobot::getCurvaturaDeAdelante()     { return _curvaturaAdelante; }
bool  ComunicacionRobot::getPararDeAdelante()         { return _pararAdelante; }
float ComunicacionRobot::getSetting_VelocidadReferencia() { return _setting_sp_vel; }
bool  ComunicacionRobot::getSetting_Parar()               { return _setting_flag_parar; }
float ComunicacionRobot::getSetting_Kp_angulo()           { return _setting_Kp_theta; }
float ComunicacionRobot::getSetting_Ki_angulo()           { return _setting_Ki_theta; }
float ComunicacionRobot::getSetting_Kd_angulo()           { return _setting_Kd_theta; }


// =======================================================
// -------------------- FUNCIONES --------------------
// =======================================================

void ComunicacionRobot::_procesarMensaje(int largoPaquete) {
  // Lógica de `udp_recep`
  int len = udp.read(paquete_entrante, 64);
  if (len > 0) {
    paquete_entrante[len] = 0;
  }

  if (paquete_entrante[0] == 'L') {      //SOLICITUD DE ESTADO
    _procesarSolicitudDeEstado(len);
  }
  else if (paquete_entrante[0] == 'E') { //ESCRITURA DE PARAMETROS
    _procesarComandoComputador(len);
  }
  else if (paquete_entrante[0] == 'V') { //RECEPCION DE DATOS PREDECESOR
    _procesarEstadoRobotAdelante(len);
  }
}

void ComunicacionRobot::_procesarComandoComputador(int len) {
  // Lógica de `configuracion_remota`
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
  
  if ((dato.toFloat() == 0 && (j < len && paquete_entrante[j] != '0')) && variable != "/parar") {
    // Dato incorrecto
    return;
  }

  // Guardamos los valores en nuestras variables internas
  if (variable == "/co_p") {
    _setting_Kp_theta = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/co_i") {
    _setting_Ki_theta = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/co_d") {
    _setting_Kd_theta = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/cv_ref") { 
    _setting_sp_vel = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/parar") { 
    _setting_flag_parar = (dato == "si");
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/cv_p") { 
    _setting_Kp_vel = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/cv_i") { 
    _setting_Ki_vel = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/cv_d") { 
    _setting_Kd_vel = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/cd_p") { 
    _setting_Kp_d = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/cd_i") { 
    _setting_Ki_d = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/cd_d") { 
    _setting_Kd_d = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/cd_delta") { 
    _setting_delta = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/cd_ref") { 
    _setting_d_ref = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
  else if (variable == "/calibrar") { 
    _setting_calibrar = dato.toFloat();
    _nuevoMensajeComputador = true;
  }
}

void ComunicacionRobot::_procesarEstadoRobotAdelante(int len) {
  // Lógica de `estado_predecesor`
  String PARAR;
  int i = 2;
  while (paquete_entrante[i] != '/' && i < len) {
    char nuevo = paquete_entrante[i];
    PARAR.concat(nuevo);
    i++;
  }
  String refVel;
  i++; 
  while (paquete_entrante[i] != '/' && i < len) {
    char nuevo = paquete_entrante[i];
    refVel.concat(nuevo);
    i++;
  }
  String CURVA_predecesor;
  i++; 
  while (i < len - 1) {
    char nuevo = paquete_entrante[i + 1];
    CURVA_predecesor.concat(nuevo);
    i++;
  }

  // Guardamos los valores en nuestras variables internas
  _pararAdelante = (PARAR == "si");
  _velRefAdelante = refVel.toFloat(); // Usado como 'sat_d' 
  _curvaturaAdelante = CURVA_predecesor.toFloat(); 
  _nuevoMensajeAdelante = true; // Avisamos que hay datos nuevos
}

void ComunicacionRobot::_procesarSolicitudDeEstado(int len) {
  // Lógica de `lectura_estado`
  String mensaje;
  for (int i = 1; i < len; i++) {
    char nuevo = paquete_entrante[i];
    mensaje.concat(nuevo);
  }
  String cadena;
  if (mensaje == "/estado_predecesor") {
    cadena = "V/ok/no/0/0"; 
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


float ComunicacionRobot::getSetting_Kp_velocidad() { return _setting_Kp_vel; }
float ComunicacionRobot::getSetting_Ki_velocidad() { return _setting_Ki_vel; }
float ComunicacionRobot::getSetting_Kd_velocidad() { return _setting_Kd_vel; }
float ComunicacionRobot::getSetting_Kp_distancia() { return _setting_Kp_d; }
float ComunicacionRobot::getSetting_Ki_distancia() { return _setting_Ki_d; }
float ComunicacionRobot::getSetting_Kd_distancia() { return _setting_Kd_d; }
float ComunicacionRobot::getSetting_Distancia_Delta() { return _setting_delta; }
float ComunicacionRobot::getSetting_Distancia_Referencia() { return _setting_d_ref; }
float ComunicacionRobot::getSetting_Calibrar() { return _setting_calibrar; }

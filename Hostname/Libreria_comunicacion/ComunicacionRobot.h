/*
  ComunicacionRobot.h - Una librería simple para la comunicación 
  del pelotón de robots. Hecha para ser fácil de entender.
*/
#ifndef ComunicacionRobot_h
#define ComunicacionRobot_h

#include "Arduino.h"
#include <WiFi.h>
#include <WiFiUdp.h>

class ComunicacionRobot
{
  public:
    // --- Botones Principales ---
    
    // Constructor (se llama al crear el objeto)
    ComunicacionRobot(); 

    // 1. Botón para conectar el robot a la red WiFi
    bool iniciar(const char* nombreRed, const char* contrasena, uint16_t miPuerto);

    // 2. Botón para decirle al robot a quién debe contactar
    void configurarDestinos(const char* ipComputador, uint16_t puertoComputador, const char* ipRobotSiguiente, uint16_t puertoRobotSiguiente);

    // 3. Botón para "escuchar" si llegan nuevos mensajes (debes ponerlo en el loop)
    void revisarMensajes();

    // 4. Botón para enviar datos al robot que te sigue
    void enviarEstadoAlRobotSiguiente(bool parar, float velReferencia, float curvatura);

    // 5. Botón para enviar datos de telemetría al computador
    void enviarTelemetria(char etiqueta, unsigned long t, float dist, float d_ref, float v_ref, float v_in, float ang, float out_d, float out_v, float out_th, float curv, float v_crucero, float curv_pred, int ctrl);

    // --- Sensores de Mensajes (Getters) ---
    
    bool hayMensajeNuevoDeAdelante();
    bool hayMensajeNuevoDelComputador();

    // --- Leer Datos Recibidos (del Robot de Adelante) ---
    float getVelocidadRefDeAdelante();
    float getCurvaturaDeAdelante();
    bool  getPararDeAdelante();

    // --- Leer Datos Recibidos (del Computador) ---
    float getSetting_VelocidadReferencia(); // sp_vel
    bool  getSetting_Parar();               // flag_parar
    float getSetting_Kp_angulo();           // Kp_theta
    float getSetting_Ki_angulo();           // Ki_theta
    float getSetting_Kd_angulo();           // Kd_theta
    
  
    float getSetting_Kp_velocidad();         // Kp_vel 
    float getSetting_Ki_velocidad();         // Ki_vel 
    float getSetting_Kd_velocidad();         // Kd_vel 
    float getSetting_Kp_distancia();         // Kp_d 
    float getSetting_Ki_distancia();         // Ki_d 
    float getSetting_Kd_distancia();         // Kd_d 
    float getSetting_Distancia_Delta();      // delta
    float getSetting_Distancia_Referencia(); // d_ref 
    float getSetting_Calibrar();             // calibrar 
  


  private:
    WiFiUDP udp;
    IPAddress ipDelComputador;
    IPAddress ipDelRobotSiguiente;
    uint16_t puertoDelComputador;
    uint16_t puertoDelRobotSiguiente;

    char paquete_entrante[64]; // Buffer para mensajes
    char msg[128];             // Buffer para enviar

    // Variables para guardar lo que recibimos
    bool _nuevoMensajeAdelante;
    bool _nuevoMensajeComputador;
    float _velRefAdelante;
    float _curvaturaAdelante;
    bool  _pararAdelante;
    float _setting_sp_vel;
    bool  _setting_flag_parar;
    float _setting_Kp_theta;
    float _setting_Ki_theta;
    float _setting_Kd_theta;
    float _setting_Kp_vel;
    float _setting_Ki_vel;
    float _setting_Kd_vel;
    float _setting_Kp_d;
    float _setting_Ki_d;
    float _setting_Kd_d;
    float _setting_delta;
    float _setting_d_ref;
    float _setting_calibrar;
  
    
    // Funciones internas para procesar los mensajes
    void _procesarMensaje(int largoPaquete);
    void _procesarComandoComputador(int largo);    // Reemplaza a configuracion_remota
    void _procesarEstadoRobotAdelante(int largo);  // Reemplaza a estado_predecesor
    void _procesarSolicitudDeEstado(int largo);    // Reemplaza a lectura_estado
};

#endif
#include "config.h"

// ==========================================
// VARIABLES PRIVADAS
// ==========================================

// Calibración
float sensoresMax[16]; 
float sensoresMin[16]; 

// Odometría y Velocidad
unsigned long t_svel = 0;
unsigned long t_actual = 0;
float v_r[5] = {0}, v_l[5] = {0}; 
float Input_vel = 0, ang_vel = 0;

// Curvatura
unsigned long t_arco = 0;
float a_sl=0, a_sr=0, b_sl=0, b_sr=0, c_sl=0, c_sr=0; 
float a_s=0, b_s=0, c_s=0;
float a_curvatura=0, b_curvatura=0, c_curvatura=0;
int recta = 0; 
float curvatura = 0; 
String parar = "";

// ==========================================
// IMPLEMENTACIÓN DE FUNCIONES
// ==========================================

void configurarMUX() {
  pinMode(PIN_MUX_S0, OUTPUT);
  pinMode(PIN_MUX_S1, OUTPUT);
  pinMode(PIN_MUX_S2, OUTPUT);
  pinMode(PIN_MUX_S3, OUTPUT);
}

void seleccionarCanalMUX(byte canal) {
  digitalWrite(PIN_MUX_S0, canal & 0x01);
  digitalWrite(PIN_MUX_S1, canal & 0x02);
  digitalWrite(PIN_MUX_S2, canal & 0x04);
  digitalWrite(PIN_MUX_S3, canal & 0x08);
}

void setMaxMin(int i, float lectura) {
  if (sensoresMax[i] < lectura) sensoresMax[i] = lectura;
  if (sensoresMin[i] > lectura) sensoresMin[i] = lectura;
}

float calcularNivelSensor(byte index, bool frontal) {
  int sensorIndex = frontal ? index : index + 8; 
  float lectura = analogRead(PIN_MUX_SIGNAL);

  float rango = sensoresMax[sensorIndex] - sensoresMin[sensorIndex];
  if (rango == 0) rango = 1; 

  float nivel = ((1- 2 * LINEA_NEGRA) * 100 * (lectura - sensoresMin[sensorIndex]) / rango) + 100 * LINEA_NEGRA;
  
  if (sensorIndex < 8 ) { 
    return (nivel < UMBRAL_RUIDO_FRONTAL ) ? 0 : nivel;
  } else {
    return (nivel < UMBRAL_RUIDO_POSTERIOR ) ? 0 : nivel;
  }
}

float getPosition(bool frontal){
  float sensorNormalizado[8];
  float sumPonderada = 0;
  float areaTotal    = 0;

  for(byte i = 0; i <= 7; i++){
    seleccionarCanalMUX(frontal ? i: i+ 8);
    sensorNormalizado[i]= calcularNivelSensor(i, frontal);
    
    int ponderacion = (-28 + 8 * i);
    sumPonderada += sensorNormalizado[i] * ponderacion;
    areaTotal    += sensorNormalizado[i];
  }
  
  if(areaTotal == 0){ 
    parar ="si";
    return 0;
  } else {
      return sumPonderada / areaTotal;
  }  
}

void calibrarSensores() {
  for (int i = 0; i <= 15; i++) { 
    sensoresMax[i] = 0; 
    sensoresMin[i] = 4095; 
  }
  
  constexpr int vel_cal = 200;
  encoder_der.clearCount();
  encoder_izq.clearCount();

  while(abs(RADIO_RUEDA * PI * (encoder_der.getCount()-encoder_izq.getCount())*N_REDUCCION / (CPR*DIST_RUEDAS)) < 4 * PI){ 
    motor(-vel_cal, vel_cal);
    for(byte canal= 0; canal <= 15; canal++){          
      seleccionarCanalMUX(canal);
      setMaxMin(canal, analogRead(PIN_MUX_SIGNAL));   
    }
  }
  
  constexpr int SENSOR_CENTRAL_FRONTAL = 4;
  seleccionarCanalMUX(SENSOR_CENTRAL_FRONTAL);
  long inicio_der = encoder_der.getCount();
  long inicio_izq = encoder_izq.getCount();

  while(calcularNivelSensor(SENSOR_CENTRAL_FRONTAL, true) < 80){ 
    motor(-vel_cal, vel_cal);                                                                             
    
    float anguloGirado = RADIO_RUEDA * PI * ((encoder_der.getCount() - inicio_der) - (encoder_izq.getCount() - inicio_izq)) * N_REDUCCION / (CPR * DIST_RUEDAS);
    if (fabs(anguloGirado) > 2*PI) {
        Serial.println("Error: Línea no encontrada tras giro extra.");
        break;
    }
  }                                                                                                          
  motor(0,0);
}

void Calculo_Velocidades() {
  t_actual = millis() - t_svel;
  if(t_actual>=10){
    for(int k=4; k>0; k--) { 
        v_r[k] = v_r[k-1]; 
        v_l[k] = v_l[k-1]; 
    }
    v_r[0]=2*PI*RADIO_RUEDA*encoder_der.getCount()*N_REDUCCION/(t_actual*0.001*CPR);
    v_l[0]=2*PI*RADIO_RUEDA*encoder_izq.getCount()*N_REDUCCION/(t_actual*0.001*CPR); 
    
    encoder_der.clearCount();
    encoder_izq.clearCount();
    t_svel=millis();
  }
  
  // (Promedio ponderado)
  v_r[0]=(0.2*(v_r[1]+v_r[2]+v_r[3]+v_r[4])/4) + 0.8*v_r[0]; 
  v_l[0]=(0.2*(v_l[1]+v_l[2]+v_l[3]+v_l[4])/4) + 0.8*v_l[0]; 
  
  Input_vel=0.5*(v_r[0]+v_l[0]); 
  ang_vel=0.5*(v_r[0]-v_l[0])/DIST_RUEDAS;
}

float getVelocidadEnc(bool izquierda) {
    if (izquierda) return v_l[0];
    else return v_r[0];
}

void Curvatura_Pista() {
  t_actual = millis() - t_arco;
  if(t_actual>=20){
    a_sl+=v_l[0]*t_actual*0.001; a_sr+=v_r[0]*t_actual*0.001;
    b_sl+=v_l[0]*t_actual*0.001; b_sr+=v_r[0]*t_actual*0.001;
    c_sl+=v_l[0]*t_actual*0.001; c_sr+=v_r[0]*t_actual*0.001;
    t_arco=millis();
  }
  a_s=0.5*(a_sr+a_sl); b_s=0.5*(b_sr+b_sl); c_s=0.5*(c_sr+c_sl);

  if(Input_vel<0.1){
    a_curvatura=0; b_curvatura=0; c_curvatura=0;
    a_sl=0; a_sr=0; b_sl=0; b_sr=0; c_sl=0; c_sr=0;
  }
  else if (a_s>=3 && b_s>=10){
    b_curvatura=abs((0.5*(b_sr-b_sl))/(DIST_RUEDAS*b_s));
    b_sl=0; b_sr=0;
  }
  else if(a_s>=3 && a_s==b_s){
    b_sl=0; b_sr=0;
  }
  else if (a_s>=6 && c_s>=10){
    c_curvatura=abs((0.5*(c_sr-c_sl))/(DIST_RUEDAS*c_s));
    c_sl=0; c_sr=0;
  }
  else if(a_s>=6 && a_s==c_s){
    c_sl=0; c_sr=0;
  }
  else if(a_s>=10){
    a_curvatura=abs((0.5*(a_sr-a_sl))/(DIST_RUEDAS*a_s));
    a_sl=0; a_sr=0;
  }
  
  if(a_curvatura<0.01 && b_curvatura<0.01 && c_curvatura<0.01){
    recta=1;
  }
  else if(a_curvatura>0.02 && b_curvatura>0.02 && c_curvatura>0.02){
    recta=0;
  }
  
  if(recta){
    if(a_curvatura >= b_curvatura && a_curvatura >= c_curvatura) curvatura=a_curvatura;
    else if(b_curvatura >= a_curvatura && b_curvatura >= c_curvatura) curvatura=b_curvatura;
    else if(c_curvatura >= a_curvatura && c_curvatura >= b_curvatura) curvatura=c_curvatura;
    else curvatura=a_curvatura;
  }
  else{
    if(a_curvatura < b_curvatura && a_curvatura < c_curvatura) curvatura=a_curvatura;
    else if(b_curvatura < a_curvatura && b_curvatura < c_curvatura) curvatura=b_curvatura;
    else if(c_curvatura < a_curvatura && c_curvatura < b_curvatura) curvatura=c_curvatura;
    else curvatura=a_curvatura;
  }
}
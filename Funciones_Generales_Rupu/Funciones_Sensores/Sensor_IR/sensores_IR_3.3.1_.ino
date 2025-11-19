#include "sensores_IR_3.3.1.h"

// --- Pines de control del MUX ---

// ===============================
// Constantes de configuración
// ===============================
//(las saque del main y las puse aqui, ya que son parte del modulo de sensor IR que ocupan el mux)
//(solo se ocupan en este modulo, por lo que no se definen en el .h)

constexpr uint8_t MUX_PIN_S0     = 25;
constexpr uint8_t MUX_PIN_S1     = 26;
constexpr uint8_t MUX_PIN_S2     = 2;
constexpr uint8_t MUX_PIN_S3     = 4;
constexpr uint8_t MUX_PIN_SIGNAL = 39;

float sensoresMax[16]; //Variable global que almacena el valor de lectura maximo de cada uno de los sensores IR (frontal y posterior)
float sensoresMin[16]; //Variable global que almacena el valor de lectura minimo de cada uno de los sensores IR (frontal y posterior)
constexpr int UMBRAL_RUIDO_FRONTAL = 30;   // Valor bajo considerado ruido en sensores IR
constexpr int UMBRAL_RUIDO_POSTERIOR = 10;
  
// ================================================
/*
Funciones para la calibracion de sensores IR y medicion de la orientacion del DDMR respecto a la trayectoria de navegacion      
*/

void calibrarSensores() {         //Funcion de calibracion (se ejecuta una sola vez)
  for (int i = 0; i <= 15; i++) { //Inicializacion de maximos y minimos
    sensoresMax[i] = 0;
    sensoresMin[i] = 4095;
  }
  Serial.println("entre a calibracion");
  constexpr int vel_motor_cal = 200;                   //Ciclo de trabajo 20% PWM (0->1023) para mover los motores del DDMR
  encoder_der.clearCount();           //Cuentas de los encoders inicializadas en 0 cuentas 
  encoder_izq.clearCount();           
  while(abs(RADIO_RUEDA * PI * (encoder_der.getCount()-encoder_izq.getCount())*N_REDUCCION / (CPR*DIST_RUEDAS))< 4 * PI){ //El lado izquierdo corresponde a la posicion angular del DDMR en [rad] (es decir mientras el robot no de 2 vueltas)
    motor(-vel_motor_cal, vel_motor_cal);                //Permite hacer girar al DDMR en torno a su eje de giro
    for(byte canal= 0; canal <= 15; canal++){          //Se recorre cada uno de los sensores IR (MUX1)
      seleccionarCanalMUX(canal);
      setMaxMin(canal, analogRead(MUX_PIN_SIGNAL));   //Esta funcion determina si el valor medido por el sensor conectado al canal i+1, es un maximo o minimo
    }
  }
  // alienear sensor central frontal con la trayectoria
  constexpr int SENSOR_CENTRAL_FRONTAL = 4;
  seleccionarCanalMUX(SENSOR_CENTRAL_FRONTAL);
  
  // Guardamos la posición inicial del giro
  long inicio_der = encoder_der.getCount();
  long inicio_izq = encoder_izq.getCount();
  //Serial.println(calcularNivelSensor(SENSOR_CENTRAL_FRONTAL, true)); // para verificar si el valor es valido para entrar al while
  while(calcularNivelSensor(SENSOR_CENTRAL_FRONTAL, true) < 80){ //Mientras el sensor IR (canal 5) no este posicinado en la trayectoria 
    //Serial.println(calcularNivelSensor(SENSOR_CENTRAL_FRONTAL, true));
    motor(-vel_motor_cal, vel_motor_cal);                    //El DDMR sigue rotando                                                                          
   // Ángulo girado en radianes
    float anguloGirado = RADIO_RUEDA * PI * ((encoder_der.getCount() - inicio_der) - (encoder_izq.getCount() - inicio_izq)) * N_REDUCCION / (CPR * DIST_RUEDAS);
    Serial.println(anguloGirado); //para verificar si entra al while o no 

    // Si ya giró más de una vuelta sin encontrar la línea, salir
    if (fabs(anguloGirado) > 2*PI) {
        Serial.println("No se encontró la línea tras media vuelta extra.");
        break;
    }

  }                                                                                                          
  motor(0,0);                         //Detiene el robot alineado con la trayectoria
}

//funcion que lee el valor actual con los valore guardados y actualiza el maximo y el minimo.
void setMaxMin(int i, float lectura) {  //Funcion que determina valores maximos y minimos de los sensores IR (se ejecuta una sola vez)
  if (sensoresMax[i] < lectura) {
    sensoresMax[i] = lectura;
  }
  if (sensoresMin[i] > lectura) {
    sensoresMin[i] = lectura;
  }
}

//Funcion que determina la posicion de la trayectoria respecto del centro del arreglo de sensores en [mm], dice que tan lejos o cerca esta la trayectoria
float getposition(bool frontal){    //si el argumento es 1 se calcula utilizando el arreglo de sensores frontal, pero si el argumento es 0 se utiliza el arreglo de sensores posterior.
  float sensorNormalizado[8];
  float areaSensor[8];                      
  float sumPonderada = 0;
  float areaTotal    = 0;
  //lectura y normalizar

                     //Calcula el valor normalizado de cada sensor del arreglo frontal en una escala comun
  for(byte i = 0; i <= 7; i++){
    seleccionarCanalMUX(frontal ? i: i+ 8); // si es frontal, ocupa del 0 al 7, si no es frontal ocupa del 8 al 15
    sensorNormalizado[i]= calcularNivelSensor(i, frontal); //Mapeo del sensor de 0 a 100, o viceversa (segun "linea")
    areaSensor[i] = 8 * sensorNormalizado[i] - 0.04 * sensorNormalizado[i] * sensorNormalizado[i];
  } 
  // en el codigo de prueba de auto calibracion aparece usa el areaSensor, asi areaTotal += areaSensor, pero en el codigo control curvatura ocupa
  // solo el sensorNormalizado, y areaTotal += sensorNormalizado.
  /* ========== Version antigua donde ocupa la variable areaSensor[i] ===========================================
  const int ponderaciones[8] = {-28, -20, -12, -4, 4, 12, 20, 28};
  for (byte i = 0; i <= 7; i++) {
    sumPonderada += ponderaciones[i]*areaSensor[i];
    areaTotal += areaSensor[i];
  }                                  //Suma total de las areas
  if(areaTotal == 0) {    //Si el area es cero, implica que el robot se salio de la trayectoria
    parar ="si";
    return 0;
  }
  else{
      return sumPonderada / areaTotal; //Se retorna el promedio del nivel del sensor normalizado, poderado por la ubicacion espacial de cada sensor (Retorna la posicion de la trayectoria respecto del centro del arreglo de sensores en [mm])
  }                     //Rango +- 28[mm]
  */
  //===============================================================================================================
  for(byte i = 0; i <= 7; i++){
    int ponderacion = (-28 + 8 * i);
    sumPonderada += sensorNormalizado[i] * ponderacion;
    areaTotal    += sensorNormalizado[i];
  }//
  if(areaTotal == 0){ //Si el area es cero, implica que el robot se salio de la trayectoria
    parar ="si";
    return 0;
  }
  else{
      return sumPonderada / areaTotal; //centroide
  }  



}

//Activa el canal correspondiente del MUX para leer el sensor IR indicado 
void seleccionarCanalMUX(byte canal) {
  digitalWrite(MUX_PIN_S0, canal & 0x01);
  digitalWrite(MUX_PIN_S1, canal & 0x02);
  digitalWrite(MUX_PIN_S2, canal & 0x04);
  digitalWrite(MUX_PIN_S3, canal & 0x08);  
}

// lee el sensor IR, lo convierte en un valor normalizado e indica que tan negra o blanca es la superficie que ve el sensor
float calcularNivelSensor(byte index, bool frontal) {
  int sensorIndex = frontal ? index : index + 8; 
  float lectura = analogRead(MUX_PIN_SIGNAL);

  float rango = sensoresMax[sensorIndex] - sensoresMin[sensorIndex];
  if (rango == 0) rango = 1; // evita la division por cero

  float nivel = ((1- 2 * LINEA_NEGRA) * 100 * (lectura - sensoresMin[sensorIndex]) / rango) + 100 * LINEA_NEGRA;
  // descarta las muestras menores al umbral definido
  if (sensorIndex < 8 )  { //sensor frontal 
    return (nivel < UMBRAL_RUIDO_FRONTAL ) ? 0 : nivel;
  }
  else {
    return (nivel < UMBRAL_RUIDO_POSTERIOR ) ? 0 : nivel;
  }
}

//calcula las velocidades lineales de cada rueda usando los encoders,
//aplica un fitro promediado y obtiene la velocidad lineal y angular
void Calculo_Velocidades() {
    //falta rellenar este codigo 
  t_actual = millis() - t_svel;
  if(t_actual>=10){
    v_r[1]=v_r[0];
    v_r[2]=v_r[1];
    v_r[3]=v_r[2];
    v_r[4]=v_r[3];
    v_l[1]=v_l[0];
    v_l[2]=v_l[1];
    v_l[3]=v_l[2];
    v_l[4]=v_l[3];
    v_r[0]=2*PI*RADIO_RUEDA*encoder_der.getCount()*N_REDUCCION/(t_actual*0.001*CPR); // ENCODERS:28CPR---CAJA_REDUCTORA:100:1  [cm/s]
    v_l[0]=2*PI*RADIO_RUEDA*encoder_izq.getCount()*N_REDUCCION/(t_actual*0.001*CPR); // ENCODERS:28CPR---CAJA_REDUCTORA:100:1  [cm/s]
    encoder_der.clearCount();
    encoder_izq.clearCount();
    t_svel=millis();
  }
  v_r[0]=(0.2*(v_r[1]+v_r[2]+v_r[3]+v_r[4])/4) + 0.8*v_r[0]; //velocidad derecha 
  v_l[0]=(0.2*(v_l[1]+v_l[2]+v_l[3]+v_l[4])/4) + 0.8*v_l[0]; //velocidad izquierda
  Input_vel=0.5*(v_r[0]+v_l[0]); // [cm/s]
  ang_vel=0.5*(v_r[0]-v_l[0])/DIST_RUEDAS;
    

}
//Determina si el robot esta en una recta o curva y calcula la curvatura del trayecto segun las velocidadeds de las ruedas
//se usa para ajustar la velocidad y el control del giro 
void Curvatura_Pista() {
  //falta rellenar este codigo
  t_actual = millis() - t_arco;
  if(t_actual>=20){
    a_sl+=v_l[0]*t_actual*0.001;
    a_sr+=v_r[0]*t_actual*0.001;
    b_sl+=v_l[0]*t_actual*0.001;
    b_sr+=v_r[0]*t_actual*0.001;
    c_sl+=v_l[0]*t_actual*0.001;
    c_sr+=v_r[0]*t_actual*0.001;
    t_arco=millis();
  }
  a_s=0.5*(a_sr+a_sl);
  b_s=0.5*(b_sr+b_sl);
  c_s=0.5*(c_sr+c_sl);
  if(Input_vel<0.1){
    a_curvatura=0;
    b_curvatura=0;
    c_curvatura=0;
    a_sl=0;
    a_sr=0;
    b_sl=0;
    b_sr=0;
    c_sl=0;
    c_sr=0;
  }
  else if (a_s>=3 && b_s>=10){
    b_curvatura=abs((0.5*(b_sr-b_sl))/(DIST_RUEDAS*b_s));
    b_sl=0;
    b_sr=0;
  }
  else if(a_s>=3 && a_s==b_s){
    b_sl=0;
    b_sr=0;
  }
  else if (a_s>=6 && c_s>=10){
    c_curvatura=abs((0.5*(c_sr-c_sl))/(DIST_RUEDAS*c_s));
    c_sl=0;
    c_sr=0;
  }
  else if(a_s>=6 && a_s==c_s){
    c_sl=0;
    c_sr=0;
  }
  else if(a_s>=10){
    a_curvatura=abs((0.5*(a_sr-a_sl))/(DIST_RUEDAS*a_s));
    a_sl=0;
    a_sr=0;
  }
  if(a_curvatura<0.01 && b_curvatura<0.01 && c_curvatura<0.01){
    recta=1;
  }
  else if(a_curvatura>0.02 && b_curvatura>0.02 && c_curvatura>0.02){
    recta=0;
  }
  if(recta){
    if(a_curvatura >= b_curvatura && a_curvatura >= c_curvatura){
      curvatura=a_curvatura;
    }
    else if(b_curvatura >= a_curvatura && b_curvatura >= c_curvatura){
      curvatura=b_curvatura;
    }
    else if(c_curvatura >= a_curvatura && c_curvatura >= b_curvatura){
      curvatura=c_curvatura;
    }
    else{
      curvatura=a_curvatura;
    }
  }
  else{
    if(a_curvatura < b_curvatura && a_curvatura < c_curvatura){
      curvatura=a_curvatura;
    }
    else if(b_curvatura < a_curvatura && b_curvatura < c_curvatura){
      curvatura=b_curvatura;
    }
    else if(c_curvatura < a_curvatura && c_curvatura < b_curvatura){
      curvatura=c_curvatura;
    }
    else{
      curvatura=a_curvatura;
    }
  }
}

//configura los pinea de control del multiplexor como salidas
void configurarMUX() {
  //configura los pines de control del mux
  pinMode(MUX_PIN_S0, OUTPUT);     //PIN S0
  pinMode(MUX_PIN_S1, OUTPUT);     //PIN S1
  pinMode(MUX_PIN_S2, OUTPUT);      //PIN S2
  pinMode(MUX_PIN_S3, OUTPUT);      //PIN S3
}


  








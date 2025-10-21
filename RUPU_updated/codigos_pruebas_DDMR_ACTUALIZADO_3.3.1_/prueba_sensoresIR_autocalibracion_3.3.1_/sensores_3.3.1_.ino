/*
Funciones para la calibracion de sensores IR y medicion de la orientacion del DDMR respecto a la trayectoria de navegacion      
*/

float sensoresMax[16]; //Variable global que almacena el valor de lectura maximo de cada uno de los sensores IR (frontal y posterior)
float sensoresMin[16]; //Variable global que almacena el valor de lectura minimo de cada uno de los sensores IR (frontal y posterior)
constexpr int UMBRAL_RUIDO = 30;   // Valor bajo considerado ruido en sensores IR

// ============= ===================================
void calibrarSensores() {         //Funcion de calibracion (se ejecuta una sola vez)
  for (int i = 0; i <= 15; i++) { //Inicializacion de maximos y minimos
    sensoresMax[i] = 0;
    sensoresMin[i] = 4095;
  }
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
    //Serial.println(anguloGirado); //para verificar si entra al while o no 

    // Si ya giró más de media vuelta sin encontrar la línea, salir
    if (fabs(anguloGirado) > 2*PI) {
        Serial.println("No se encontró la línea tras media vuelta extra.");
        break;
    }

  }                                                                                                          
  motor(0,0);                         //Detiene el robot alineado con la trayectoria
}

void setMaxMin(int i, float lectura) {  //Funcion que determina valores maximos y minimos de los sensores IR (se ejecuta una sola vez)
  if (sensoresMax[i] < lectura) {
    sensoresMax[i] = lectura;
  }
  if (sensoresMin[i] > lectura) {
    sensoresMin[i] = lectura;
  }
}
                                      //Funcion que determina la posicion de la trayectoria respecto del centro del arreglo de sensores en [mm],
float getposition(bool frontal){    //si el argumento es 1 se calcula utilizando el arreglo de sensores frontal, pero si el argumento es 0 se utiliza el arreglo de sensores posterior.
  float sensorNormalizado[8];
  float areaSensor[8];                      
  float sumPonderada = 0;
  float areaTotal    = 0;
  //lectura y normalizar

                     //Calcula el valor normalizado de cada sensor del arreglo frontal en una escala comun
  for(byte i = 0; i <= 7; i++){
    seleccionarCanalMUX(frontal ? i: i+ 8);
    sensorNormalizado[i]= calcularNivelSensor(i, frontal); //Mapeo del sensor de 0 a 100, o viceversa (segun "linea")
    areaSensor[i] = 8 * sensorNormalizado[i] - 0.04 * sensorNormalizado[i] * sensorNormalizado[i];
  } 
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
}

void seleccionarCanalMUX(byte canal) {
  digitalWrite(MUX_PIN_S0, canal & 0x01);
  digitalWrite(MUX_PIN_S1, canal & 0x02);
  digitalWrite(MUX_PIN_S2, canal & 0x04);
  digitalWrite(MUX_PIN_S3, canal & 0x08);  
}

float calcularNivelSensor(byte index, bool frontal) {
  int sensorIndex = frontal ? index : index + 8; 
  float lectura = analogRead(MUX_PIN_SIGNAL);

  float rango = sensoresMax[sensorIndex] - sensoresMin[sensorIndex];
  if (rango == 0) rango = 1; // evita la division por cero

  float nivel = ((1- 2 * LINEA_NEGRA) * 100 * (lectura - sensoresMin[sensorIndex]) / rango) + 100 * LINEA_NEGRA;
  return (nivel < UMBRAL_RUIDO ) ? 0 : nivel;
}

  








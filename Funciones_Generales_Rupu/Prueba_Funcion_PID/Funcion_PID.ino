//Funcion PID retorna actuacion a motor

static long ultima_posicion = 0;
static unsigned long ultimo_tiempo = 0;

// terminos de error para la actuacion
static float error_0 = 0;
static float error_1 = 0;
static float error_2 = 0;

//Actuaciones u_0 es la mas actual
static float u_0 = 0;
static float u_1 = 0;

static float velocidad_cm_s = 0;

float funcion_PID(float referencia_cm_s, float Kp, float Ki, float Kd)
{
  float referencia = (referencia_cm_s*2800)/(2*PI*0.0215*100); //pasar de cm/s a pulsos/s
  long posicion_actual = encoder_der.read();
  unsigned long tiempo_actual = millis();

  float delta_tiempo = (tiempo_actual - ultimo_tiempo)/1000.0;
  if(delta_tiempo == 0)
  {
    delta_tiempo = Tiempo_muestreo/1000.0;
  }

  float velocidad = (posicion_actual - ultima_posicion)/delta_tiempo;
  velocidad_cm_s = (velocidad*2*PI*0.0215*100)/2800;

  //Calculo del error
  error_0 = referencia - velocidad;
  //Actuacion a la planta
  u_0 = u_1 + (Kp + (Kd/delta_tiempo))*error_0 + (-Kp + Ki*delta_tiempo - 2*(Kd/delta_tiempo))*error_1 + (Kd/delta_tiempo)*error_2; 

  // Debugear Enviar datos al Serial Plotter 
  Serial.print(contador); // contador
  Serial.print("\t");  
  Serial.print(referencia_cm_s); // Referencia
  Serial.print("\t");
  Serial.print(velocidad_cm_s); // Respuesta
  Serial.print("\t");
  Serial.print(u_0); // Salida PI
  Serial.print("\t");
  Serial.print(delta_tiempo);
  Serial.print("\t");
  Serial.print(posicion_actual);
  Serial.print("\t");
  Serial.println(ultima_posicion);

  actuacion = constrain(u_0, -1023, 1023);

  //Actualizacion de variables
  ultimo_tiempo = tiempo_actual;
  ultima_posicion = posicion_actual;
  error_2 = error_1;
  error_1 = error_0; 
  u_1 = u_0;

  return actuacion;
}
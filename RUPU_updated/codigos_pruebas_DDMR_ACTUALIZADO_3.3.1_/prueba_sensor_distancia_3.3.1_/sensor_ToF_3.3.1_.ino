/*
Funciones para la configuracion y uso del sensor de distancia VL53L0X              
*/

void InicializarSensor(VL53L0X &sensor){ //Funcion de configuracion              
  Wire.begin();                                                       
  sensor.setTimeout(500);                                             
  while (!sensor.init())                                              
  {                                                                   
    Serial.println("Falla de conexión VL53L0X");                                                            
    delay(1000);                                                      
  }                                                                   
  sensor.setSignalRateLimit(0.25);                                    
  sensor.setMeasurementTimingBudget(33000);        //Actualiza la medición cada {Argumento}[us]                      
  sensor.startContinuous();                        //toma medidas cada {Argumento}[ms]      
  }
  
double leerDistancia(VL53L0X &sensor){
  double distanciaCm = sensor.readReg16Bit(sensor.RESULT_RANGE_STATUS + 10) * 0.1 - OFFSET_SENSOR_CM; // mm -> cm //Lee el registro que almacena las mediciones del sensor, se realiza una
  if(distanciaCm > DISTANCIA_MAXIMA_CM) 
  {  //Saturación de la medicion en 80[cm]
    contadorFiltro = 0 ;
    distanciaCm   = DISTANCIA_MAXIMA_CM;
  }
  else if(distanciaCm <= DISTANCIA_MAXIMA_CM - 20  && distanciaActual == DISTANCIA_MAXIMA_CM && contadorFiltro < FILTRO_MUESTRAS)
  { //Discriminacion de una muestra valida
    contadorFiltro++;
    return DISTANCIA_MAXIMA_CM;
  }
  else  if(distanciaCm <= DISTANCIA_MAXIMA_CM - 20  && contadorFiltro < FILTRO_MUESTRAS )
  {
    return distanciaCm;
  }
  else
  {
    contadorFiltro = 0;
  }
  return distanciaCm;
}

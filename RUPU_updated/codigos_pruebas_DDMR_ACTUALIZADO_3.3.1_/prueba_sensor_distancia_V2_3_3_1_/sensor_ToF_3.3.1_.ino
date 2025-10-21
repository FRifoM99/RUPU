/*
Funciones para la configuracion y uso del sensor de distancia VL53L0X              
*/
double bufferDistancia[N_MUESTRAS];
int indexBuffer = 0;


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
    // 1. Leer distancia directamente desde registro (mm -> cm)
    double distanciaCm = sensor.readReg16Bit(sensor.RESULT_RANGE_STATUS + 10) * 0.1 - OFFSET_SENSOR_CM;

    // 2. Evitar valores 0 o negativos
    //if(distanciaCm <= 0) distanciaCm = distanciaActual;

    // 3. Saturación máxima
    if(distanciaCm > DISTANCIA_MAXIMA_CM){
        contadorFiltro = 0;
        distanciaCm = DISTANCIA_MAXIMA_CM;
    }

    // 4. Recuperación desde saturación solo después de varias muestras
    else if(distanciaCm <= DISTANCIA_MAXIMA_CM - 20 && contadorFiltro < FILTRO_MUESTRAS ){
        if( distanciaActual == DISTANCIA_MAXIMA_CM){
            contadorFiltro++;
            distanciaCm = DISTANCIA_MAXIMA_CM;
        } else {
            contadorFiltro = 0;
        }
    }

    // 6. Actualizar buffer y filtrar con mediana
    agregarBuffer(distanciaCm);
    distanciaCm = medianaBuffer();

    // 7. Guardar distancia actual
    distanciaActual = distanciaCm;

    return distanciaCm;
}


// ---- FUNCIONES AUXILIARES ----
double medianaBuffer() {
    double copia[N_MUESTRAS];
    memcpy(copia, bufferDistancia, sizeof(copia));
    // Ordenamiento simple (bubble sort)
    for(int i=0;i<N_MUESTRAS-1;i++)
        for(int j=i+1;j<N_MUESTRAS;j++)
            if(copia[i]>copia[j]){
                double tmp=copia[i];
                copia[i]=copia[j];
                copia[j]=tmp;
            }
    return copia[N_MUESTRAS/2];
}

void agregarBuffer(double lectura){
    bufferDistancia[indexBuffer++] = lectura;
    if(indexBuffer >= N_MUESTRAS) indexBuffer = 0;
}
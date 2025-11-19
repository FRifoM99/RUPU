#include "sensor_ToF_3.3.1.h"


/*
Definicion de Funciones para la configuracion y uso del sensor de distancia VL53L0X              
*/

double leerDistancia(VL53L0X &sensor){ 
  double distanciaCm = sensor.readRangeContinuousMillimeters() * 0.1 - OFFSET_SENSOR_CM; // lee las mediciones del sensor ToF usando la llamada nativa, en lugar de leer el registro
  if(distanciaCm > DISTANCIA_MAXIMA_CM) 
  {  
    distanciaCm   = DISTANCIA_MAXIMA_CM;
  }
  // si se quiere hacer el filtro de muestras, se hace aqui.
    return distanciaCm;
}

// =================================================================================
 
//Funcion de configuracion 
void configurarSensorToF(VL53L0X &sensor){ // no es necesario poner aqui el identificador del sensor
  Wire.begin();                                                       
  sensor.setTimeout(500);                                             
  while (!sensor.init())                                              
  {                                                                   
    Serial.println("Falla de conexión VL53L0X");                                                            
    delay(1000);                                                      
  }                                                                   
  sensor.setSignalRateLimit(0.25);                                    
  sensor.setMeasurementTimingBudget(33000);        //Actualiza la medición cada {Argumento}[us]       <-- ajusta tiempo maximo permitido para una medicion               
  sensor.startContinuous(40);                        //toma medidas cada {Argumento}[ms]                <-- el argumento ajusta el tiempo Entre mediciones, ajusta el T_muestreo
  //se dejan 33ms de medicion, y 40ms entre mediciones (hay 7ms de holgura entre cada medicion)
}  

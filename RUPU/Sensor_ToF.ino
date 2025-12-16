#include "config.h"

void configurarSensorToF(){
  sensor.setTimeout(500);                                             
  while (!sensor.init()) {                                                  
    Serial.println("Falla VL53L0X");
    delay(1000);                                                      
  }                                                                   
  sensor.setSignalRateLimit(0.25);
  sensor.setMeasurementTimingBudget(33000);                      
  sensor.startContinuous(40);
}

double leerDistancia(){ 
  double distanciaCm = sensor.readRangeContinuousMillimeters() * 0.1 - OFFSET_SENSOR_CM;
  if(distanciaCm > DISTANCIA_MAXIMA_CM) {  
    distanciaCm = DISTANCIA_MAXIMA_CM;
  }
  return distanciaCm;
}
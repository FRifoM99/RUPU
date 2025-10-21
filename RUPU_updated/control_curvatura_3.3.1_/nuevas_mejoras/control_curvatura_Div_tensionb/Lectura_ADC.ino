/* en este archivo estara la funcion que toma las lecturas del ADC para comprobar mediante 
el divisor de tension, si la bateria esta cargada o no.
*/


float Leer_nivel_bateria() {
  const int N = 10; // número de lecturas para promediar
  int sum = 0;
  for (int i = 0; i < N; i++) {
      sum += analogRead(ADC_PIN_BATERIA); // pin de ADC de batería
  }
  float raw_avg = sum / (float)N;
  float Vadc = (raw_avg / 4095.0) * 3.0;       // Voltaje en el pin ADC
  float Vbat = Vadc / divider_ratio;           // Voltaje real de la batería
  float Vbat_corr = 1 * Vbat + 0.92;
  return Vbat_corr;
}

void Actualizar_Flag_bateria() {
  unsigned long now = millis();
  if (now - t_adc >= ADC_INTERVAL) {
    float VOLTAJE_BATTERY = Leer_nivel_bateria();
    LOW_BATTERY = (VOLTAJE_BATTERY < UMBRAL_BATERIA_BAJA);
    t_adc = now;
  }  
}


void Configurar_ADC() {
  analogSetPinAttenuation(ADC_PIN_BATERIA, ADC_11db); // Para medir hasta ~3.9V
}
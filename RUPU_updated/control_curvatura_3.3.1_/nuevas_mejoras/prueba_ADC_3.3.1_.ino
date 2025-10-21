const int ADC_PIN = 36;          // Pin donde está conectado el divisor
const float Rtop = 9920.0;       // Valor medido
const float Rbot = 5080.0;       // Valor medido
const float divider_ratio = Rbot / (Rtop + Rbot); // 0.339 aprox.

void setup() {
  Serial.begin(115200);
  analogSetPinAttenuation(ADC_PIN, ADC_11db); // Para medir hasta ~3.9V
  delay(1000);
}

void loop() {
  const int N = 50;     // Promediamos N lecturas para estabilidad
  long suma = 0;
  for(int i=0; i<N; i++){
    suma += analogRead(ADC_PIN);
    delay(5);           // Pequeña espera entre lecturas
  }

  float raw_avg = suma / (float)N;
  float Vadc = (raw_avg / 4095.0) * 3.0;       // Voltaje en el pin ADC
  float Vbat = Vadc / divider_ratio;           // Voltaje real de la batería
  float Vbat_corr = 1 * Vbat + 0.92;


  Serial.print("Vadc = "); Serial.print(Vadc, 3); Serial.print(" V, ");
  Serial.print("Vbat = "); Serial.print(Vbat_corr, 3); Serial.println(" V");

  delay(1000); // Actualiza 1 vez por segundo
}

/*
Este programa permite la lectura de los pulsos de salida de los encoders
dispuestos en cada una de las ruedas del RMD (RUPU). 

- Requisitos: Librería ESP32Encoder.h <https://github.com/madhephaestus/ESP32Encoder>
- Configuración: 
    1. Se activa la resistencia interna de pull-up en los pines de los encoders.
    2. Se conectan los pines A y B de cada encoder usando attachFullQuad() para obtener máxima resolución.
    3. Los contadores se inicializan en 0 al inicio del programa.
- Uso: 
    - La función getCount() retorna el conteo acumulado de pulsos del encoder.
    - Se debe verificar que el incremento/disminución de los valores coincida con el sentido de giro de las ruedas (adelante/atrás).
- Notas de actualización:
    - Se reemplazó Encoder.h (no compatible con ESP32) por ESP32Encoder.h.
    - Se utilizan métodos nuevos: attachFullQuad(), clearCount() y getCount().
    - Compatible con gestor de placa ESP32 v3.3.1.
*/

#include <ESP32Encoder.h>

ESP32Encoder encoder_izq;
ESP32Encoder encoder_der;

/* se actualiza la biblioteca de Encoder.h a ESP32Encoder.h */

void setup() {
  Serial.begin(115200);
  configurarEncoder();
}

void loop() {                       //Se imprime en pantalla las cuentas de los encoder utilizando la funcion encoder.read()
  Serial.print(encoder_izq.getCount()); 
  Serial.print(" - ");
  Serial.print(encoder_der.getCount());
  Serial.println();
}

void configurarEncoder() {
  ESP32Encoder::useInternalWeakPullResistors = puType::up;; // Configura resistencias de pull-up internas
  encoder_izq.attachFullQuad(23, 5); // Conecta los pines del encoder al ESP32
  encoder_der.attachFullQuad(19, 18); 
  encoder_izq.clearCount(); // Inicializa el contador del encoder en 0
  encoder_der.clearCount();
}


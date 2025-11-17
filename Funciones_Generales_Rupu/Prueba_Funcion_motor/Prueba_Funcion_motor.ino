// RUPU - Prueba simple de motor()
// Pide un número (izquierda) y luego otro (derecha), llama motor(izq, der),
// espera 2 segundos y repite.

#include <analogWrite.h>

// Declaradas en tu archivo actuadores.ino
void motor(int vel_izq, int vel_der);

// Tiempo máximo para tipear (ms)
static const unsigned long SERIAL_TIMEOUT_MS = 30000;

// Lee un entero bloqueante, mostrando un prompt
int readIntBlocking(const char* prompt) {
  Serial.println();
  Serial.print(prompt);
  // Asegura tiempo suficiente para tipear
  Serial.setTimeout(SERIAL_TIMEOUT_MS);

  // Espera a que llegue algo
  while (Serial.available() == 0) { /* esperar */ }

  // Toma el entero
  long v = Serial.parseInt(); // admite signos +/- y espacios
  // Limpia el resto de la línea (si quedó basura)
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') break;
  }
  return (int)v;
}

void setup() {
  Serial.begin(115200);

  Serial.println("=== Tester simple motor(izq, der) ===");
  Serial.println("Ingresa enteros (p. ej. -1023..1023) primero IZQ y luego DER.");
  Serial.println("Ej: 300 (ENTER), luego 300 (ENTER) => avanza.");
}

void loop() {
  // 1) Pide IZQUIERDA
  int vL = readIntBlocking("Velocidad IZQUIERDA: ");

  // 2) Pide DERECHA
  int vR = readIntBlocking("Velocidad DERECHA:  ");

  // 3) Ejecuta
  Serial.print("Llamando motor(");
  Serial.print(vL);
  Serial.print(", ");
  Serial.print(vR);
  Serial.println(")");
  motor(vL, vR);

  // 4) Espera 2s y repite
  delay(2000);
}

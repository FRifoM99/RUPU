/*
Este programa permite la lectura de los sensores IR conectados a traves del multiplexor CD74HC4067
-Requisitos:
-Cofiguracion: Verificar la lectura de los arreglos de sensores IR, observando por consola los valores (12-bits)
               desplegados en forma de columnas (1-16), las primeras 8 columnas corresponden a los primeros 8 canales del multiplexor (MUX1),
               donde esta conectado el arreglo de sensores frontal, el resto de columnas corresponden a las mediciones del arreglo de sensores
               posterior, que esta conectado entre los canales 8-15 del MUX1.
*/

#define MUX_PIN_S0 25
#define MUX_PIN_S1 26
#define MUX_PIN_S2 2
#define MUX_PIN_S3 4

#define MUX_PIN_SIGNAL 39


void setup() {
  Serial.begin(115200);
  configurarMUX();

}

void loop() {
  leerSensores();
}

void configurarMUX() {
//Pines de control del multiplexor
  pinMode(MUX_PIN_S0, OUTPUT);     //PIN S0
  pinMode(MUX_PIN_S1, OUTPUT);     //PIN S1
  pinMode(MUX_PIN_S2, OUTPUT);      //PIN S2
  pinMode(MUX_PIN_S3, OUTPUT);      //PIN S3
}

void seleccionarCanal(byte canal) {
  digitalWrite(MUX_PIN_S0, canal & 0x01);   //Permite extraer el bit 2^0 del numero canal, el valor se asigna al pin de control S0
  digitalWrite(MUX_PIN_S1, canal & 0x02);   //Permite extraer el bit 2^1 del numero canal, el valor se asigna al pin de control S1
  digitalWrite(MUX_PIN_S2, canal & 0x04);   //Permite extraer el bit 2^2 del numero canal, el valor se asigna al pin de control S2
  digitalWrite(MUX_PIN_S3, canal & 0x08);   //Permite extraer el bit 2^3 del numero canal, el valor se asigna al pin de control S3 
}

void leerSensores() {
for (byte canal = 0; canal <= 15; canal++) {
    seleccionarCanal(canal);
    int valor = analogRead(MUX_PIN_SIGNAL);
    Serial.print(valor);
    Serial.print(" - "); // Separador entre lecturas
  }
  Serial.println(); // Nueva línea al final de cada lectura completa
}



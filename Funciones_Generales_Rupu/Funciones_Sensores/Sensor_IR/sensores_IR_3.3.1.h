#pragma once

#include <Arduino.h>



/**
 * @brief Configura los pines del multiplexor de los sensores IR.
 *
 * Define los pines S0–S3 como salidas para seleccionar cada sensor IR
 * a través del MUX. Debe ejecutarse en el setup() antes de leer sensores.
 */
void configurarMUX();

/**
 * @brief Calibra los sensores IR.
 *
 * Registra los valores mínimos y máximos de cada sensor al pasar
 * sobre la superficie blanca y la línea negra, respectivamente.
 * Estos valores se usan luego para normalizar las lecturas y 
 * discriminar si la linea es blanca o negra
 */
void calibrarSensores();

/**
 * @brief Actualiza los valores máximos y mínimos del sensor indicado.
 *
 * Compara la lectura actual con los valores guardados para ese sensor,
 * actualizando el máximo o el mínimo según corresponda.
 * Se usa durante la calibración.
 *
 * @param i Índice del sensor (0–15, considerando frontales y posteriores).
 * @param lectura Valor analógico leído del sensor.
 */
void setMaxMin(int i, float lectura);

/**
 * @brief Selecciona el canal del MUX correspondiente al sensor IR indicado.
 *
 * Usa los pines S0–S3 para activar el canal deseado y permitir la lectura
 * del sensor IR seleccionado mediante el pin analógico de señal.
 *
 * @param canal Número de canal del MUX (0–15).
 */
void seleccionarCanalMUX(byte canal);


/**
 * @brief Calcula la posición de la línea detectada.
 *
 * Usa las lecturas de los sensores IR frontales o posteriores
 * para estimar la posición relativa del centro de la línea respecto al robot.
 *
 * @param frontal Si es true, usa los sensores frontales; si es false, los posteriores.
 * @return Valor numérico proporcional a la posición de la línea respecto al centro.
 */
 
float getPosition(bool frontal);

/**
 * @brief Calcula el nivel normalizado de un sensor IR específico.
 *
 * Convierte la lectura analógica de un sensor IR a un porcentaje (0–100%)
 * usando los valores mínimos y máximos calibrados. Aplica un umbral
 * para descartar ruido y corrige según si la línea es negra o blanca.
 *
 * @param index Índice del sensor dentro del grupo frontal o posterior (0–7).
 * @param frontal Indica si el sensor pertenece al grupo frontal (true) o posterior (false).
 * @return Nivel del sensor en porcentaje (0–100%).
 */
float calcularNivelSensor(byte index, bool frontal);

/**
 * @brief Calcula las velocidades lineales de cada rueda usando los encoders.
 *
 * Usa las lecturas de los encoders de ambas ruedas para determinar
 * las velocidades lineales, aplica un filtro promediado para suavizar
 * la señal y calcula la velocidad lineal y angular del robot.
 *
 * @note Requiere que las variables globales de encoders y constantes
 *       como CPR, N_REDUCCION y DIST_RUEDAS estén definidas.
 */
void Calculo_Velocidades();

/**
 * @brief Determina la curvatura del trayecto según las velocidades de las ruedas.
 *
 * Evalúa si el robot se encuentra en una recta o curva y calcula la
 * curvatura del recorrido usando las distancias recorridas por las ruedas.
 * Esta información se utiliza para ajustar la velocidad y el control de giro.
 *
 * @note Usa variables globales de posición y velocidad acumuladas.
 */
void Curvatura_Pista();

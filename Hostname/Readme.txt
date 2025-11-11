Para la libreria de comunicacion, se hace uso de las librerias actualizadas en 

control_curvatura_3.3.1_ > nuevas_mejoras > control_curvatura_Div_tensionb

Notar que es necesario el uso de las siguientes librerias (Tal como declara "dependencies.txt")

# =======================
# DEPENDENCIAS RUPU ACTUALIZADO
# =======================

[MANTENER]
- WiFi.h (nativa)       # Conexión Wi-Fi y gestión básica de red en ESP32
- Wire.h (nativa)       # Comunicación I2C (usada por sensores)
- VL53L0X.h (1.3.1)       # Sensor de distancia ToF (Pololu)

[ACTUALIZAR / REEMPLAZAR]
- Encoder.h        -> Reemplazar por ESP32Encoder.h (0.11.8)
                     (optimizado para ESP32, usa periférico PCNT en lugar de interrupciones simples)
- analogWrite.h    -> Eliminar y reemplazar por API LEDC nativa de ESP32:
                     ledcAttach(), ledcWrite(), ...

[ELIMINAR / INNECESARIAS]
- esp_wifi.h       # Solo necesario si se requiere acceso avanzado al stack Wi-Fi (potencia TX, protocolos, PS modes).
                   En el código actual no se usa, se elimina.

OCUPA UNA INSTANCIA DE LA LIBRERIA esp_wifi.h PARA DESACTIVAR EL POWER SAVE DEL WIFI QUE
LA LIBRERIA wifi.h NO PERMITE MODIFICAR. 
Aunque existe una funcion similar en el wifi.h  ==> Wifi.setSleep(false), por el momento dejare la 
libreria esp_wifi.h, aun asi es nativa.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%    Vamos con lo nuevo
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Se modifico 1 archivo y crearon 2 (para la libreria de comunicacion), el archivo "comunicacion_3.3.1_.ino" debe de ser eliminado 
para no entrar en conflicto con los archivos creados

Archivo modificado: "control_curvatura_Div_tensionb.ino"
Archivos creados:   "ComunicacionRobot.h"
                    "ComunicacionRobot.cpp"

Notar que los demas archivos no sufrieron cambios

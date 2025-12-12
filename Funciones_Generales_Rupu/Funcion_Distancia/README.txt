# Función PID distancia 

1. double PID_distancia(double y, double ref, double &error_ant, double &error_integral, double kp, double ki, double kd, double limite, String direccion)

PID distancia, necesita como entrada los errores 

Parámetros:
-y: distancia medida con el sensor ToF
-ref: referencia
-error_ant: error anterior
-error_integral: error integral
-kp,ki,dp: parametros del controlador
-limite: máxima distancia
-dirección: "DIRECTO", "INVERSO".



2. double PID_distancia2(double y, double ref, double kp, double ki, double kd, double limite, String direccion)

PID distancia, calcula los errores dentro de la misma función
Parámetros:
-y: distancia medida con el sensor ToF
-ref: referencia
-kp,ki,dp: parametros del controlador
-limite: máxima distancia
-dirección: "DIRECTO", "INVERSO".

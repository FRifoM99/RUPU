/*función calculo PID distancia*/
double PID_distancia(double y, double ref, double &error_ant, double &error_integral, double kp, double ki, double kd, double limite, String direccion)
{
    int satMax = 0.1;
    static long posicion_prev = 0;
    long posicion_actual = encoder_der.read();
    static unsigned long t_prev = 0;
    unsigned long t_actual = millis();
    double dt = (t_actual - t_prev)/1000.0;
    double error; 
   
    //dirección 
    if (direccion == "DIRECTO"){
        error = ref - y;
    }
    else{ 
        error = y - ref;
    }
    //error integral
    error_integral += error*dt;

    double i_term = ki*error_integral;

    if (i_term > limite){
        error_integral = limite/ki;
    }
    else if (i_term < -limite){
        error_integral = -limite/ki;
    }

    double d_term = kd*(error - error_ant)/dt;
    double u = kp*error + i_term + d_term;

    //se actualiza el error
    error_ant = error;

    //se limita la salida
    if (u > limite*(1+satMax)+1){
        u = limite*(1+satMax)+1;
    }
    if (u < -limite*(1-satMax)-1){
        u = -limite*(1-satMax)-1;
    }

    float distancia = (posicion_actual - posicion_prev)*2*PI*0.0215*100;
    
    //visualizar datos
    Serial.print("ref: ");
    Serial.print(ref);
    Serial.print("\t");

    Serial.print("y: ");
    Serial.print(y);
    Serial.print("\t");

    Serial.print("dist(cm): ");
    Serial.print(distancia);
    Serial.print("\t");
    
    Serial.print("u: ");
    Serial.print(u);
    Serial.print("\t");

    Serial.print("dt(s): ");
    Serial.print(dt);
    Serial.print("\t");
    
    // se actualizan las variables
    posicion_prev = posicion_actual;
    t_prev = t_actual;

    return u;
}


double PID_distancia2(double y, double ref, double kp, double ki, double kd, double limite, String direccion)
{
    static double e0_d = 0;
    static double e1_d = 0;
    static double e2_d = 0;
    
    static double u0_d = 0;
    static double u1_d = 0;

    //tiempo en seg
    static unsigned long t_prev = 0;
    unsigned long t_now = millis();
    double dt = (t_now - t_prev)/1000.0;
    static long posicion_prev = 0;
    long posicion_actual = encoder_der.read();
    double error;

    if (direccion == "DIRECTO"){
        error = ref - y;
    }     
    else if (direccion == "INVERSO"){
        error = y - ref;
    } 
    e0_d = error;

    double a0 = kp + (kd/dt);
    double a1 = -kp + (ki*dt) -2*(kd/dt);
    double a2 = kd/dt;

    u0_d = u1_d + a0*e0_d + a1*e1_d + a2*e2_d;

    //saturación
    if (u0_d > limite){
        u0_d = limite;
    } 
    if (u0_d < -limite){
        u0_d = -limite;
    } 

    float distancia = (posicion_actual - posicion_prev)*2*PI*0.0215*100;

    //visualizar datos
    Serial.print("ref: ");
    Serial.print(ref);
    Serial.print("\t");

    Serial.print("y: ");
    Serial.print(y);
    Serial.print("\t");

    Serial.print("dist(cm): ");
    Serial.print(distancia);
    Serial.print("\t");
    
    Serial.print("u: ");
    Serial.print(u0_d);
    Serial.print("\t");

    Serial.print("dt(s): ");
    Serial.print(dt);
    Serial.print("\t");

    // se actualizan las variables
    e2_d = e1_d;
    e1_d = e0_d;
    u1_d = u0_d;
    posicion_prev = posicion_actual;
    t_prev = t_now;

    return u0_d;
}
#include "RUPU_Sensores.h"

void inicializar_sensores() {
  pinMode(PIN_LED, OUTPUT); // Pin 2
  pinMode(4, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  
  // Inicialización de hardware para encoders
  ESP32Encoder::useInternalWeakPullResistors = puType::up;;
  encoder_der.attachHalfQuad(5, 23);
  encoder_izq.attachHalfQuad(18, 19);
  
  configuracion_sensor_d();
}

void calibrarSensores() {
  for (int i = 0; i <= 15; i++) {
    sensoresMax[i] = 0;
    sensoresMin[i] = 4095;
  }
  int vcal=200;
  encoder_der.clearCount();
  encoder_izq.clearCount();
  
  unsigned long t_start_calib = millis();
  while(abs(RADIO_RUEDA*PI*(encoder_der.getCount()-encoder_izq.getCount())*REDUCCION_N/(CPR_ENCODER*DISTANCIA_L))< 4*PI){
    motor(-vcal,vcal);
    for(byte i=0;i<=15;i++){
      digitalWrite(25, (i & 0x01) ? HIGH : LOW);
      digitalWrite(26, (i & 0x02) ? HIGH : LOW);
      digitalWrite(PIN_LED, (i & 0x04) ? HIGH : LOW);
      digitalWrite(4, (i & 0x08) ? HIGH : LOW);
      setMaxMin(i, analogRead(39));
    }
    // Evita el loop infinito si los encoders no reportan movimiento
    if (millis() - t_start_calib > 10000) {
      Serial.println("ERROR: Timeout en calibracion (Revisa los encoders)");
      break;
    }
  }

  digitalWrite(25, (4 & 0x01) ? HIGH : LOW);
  digitalWrite(26, (4 & 0x02) ? HIGH : LOW);
  digitalWrite(PIN_LED, (4 & 0x04) ? HIGH : LOW);
  digitalWrite(4, (4 & 0x08) ? HIGH : LOW);
  
  t_start_calib = millis();
  while((((1-2*VALOR_LINEA)*100*(analogRead(39)-sensoresMin[4])/(sensoresMax[4]-sensoresMin[4])) + 100*VALOR_LINEA)<80){
    motor(-vcal,vcal);
    if (millis() - t_start_calib > 5000) {
      Serial.println("ERROR: Timeout centrando linea");
      break;
    }
  }
  motor(0,0);
}

void setMaxMin(int i, float aread) {
  if (sensoresMax[i] < aread) {
    sensoresMax[i] = aread;
  }
  if (sensoresMin[i] > aread) {
    sensoresMin[i] = aread;
  }
}

float getposition(bool direccion){
  float sensorNormalizado[8];
  float sum1=0;
  float area=0;
  if(direccion){
    for(byte i=0;i<=7;i++){
      digitalWrite(25, (i & 0x01) ? HIGH : LOW);
      digitalWrite(26, (i & 0x02) ? HIGH : LOW);
      digitalWrite(PIN_LED, (i & 0x04) ? HIGH : LOW);
      digitalWrite(4, (i & 0x08) ? HIGH : LOW);
      sensorNormalizado[i]=((1-2*VALOR_LINEA)*100*(analogRead(39)-sensoresMin[i])/(sensoresMax[i]-sensoresMin[i])) + 100*VALOR_LINEA; 
      if( sensorNormalizado[i]<30){
        sensorNormalizado[i]=0;
      }
    }
  }
  else{
    for(byte i=0;i<=7;i++){
      digitalWrite(25, ((i+8) & 0x01) ? HIGH : LOW);
      digitalWrite(26, ((i+8) & 0x02) ? HIGH : LOW);
      digitalWrite(PIN_LED, ((i+8) & 0x04) ? HIGH : LOW);
      digitalWrite(4, ((i+8) & 0x08) ? HIGH : LOW);
      sensorNormalizado[i]=((1-2*VALOR_LINEA)*100*(analogRead(39)-sensoresMin[i+8])/(sensoresMax[i+8]-sensoresMin[i+8])) + 100*VALOR_LINEA; 
      if( sensorNormalizado[i]<10){
        sensorNormalizado[i]=0;
      }
    }
  }
  for(int i=0; i<=7;i++){
    sum1+=sensorNormalizado[i]*(-28+8*i);
    area+=sensorNormalizado[i];
  }
  if(area==0){
    Serial.println("DEBERIAMOS PARAR");
    return 0;
  }
  else{
      return sum1/area; 
  }
}

void configuracion_sensor_d(){
  Wire.begin();                                                       
  sensor.setTimeout(500);                                             
  while (!sensor.init())                                                 
  {                                                                   
    Serial.println("Failed to detect and initialize sensor!");        
    delay(1000); 
  }                                                                   
  sensor.setSignalRateLimit(0.25);                                    
  sensor.setMeasurementTimingBudget(33000);                           
  sensor.startContinuous(); 
}

double distancia(){
  double dist = sensor.readReg16Bit(sensor.RESULT_RANGE_STATUS + 10)*0.100 - 2; 
  if(dist>80){ 
    cuenta=0;
    dist=80;
  }
  return dist;
}

void velocidades(){
  t_actual = millis() - t_svel;
  if(t_actual>=10){
    v_r[1]=v_r[0];
    v_r[2]=v_r[1];
    v_r[3]=v_r[2];
    v_r[4]=v_r[3];
    v_l[1]=v_l[0];
    v_l[2]=v_l[1];
    v_l[3]=v_l[2];
    v_l[4]=v_l[3];
    v_r[0]=2*PI*RADIO_RUEDA*encoder_der.getCount()*REDUCCION_N/(t_actual*0.001*CPR_ENCODER); 
    v_l[0]=2*PI*RADIO_RUEDA*encoder_izq.getCount()*REDUCCION_N/(t_actual*0.001*CPR_ENCODER); 
    encoder_der.clearCount();
    encoder_izq.clearCount();
    t_svel=millis();
  }
  v_r[0]=(0.2*(v_r[1]+v_r[2]+v_r[3]+v_r[4])/4) + 0.8*v_r[0];
  v_l[0]=(0.2*(v_l[1]+v_l[2]+v_l[3]+v_l[4])/4) + 0.8*v_l[0];
  Input_vel=0.5*(v_r[0]+v_l[0]); 
  ang_vel=0.5*(v_r[0]-v_l[0])/DISTANCIA_L;
}

void curvatura_pista(){
  t_actual = millis() - t_arco;
  if(t_actual>=20){
    a_sl+=v_l[0]*t_actual*0.001;
    a_sr+=v_r[0]*t_actual*0.001;
    b_sl+=v_l[0]*t_actual*0.001;
    b_sr+=v_r[0]*t_actual*0.001;
    c_sl+=v_l[0]*t_actual*0.001;
    c_sr+=v_r[0]*t_actual*0.001;
    t_arco=millis();
  }
  a_s=0.5*(a_sr+a_sl);
  b_s=0.5*(b_sr+b_sl);
  c_s=0.5*(c_sr+c_sl);
  if(Input_vel<0.1){
    a_curvatura=0;
    b_curvatura=0;
    c_curvatura=0;
    a_sl=0; a_sr=0;
    b_sl=0; b_sr=0;
    c_sl=0; c_sr=0;
  }
  else if (a_s>=3 && b_s>=10){
    b_curvatura=abs((0.5*(b_sr-b_sl))/(DISTANCIA_L*b_s));
    b_sl=0; b_sr=0;
  }
  else if(a_s>=3 && a_s==b_s){
    b_sl=0; b_sr=0;
  }
  else if (a_s>=6 && c_s>=10){
    c_curvatura=abs((0.5*(c_sr-c_sl))/(DISTANCIA_L*c_s));
    c_sl=0; c_sr=0;
  }
  else if(a_s>=6 && a_s==c_s){
    c_sl=0; c_sr=0;
  }
  else if(a_s>=10){
    a_curvatura=abs((0.5*(a_sr-a_sl))/(DISTANCIA_L*a_s));
    a_sl=0; a_sr=0;
  }
  if(a_curvatura<0.01 && b_curvatura<0.01 && c_curvatura<0.01){
    recta=1;
  }
  else if(a_curvatura>0.02 && b_curvatura>0.02 && c_curvatura>0.02){
    recta=0;
  }
  if(recta){
    if(a_curvatura >= b_curvatura && a_curvatura >= c_curvatura){
      curvatura=a_curvatura;
    }
    else if(b_curvatura >= a_curvatura && b_curvatura >= c_curvatura){
      curvatura=b_curvatura;
    }
    else if(c_curvatura >= a_curvatura && c_curvatura >= b_curvatura){
      curvatura=c_curvatura;
    }
    else{
      curvatura=a_curvatura;
    }
  }
  else{
    if(a_curvatura < b_curvatura && a_curvatura < c_curvatura){
      curvatura=a_curvatura;
    }
    else if(b_curvatura < a_curvatura && b_curvatura < c_curvatura){
      curvatura=b_curvatura;
    }
    else if(c_curvatura < a_curvatura && c_curvatura < b_curvatura){
      curvatura=c_curvatura;
    }
    else{
      curvatura=a_curvatura;
    }
  }
}

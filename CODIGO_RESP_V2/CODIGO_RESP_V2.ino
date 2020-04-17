#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define A 2                       //Variable A a pin digital 2 (DT en módulo)
#define B 4                       //Variable B a pin digital 4 (CLK en módulo)
#define push  3                   //Variable Push

#define PINdireccion 9
#define PINpasos 8

unsigned int FR=8, VA=215, IE=2, SEN=1;
unsigned int FR_TEN=8, VA_TEN=215, IE_TEN=2, SEN_TEN=1;
unsigned int pantalla = 1, limpiar_pantalla=1;
unsigned short opcion = 1, compresion = 1;
unsigned int cuenta = 0;
unsigned int pulsos=500, retraso_compresion=0xFC40, retraso_descompresion=0xF880, pulsos_actuales=0;

LiquidCrystal_I2C lcd(0x27,20,4);
uint8_t arrow[8] = {0x0, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00, 0x00};   //CARACTER DE FLECHA PARA PANTALLA

void setup(){
  pinMode(13, OUTPUT);  // Pin para buzzer
  pinMode(12, OUTPUT);  // Alarma no puede inspirar
  pinMode(11, OUTPUT);  // Alarma presión baja
  pinMode(10, OUTPUT);  // Alarma presión alta
  pinMode(PINdireccion, OUTPUT);
  pinMode(PINpasos, OUTPUT);
  pinMode(5, INPUT_PULLUP);     // Final de carrera
  pinMode(A, INPUT_PULLUP);     // A como entrada
  pinMode(B, INPUT_PULLUP);     // B como entrada
  pinMode(push,INPUT_PULLUP);   // Push como entrada
  Serial.begin(9600);   // Incialización de comunicacion serie a 9600 bps

  attachInterrupt(digitalPinToInterrupt(push),enter, RISING); // Interrupción sobre pin push
  attachInterrupt(digitalPinToInterrupt(A),encoder, RISING);     // Interrupción sobre pin A

  // Timer 1 de 4.1943 s
  TCCR1A=0x00;                                      // Declaración del funcionamiento normal del timer 1 (10 bits)
  TCCR1B=0x00;                                      // Timer 1 apagado (hasta que no se use el encoder)
  TCNT1=0x0000;                                     // Cuenta inicial T1 para 4.1943 segundos
  TIMSK1|=(1<<TOIE1);                               // Activación del OverFlow

  // Timer 3 de 4.1943 s
  TCCR3A=0x00;                                      // Declaración del funcionamiento normal del timer 3 (10 bits)
  TCCR3B=0x00;                                      // Timer 3 apagado (hasta que no se use el encoder)
  TCNT3=0xFC40;                                     // Cuenta inicial T3 para 4.1943 segundos
  TIMSK3|=(1<<TOIE3);                               // Activación del OverFlow

  lcd.init();                 // Iniciar la LCD
  lcd.backlight();            // Activar Backligth
  lcd.createChar(0, arrow);   // Crear el caracter flecha

  lcd.clear();
  lcd.setCursor(1,1);  
  lcd.print("<FUERZA ZACATECAS>");
  moverMotorAtras();
  Serial.println("terminado");
  TCCR3B=0x05;
  lcd.clear();

  SEN=analogRead(A15);
  mostrarDatos();
}

void loop(){

  SEN=analogRead(A15);
  
  switch(pantalla){
    case 1:
      limpiarPantalla();
      mostrarDatos();
    break;
    case 2:
      limpiarPantalla();
      mostrarMenu();
    break;
    case 3:
      limpiarPantalla();
      mostrarFR();
    break;
    case 4:
      limpiarPantalla();
      mostrarVA();
    break;
    case 5:
      limpiarPantalla();
      mostrarIE();
    break;
    case 6:
      limpiarPantalla();
      mostrarSEN();
    break;
  }
  
  mostrarAlarmas();
}

void enter(){
  TCCR1B=0x05;                                          // Inicia el timer 1 con el máximo preescalador de 1024
  TCNT1=0x0000;                                         // Cuenta inicial T1 para 4.1943 segundos
  digitalWrite(13,HIGH);
  delay(10);
  digitalWrite(13,LOW);
  resetearAlarmas();
  if(pantalla==1){
    limpiar_pantalla=1;
    pantalla=2;
  }else if(pantalla==2){
    if(opcion==1){
      limpiar_pantalla=1;
      pantalla=3;
    }else if(opcion==2){
      limpiar_pantalla=1;
      pantalla=4;
    }else if(opcion==3){
      limpiar_pantalla=1;
      pantalla=5;
    }else if(opcion==4){
      limpiar_pantalla=1;
      pantalla=6;
    }
  }else{
    limpiar_pantalla=1;
    pantalla=2;
    FR=FR_TEN;
    VA=VA_TEN;
    IE=IE_TEN;
    SEN=SEN_TEN;
    actualizarMovimiento();
  }
}

void encoder(){
  TCCR1B=0x05;                        // Inicia el timer 1 con el máximo preescalador de 1024
  TCNT1=0x0000;                       // Cuenta inicial T1 para 4.1943 segundos
  if (pantalla==2){
    static unsigned long ultimaInterrupcion = 0;
    unsigned long tiempoInterrupcion = millis();        
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {
      if (digitalRead(B) == 1){                      // Si B es HIGH, sentido horario
        limpiar_pantalla=1;
        if(opcion<4){
          opcion++;
        }else{
          opcion=1;
        }
       }else{
          limpiar_pantalla=1;
          if (opcion>1){
            opcion--;
          }else{
            opcion=4;
          }
        }
      ultimaInterrupcion = tiempoInterrupcion;          // Guarda valor actualizado del tiempo
    }
  }else if(pantalla==3){
    static unsigned long ultimaInterrupcion = 0;
    unsigned long tiempoInterrupcion = millis();        
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {
      if (digitalRead(B)==1){
        limpiar_pantalla=1;
        if(FR_TEN<30){
          FR_TEN++;
        }
      }else{
        limpiar_pantalla=1;
        if(FR_TEN>8){
          FR_TEN--;
        }
      }
      ultimaInterrupcion = tiempoInterrupcion;          // Guarda valor actualizado del tiempo
    }
  }else if(pantalla==4){
    static unsigned long ultimaInterrupcion = 0;
    unsigned long tiempoInterrupcion = millis();        
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {
      if (digitalRead(B)==1){
        limpiar_pantalla=1;
        if(VA_TEN<800){
          VA_TEN++;
        }
      }else{
        limpiar_pantalla=1;
        if(VA_TEN>215){
          VA_TEN--;
        }
      }
      ultimaInterrupcion = tiempoInterrupcion;          // Guarda valor actualizado del tiempo
    }
  }else if(pantalla==5){
    static unsigned long ultimaInterrupcion = 0;
    unsigned long tiempoInterrupcion = millis();        
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {
      if (digitalRead(B)==1){
        limpiar_pantalla=1;
        if (IE_TEN<4){
          IE_TEN++;
        }else{
          IE_TEN=1;
        }
      }else{
        limpiar_pantalla=1;
        if (IE_TEN>1){
          IE_TEN--;
        }else{
          IE_TEN=4;
        }
      }
      ultimaInterrupcion = tiempoInterrupcion;          // Guarda valor actualizado del tiempo
    }
  }else if(pantalla==3){
    limpiar_pantalla=1;
    /*
    if (digitalRead(B)==1){
      limpiar_pantalla=1;
      SEN_TEN++;
    }else{
      limpiar_pantalla=1;
      SEN_TEN--;
    }
    */
  }
}

// Función del timer
ISR(TIMER1_OVF_vect){
    TCCR1B=0x00;              // Detiene el timer 1
    TCNT1=0x0000;             // Cuenta inicial T1 para 4.1943 segundos

    limpiar_pantalla=1;
    pantalla=1;
    
    FR_TEN=FR;                // Si pasan 4 segundos y se sale, las tentativas se resetean a lo real
    VA_TEN=VA;
    IE_TEN=IE;
}

void limpiarPantalla(){
  if(limpiar_pantalla==1){
    lcd.clear();
    limpiar_pantalla=0;
  }
}

void mostrarDatos(){
  lcd.setCursor(1,0);  
  lcd.print("FREC. RESP:  ");
  lcd.print(FR);
  lcd.setCursor(1,1);  
  lcd.print("VOL. AIRE:   ");
  lcd.print(VA);
  lcd.setCursor(1,2);  
  lcd.print("REL. I/E:    ");
  lcd.print("1:"); lcd.print(IE);
  lcd.setCursor(1,3);  
  lcd.print("SENS:        ");
  if(SEN != analogRead(15)){
    lcd.setCursor(15,3); 
    lcd.print("   ");
    SEN=analogRead(A15);
  }
  lcd.setCursor(14,3);
  lcd.print(SEN);
}

void mostrarMenu(){
  lcd.setCursor(0,opcion-1);
  lcd.write(0);
  lcd.setCursor(1,0);
  lcd.print("Frecuencia Resp.");
  lcd.setCursor(1,1); 
  lcd.print("Volumen de Aire");
  lcd.setCursor(1,2);  
  lcd.print("Relacion I / E");
  lcd.setCursor(1,3);  
  lcd.print("Presion / deteccion");
}

void mostrarFR(){
  lcd.setCursor(5,0);
  lcd.print("FRECUENCIA");
  lcd.setCursor(4,1);
  lcd.print("RESPIRATORIA");
  lcd.setCursor(9,2);
  lcd.print(FR_TEN);
}

void mostrarVA(){
  lcd.setCursor(2,0);
  lcd.print("VOLUMEN DE AIRE");
  lcd.setCursor(9,2);
  lcd.print(VA_TEN);
}

void mostrarIE(){
  lcd.setCursor(1,1);
  lcd.print("RELACION");
  lcd.setCursor(3,2);
  lcd.print("IE");
  lcd.setCursor(12,IE_TEN-1);
  lcd.write(0);
  lcd.setCursor(13,0);
  lcd.print("1 : 1");
  lcd.setCursor(13,1);
  lcd.print("1 : 2");
  lcd.setCursor(13,2);
  lcd.print("1 : 3");
  lcd.setCursor(13,3);
  lcd.print("1 : 4");
}

void mostrarSEN(){
  lcd.setCursor(0,1);
  lcd.print("SENSOR DE PRESION.");
  if(SEN != analogRead(15)){
    lcd.setCursor(9,2); 
    lcd.print("   ");
    SEN=analogRead(A15);
  }
  lcd.setCursor(8,2);
  lcd.print(SEN);
}

void mostrarAlarmas(){
  if (SEN < 100){             // Baja presión
    digitalWrite(12, HIGH);
  }else if(SEN>900){          //Alta presión
    digitalWrite(11, HIGH);
  }else if(SEN>650){
    if (cuenta<20){
      cuenta++;
    }else{
      digitalWrite(10, HIGH);
      cuenta=0;
    }
  }else{
    cuenta=0;
  }
}

void resetearAlarmas(){
  digitalWrite(12, LOW);
  digitalWrite(11, LOW);
  digitalWrite(10, LOW);
}

void actualizarMovimiento(){
  pulsos = map(VA, 215, 800, 500, 2000);    // Compresión
  retraso_compresion = (int)(65536-((30 * pulsos)/(2000*FR)*128));        // Magia
  retraso_descompresion = (int)(65536-((30 * pulsos)/(2000*FR)*128*IE));  // Magia
  Serial.println(pulsos);
  Serial.println(retraso_compresion);
  Serial.println(retraso_descompresion);
}

// Función del timer
ISR(TIMER3_OVF_vect){
    //TCCR3B=0x00;              // Detiene el timer 3
    if (compresion==1){
      TCNT3=retraso_compresion;
      digitalWrite(PINdireccion,HIGH);
      if(pulsos_actuales<pulsos){
        if(digitalRead(PINpasos)==0){
          digitalWrite(PINpasos,HIGH);
        }else{
          digitalWrite(PINpasos,LOW);
        }
        pulsos_actuales++;
      }else{
        compresion=0;
        Serial.println("Exhala");
      }
    }else{
      TCNT3=retraso_descompresion;
      digitalWrite(PINdireccion,LOW);
      if(pulsos_actuales>0){
        if(digitalRead(PINpasos)==0){
          digitalWrite(PINpasos,HIGH);
        }else{
          digitalWrite(PINpasos,LOW);
        }
        pulsos_actuales--;
      }else{
        compresion=1;
        Serial.println("Inhala");
      }
    }
}

void moverMotorAtras(){
  digitalWrite(PINdireccion,LOW);
  Serial.println("atras");
  while(digitalRead(5)==1){         // Atrás hasta que el final de carrera se accione
    digitalWrite(PINpasos,HIGH);
    delay(3);
    digitalWrite(PINpasos,LOW);
    delay(3);
  }
  moverMotorAdelante();
}

void moverMotorAdelante(){
  digitalWrite(PINdireccion,HIGH);
  Serial.println("adelante");
  for (int i = 0; i<375; i++){    // Adelante 3 cm
    digitalWrite(PINpasos,HIGH);
    delay(3);
    digitalWrite(PINpasos,LOW);
    delay(3);
  }
}

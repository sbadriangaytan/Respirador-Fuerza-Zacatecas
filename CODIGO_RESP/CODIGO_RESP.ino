#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define A 2                       //Variable A a pin digital 2 (DT en módulo)
#define B 4                       //Variable B a pin digital 4 (CLK en módulo)
#define push  3                   //Variable Push

void mostrarDatos(void);
void mostrarInicio(void);
void limpiarAlarmas(void);

LiquidCrystal_I2C lcd(0x27,20,4);
uint8_t arrow[8] = {0x0, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00, 0x00};   //CARACTER DE FLECHA PARA PANTALLA
//int REBOTE=45; Rebote ya no es necesario después de las interrupciones 

boolean p_inicio=false;
boolean pulso=false;                  // Variable del botón del encoder 

boolean salida_FR=false;              // Variable de salida para submenús
boolean MENU_INICIO=false;            // Variable para saber la posición del menú y delimitar el encoder de 0 a 3 
volatile int POSICION = 0;            // Variable de posicion del encoder
volatile int POSICION_ANTERIOR=0;     // Para comprobar si hubo algún cambio

boolean MENU_FR=false;                // Variable para saber la posición del menú y delimitar el encoder de 8 a 40
boolean PAGINA_FR=false;              // Variable para posición de menu y hacer regresar del menú de donde se elige el valor de FR
volatile int POSICION_FR = 20;        // Variable de inicio para valor FR
volatile int POSICION_FR_TEN = 20;    // Variable de inicio para valor FR mostrado en pantalla (tentativo)
boolean salida_VC=false;              // Variable de salida para submenús

boolean MENU_VC=false;                // Variable para saber la posición del menú y delimitar el encoder de 200 a 300
boolean PAGINA_VC=false;              // Variable para posicion de menu y hacer regresar del menú de donde se elige el valor de VC
volatile int POSICION_VC = 300;       // Variable de inicio para valor VC
volatile int POSICION_VC_TEN = 300;   // Variable de inicio para valor VC mostrado en pantalla (tentativo)

boolean MENU_IE=false;                // Variable para saber posición del menú y delimitar el valor de IE
boolean PAGINA_IE=false;              // Variable para saber cuándo guardar el valor de IE
volatile int POSICION_IE = 1;         // Variable de inicio para valor VC
volatile int POSICION_IE_TEN = 1;     // Variable de inicio para valor VC mostrado en pantalla (tentativo)
boolean salida_IE=false;              // Variable de salida para submenús
String IE="vacio";

unsigned short limpiar_pantalla=0;
volatile short espera_alarma=1;
unsigned int cuenta_timer2;

// Almacena valor anterior de la variable POSICION
// Variable POSICION con valor inicial de 50 y definida

void setup() {
  pinMode(13, OUTPUT);  // Pin para buzzer
  pinMode(12, OUTPUT);  // Alarma no puede inspirar
  pinMode(11, OUTPUT);  // Alarma presión baja
  pinMode(10, OUTPUT);  // Alarma presión alta
  pinMode(A, INPUT);    // A como entrada
  pinMode(B, INPUT);    // B como entrada
  pinMode (push,INPUT); 
  Serial.begin(9600);   // Incialización de comunicacion serie a 9600 bps

  attachInterrupt(digitalPinToInterrupt(push),enter, CHANGE); // Interrupción sobre pin A con
  attachInterrupt(digitalPinToInterrupt(A),encoder, LOW);     // Interrupción sobre pin A con

  // Timer 1 de 4.1943 s
  TCCR1A=0x00;                                      // Declaración del funcionamiento normal del timer 1 (10 bits)
  TCCR1B=0x00;                                      // Timer 1 apagado (hasta que no se use el encoder)
  TCNT1=0x0000;                                     // Cuenta inicial T1 para 4.1943 segundos
  TIMSK1|=(1<<TOIE1);                               // Activación del OverFlow

  // Timer 2 de 16.384 ms
  ASSR=(0<<EXCLK) | (0<<AS2);
  TCNT2=0x00;
  TCCR2A=0x00;                                      // Declaración del funcionamiento normal del timer 2 (8 bits)
  TCCR2B=0x00;                                      // Timer 2 apagado (hasta que la bolsa esté inflada)
  TCNT2=0x00;                                       // Cuenta inicial T2 para 16.384 segundos
  TIMSK2|=(1<<TOIE2);                               // Activación del OverFlow
  cuenta_timer2=0;                                  // Para llegar a los 2 segundos 2000/16.384=122

  lcd.init();                 // Iniciar la LCD
  lcd.backlight();            // Activar Backligth
  lcd.createChar(0, arrow);   // Crear el caracter flecha

  /////////PANTALLA INICIO//////////////

  lcd.clear();
  lcd.setCursor(1,1);  
  lcd.print("<FUERZA ZACATECAS>");
  delay(300);
  lcd.clear();
}

void loop() {
  
  // Función para limpiar pantalla sólo 1 vez en el loop, se activa poniendo la bandera limpiar_pantalla=1;
  if(limpiar_pantalla==1){
    lcd.clear();
    limpiar_pantalla=0;
  }
  
  if(p_inicio==true){
    
    switch(POSICION){
      
      //-------------------------------CASO FRECUENCIA RESPIRATORIA--------------------------------
      case 0:
        if(PAGINA_FR==1){
          MENU_FR=true;
          MENU_INICIO=false;
          salida_FR=true;
          lcd.setCursor(5,0);
          lcd.print("FRECUENCIA");
          lcd.setCursor(4,1);
          lcd.print("RESPIRATORIA");
          lcd.setCursor(9,2);
          lcd.print(POSICION_FR_TEN);
        }else{
          salida_FR=false;
          mostrarInicio();
          lcd.setCursor(0,0);
          lcd.write(0);
        }
      break;
      //-------------------------------CASO VOLUMEN CORRIENTE--------------------------------
      case 1:
        if(PAGINA_VC==1){
          MENU_VC=true;
          MENU_INICIO=false;
          salida_VC=true;
          lcd.setCursor(2,0);
          lcd.print("VOLUMEN CORRIENTE");
          lcd.setCursor(9,2);
          lcd.print(POSICION_VC_TEN);
        }else{
          salida_VC=false;
          mostrarInicio();
          lcd.setCursor(0,1); 
          lcd.write(0);  
        }  
      break;
      //-----------------------------------CASO VOLUMEN CORRIENTE----------------------------    
      case 2:
        if(PAGINA_IE==1){
          MENU_IE=true;
          MENU_INICIO=false;
          salida_IE=true;
          if(POSICION_IE_TEN==0){
            lcd.setCursor(4,0);
            lcd.print("RELACION I/E");
            lcd.setCursor(3,2);
            lcd.write(0);
            lcd.print("1:1");
            lcd.setCursor(3,3);
            lcd.print(" 1:2");
            lcd.setCursor(12,2);
            lcd.print(" 1:3");
            lcd.setCursor(12,3);
            lcd.print(" 1:4");
          }
          if(POSICION_IE_TEN==1){
            lcd.setCursor(4,0);
            lcd.print("RELACION I/E");
            lcd.setCursor(3,2);
            lcd.print(" 1:1");
            lcd.setCursor(3,3);
            lcd.write(0);
            lcd.print("1:2");
            lcd.setCursor(12,2);
            lcd.print(" 1:3");
            lcd.setCursor(12,3);
            lcd.print(" 1:4");
          }
          if(POSICION_IE_TEN==2){
            lcd.setCursor(4,0);
            lcd.print("RELACION I/E");
            lcd.setCursor(3,2);
            lcd.print(" 1:1");
            lcd.setCursor(3,3);
            lcd.print(" 1:2");
            lcd.setCursor(12,2);
            lcd.write(0);
            lcd.print("1:3");
            lcd.setCursor(12,3);
            lcd.print(" 1:4");
          }
          if(POSICION_IE_TEN==3){
            lcd.setCursor(4,0);
            lcd.print("RELACION I/E");
            lcd.setCursor(3,2);
            lcd.print(" 1:1");
            lcd.setCursor(3,3);
            lcd.print(" 1:2");
            lcd.setCursor(12,2);
            lcd.print(" 1:3");
            lcd.setCursor(12,3);
            lcd.write(0);
            lcd.print("1:4");
          }
        }else{
          salida_IE=false;
          mostrarInicio();
          lcd.setCursor(0,2); 
          lcd.write(0);
        }
      break;
      //---------------------------------------------------------------    
      case 3:
        mostrarInicio();
        lcd.setCursor(0,3);  
        lcd.write(0);
      break;
      default:
      break;
    }
  }else{
    mostrarDatos();       // Se movió a la parte de abajo como función para poderla llamar facilmente
  }
  // Alertas según MIT
  // Cuando el paciente no puede activar la innalación
  // Esta alarma estaría bien implementarla ya con el dispositivo y ver lo que arroja el monitor serie para establecer los parámetros correctos
  // Creo que será que si hay una presión con la bolsa inflada debe haber una pequeña baja de presión después de cierto tiempo
  // Suponiendo que la bolsa inflada da como resultado 650, y cuando inhalo tiene que caer a 550 la lógica que seguí es la siguiente.
  
  if(analogRead(A3)<200){           // Baja presión
    digitalWrite(11, HIGH);
  }else if(analogRead(A3)>800){     // Alta presión
    digitalWrite(10, HIGH);
  }else if(analogRead(A3)>600){       // Si la bolsa está inflada (presión normal)
    if(espera_alarma==1){
      TCCR2B=0x07;                    // Activación del Timer 2 con el preescalador MAX (1024)  
    }else{                            // Pasaron los 2 segundos en el timer2 (revisar la función del timer para más info)
      if (analogRead(A3)>600){        // Si la bolsa sigue inflada y no bajó la presión hasta el deseado
        digitalWrite(12, HIGH);       // Enciende la alarma de inhalación
      }
    }
  }else{                              // Funcionamiento normal
    TCCR2B=0x00;
    TCNT2=0x00;
    cuenta_timer2=0;
    espera_alarma=1;                  // Prepara la siguiente vez
    digitalWrite(12, LOW);            // Apaga la alarma de inhalación
  }
}

void encoder()  {
  TCCR1B=0x05;                                          // Inicia el timer 1 con el máximo preescalador de 1024
  TCNT1=0x0000;                                         // Cuenta inicial T1 para 4.1943 segundos
  if(MENU_INICIO==1){
    static unsigned long ultimaInterrupcion = 0;        // Variable static con último valor de
    // tiempo de interrupcion
    unsigned long tiempoInterrupcion = millis();        // Variable almacena valor de func. millis
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // Rutina antirebote desestima
      // pulsos menores a 5 mseg.
      if (digitalRead(B) == HIGH){                      // Si B es HIGH, sentido horario
        POSICION++;                                     // Incrementa POSICION en 1
      }
      else{                                             // Si B es LOW, senti anti horario
        POSICION--;                                     // Decrementa POSICION en 1
      }
      POSICION = min(3, max(0, POSICION));              // Establece limite inferior de 0 y
      //superior de 100 para POSICION
      ultimaInterrupcion = tiempoInterrupcion;          // Guarda valor actualizado del tiempo
    }
  }                                                     // De la interrupcion en variable static

  if(MENU_FR==1){
    static unsigned long ultimaInterrupcion = 0;        // Variable static con ultimo valor de
    // tiempo de interrupcion
    unsigned long tiempoInterrupcion = millis();        // Variable almacena valor de func. millis
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // Rutina antirebote desestima
      // pulsos menores a 5 mseg.
      if (digitalRead(B) == HIGH){                      // Si B es HIGH, sentido horario
        POSICION_FR_TEN++;                              // Incrementa POSICION en 1
      }else {                                           // Si B es LOW, senti anti horario
        POSICION_FR_TEN--;                              // Decrementa POSICION en 1
      }
      POSICION_FR_TEN = min(40, max(8, POSICION_FR_TEN));       // Establece limite inferior de 0 y
      // superior de 100 para POSICION
      ultimaInterrupcion = tiempoInterrupcion;          // Guarda valor actualizado del tiempo
    }
  }  

  //-------------------------------------------VOLUMEN CORRIENTE-------------------------------
  if(MENU_VC==1){
    static unsigned long ultimaInterrupcion = 0;        // Variable static con ultimo valor de
    // tiempo de interrupcion
    unsigned long tiempoInterrupcion = millis();        // Variable almacena valor de func. millis
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // Rutina antirebote desestima
      // pulsos menores a 5 mseg.
      if (digitalRead(B) == HIGH){                      // Si B es HIGH, sentido horario
        POSICION_VC_TEN++;                              // Incrementa POSICION en 1
      }
      else {                                            // Si B es LOW, senti anti horario
        POSICION_VC_TEN--;                              // Decrementa POSICION en 1
      }
      POSICION_VC_TEN = min(800, max(200, POSICION_VC_TEN));    // Establece limite inferior de 0 y
      // superior de 100 para POSICION
      ultimaInterrupcion = tiempoInterrupcion;          // Guarda valor actualizado del tiempo
    }
  }
  //-------------------------------------------VOLUMEN RELACION I/E-------------------------------
  if(MENU_IE==1){
    static unsigned long ultimaInterrupcion = 0;        // Variable static con ultimo valor de
    // tiempo de interrupcion
    unsigned long tiempoInterrupcion = millis();        // Variable almacena valor de func. millis
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // Rutina antirebote desestima
      // pulsos menores a 5 mseg.
      if (digitalRead(B) == HIGH){                      // Si B es HIGH, sentido horario
        POSICION_IE_TEN++;                              // Incrementa POSICION en 1
      }
      else {                                            // Si B es LOW, senti anti horario
        POSICION_IE_TEN--;                              // Decrementa POSICION en 1
      }
      POSICION_IE_TEN = min(3, max(0, POSICION_IE_TEN));        // Establece limite inferior de 0 y
      // superior de 100 para POSICION
      ultimaInterrupcion = tiempoInterrupcion;          // Guarda valor actualizado del tiempo
    }
  }
}

void enter(){
  TCCR1B=0x05;                        // Inicia el timer 1 con el máximo preescalador de 1024
  TCNT1=0x0000;                       // Cuenta inicial T1 para 4.1943 segundos
  if (digitalRead(push) == LOW){      // Si B es HIGH, sentido horario
    pulso=true;
    digitalWrite(13,HIGH);
    delay(25);                        // Retardo para que se alcance a percibir el buzzer
    digitalWrite(13,LOW);
    limpiarAlarmas();
  }else{
    pulso=false;
  }

  if(MENU_INICIO==1){                // Cambio de menu principal a submenus o a seleccionar variable 
    if(pulso==1 && POSICION==0){
      pulso=false;
      PAGINA_FR=true;
      limpiar_pantalla=1;
    }
    if(pulso==1 && POSICION==1){
      PAGINA_VC=true;
      //lcd.clear();
      limpiar_pantalla=1;
    }
    if(pulso==1 && POSICION==2){
      PAGINA_IE=true;
      limpiar_pantalla=1;
    }
  }

  if(pulso==1){                     // Regreso a menu principal despues de seleccionar el valor de variable FR
    if(p_inicio==false){
      p_inicio=true;
      MENU_INICIO=true;
      limpiar_pantalla=1;
    }
    if(salida_FR==1){
      PAGINA_FR=false;
      MENU_FR=false;
      MENU_INICIO=true;
      limpiar_pantalla=1;
      POSICION_FR=POSICION_FR_TEN;  // Cuando se valida el parámetro la variable cambia
    }
    if(salida_VC==1){               // Regreso a menu principal despues de seleccionar el valor de variable VC
      PAGINA_VC=false;
      MENU_VC=false;
      MENU_INICIO=true;
      limpiar_pantalla=1;
      POSICION_VC=POSICION_VC_TEN;
    }
    if(salida_IE==1){               // Regreso a menu principal despues de seleccionar el valor de variable IE
      PAGINA_IE=false;
      MENU_IE=false;
      MENU_INICIO=true;
      limpiar_pantalla=1;
      POSICION_IE=POSICION_IE_TEN;
    }
  }    
}

// Función del timer (no meter ningun lcd.***() porque crashea, usar mejor banderas)
ISR(TIMER1_OVF_vect){
    TCCR1B=0x00;                                      // Detiene el timer 1
    TCNT1=0x0000;                                     // Cuenta inicial T1 para 4.1943 segundos
    p_inicio=false;                                   // Retorna a la pantalla de datos
    limpiar_pantalla=1;                               // Bandera para limpiar pantalla
    MENU_INICIO=false;                                // Para que al presionar algo no se meta a la opción
    PAGINA_FR=false;                                  // Si ya estaba dentro de una opción, para que no se actualice al salir
    MENU_FR=false;
    PAGINA_VC=false;
    MENU_VC=false;
    PAGINA_IE=false;
    MENU_IE=false;
    
    POSICION_FR_TEN=POSICION_FR;                      // Si pasan 4 segundos y se sale, las tentativas se resetean a lo real
    POSICION_VC_TEN=POSICION_VC;
    POSICION_IE_TEN=POSICION_IE;
    
}

// Función del timer (no meter ningun lcd.***() porque crashea, usar mejor banderas)
ISR(TIMER2_OVF_vect){
  if(cuenta_timer2<122){
    cuenta_timer2++;
    TCNT2=0x00;
  }else{
    TCCR2B=0x00;                    // Apaga el timer 2
    TCNT2=0x00;
    cuenta_timer2=0;
    espera_alarma=0;
  }
}

void mostrarDatos(){
  
  if (limpiar_pantalla){
    lcd.clear();
    limpiar_pantalla=0;
  }
  
  lcd.setCursor(0,0);  
  lcd.print(" FREC.RESP");
  lcd.setCursor(5,1);  
  lcd.print(POSICION_FR);
  
  lcd.setCursor(0,2);  
  lcd.print(" VOL.CORR.");
  lcd.setCursor(4,3);  
  lcd.print(POSICION_VC);

  if(POSICION_IE==0){
    IE="1:1";
  }
  if(POSICION_IE==1){
    IE="1:2";
  }
  if(POSICION_IE==2){
    IE="1:3";
  }
  if(POSICION_IE==3){
    IE="1:4";
  }
  
  lcd.setCursor(14,0);  
  lcd.print("I/E");
  lcd.setCursor(14,1);  
  lcd.print(IE);

  lcd.setCursor(12,2);  
  lcd.print("PRESION");
  lcd.setCursor(14,3);  
  //lcd.print(POSICION_FR_TEN); AUN NO SE CREA MENU
}

void mostrarInicio(){
  // Borrar el caracter flecha de posiciones anteriores
  if(POSICION!=POSICION_ANTERIOR){              // Si hubo un cambio en la posición borra las flechas remanentes.
    limpiar_pantalla=1;
    POSICION_ANTERIOR=POSICION;
  }else{
    // Imprimir el menú de inicio
    lcd.setCursor(1,0);
    lcd.print("Frecuencia Resp.");
    lcd.setCursor(1,1); 
    lcd.print("Volumen corriente");
    lcd.setCursor(1,2);  
    lcd.print("Relacion I / E");
    lcd.setCursor(1,3);  
    lcd.print("Presion / deteccion");
  }
}

void limpiarAlarmas(){
  digitalWrite(12,LOW);
  digitalWrite(11,LOW);
  digitalWrite(10,LOW);
}

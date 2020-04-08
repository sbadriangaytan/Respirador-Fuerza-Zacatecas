#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define A 2                       //Variable A a pin digital 2 (DT en módulo)
#define B 4                       //Variable B a pin digital 4 (CLK en módulo)
#define push  3                   //Variable Push

LiquidCrystal_I2C lcd(0x27,20,4);
uint8_t arrow[8] = {0x0, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00, 0x00};   //CARACTER DE FLECHA PARA PANTALLA
//int REBOTE=45; Rebote ya no es necesario después de las interrupciones 

boolean p_inicio=false;

boolean pulso=false;            // Variable del botón del encoder 
int pulsoAnterior=999;

boolean salida_FR=false;        // Variable de salida para submenús
boolean MENU_INICIO=false;       // Variable para saber la posición del menú y delimitar el encoder de 0 a 3 
int ANTERIOR = 999;
volatile int POSICION = 0;      // Variable de posicion del encoder

boolean MENU_FR=false;          // Variable para saber la posición del menú y delimitar el encoder de 8 a 40
boolean PAGINA_FR=false;        // Variable para posición de menu y hacer regresar del menú de donde se elige el valor de FR
int ANTERIOR_FR = 999;
volatile int POSICION_FR = 20;  // Variable de inicio para valor FR
boolean salida_VC=false;        // Variable de salida para submenús

boolean MENU_VC=false;          // Variable para saber la posición del menú y delimitar el encoder de 200 a 300
boolean PAGINA_VC=false;        // Variable para posicion de menu y hacer regresar del menú de donde se elige el valor de VC
int ANTERIOR_VC = 999;
volatile int POSICION_VC = 300; // Variable de inicio para valor VC

boolean MENU_IE=false;          // Variable para saber posición del menú y delimitar el valor de IE
boolean PAGINA_IE=false;        // Variable para saber cuándo guardar el valor de IE
int ANTERIOR_IE = 999;
volatile int POSICION_IE = 1;   // Variable de inicio para valor VC
boolean salida_IE=false;        // Variable de salida para submenús
String IE="vacio";
// Almacena valor anterior de la variable POSICION
// Variable POSICION con valor inicial de 50 y definida

void setup() {
  pinMode(13,OUTPUT); //pin para buzzer
  pinMode(A, INPUT);    // A como entrada
  pinMode(B, INPUT);    // B como entrada
  pinMode (push,INPUT); 
  Serial.begin(9600);   // Incialización de comunicacion serie a 9600 bps

  attachInterrupt(digitalPinToInterrupt(push),enter, CHANGE); // Interrupción sobre pin A con
  attachInterrupt(digitalPinToInterrupt(A),encoder, LOW);     // Interrupción sobre pin A con

  lcd.init();                 // Iniciar la LCD
  lcd.backlight();            // Activar Backligth
  lcd.createChar(0, arrow);   // Crear el caracter flecha

  /////////PANTALLA INICIO//////////////

  lcd.clear();
  lcd.setCursor(1,1);  
  lcd.print("<FUERZA ZACATECAS>");

  delay(3000);

 /* lcd.clear();
  lcd.setCursor(0,0);
  lcd.write(0);  
  lcd.print("Frecuencia Resp.");
  lcd.setCursor(0,1);  
  lcd.print(" Volumen corriente");
  lcd.setCursor(0,2);  
  lcd.print(" Relacion I / E");
  lcd.setCursor(0,3);  
  lcd.print(" Presion / deteccion");*/

}

void loop() {
    digitalWrite(13,LOW);
  
  //---------------------------------LECTURA DE ENCODER MENÚ INICIAL---------------------------------------------------------
  
  if (POSICION != ANTERIOR) {         // Si el valor de POSICION es distinto de ANTERIOR
    // Serial.println(POSICION);      // Imprime valor de POSICION
    ANTERIOR = POSICION ;             // Asigna a ANTERIOR el valor actualizado de POSICION
    lcd.clear();
  }
  
  //----------------------------------LECTURA DE ENCODER MENU FRECUENCIA RESPIRATORIA-----------------------------------------
  
  if (POSICION_FR != ANTERIOR_FR) {   // Si el valor de POSICION es distinto de ANTERIOR
    // Serial.println(POSICION_FR);   // Imprime valor de POSICION
    ANTERIOR_FR = POSICION_FR ;       // Asigna a ANTERIOR el valor actualizado de POSICION
    lcd.setCursor(9,2);
    lcd.print("  ");
  }
  
  //----------------------------------LECTURA DE ENCODER MENU FRECUENCIA RESPIRATORIA-----------------------------------------
  
  if (POSICION_VC != ANTERIOR_VC) {   // Si el valor de POSICION es distinto de ANTERIOR
    // Serial.println(POSICION_VC);   // Imprime valor de POSICION
    ANTERIOR_VC = POSICION_VC ;       // Asigna a ANTERIOR el valor actualizado de POSICION
    lcd.setCursor(9,2);
    lcd.print("    ");
  }
  
  //----------------------------------LECTURA DE ENCODER MENU RELACION I/E-----------------------------------------
  
  if (POSICION_IE != ANTERIOR_IE) {   // Si el valor de POSICION es distinto de ANTERIOR
    // Serial.println(POSICION_IE);   // Imprime valor de POSICION
    ANTERIOR_IE = POSICION_IE;        // Asigna a ANTERIOR el valor actualizado de POSICION
    lcd.clear();
  }
  //----------------------------------LECTURA DE PULSO-----------------------------------------
  
  if (pulso != pulsoAnterior) {       // Si el valor de POSICION es distinto de ANTERIOR

    delay(250);
    lcd.clear();
    //Serial.println("pulso");
    // imprime valor de POSICION
    pulsoAnterior = pulso ;           // Asigna a ANTERIOR el valor actualizado de POSICION
  }

if(p_inicio==true){
  

  
  switch(POSICION){
    
    //-------------------------------CASO FRECUENCIA RESPIRATORIA--------------------------------
    case 0:
      if(PAGINA_FR==1){
        MENU_FR=true;;
        MENU_INICIO=false;
        salida_FR=true;
        lcd.setCursor(5,0);
        lcd.print("FRECUENCIA");
        lcd.setCursor(4,1);
        lcd.print("RESPIRATORIA");
        lcd.setCursor(9,2);
        lcd.print(POSICION_FR);
      }else{
        salida_FR=false;
        lcd.setCursor(0,0);
        lcd.write(0);  
        lcd.print("Frecuencia Resp.");
        lcd.setCursor(0,1);  
        lcd.print(" Volumen corriente");
        lcd.setCursor(0,2);  
        lcd.print(" Relacion I / E");
        lcd.setCursor(0,3);  
        lcd.print(" Presion / deteccion");
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
        lcd.print(POSICION_VC);
      }else{
        salida_VC=false;
        lcd.setCursor(0,0);
        lcd.print(" Frecuencia Resp.");
        lcd.setCursor(0,1); 
        lcd.write(0);  
        lcd.print("Volumen corriente");
        lcd.setCursor(0,2);  
        lcd.print(" Relacion I / E");
        lcd.setCursor(0,3);  
        lcd.print(" Presion / deteccion");
      }  
    break;
    //-----------------------------------CASO VOLUMEN CORRIENTE----------------------------    
    case 2:
      if(PAGINA_IE==1){
        MENU_IE=true;
        MENU_INICIO=false;
        salida_IE=true;
        if(POSICION_IE==0){
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
        if(POSICION_IE==1){
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
        if(POSICION_IE==2){
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
        if(POSICION_IE==3){
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
        lcd.setCursor(0,0);
        lcd.print(" Frecuencia Resp.");
        lcd.setCursor(0,1); 
        lcd.print(" Volumen corriente");
        lcd.setCursor(0,2); 
        lcd.write(0); 
        lcd.print("Relacion I / E");
        lcd.setCursor(0,3);  
        lcd.print(" Presion / deteccion");
      }
    break;
    //---------------------------------------------------------------    
    case 3:
      lcd.setCursor(0,0);
      lcd.print(" Frecuencia Resp.");
      lcd.setCursor(0,1); 
      lcd.print(" Volumen corriente");
      lcd.setCursor(0,2);  
      lcd.print(" Relacion I / E");
      lcd.setCursor(0,3);  
      lcd.write(0); 
      lcd.print("Presion / deteccion");
    break;
    default:
    break;
  }
}else{

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
  //lcd.print(POSICION_FR); AUN NO SE CREA MENU

}

}

void encoder()  {
  if(MENU_INICIO==1){
    static unsigned long ultimaInterrupcion = 0;        // Variable static con último valor de
    // tiempo de interrupcion
    unsigned long tiempoInterrupcion = millis();        // Variable almacena valor de func. millis
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // Rutina antirebote desestima
      // pulsos menores a 5 mseg.
      if (digitalRead(B) == HIGH){                      // Si B es HIGH, sentido horario
        POSICION++ ;                                    // Incrementa POSICION en 1
      }
      else{                                             // Si B es LOW, senti anti horario
        POSICION-- ;                                    // Decrementa POSICION en 1
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
        POSICION_FR++ ;                                 // Incrementa POSICION en 1
      }else {                                           // Si B es LOW, senti anti horario
        POSICION_FR-- ;                                 // Decrementa POSICION en 1
      }
      POSICION_FR = min(40, max(8, POSICION_FR));       // Establece limite inferior de 0 y
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
        POSICION_VC++ ;                                 // Incrementa POSICION en 1
      }
      else {                                            // Si B es LOW, senti anti horario
        POSICION_VC-- ;                                 // Decrementa POSICION en 1
      }
      POSICION_VC = min(800, max(200, POSICION_VC));    // Establece limite inferior de 0 y
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
        POSICION_IE++ ;                                 // Incrementa POSICION en 1
      }
      else {                                            // Si B es LOW, senti anti horario
        POSICION_IE-- ;                                 // Decrementa POSICION en 1
      }
      POSICION_IE = min(3, max(0, POSICION_IE));        // Establece limite inferior de 0 y
      // superior de 100 para POSICION
      ultimaInterrupcion = tiempoInterrupcion;          // Guarda valor actualizado del tiempo
    }
  }
}

void enter(){
  
  if (digitalRead(push) == LOW){     // Si B es HIGH, sentido horario
    pulso=true;
    digitalWrite(13,HIGH);
  }else{
    pulso=false;
  }


  if(MENU_INICIO==1){                // Cambio de menu principal a submenus o a seleccionar variable 
    if(pulso==1 && POSICION==0){
      PAGINA_FR=true;
    }
    if(pulso==1 && POSICION==1){
      PAGINA_VC=true;
    }
    if(pulso==1 && POSICION==2){
      PAGINA_IE=true;  
    }
  }

  if(pulso==1){                     // Regreso a menu principal despues de seleccionar el valor de variable FR
    if(p_inicio==false){
      p_inicio=true;
      MENU_INICIO=true;
    }
    if(salida_FR==1){
      PAGINA_FR=false;
      MENU_FR=false;
      MENU_INICIO=true;
    }
    if(salida_VC==1){               // Regreso a menu principal despues de seleccionar el valor de variable VC
      PAGINA_VC=false;
      MENU_VC=false;
      MENU_INICIO=true;
    }
    if(salida_IE==1){               // Regreso a menu principal despues de seleccionar el valor de variable IE
      PAGINA_IE=false;
      MENU_IE=false;
      MENU_INICIO=true;
    }
  }    
}

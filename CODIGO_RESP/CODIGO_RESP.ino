#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);
uint8_t arrow[8] = {0x0, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00, 0x00};//CARACTER DE FLECHA PARA PANTALLA
///int REBOTE=45; Rebote ya no es necesario despues de las interrupciones 

boolean pulso=false;            // variable del boton del encoder 
int pulsoAnterior=999;

boolean salida_FR=false;        //variable de salida para submenus
boolean MENU_INICIO=true;       // variable para saber la posicion del menu y delimitar el encoder de 0 a 3 
int ANTERIOR = 999;
volatile int POSICION = 0;      //variable de posicion del encoder


boolean MENU_FR=false;          //variable para saber la posicion del menu y delimitar el encoder de 8 a 40
boolean PAGINA_FR=false;        // variabla para posicion de menu y hacer regresar del menu de donde se elige el valor de FR
int ANTERIOR_FR = 999;
volatile int POSICION_FR = 20;  //variable de inicio para valor FR
boolean salida_VC=false;        //variable de salida para submenus

boolean MENU_VC=false;          //variable para saber la posicion del menu y delimitar el encoder de 200 a 300
boolean PAGINA_VC=false;        // variabla para posicion de menu y hacer regresar del menu de donde se elige el valor de Vc
int ANTERIOR_VC = 999;
volatile int POSICION_VC = 300; //variable de inicio para valor VC

boolean MENU_IE=false;          // variable para sber posicion del menu y delimitar el valor de IE
boolean PAGINA_IE=false;        //Variable para saber cuando guardar el valor de IE
int ANTERIOR_IE = 999;
volatile int POSICION_IE = 0;   //variable de inicio para valor VC
boolean salida_IE=false;        //variable de salida para submenus


int A = 2;                      //variable A a pin digital 2 (DT en modulo)
int B = 4;                      //variable B a pin digital 4 (CLK en modulo)

// almacena valor anterior de la variable POSICION
// variable POSICION con valor inicial de 50 y definida

#define push 3

void setup() {

  pinMode(A, INPUT);    // A como entrada
  pinMode(B, INPUT);    // B como entrada
  pinMode (push,INPUT); 
  Serial.begin(9600);   // incializacion de comunicacion serie a 9600 bps

  attachInterrupt(digitalPinToInterrupt(push),enter, CHANGE); // interrupcion sobre pin A con
  attachInterrupt(digitalPinToInterrupt(A),encoder, LOW);     // interrupcion sobre pin A con

  lcd.init();                 //Iniciar la LCD
  lcd.backlight();            //Activate backlight
  lcd.createChar(0, arrow);   //Crear el caracter flecha

  /////////PANTALLA INICIO//////////////

  lcd.clear();
  lcd.setCursor(1,1);  
  lcd.print("<FUERZA ZACATECAS>");

  delay(3000);

  lcd.clear();
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

void loop() {
  //---------------------------------LECTURA DE ENCODER MENU INICIAL---------------------------------------------------------
  
  if (POSICION != ANTERIOR) {     // si el valor de POSICION es distinto de ANTERIOR
    // Serial.println(POSICION);   // imprime valor de POSICION
    ANTERIOR = POSICION ;         // asigna a ANTERIOR el valor actualizado de POSICION
    lcd.clear();
  }
  
  //----------------------------------LECTURA DE ENCODER MENU FRECUENCIA RESPIRATORIA-----------------------------------------
  
  if (POSICION_FR != ANTERIOR_FR) {   // si el valor de POSICION es distinto de ANTERIOR
    // Serial.println(POSICION_FR);    // imprime valor de POSICION
    ANTERIOR_FR = POSICION_FR ;       // asigna a ANTERIOR el valor actualizado de POSICION
    lcd.setCursor(9,2);
    lcd.print("  ");
  }
  
  //----------------------------------LECTURA DE ENCODER MENU FRECUENCIA RESPIRATORIA-----------------------------------------
  
  if (POSICION_VC != ANTERIOR_VC) {   // si el valor de POSICION es distinto de ANTERIOR
    // Serial.println(POSICION_VC);   // imprime valor de POSICION
    ANTERIOR_VC = POSICION_VC ;       // asigna a ANTERIOR el valor actualizado de POSICION
    lcd.setCursor(9,2);
    lcd.print("    ");
  }
  
  //----------------------------------LECTURA DE ENCODER MENU RELACION I/E-----------------------------------------
  
  if (POSICION_IE != ANTERIOR_IE) {   // si el valor de POSICION es distinto de ANTERIOR
    // Serial.println(POSICION_IE);    // imprime valor de POSICION
    ANTERIOR_IE = POSICION_IE;         // asigna a ANTERIOR el valor actualizado de POSICION
    lcd.clear();
  }
  //----------------------------------LECTURA DE PULSO-----------------------------------------
  
  if (pulso != pulsoAnterior) {       // si el valor de POSICION es distinto de ANTERIOR
    lcd.clear();
    //Serial.println("pulso");
    // imprime valor de POSICION
    pulsoAnterior = pulso ;           // asigna a ANTERIOR el valor actualizado de POSICION
  }
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
}

void encoder()  {
  if(MENU_INICIO==1){
    static unsigned long ultimaInterrupcion = 0;        // variable static con último valor de
    // tiempo de interrupcion
    unsigned long tiempoInterrupcion = millis();        // variable almacena valor de func. millis
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
      // pulsos menores a 5 mseg.
      if (digitalRead(B) == HIGH){                      // si B es HIGH, sentido horario
        POSICION++ ;                                    // incrementa POSICION en 1
      }
      else{                                             // si B es LOW, senti anti horario
        POSICION-- ;                                    // decrementa POSICION en 1
      }
      POSICION = min(3, max(0, POSICION));              // establece limite inferior de 0 y
      //superior de 100 para POSICION
      ultimaInterrupcion = tiempoInterrupcion;          // guarda valor actualizado del tiempo
    }
  }                                                     // de la interrupcion en variable static

  if(MENU_FR==1){
    static unsigned long ultimaInterrupcion = 0;        // variable static con ultimo valor de
    // tiempo de interrupcion
    unsigned long tiempoInterrupcion = millis();        // variable almacena valor de func. millis
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
      // pulsos menores a 5 mseg.
      if (digitalRead(B) == HIGH){                      // si B es HIGH, sentido horario
        POSICION_FR++ ;                                 // incrementa POSICION en 1
      }else {                                           // si B es LOW, senti anti horario
        POSICION_FR-- ;                                 // decrementa POSICION en 1
      }
      POSICION_FR = min(40, max(8, POSICION_FR));       // establece limite inferior de 0 y
      // superior de 100 para POSICION
      ultimaInterrupcion = tiempoInterrupcion;          // guarda valor actualizado del tiempo
    }
  }  

  //-------------------------------------------VOLUMEN CORRIENTE-------------------------------
  if(MENU_VC==1){
    static unsigned long ultimaInterrupcion = 0;        // variable static con ultimo valor de
    // tiempo de interrupcion
    unsigned long tiempoInterrupcion = millis();        // variable almacena valor de func. millis
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
      // pulsos menores a 5 mseg.
      if (digitalRead(B) == HIGH){                      // si B es HIGH, sentido horario
        POSICION_VC++ ;                                 // incrementa POSICION en 1
      }
      else {                                            // si B es LOW, senti anti horario
        POSICION_VC-- ;                                 // decrementa POSICION en 1
      }
      POSICION_VC = min(800, max(200, POSICION_VC));    // establece limite inferior de 0 y
      // superior de 100 para POSICION
      ultimaInterrupcion = tiempoInterrupcion;          // guarda valor actualizado del tiempo
    }
  }
  //-------------------------------------------VOLUMEN RELACION I/E-------------------------------
  if(MENU_IE==1){
    static unsigned long ultimaInterrupcion = 0;        // variable static con ultimo valor de
    // tiempo de interrupcion
    unsigned long tiempoInterrupcion = millis();        // variable almacena valor de func. millis
    if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
      // pulsos menores a 5 mseg.
      if (digitalRead(B) == HIGH){                      // si B es HIGH, sentido horario
        POSICION_IE++ ;                                 // incrementa POSICION en 1
      }
      else {                                            // si B es LOW, senti anti horario
        POSICION_IE-- ;                                 // decrementa POSICION en 1
      }
      POSICION_IE = min(3, max(0, POSICION_IE));        // establece limite inferior de 0 y
      // superior de 100 para POSICION
      ultimaInterrupcion = tiempoInterrupcion;          // guarda valor actualizado del tiempo
    }
  }
}

void enter(){
  if (digitalRead(push) == LOW){     // si B es HIGH, sentido horario
    pulso=true;
  }else{
    pulso=false;
  }

  if(MENU_INICIO==1){                // cambio de menu principal a submenus o a seleccionar variable 
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
    if(salida_FR==1){
      PAGINA_FR=false;
      MENU_FR=false;
      MENU_INICIO=true;
    }
    if(salida_VC==1){               //regreso a menu principal despues de seleccionar el valor de variable VC
      PAGINA_VC=false;
      MENU_VC=false;
      MENU_INICIO=true;
    }
    if(salida_IE==1){               //regreso a menu principal despues de seleccionar el valor de variable IE
      PAGINA_IE=false;
      MENU_IE=false;
      MENU_INICIO=true;
    }
  }    
}

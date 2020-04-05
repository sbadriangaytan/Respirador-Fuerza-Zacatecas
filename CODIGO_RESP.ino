#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);
uint8_t arrow[8] = {0x0, 0x04 ,0x06, 0x1f, 0x06, 0x04, 0x00, 0x00};//CARACTER DE FLECHA PARA PANTALLA


int MENU_INICIO=1;
int ANTERIOR = 50;
volatile int POSICION = 0;


int MENU_FR=0;
int PAGINA_FR=0;
int ANTERIOR_FR = 50;
volatile int POSICION_FR = 20;



int A = 2;      //variable A a pin digital 2 (DT en modulo)
int B = 3;        //variable B a pin digital 4 (CLK en modulo)

// almacena valor anterior de la variable POSICION
 // variable POSICION con valor inicial de 50 y definida

#define push 10

void setup() {

pinMode(A, INPUT);    // A como entrada
  pinMode(B, INPUT);    // B como entrada
  Serial.begin(9600);   // incializacion de comunicacion serie a 9600 bps
  attachInterrupt(digitalPinToInterrupt(A),encoder, LOW);// interrupcion sobre pin A con

  
  pinMode (push,INPUT); 
  
  lcd.init();                 //Init the LCD
  lcd.backlight();            //Activate backlight
  lcd.createChar(0, arrow);   //Create the arrow symbol

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
  if (POSICION != ANTERIOR) { // si el valor de POSICION es distinto de ANTERIOR
    Serial.println(POSICION); // imprime valor de POSICION
    ANTERIOR = POSICION ; // asigna a ANTERIOR el valor actualizado de POSICION
   lcd.clear();
  }
//----------------------------------LECTURA DE ENCODER MENU FRECUENCIA RESPIRATORIA-----------------------------------------
  
  if (POSICION_FR != ANTERIOR_FR) { // si el valor de POSICION es distinto de ANTERIOR
    Serial.println(POSICION_FR); // imprime valor de POSICION
    ANTERIOR_FR = POSICION_FR ; // asigna a ANTERIOR el valor actualizado de POSICION
   lcd.clear();
  }
  

  switch(POSICION){
//-------------------------------CASO FRECUENCIA RESPIRATORIA--------------------------------
     case 0:
if(digitalRead(push)==0){
  lcd.clear();
  PAGINA_FR=1;
  }
if(PAGINA_FR==1){
  MENU_FR=1;
  MENU_INICIO=0;
  
lcd.setCursor(5,0);
  lcd.print("FRECUENCIA");
  lcd.setCursor(4,1);
  lcd.print("RESPIRATORIA");
  lcd.setCursor(9,2);
  lcd.print(POSICION_FR);
  if(digitalRead(push)==0){
  lcd.clear();
  PAGINA_FR=0;
  MENU_FR=0;
  MENU_INICIO=1;
  delay(50);
  }

}else{

  
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
  MENU_INICIO=1;
  lcd.setCursor(0,0);
  lcd.print(" Frecuencia Resp.");
  lcd.setCursor(0,1); 
  lcd.write(0);  
  lcd.print("Volumen corriente");
  lcd.setCursor(0,2);  
  lcd.print(" Relacion I / E");
  lcd.setCursor(0,3);  
  lcd.print(" Presion / deteccion");
        
     break;
 //---------------------------------------------------------------    
     case 2:
  MENU_INICIO=1;
  lcd.setCursor(0,0);
  lcd.print(" Frecuencia Resp.");
  lcd.setCursor(0,1); 
  lcd.print(" Volumen corriente");
  lcd.setCursor(0,2); 
  lcd.write(0); 
  lcd.print("Relacion I / E");
  lcd.setCursor(0,3);  
  lcd.print(" Presion / deteccion");
        
     break;

 //---------------------------------------------------------------    
     case 3:
  MENU_INICIO=1;
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
  static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de
            // tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
              // pulsos menores a 5 mseg.
    if (digitalRead(B) == HIGH)     // si B es HIGH, sentido horario
    {
      POSICION++ ;        // incrementa POSICION en 1
    }
    else {          // si B es LOW, senti anti horario
      POSICION-- ;        // decrementa POSICION en 1
    }
    

   POSICION = min(3, max(0, POSICION));  // establece limite inferior de 0 y
            // superior de 100 para POSICION
  ultimaInterrupcion = tiempoInterrupcion;  // guarda valor actualizado del tiempo
}

  }           // de la interrupcion en variable static


  
if(MENU_FR==1){
  static unsigned long ultimaInterrupcion = 0;  // variable static con ultimo valor de
            // tiempo de interrupcion
  unsigned long tiempoInterrupcion = millis();  // variable almacena valor de func. millis

  if (tiempoInterrupcion - ultimaInterrupcion > 5) {  // rutina antirebote desestima
              // pulsos menores a 5 mseg.
    if (digitalRead(B) == HIGH)     // si B es HIGH, sentido horario
    {
      POSICION_FR++ ;        // incrementa POSICION en 1
    }
    else {          // si B es LOW, senti anti horario
      POSICION_FR-- ;        // decrementa POSICION en 1
    }
    

   POSICION_FR = min(40, max(8, POSICION_FR));  // establece limite inferior de 0 y
            // superior de 100 para POSICION
  ultimaInterrupcion = tiempoInterrupcion;  // guarda valor actualizado del tiempo
}

  }      

}

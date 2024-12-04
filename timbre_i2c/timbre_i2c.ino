/*----------------------------------------------------------------------------------------------------------------------------------------------------
.                                                       "TIMBRE AUTOMATIZADO PARA COLEGIO"
. Por Tomás Caratti - tomircaratti@gmail.com
. (Para horarios de tuno mañana, (Secundario) y turno tarde (Terciario)
                                        
.        Conexion RTC:  Arduino UNO    GND-->GND  VCC-->5V  SCL-->A5  SDA-->A4
.        Conexion Rele: Arduino UNO    GND-->GND  VCC-->5V  IN-->13
.        Conexion LCD_i2c:  Arduino UNO    GND-->GND  VCC-->5V  SCL-->SCL  SDA-->SCL

.        NOTA: se debe cargar dos veces este programa   1. Con la linea RTC.adjust(DateTime(__DATE__, __TIME__));
.                                                       2. Sin esa linea con "//" delante de la lina,
.                                                          para que quede en comnetario y no se cargue nuevamente
------------------------------------------------------------------------------------------------------------------------------------------------------*/
//************** LIBRERIAS ******************//
#include <Wire.h> 
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>

//************** LCD - RTC ****************//

LiquidCrystal_I2C lcd(0x27,16,2); // inicializa la interfaz I2C del LCD 16x2

RTC_DS1307 RTC;     // inicializa el modulo RTC

//************* BOTONES ***************//
int BTN_MENU = 5; // Boton SET MENU
int BTN_PLUS = 4; // Boton +
int BTN_MINUS = 3; // Boton -

//************ VARIABLES **************//
int r_diaSemana;  // almacena el resultado del dia de la semana calculad
int segundo;
int minuto;
int hora;
int anio;
int mes;
int dia;
int menu=0;
String name;
const int activador = 13;
const int debounce = 100;
/*----------------------------- Duracion timbre Corto y Largo ------------------------------*/
int t_corto = 2000;
int t_largo = 4000;      // Tiempo continuo que dura sonando (en milisegundos 5000 = 5 seg)
/*-------------------------------------------------------------------------------------------*/
struct Horario {
  int hora;
  int minuto;
  int segundo;
  bool esLargo;
  String nombre;
};
/*----------------------------- CONFIGURACION HORARIOS -------------------------------
Hora en la que suena el timbre escrito:
{hora(0-23), minuto(0-59), segundo(0-59), largo(true=largo o false=corto), "nombre"}
Cuando no se programa ninguna hora se debe dejar escrito el numero 99  
Se pueden programar 16 timbres por horario
---------------------- Horarios Timbres Terciario - Lunes a Viernes ------------------*/
Horario horario1[] = {
  {16, 0, 0, true, " ENTRADA"},
  {16, 30, 0, false, "MEDIO MOD"},
  {17, 10, 0, true, " RECREO"},
  {17, 25, 0, true, "FIN RECREO"},
  {18, 5, 0, false, "MEDIO MOD"},
  {18, 40, 0, true, " RECREO"},
  {19, 0, 0, true, "FIN RECREO"},
  {19, 40, 0, false, "MEDIO MOD"},
  {20, 15, 0, true, " RECREO"},
  {20, 20, 0, true, "FIN RECREO"},
  {20, 55, 0, false, "MEDIO MOD"},
  {21, 30, 0, true, "  SALIDA"},
};
Horario horario2[] = {
  // Agrega los horarios para el segundo conjunto aquí
  {16, 43, 0, false, " RECREO"},
  {16, 43, 10, true, ""},
  {16, 43, 20, false, " RECREO"},
  {99, 0, 0, false, " RECREO"},
  {99, 0, 0, false, " RECREO"},
  {99, 0, 0, false, " RECREO"},
  {99, 0, 0, false, " RECREO"},
  {99, 0, 0, false, " RECREO"},
};
Horario horario3[] = {
  // Agrega los horarios para el tercer conjunto aquí
  {99, 0, 0, true, " SALIDA"},
  {99, 0, 0, true, " SALIDA"},
  {99, 0, 0, true, " SALIDA"},
  {99, 0, 0, true, " SALIDA"},
  {99, 0, 0, true, " SALIDA"},
  {99, 0, 0, true, " SALIDA"},
  {99, 0, 0, true, " SALIDA"},
  {99, 0, 0, true, " SALIDA"},
};
///////////////////////////////// Void Setup() ////////////////////////////////////
void setup(){
  pinMode(BTN_MENU,INPUT);
  pinMode(BTN_PLUS,INPUT);
  pinMode(BTN_MINUS,INPUT);
  pinMode(activador, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  if (! RTC.begin()){
    Serial.println ("Modulo RTC no encontrado!");
    while (1);
  }
  //RTC.adjust(DateTime(__DATE__, __TIME__));
  /* Lee la fecha y hora del PC (Solo en la primera carga) se usa solo en la configuracion inicial, luego se pone como comentario.
  Se vuelve a cargar el programa sin esta linea */
  lcd.init();
  lcd.backlight(); 
  lcd.setCursor (0,0);
  lcd.print(" COLEGIO ISABEL ");
  lcd.setCursor (0,1);
  lcd.print("  LA  CATOLICA");
  delay (2000);
  lcd.clear();
  lcd.setCursor (0,0);
  lcd.print(" Timbre ARDUINO ");
  lcd.setCursor (0,1);
  lcd.print("by TOMAS CARATTI");
  delay (2000);
  lcd.clear();
}
///////////////////////////////// Void loop() ////////////////////////////////////
void loop(){ 
  switch(menu) {
    case 0:
      DisplayDateTime();
      break;
    case 1:
      DisplaySetHour();
      break;
    case 2:
      DisplaySetMinute();
      break;
    case 3:
      DisplaySetYear();
      break;
    case 4:
      DisplaySetMonth();
      break;
    case 5:
      DisplaySetDay();
      break;
    case 6:
      StoreAgg();
      //delay(500);
      menu = 0;
      break;
  }
  DateTime now = RTC.now();         //fecha y hora del RTC
  Serial.print(now.year(), DEC);    // Anio
  Serial.print('/');
  Serial.print(now.month(), DEC);   // Mes
  Serial.print('/');
  Serial.print(now.day(), DEC);     // Dia
  Serial.print(' ');
  Serial.print(now.hour(), DEC);    // Horas
  Serial.print(':');
  Serial.print(now.minute(), DEC);  // Minutos
  Serial.print(':');
  Serial.print(now.second(), DEC);  // Segundos
  Serial.println();
  segundo=now.second();
  minuto=now.minute();
  hora=now.hour();
  int r_diaSemana = dia_de_semana();
  if (!(r_diaSemana == 6 || r_diaSemana == 0)){
    horario(horario1, sizeof(horario1) / sizeof(horario1[0]));
    horario(horario2, sizeof(horario2) / sizeof(horario2[0]));
    horario(horario3, sizeof(horario3) / sizeof(horario3[0]));
   }
  digitalWrite(activador, LOW);
  delay(200);
}
/*-------------------------------- Funcion que activa el Timbre -------------------------------*/
void activar_t(int largo_t, String name){
  DateTime now = RTC.now();
  digitalWrite(activador, HIGH);
  lcd.clear();  
  lcd.setCursor(0,0);
  lcd.print(" Timbre  ACTIVO ");
  lcd.setCursor(0,1);
  lcd.print(name + " ");
  lcd.print(now.hour(), DEC);
  lcd.print(":");
  lcd.print(now.minute(), DEC);
  Serial.println("Timbre Activo");
  Serial.println(name);
  delay(largo_t);
  lcd.clear();
  }
/*-------------------------------- Calcula el dia de la Semana -------------------------------*/
  int dia_de_semana(){                         // Encuentra el dia de la semana de una fecha
  int n_dia;
  int r_dia;
  int n_mes;
  int t_mes;
  int n_anno;
  int d_anno;
  int t_siglo = 6;
  DateTime now = RTC.now();                   //fecha y hora del RTC
  n_anno = (now.year()-2000);
  d_anno = n_anno/4;
  n_dia = now.day();
  n_mes = now.month();

  switch (n_mes) {
    case 1:
      t_mes=0;
      break;
    case 2:
      t_mes=3;
      break;
    case 3:
      t_mes=3;
      break;
    case 4:
      t_mes=6;
      break;
    case 5:
      t_mes=1;
      break;
    case 6:
      t_mes=4;
      break;
    case 7:
      t_mes=6;
      break;
    case 8:
      t_mes=2;
      break;
    case 9:
      t_mes=5;
      break;
    case 10:
      t_mes=0;
      break;
    case 11:
      t_mes=3;
      break;
    case 12:
      t_mes=5;
      break;
    default: 
      t_mes=t_mes;
    break;
  }
  r_dia = n_dia + t_mes + n_anno + d_anno + t_siglo;
  r_dia = r_dia % 7;
  switch (r_dia){
    case 1:
        lcd.setCursor(13,0);
        lcd.print("Lun");
        Serial.print(" Lun ");
      break;
      case 2:
        lcd.setCursor(13,0);
        lcd.print("Mar");
        Serial.println(" Mar ");
      break;
      case 3:
        lcd.setCursor(13,0);
        lcd.print("Mie");
        Serial.print(" Mie ");
      break;
      case 4:
        lcd.setCursor(13,0);
        lcd.print("Jue");
        Serial.print(" Jue ");
      break;
      case 5:
        lcd.setCursor(13,0);
        lcd.print("Vie");
        Serial.print(" Vie ");
      break;
      case 6:
        lcd.setCursor(13,0);
        lcd.print("Sab");
        Serial.print(" Sab ");
      break;
      case 0:
        lcd.setCursor(13,0);
        lcd.print("Dom");
        Serial.print(" Dom ");
      break;
      default:
        lcd.setCursor(13,0); 
        lcd.print("---");
        Serial.print(" ---");
      break;
  } 
  return r_dia; 
}
/*-------------------------------- Condicional del Horario 1 -------------------------------*/
void horario(const Horario *horarios, int numHorarios) {
  for (int i = 0; i < numHorarios; i++) {
    const Horario &horario = horarios[i];
    if (hora == horario.hora && minuto == horario.minuto && segundo == horario.segundo) {
      name = horario.nombre;
      if (horario.esLargo) {
        activar_t(t_largo, name);
      } else { 
        activar_t(t_corto, name);
      }
      break; // Sale del bucle tan pronto como se encuentra una coincidencia
    }
  }
}
/*-------------------------------- DisplayDateTime -------------------------------*/
void DisplayDateTime (){
  DateTime now = RTC.now();   // Obtiene la fecha y hora del RTC
  if(digitalRead(BTN_MENU)){
    menu=1;
    delay(debounce);
  }
  lcd.setCursor(0, 1);
  lcd.print("H: ");
  if (now.hour()<=9){
    lcd.print("0");
  }
  lcd.print(now.hour(), DEC);
  hora=now.hour();
  lcd.print(":");
  if (now.minute()<=9){
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC);
  minuto=now.minute();
  lcd.print(":");
  if(digitalRead(BTN_MENU)){
    menu=1;
    delay(debounce);
  }
  if (now.second()<=9){
    lcd.print("0");
  }
  lcd.print(now.second(), DEC);
  lcd.setCursor(0, 0);
  lcd.print("F: ");
  if (now.day()<=9){
    lcd.print("0");
  }
  lcd.print(now.day(), DEC);
  dia=now.day();
  lcd.print("/");
  if(digitalRead(BTN_MENU)){
    menu=1;
    delay(debounce);
  }
  if (now.month()<=9){
    lcd.print("0");
  }
  lcd.print(now.month(), DEC);
  mes=now.month();
  lcd.print("/");
  lcd.print(now.year() - 2000, DEC);
  anio=now.year();
  if(digitalRead(BTN_MENU)){
    menu=1;
    delay(debounce);
  }
}
/*-------------------------------- DisplaySetHour -------------------------------*/
void DisplaySetHour(){
// time setting
  lcd.clear();
  DateTime now = RTC.now();
  lcd.setCursor(0,0);
  lcd.print("Conf Hora:");
  lcd.setCursor(0,1);
  lcd.print(hora,DEC);
  while(!digitalRead(BTN_MENU)){
  if(digitalRead(BTN_PLUS)==HIGH){
    if(hora==23){
      hora=0;
    }else{
      hora=hora+1;
    }
    delay(debounce);  
  }
  if(digitalRead(BTN_MINUS)==HIGH){
    if(hora==0){
      hora=23;
    }else{
      hora=hora-1;
    }
    delay(debounce);
  }
  DateTime now = RTC.now();
  lcd.setCursor(0,0);
  lcd.print("Conf Hora:");
  lcd.setCursor(0,1);
  lcd.print(hora,DEC);
  lcd.print(" ");
  }
  menu=2;
  delay(debounce);
}
/*-------------------------------- DisplaySetMinute -------------------------------*/
void DisplaySetMinute(){
// Setting the minutes
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Conf Minutos:");
  lcd.setCursor(0,1);
  lcd.print(minuto,DEC);
  while(!digitalRead(BTN_MENU)){
    if(digitalRead(BTN_PLUS)==HIGH){
      if (minuto==59){
        minuto=0;
      }else{
        minuto=minuto+1;
      }
      delay(debounce);
    }
     if(digitalRead(BTN_MINUS)==HIGH){
      if (minuto==0){
        minuto=59;
      }else{
        minuto=minuto-1;
      }
      delay(debounce);
    }
    lcd.setCursor(0,0);
    lcd.print("Conf Minutos:");
    lcd.setCursor(0,1);
    lcd.print(minuto,DEC);
    lcd.print(" ");
    //delay(200);
  }
  menu=3;
  delay(debounce);
}/*-------------------------------- DisplaySetYear -------------------------------*/
void DisplaySetYear(){
// setting the year
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Conf Anio:");
  lcd.setCursor(0,1);
  lcd.print(anio,DEC);
  while(!digitalRead(BTN_MENU)){
    if(digitalRead(BTN_PLUS)==HIGH){    
      anio=anio+1;
    }
     if(digitalRead(BTN_MINUS)==HIGH){
      anio=anio-1;
    }
    delay(debounce);
    lcd.setCursor(0,0);
    lcd.print("Conf Anio:");
    lcd.setCursor(0,1);
    lcd.print(anio,DEC);
    lcd.print(" ");
  }
  menu=4;
  delay(debounce);
}
/*-------------------------------- DisplaySetMonth -------------------------------*/
void DisplaySetMonth(){
// Setting the month
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Conf Mes:");
  lcd.setCursor(0,1);
  lcd.print(mes,DEC);
  while(!digitalRead(BTN_MENU)){
    if(digitalRead(BTN_PLUS)==HIGH){
      if (mes==12){
        mes=1;
      }else{
        mes=mes+1;
      }
    }
    if(digitalRead(BTN_MINUS)==HIGH){
      if (mes==1){
        mes=12;
      }else{
        mes=mes-1;
      }
    }
    lcd.setCursor(0,0);
    lcd.print("Conf Mes:");
    lcd.setCursor(0,1);
    lcd.print(mes,DEC);
    lcd.print(" ");
    delay(debounce);
  }
  menu=5;
  delay(debounce);
  //delay(200);
}
/*-------------------------------- DisplaySetDay -------------------------------*/
void DisplaySetDay()
{
// Setting the day
  lcd.clear();
  delay(debounce);
  while(!digitalRead(BTN_MENU)){    
    if(digitalRead(BTN_PLUS)==HIGH){
      if (dia==31){
        dia=1;
      }else{
        dia=dia+1;
      }
    }
    if(digitalRead(BTN_MINUS)==HIGH){
      if (dia==1){
        dia=31;
      }else{
        dia=dia-1;
      }
    }
    lcd.setCursor(0,0);  
    lcd.print("Conf Dia:");
    lcd.setCursor(0,1);
    lcd.print(dia,DEC);
    lcd.print(" ");
    delay(debounce);
  }
  menu=6;
  delay(debounce);
}
/*-------------------------------- StoreAgg -------------------------------*/
void StoreAgg(){
// Variable saving
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("GUARDANDO");
  lcd.setCursor(0,1);
  lcd.print("CONFIGURACION");

  RTC.adjust(DateTime(anio,mes,dia,hora,minuto,0));

  delay(800);
  lcd.clear();
}
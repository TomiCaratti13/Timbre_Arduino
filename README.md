# Timbre Automatizado para Colegio
Este proyecto implementa un sistema de timbre automatizado utilizando un Arduino UNO, un módulo RTC DS1307, un LCD con interfaz I2C y un módulo relé. Está diseñado para colegios que operan en cualquier turno.

## Características
- Configuración de horarios para activar un timbre corto o largo.
- Función de ajuste de fecha y hora mediante botones.
- Visualización de información en una pantalla LCD.
- Modularidad para horarios de diferentes turnos o días.

## Conexiones del Hardware
### RTC DS1307
```cpp
GND  ->  GND
VCC  ->  5V
SCL  ->  A5
SDA  ->  A4
```
### Módulo Relé
```cpp
GND  ->  GND
VCC  ->  5V
IN   ->  Pin 13
```
### Pantalla LCD I2C
```cpp
GND   ->   GND
VCC   ->   5V
SCL   ->   SCL
SDA   ->   SDA
```

## Configuración Inicial

> [!IMPORTANT] 
> 1. **Primera carga del programa**:
>    - Descomenta la línea `RTC.adjust(DateTime(__DATE__, __TIME__));`
>    - Sube el código al Arduino
> 2. **Segunda carga del programa**:
>    - Comenta la línea `// RTC.adjust(DateTime(__DATE__, __TIME__));`
>    - Vuelve a subir el código para evitar que el RTC se reajuste cada vez que se reinicie el Arduino.


## Librerías Necesarias
Asegúrate de instalar las siguientes librerías antes de cargar el código:

- `Wire.h` (para comunicación I2C)
- `RTClib.h` (para el módulo RTC DS1307)
- `LiquidCrystal_I2C.h` (para el LCD)


## Programación de Horarios
El programa permite definir hasta 16 timbres por turno. Cada horario se configura mediante la estructura:
```cpp
struct Horario {
  int hora;
  int minuto;
  int segundo;
  bool esLargo; // true para timbre largo, false para timbre corto
  String nombre;
};
```
Ejemplo:
```cpp
Horario horario1[] = {
  {16, 0, 0, true, "ENTRADA"},
  {16, 30, 0, false, "MEDIO MOD"},
  {17, 10, 0, true, "RECREO"},
};

// Horas no utilizadas deben configurarse como 99
```
#### Botones de Control:
```cpp
BTN_MENU (Pin 5)   // Acceso al menú de configuración.
BTN_PLUS (Pin 4)   // Incrementar valores.
BTN_MINUS (Pin 3)  // Decrementar valores.
```
#### Funciones Principales:
```cpp
// Activa el timbre durante el tiempo especificado
// y muestra el nombre del evento en la pantalla LCD.
activar_t(int largo_t, String name)
// Muestra la fecha y hora actuales en la pantalla.
DisplayDateTime()
```
#### Funciones de configuración:
```cpp
DisplaySetHour()    // Ajusta la hora
DisplaySetMinute()  // Ajusta los minutos
DisplaySetYear()    // Ajusta el año
DisplaySetMonth()   // Ajusta el mes
DisplaySetDay()     // Ajusta el día
```

## Uso
- Configura el hardware como se describe en la sección de conexiones
- Sube el código siguiendo las instrucciones de configuración inicial
- Ajusta los horarios según las necesidades del colegio
- Observa el funcionamiento en tiempo real en el LCD


> [!NOTE]
> Este programa está diseñado para operar únicamente en días laborales (lunes aviernes). Los fines de semana se omiten los horarios.<br>
> Se incluye un sistema de debounce para los botones, ajustable mediante la constante `debounce`.

<hr>

#### Créditos:
Desarrollado por Tomás Caratti <br>
Email: tomircaratti@gmail.com

<hr>  

# Software - Documentación

## Contenido
Esta carpeta contiene todo el código y documentación del software del proyecto.

### 📁 Carpetas

#### `src/`
Código fuente del proyecto [AquaSafe_ESP32.ino](AquaSafe_ESP32.ino).

#### `librerias/`
Librerías adicionales necesarias (no incluidas en Arduino IDE)

#### `docs/`
Documentación técnica del software

---

## Stack Tecnológico

**Plataforma:** Arduino / ESP32  

**Lenguaje:** C/C++  

**IDE:** Arduino IDE 2.x  

**Librerías principales:**
- OneWire: comunicación con el sensor DS18B20 - Versión X.X
- DallasTemperature: lectura de temperatura - Versión X.X
- Preferences: almacenamiento de parámetros de calibracion en el ESP32 - Versión X.X
- math.h: operaciones matemáticas y validación numérica - Versión: estándar

---

## Instalación y Configuración

### 1. Instalar Arduino IDE

Para programar el ESP32 de AquaSafe se utiliza, Arduino IDE, un programa que permite escribir, modificar y cargar un código al microcontrolador.

1. Ingresar a https://www.arduino.cc/en/software
2. Descargar el Arduino IDE según el sistema operativo.
3. Instalar el programa.
4. Abrir Arduino IDE

### 2. Configurar ESP32/Arduino
Para poder utilizar el ESP32 es necesario instalar un soporte en ARDUINO IDE.

1. Abrir Arduino IDE
2. Ingresar a Archivo → Preferencias.
3. Buscar el apartado "URLs adicionales de gestor de tarjetas".
4. Agregar el siguente enlace:
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
5. Guaradar los cambios.
6. Ingresar a Herramientas → Placa → Gestor de tarjetas.
7. Buscar "ESP32"
8. Instalar el paquete "ESP32 by Espressif Systems". 

### 3. Instalar Librerías

Para utilizar los sensores y componentes de AquaSafe se deben instalar las librerías utilizadas en el código.

1. Abrir Arduino IDE.
2. Ingresar al Gestor de bibliotecas.
3. Buscar e instalar las siguentes librerías:

-OneWire (versión por confirmar).

-DallasTemperature (versión por confirmar).

-Adafruit GFX Library (versión por confirmar).

-Adafruit SSD1306 (versión por confirmar).

-Adafruit BusIO (dependencia utilizada por las bibliotecas Adafruit, según instalación).

-Wire,Preferences y Arduino.h (incluidas con el entorno ESP32/Arduino).

El resto de librerías necesarias deben comprobarse segun la versión final del código, especialmente las usadas en la pantalla OLED.

### 4. Cargar el código al ESP32

1. Conectar el ESP32 al computador mediante un cable USB.
2. Abrir el archivo `src/main.ino` del programa de AquaSafe en Arduino IDE.
3. Seleccionar el modelo de placa ESP-WROOM-32.
4. Seleccionar el puerto al que está conectado.
5. Presionar "Verificar" para comprobar que el código compile.
6. Presionar "Subir" para cargar el programa al ESP32.


### 5. Visualización de las mediciones

Una vez que está cargado el programa, el ESP32 obtiene las lectruras de los sensores conectados.

El sistema electrónico contempla la medición de:

- Temperatura del agua (°C).
- pH del agua.
- Turbidez del agua.
- Sólidos disueltos totales, TDS (ppm).

Las mediciones se van a poder consultar mediante una pantalla OLED, y según las configuración del programa, mediante el Monitor Serie de Arduino IDE.

---

## Configuración de Parámetros

Editar las siguientes constantes en el código:

```cpp
// WiFi
#define WIFI_SSID "tu_red"
#define WIFI_PASS "tu_password"

// Sensores
#define SENSOR_PIN 4
#define SAMPLE_RATE 1000

// Otros parámetros
```

---

## Arquitectura del Software

[Incluir diagrama de flujo o arquitectura]

### Módulos Principales
1. **Inicialización:**  El ESP32 prepara los sensores y la pantalla OLED (setup del sistema)
2. **Lectura de Sensores:** obtiene información de temperatura, pH, turbidez y TDS. (captura de datos)
3. **Procesamiento:** interpreta las señales recibidas mediante las funciones programadas. (análisis y decisiones)
4. **Comunicación:** - envío de datos
5. **Visualizción:** presenta los valores de las mediciones obtenidos en la pantalla OLED.
6. **Calibración:** permite almacenar parámetros de referencia necesarios para interpretar las mediciones.

---



# Software de AquaSafe

## Descripción

Esta carpeta contiene el firmware del prototipo AquaSafe, desarrollado en Arduino/C++ para un ESP32 DevKit de 30 pines. El programa lee temperatura, pH, turbidez y TDS, procesa las señales y muestra información en una pantalla OLED y en el Monitor Serie. El dispositivo es un prototipo académico de caracterización preliminar de agua: **no certifica su potabilidad**.

**Código principal:** [AquaSafe_ESP32.ino](AquaSafe_ESP32.ino).

## Hardware y pines

| Elemento | Pin ESP32 | Función |
|---|---|---|
| DS18B20 | GPIO 27 | Señal OneWire de temperatura |
| Módulo pH | GPIO 35 | Entrada analógica |
| Módulo de turbidez | GPIO 34 | Entrada analógica |
| Módulo TDS | GPIO 32 | Entrada analógica; señal acondicionada mediante divisor |
| OLED SDA | GPIO 21 | Comunicación I2C |
| OLED SCL | GPIO 22 | Comunicación I2C |

El firmware está configurado para una OLED SSD1306 de 128 × 64 píxeles. Detecta las direcciones I2C `0x3C` y `0x3D`. El modelo físico exacto y las conexiones se deben verificar con el equipo responsable del montaje.




## Funcionamiento del programa

Al encender, la función `setup()` inicia la comunicación serie, recupera los datos de calibración desde `Preferences`, configura el ADC, detecta la pantalla OLED e inicia el DS18B20.

En `loop()`, el programa obtiene la temperatura y las señales analógicas de pH, TDS y turbidez; procesa las lecturas, atiende comandos de calibración, entrega información por el Monitor Serie y actualiza la pantalla aproximadamente cada medio segundo.

La pantalla OLED muestra la temperatura en °C, la señal de pH en voltios, el TDS estimado o calibrado en ppm cuando está disponible (o en voltios si no lo está), la señal de turbidez en voltios y un texto de evaluación demostrativa. **Los voltajes de pH y turbidez mostrados en la OLED no equivalen por sí solos a pH calibrado ni a NTU.** En el Monitor Serie, el programa también calcula pH y turbidez en UNT cuando existen referencias de calibración válidas.

El programa puede mostrar etiquetas de demostración como `REF. POTABLE`, pero estas **no deben interpretarse como una declaración de agua segura para beber**

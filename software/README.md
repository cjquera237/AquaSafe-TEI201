# Software - Documentación

## Contenido
Esta carpeta contiene todo el código y documentación del software del proyecto.

### 📁 Carpetas

#### `src/`
Código fuente del proyecto (.ino para Arduino/ESP32)

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
- [Librería 1] - Versión X.X
- [Librería 2] - Versión X.X
- [Librería 3] - Versión X.X

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

-OneWire

-DallasTemperature

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
1. **Inicialización** - Setup del sistema
2. **Lectura de Sensores** - Captura de datos
3. **Procesamiento** - Análisis y decisiones
4. **Actuación** - Control de actuadores
5. **Comunicación** - Envío de datos

---

## API / Endpoints (si aplica)

| Endpoint | Método | Descripción |
|----------|--------|-------------|
| `/status` | GET | Estado del sistema |
| `/data` | GET | Datos de sensores |
| `/control` | POST | Control de actuadores |

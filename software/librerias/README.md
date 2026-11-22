# Librerías

## Librerías Estándar (Arduino IDE)


### Librerías utilizadas por AquaSafe

El firmware de AquaSafe utiliza bibliotecas para comunicarse con los sensores, controlar la pantalla OLED y almacenar parámetros de calibración.

### Librerías externas

Estas bibliotecas se instalan desde el Administrador de Bibliotecas de Arduino IDE:

| Librería | Versión utilizada | Función |
|---|---|---|
| OneWire | Por confirmar | Comunicación con el sensor DS18B20 |
| DallasTemperature | Por confirmar | Lectura de temperatura |
| Adafruit GFX Library | Por confirmar | Funciones gráficas de la pantalla OLED |
| Adafruit SSD1306 | Por confirmar | Control de la pantalla OLED |
| Adafruit BusIO | Por confirmar | Dependencia de las bibliotecas Adafruit |

### Librerías incluidas en el entorno ESP32

| Librería | Función |
|---|---|
| Wire | Comunicación I2C con la pantalla OLED |
| Preferences | Almacenamiento de las calibraciones en memoria no volátil |
| Arduino.h | Funciones generales del entorno Arduino |

Las versiones exactas deben ser confirmadas por el integrante encargado de programación.

### Instalación

1. Abrir Arduino IDE.
2. Seleccionar el Administrador de Bibliotecas.
3. Buscar las bibliotecas externas indicadas.
4. Instalar las versiones utilizadas por el equipo.
5. Comprobar que el soporte para placas ESP32 esté instalado.
6. Abrir y compilar el firmware `AquaSafe_ESP32.ino`.

---

## Librerías Personalizadas/Modificadas

Si hay librerías modificadas o no disponibles en Arduino IDE, incluir en esta carpeta:

### Estructura
```
librerias/
├── MiLibreria/
│   ├── MiLibreria.h
│   ├── MiLibreria.cpp
│   ├── keywords.txt
│   └── README.md
```

## Librerías Personalizadas/Modificadas

El firmware actual de AquaSafe no requiere bibliotecas personalizadas desarrolladas por el equipo. Las funciones de lectura, procesamiento, calibración y visualización se encuentran implementadas directamente en el archivo principal del programa.

Las bibliotecas externas utilizadas se mantienen sin modificaciones conocidas respecto de sus distribuciones originales.

Por lo tanto, no es necesario instalar manualmente una carpeta de librerías personalizadas.

### Instalación Manual
```bash
# Copiar carpeta a Arduino libraries
cp -r MiLibreria ~/Documents/Arduino/libraries/
```

---

## Dependencias Completas

Listar TODAS las librerías necesarias con versiones exactas:


## Dependencias Completas

La siguiente tabla reúne las dependencias necesarias para utilizar el firmware de AquaSafe.

| Biblioteca | Versión | Fuente | Instalación |
|---|---|---|---|
| OneWire | Por confirmar | Arduino Library Manager | Externa |
| DallasTemperature | Por confirmar | Arduino Library Manager | Externa |
| Adafruit GFX Library | Por confirmar | Arduino Library Manager | Externa |
| Adafruit SSD1306 | Por confirmar | Arduino Library Manager | Externa |
| Adafruit BusIO | Por confirmar | Arduino Library Manager | Dependencia |
| Wire | Incluida en el entorno | Arduino/ESP32 | Integrada |
| Preferences | Incluida en ESP32 Core | Espressif | Integrada |

### Configuración del entorno

- Plataforma: ESP32.
- Lenguaje: Arduino/C++.
- Placa utilizada: ESP32 DevKit de 30 pines.
- Versión del paquete ESP32: pendiente de confirmar.
- Velocidad del Monitor Serie: 115200 baudios.

Las versiones exactas deben verificarse en el computador donde se desarrolló y compiló el programa.

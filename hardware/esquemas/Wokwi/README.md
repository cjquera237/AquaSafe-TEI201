# Simulación de AquaSafe en Wokwi

## 1. Descripción general

Esta carpeta contiene los archivos de la simulacion electrónica de AquaSafe, desarrollada en Wokwi.

La simulación permite representar parte del funcionamiento del prototipo utilizando un microcontrolador ESP32, un sensor de temperatura DS18B20 y una pantalla OLED.

Debido a que los sensores de pH, turbidez y TDS utilizados en el prototipo AquaSafe no están disponibles en esta simulación, sus señales se representan a través de unos potenciómetros.

 **Observacion:** la simulación en Wokwi es una representación simplificada del sistema y no reemplaza el funcionamiento ni las mediciones de los sensores reales.


## 2. Componentes utilizados

| Componente | Función en la simulación |
|---|---|
| ESP32 DevKit V1 | Recibe y procesa las señales de los componentes |
| Sensor DS18B20 | Simula la medición de la temperatura |
| Pantalla OLED | Muestra la información procesada por el ESP32 |
| Potenciómetro de pH | Representa la señal del sensor de pH |
| Potenciómetro de turbidez | Representa la señal del sensor de turbidez |
| Potenciómetro de TDS | Representa la señal del sensor TDS |
| Resistencias de 4,7 kΩ | Se utiliza en la conexión de datos del DS18B20 |

## 3. Conexiones electrónicas

| Componente | Pin del ESP32 |
|---|---|
| Senor DS18B20| GPIO 27 |
| Potenciómetro de pH | GPIO 35 |
| Potenciómetro de turbidez | GPIO 34 |
| Potenciómetro de TDS | GPIO 32 |
| Pantalla OLED (SDA) | GPIO 21 |
| Pantalla OLED (SCL) | GPIO 22 |

## 4. Archivos de Wokwi
| Archivo| Descripción |
|---|---|
| diagram.json | Contiene los componentes y sus conexiones |
| sketch.ino | Contiene el programa Arduino utilizado para la simulación. |
| libraries.txt | Contiene las librerías necesarias |

## 5. Instalación y ejecución 

Para ejecutar la simulación: 
1. Ingresea a Wokwi
2. Crear un proyecto nuevo usando ESP32 DevKit V1.
3. Copiar el contenido de diagram.json al archivo de conexiones del proyecto.
4. Copiar el contenido de sketch.ino al editor del programa.
5. Incorporar las librerpias indicadas en libraries.txt, según las opciones de gestión de librerías de Wokwi.
6. Iniciar simulación.
7. Abrir el Monitor Serie a 115200 baudios para observar la informacion generada.

**Observación:** Durante la simulación es posible modificar los potenciómetros para representar distintas señales de entrada.

## 6. Pruebas de simulación

Se definieron tres escenarios para observar la respuesta del programa ante diferentes combianciones de señales.

| Escenario | pH (0-1023) | Turbidez (0-1023) | Mensaje esperado |
|---|---|---|---|
| Vaso de referencia | 459 | 930 | REF. POTABLE |
| Vaso con tierra y limón | 617 | 837 | MUESTRA TURBIA |
| Lecturas fuera de ambos parámetros | 530 | 930 | NO SE RECOMIENDA CONSUMIR |

**Nota:** los números de la tabla corresponden a entradas de simulación, no a valores de pH ni a unidades de turbidez. Los mensajes son etiquetas de demostración del programa y no determinan científicamente la potabilidad del agua.

## 7. Funcionamiento de la simulación

El funcionamiento general se divide en las siguentes etapas:

1. El ESP32 recibe la información del sensor de temperatura y de los potenciómetros.
2. El programa procesa las señales recibidas.
3. Las lecturas se comparan con los parámetros definidos para la demostración.
4. La pantalla OLED presenta la información y el mensaje correspondiente.

## 8. Limitaciones

La simulación presenta algunas limitaciones respecto al prototipo físico:

-Los potenciómetros reemplazan las señales de los sensores de pH, turbidez y TDS.

-No se reproduce el comportamiento físico completo de las sondas.
-Los valores utilizados no constituyen calibraciones de sensores reales.

-El circuito simulado no reproduce necesariamente todos los componentes eléctricos del montaje físico.

-Los mensajes mostrados permiten representar situaciones de demostración, pero no certifican la seguridad del agua para consumo.

Por lo tanto, Wokwi se emplea como una herramienta complementaria para comprender el funcionamiento generak del sistema AquaSafe. 

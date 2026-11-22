# Código Fuente

## Archivos Principales

#### El firmware definitivo del prototipo AquaSafe se encuentra en la carpeta `software/`.

#### `[AquaSafe_ESP32.ino](../AquaSafe_ESP32.ino)`
Archivo principal del proyecto Arduino/ESP32

## **Estructura:**

  ### setup():

Inicializa los sensores, la pantalla OLED, la comunicación serie y las calibraciones almacenadas.
   
   ### loop():

Ejecuta el ciclo principal del sistema, incluyendo la lectura de sensores, el procesamiento de datos y la actualización de los resultados.

   #### Funciones auxiliares

El programa incorpora funciones para:

- Obtener mediciones de los sensores.
- Convertir señales analógicas.
- Gestionar comandos de calibración.
- Calcular una estimación de TDS.
- Actualizar la pantalla OLED.

## Funcionalidades principales

El firmware integra:

- Lectura de temperatura mediante el sensor DS18B20.
- Lectura de las señales de pH, turbidez y TDS.
- Procesamiento de las mediciones y aplicación de calibraciones.
- Visualización de información en una pantalla OLED.
- Comunicación mediante el Monitor Serie.
- Almacenamiento de parámetros de calibración utilizando Preferences.


### Comentarios
```cpp
// Comentarios en español
// Explicar QUÉ hace y POR QUÉ (no solo el CÓMO)

/**
 * Función: leerTemperatura
 * Descripción: Lee temperatura del sensor DS
 * Retorna: float - Temperatura en grados Celsius
 */
float leerTemperatura() {
  // código
}
```


---

## Testing y Debugging

### Modo Debug
Descomentar para activar modo debug:
```cpp
#define DEBUG true
```

## Monitor Serial
- Baudrate: 115200
- Mensajes informativos con prefijos:
  - `[INFO]` - Información general
  - `[ERROR]` - Errores
  - `[DEBUG]` - Debug
 
### Monitor Serie

El Monitor Serie utiliza una velocidad de comunicación de 115200 baudios.
Permite revisar mediciones, consultar información y utilizar los comandos de calibración implementados en el firmware.



## Documentación adicional

La documentación técnica del software se encuentra en:

[Consultar documentación técnica](../docs/README.md)

Las bibliotecas necesarias están documentadas en:

[Consultar bibliotecas](../librerias/README.md)

## Observaciones

Las mediciones y clasificaciones demostrativas no certifican la potabilidad del agua.


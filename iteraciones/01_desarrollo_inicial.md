# Iteración 01 – Desarrollo inicial de AquaSafe

**Proyecto:** AquaSafe  
**Periodo:** Septiembre–octubre de 2026  
**Estado:** Prototipo electrónico en desarrollo

## 1. Objetivo

Desarrollar un primer prototipo funcional de AquaSafe utilizando un ESP32, sensores de parámetros del agua y un sistema de alertas luminosas.

## 2. Desarrollo realizado

### Etapa 1: Simulación inicial

- Desarrollo de una primera versión del sistema mediante Wokwi.
- Definición de los sensores y del semáforo de LEDs.
- Planificación de las conexiones del ESP32.

### Etapa 2: Montaje electrónico

- Instalación del ESP32 sobre una protoboard.
- Conexión de los LEDs verde, amarillo y rojo.
- Integración del sensor de temperatura DS18B20.
- Incorporación del sensor de turbidez.
- Conexión del módulo de pH.

### Etapa 3: Programación

- Desarrollo del firmware mediante Arduino IDE.
- Obtención de lecturas de temperatura.
- Lectura de señales analógicas de pH y turbidez.
- Implementación del semáforo automático.
- Incorporación de funciones de calibración.

## 3. Problemas identificados y soluciones

| Problema | Solución aplicada |
|---|---|
| Error al compilar por falta de OneWire | Instalación de la librería correspondiente |
| Sensor DS18B20 no detectado | Revisión de conexiones y tierra común |
| LEDs sin funcionamiento correcto | Unión de los rieles GND |
| Lecturas analógicas sin unidades físicas | Desarrollo de funciones de calibración pendientes de validar |

## 4. Resultados obtenidos

Se logró implementar un montaje que permite:

- Programar y comunicarse con el ESP32.
- Obtener lecturas del sensor DS18B20.
- Recibir señales analógicas de pH y turbidez.
- Controlar el funcionamiento del semáforo.
- Mostrar información mediante el Monitor Serie.

## 5. Limitaciones actuales

- Los sensores de pH y turbidez requieren calibración.
- El sensor TDS aún no está integrado.
- Falta incorporar una pantalla para visualizar las mediciones.
- El montaje utiliza una protoboard.
- No se han completado las pruebas con usuarios outdoor.

## 6. Mejoras para la siguiente iteración

- Integrar el sensor TDS.
- Seleccionar e instalar una pantalla compatible con ESP32.
- Desarrollar la interfaz de visualización.
- Realizar calibraciones experimentales.
- Mejorar el orden y la protección del montaje.
- Avanzar en el diseño 3D de un encapsulado portátil.

## 7. Conclusión

Esta primera etapa permitió comprobar el funcionamiento inicial de los componentes electrónicos de AquaSafe e identificar problemas de conexión que fueron corregidos.

El siguiente desafío será integrar todos los componentes en un dispositivo portátil que permita consultar directamente las mediciones mediante una pantalla.

**Nota:** El sistema continúa en desarrollo y todavía no permite certificar la potabilidad del agua.

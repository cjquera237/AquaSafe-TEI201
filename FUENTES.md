# Fuentes del Proyecto – AquaSafe

**Proyecto:** AquaSafe – Sistema Portátil de Monitoreo de Agua  
**Curso:** Taller de Diseño en Ingeniería (TEI201)  
**Universidad:** Universidad Adolfo Ibáñez  
**Año:** 2026

---

## 1. Librerías Utilizadas

El software de AquaSafe está desarrollado en Arduino/C++ utilizando un microcontrolador ESP32.

| Librería | Versión | Función | Fuente |
|---|---|---|---|
| OneWire | Por verificar | Comunicación con el sensor DS18B20 | [Paul Stoffregen – GitHub](https://github.com/PaulStoffregen/OneWire) |
| DallasTemperature | Por verificar | Lectura de temperatura en °C | [Miles Burton – GitHub](https://github.com/milesburton/Arduino-Temperature-Control-Library) |
| Preferences | Incluida en ESP32 Arduino Core | Almacenamiento de datos de calibración | [Arduino ESP32](https://docs.espressif.com/projects/arduino-esp32/en/latest/) |
| math.h | Biblioteca estándar | Operaciones matemáticas y validación numérica | Entorno de compilación Arduino/C++ |

Las versiones específicas de las bibliotecas externas deberán comprobarse en el administrador de bibliotecas de Arduino IDE.

---

## 2. Herramientas y Plataformas

| Herramienta | Utilización |
|---|---|
| Arduino IDE 2.x | Programación y carga del firmware al ESP32 |
| ESP32 Arduino Core | Compatibilidad del microcontrolador con Arduino |
| Wokwi | Simulación y desarrollo inicial del sistema |
| GitHub | Control de versiones, documentación y colaboración |
| Fusion 360 | Diseño 3D del encapsulado, pendiente de desarrollo |
| ChatGPT (OpenAI) | Asistencia en programación, resolución de errores y documentación |

### Referencias del proyecto

- [Repositorio base TEI201](https://github.com/sebasdv/proyecto_final_TEI201)
- [Simulación inicial AquaSafe en Wokwi](https://wokwi.com/projects/476425182391699457)

---

## 3. Código Externo Adaptado

El firmware de AquaSafe ha sido desarrollado para el proyecto con asistencia de inteligencia artificial y utilizando las interfaces de las bibliotecas indicadas.

No se ha identificado hasta ahora un fragmento específico de código copiado literalmente de un tutorial externo.

Si posteriormente se incorpora código de terceros, se documentará su fuente original, ubicación en el programa y modificaciones realizadas.

---

## 4. Uso de Inteligencia Artificial

### Herramienta utilizada

**ChatGPT – OpenAI (2026).**

La inteligencia artificial se utilizó como herramienta de apoyo durante distintas etapas del desarrollo de AquaSafe.

### 4.1 Programación del ESP32

**Solicitudes realizadas:**
- Desarrollar un programa que obtenga las lecturas de los sensores.
- Implementar un semáforo automático utilizando tres LEDs.
- Integrar temperatura, pH y turbidez en un único programa.

**Código desarrollado con asistencia:**
- Lectura de temperatura mediante DallasTemperature.
- Lectura de entradas analógicas del ESP32.
- Promedio de muestras para reducir variaciones.
- Evaluación de parámetros mediante condiciones.
- Control de LEDs con `digitalWrite()`.

**Adaptaciones del proyecto:**
- Asignación de los pines utilizados en el montaje físico.
- Integración de los sensores disponibles.
- Ajustes de funcionamiento durante las pruebas.

### 4.2 Calibración de Sensores

**Solicitud realizada:** Incorporar funciones de calibración para interpretar las lecturas de los sensores.

**Código desarrollado con asistencia:**
- Conversión de lecturas analógicas a voltaje.
- Calibración de pH mediante dos soluciones patrón.
- Escala relativa de turbidez.
- Almacenamiento de referencias mediante Preferences.
- Comandos de calibración desde el Monitor Serie.

**Estado:** Las funciones están contempladas en el firmware, pero la calibración experimental todavía requiere validación.

### 4.3 Resolución de Problemas

Se utilizó ChatGPT como apoyo para analizar problemas de conexión y funcionamiento durante el montaje.

Entre ellos:
- Conexiones eléctricas del ESP32.
- Instalación de bibliotecas.
- Lecturas del sensor DS18B20.
- Conexión de entradas analógicas.
- Funcionamiento de los LEDs.
- Identificación del problema de tierra común (GND).

Las soluciones se evaluaron mediante pruebas realizadas sobre el prototipo físico.

### 4.4 Documentación

ChatGPT también se utilizó como apoyo para redactar y organizar:
- Descripción técnica del proyecto.
- Documentación del hardware.
- Instrucciones de funcionamiento.
- Metodología de testing.
- README del repositorio.
- Registro de fuentes.
- Propuestas de diagramas eléctricos.

Los diagramas generados requieren verificación contra el montaje real antes de considerarse documentación eléctrica definitiva.

---

## 5. Comprensión y Responsabilidad del Equipo

El uso de inteligencia artificial se considera una herramienta de apoyo al proceso de desarrollo.

El equipo es responsable de:
- Comprender el funcionamiento del código incorporado.
- Revisar y adaptar las soluciones propuestas.
- Verificar las conexiones eléctricas.
- Comprobar experimentalmente el funcionamiento.
- Calibrar correctamente los sensores.
- Detectar y corregir errores.
- Validar los resultados del sistema.

Las mediciones y los resultados experimentales deben obtenerse del prototipo real y no generarse artificialmente.

---

## 6. Actualizaciones Futuras

Este archivo se actualizará cuando se incorporen nuevas bibliotecas, componentes o fuentes externas.

Las próximas integraciones previstas incluyen:
- Sensor TDS.
- Pantalla de visualización.
- Bibliotecas para la interfaz de pantalla.
- Diseño y documentación del encapsulado.
- Pruebas experimentales adicionales.

---

## 7. Consideraciones Finales

AquaSafe es un prototipo académico orientado al monitoreo preliminar de parámetros fisicoquímicos del agua.

Las mediciones requieren calibración y validación antes de interpretarse como valores confiables.

**El sistema no certifica la potabilidad del agua ni reemplaza análisis de laboratorio.**

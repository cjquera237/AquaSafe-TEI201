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
| Adafruit SSD1306 | Por verificar | Control de la pantalla OLED 0,96" por I²C | [Adafruit – GitHub](https://github.com/adafruit/Adafruit_SSD1306) |
| Adafruit GFX | Por verificar | Texto, líneas e íconos en la OLED | [Adafruit – GitHub](https://github.com/adafruit/Adafruit-GFX-Library) |
| Wire | Incluida en ESP32 Arduino Core | Bus I²C de la OLED | [Arduino ESP32](https://github.com/espressif/arduino-esp32) |
| WiFi | Incluida en ESP32 Arduino Core | Red WiFi propia «AquaSafe» (solo en `AquaSafe_dashboard`) | [Arduino ESP32](https://github.com/espressif/arduino-esp32) |
| WebServer | Incluida en ESP32 Arduino Core | Servidor del dashboard local (solo en `AquaSafe_dashboard`) | [Arduino ESP32](https://github.com/espressif/arduino-esp32) |
| LittleFS | Incluida en ESP32 Arduino Core | Registro `datos.csv` en la memoria flash (solo en `AquaSafe_dashboard`) | [Arduino ESP32](https://github.com/espressif/arduino-esp32) |

Datos de fabricante usados en el código: la curva nominal del sensor TDS Keyestudio KS0429 (función `keyestudioCurve`) viene del manual del fabricante, página 5.

Las versiones específicas de las bibliotecas externas deberán comprobarse en el administrador de bibliotecas de Arduino IDE.

---

## 2. Herramientas y Plataformas

| Herramienta | Utilización |
|---|---|
| Arduino IDE 2.x | Programación y carga del firmware al ESP32 |
| ESP32 Arduino Core | Compatibilidad del microcontrolador con Arduino |
| Wokwi | Simulación y desarrollo inicial del sistema |
| GitHub | Control de versiones, documentación y colaboración |
| Fusion 360 | Gemelo 3D del encapsulado, planos técnicos y renders |
| ChatGPT (OpenAI) | Asistencia en programación, resolución de errores y documentación |
| Claude (Anthropic) | Scripts del gemelo 3D, dashboard local, informe y presentación (ver sección 4.5) |

### Modelos 3D de componentes

Los siguientes modelos descargados se importaron en `diseno_3d/fusion360/encapsulado.f3d`. Solo se reposicionaron, sin cambiar su geometría:

| Componente | Origen |
|---|---|
| ESP32 WROOM-32, 30 pines, USB-C | GrabCAD, modelo «esp32 Wroom 30pins C-Type» |
| Shield de expansión ESP32 | Modelo 3D exportado de EasyEDA |
| Power bank con shield 2×18650 | Modelo STL descargado |
| Pantalla OLED 0,96" SSD1306 | Modelo STEP descargado |
| Módulo de pH PH-4502C | Medidas tomadas del modelo de J-B Jardiné en GrabCAD; se modeló de nuevo en forma paramétrica |

Los modelos de la caja, la tapa, los prensaestopas, la placa de acrílico, las pilas 18650, los adaptadores y las sondas se modelaron desde cero en `GemeloAgua.py`.

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

- Integrar temperatura, pH, turbidez y sólidos disueltos totales en un único programa.

**Código desarrollado con asistencia:**
- Lectura de temperatura mediante DallasTemperature.
- Lectura de entradas analógicas del ESP32.
- Promedio de muestras para reducir variaciones.
- Evaluación de parámetros mediante condiciones.

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

### 4.5 Claude (Anthropic), noviembre 2026

**Gemelo 3D (`diseno_3d/`)**
- **Uso:** generación y depuración de `GemeloAgua.py`, el script de la API de Fusion 360 que construye el ensamble paramétrico (caja, tapa, prensaestopas, placa y ubicación de módulos), importa los modelos STEP/STL y revisa las interferencias. También `GemeloPost.py` (exportación y renders), el dibujo técnico, la vista explosionada y los README de `diseno_3d/`.
- **Adaptación:** las medidas de la caja y la ubicación de los módulos se ajustaron a las fotos del prototipo real del equipo.
- **Comprensión:** cada medida es una constante con nombre al comienzo del script (`LX`, `WY`, `HB`, `WALL`…). Cada función `build_*` crea una pieza y `check_interference` separa los choques reales de los acoples esperados (pines en zócalos).

**Dashboard local (`software/AquaSafe_dashboard/`)**
- **Uso:** a partir de `AquaSafe_ESP32.ino` se agregaron la red WiFi propia, el registro `datos.csv` en LittleFS cada 10 s, el servidor web y la página del dashboard. Las funciones nuevas son `setupDashboard`, `logReading`, `handleNow`, `handleCsv`, `handleTime`, `handleClear` y `serveFor`. La lectura de sensores, la calibración y los estados no se modificaron.
- **Comprensión:** el ESP32 funciona como punto de acceso. Cada 10 s agrega una fila al CSV. La página consulta `/api/now` cada 2 s y descarga `/datos.csv` para los gráficos. La hora la entrega el celular al abrir la página, porque el ESP32 no tiene reloj.

**Informe final y presentación**
- **Uso:** borrador de la estructura y la redacción del informe y de los slides, a partir del Avance 1, la pauta, el firmware y las fotos del prototipo. El equipo revisa el contenido y completa los resultados de las pruebas.

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
- Pruebas experimentales adicionales.

---

## 7. Consideraciones Finales

AquaSafe es un prototipo académico orientado al monitoreo preliminar de parámetros fisicoquímicos del agua.

Las mediciones requieren calibración y validación antes de interpretarse como valores confiables.

**El sistema no certifica la potabilidad del agua ni reemplaza análisis de laboratorio.**

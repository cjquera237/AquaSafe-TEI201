# AquaSafe – Sistema Portátil de Monitoreo de Agua

**Proyecto Final – Taller de Diseño en Ingeniería (TEI201)**  
Universidad Adolfo Ibáñez | 2026

---

## 1. Información del Equipo

### Integrantes y responsabilidades

**Clemente Quera – Hardware, electrónica y programación**

Responsable del montaje electrónico del ESP32, conexión de sensores, desarrollo del código Arduino, calibración de los parámetros e integración y programación de la pantalla para visualizar las mediciones del agua.

**Nicolás Weinlaub – Diseño 3D y desarrollo físico**

Responsable del diseño del encapsulado portátil de AquaSafe, modelado en Fusion 360, elaboración de planos técnicos y optimización de la estructura física del dispositivo para su utilización en actividades outdoor.

**Francisca Rayo Pedreros – Testing y validación**

Responsable de la planificación y realización de pruebas con usuarios outdoor, aplicación de encuestas, recopilación de datos, análisis de resultados y evaluación de la utilidad del prototipo en relación con el ODS.

**Oscar Gabriel Andres Fuentes Rodríguez – Documentación y desarrollo del proyecto**

Responsable de la organización de la documentación técnica, elaboración de informes, registro de iteraciones, recopilación de evidencias y apoyo en las pruebas e integración del sistema.

**Organización del equipo:** Todos los integrantes participan en igualdad de condiciones, sin jerarquías, colaborando en las distintas etapas del desarrollo de AquaSafe.

### ODS Seleccionado

**ODS 6 – Agua limpia y saneamiento (propuesto).**

### Problema a Resolver

Las personas que realizan actividades outdoor, como trekking, camping, montañismo o ciclismo de montaña, pueden encontrarse con fuentes naturales de agua sin disponer de herramientas portátiles que les permitan conocer e interpretar fácilmente algunos de sus parámetros fisicoquímicos.

---

## 2. Descripción del Proyecto

**AquaSafe** es un dispositivo electrónico portátil orientado a personas que realizan actividades al aire libre y que tienen contacto con fuentes naturales de agua, como ríos, lagos y esteros.

El proyecto busca desarrollar un sistema que permita monitorear parámetros del agua mediante sensores conectados a un microcontrolador ESP32.

La información será procesada y visualizada directamente en una pantalla incorporada al dispositivo, facilitando su interpretación sin necesidad de utilizar un computador.

Además, AquaSafe contará con un semáforo de LEDs que entregará alertas visuales complementarias según los parámetros registrados.

### Parámetros Monitoreados

| Parámetro | Sensor | Unidad | Estado |
|---|---|---|---|
| Temperatura | DS18B20 | °C | Lectura funcional |
| pH | PH-4502C | Escala pH | Calibración pendiente |
| Turbidez | Sensor óptico | NTU tras calibración adecuada | Calibración pendiente |
| Sólidos disueltos totales | Sensor TDS | ppm | Integración pendiente |

Actualmente se han obtenido lecturas analógicas de pH y turbidez, pero todavía es necesario realizar las calibraciones correspondientes para obtener valores físicos confiables.

### Funcionamiento del Sistema

El funcionamiento previsto de AquaSafe consiste en:

1. Los sensores realizan mediciones de los parámetros del agua.
2. El ESP32 recibe las señales de los sensores.
3. El programa procesa los datos utilizando las calibraciones correspondientes.
4. La pantalla muestra los parámetros medidos.
5. Los LEDs indican el estado general de las mediciones.

### Pantalla de Visualización

Como mejora solicitada por el profesor, AquaSafe incorporará una pantalla que permita consultar los datos directamente desde el dispositivo.

La pantalla mostrará:

- Temperatura del agua (°C).
- Nivel de pH.
- Turbidez del agua.
- Sólidos disueltos totales (TDS).
- Estado general de los parámetros.
- Advertencias de errores o sensores sin calibrar.

La selección del modelo, sus conexiones y su programación se encuentran pendientes de implementación.

### Semáforo de LEDs

El sistema contempla tres indicadores luminosos:

- **Verde:** Mediciones dentro de los rangos configurados.
- **Amarillo:** Precaución, desviaciones moderadas o calibración pendiente.
- **Rojo:** Mediciones fuera de los límites configurados o errores críticos.

Los umbrales actuales son demostrativos y requieren validación experimental.

**Importante:** AquaSafe permite monitorear algunos parámetros fisicoquímicos, pero no certifica la potabilidad del agua ni detecta todos los posibles contaminantes.

---

## 3. Estado del Proyecto

- **Versión actual:** Prototipo en desarrollo.
- **Última actualización:** 27 de octubre de 2026.
- **Estado:** Montaje electrónico parcialmente funcional, con integraciones y validaciones pendientes.

### Avances Realizados

- Montaje inicial del ESP32 en protoboard.
- Conexión del sensor de temperatura DS18B20.
- Conexión del sensor de pH.
- Conexión del sensor de turbidez.
- Obtención de lecturas mediante Arduino IDE.
- Implementación del semáforo de LEDs.
- Desarrollo de código Arduino para el monitoreo.
- Incorporación de funciones de calibración en el programa.
- Documentación de componentes y conexiones.
- Registro fotográfico del montaje físico.
- Organización inicial del proyecto en GitHub.

### Desarrollo Pendiente

- Integración del sensor TDS.
- Selección de una pantalla compatible con ESP32.
- Conexión y programación de la pantalla.
- Calibración experimental de pH y turbidez.
- Validación de precisión y confiabilidad.
- Diseño de una carcasa portátil.
- Integración física de todos los componentes.
- Pruebas en condiciones outdoor.
- Testing con un mínimo de cinco usuarios.

---

## 4. Estructura del Repositorio

```text
AquaSafe-TEI201/
│
├── hardware/
│   └── Circuitos, componentes y esquemas
│
├── software/
│   └── Código Arduino y librerías
│
├── diseno_3d/
│   └── Modelos Fusion 360 y planos técnicos
│
├── testing/
│   └── Validación técnica y con usuarios
│
├── documentacion/
│   └── Informes y presentación
│
├── iteraciones/
│   └── Historial de versiones y mejoras
│
├── fotos/
│   └── Registro fotográfico del prototipo
│
├── README.md
├── FUENTES.md
└── LICENSE
```

---

## 5. Inicio Rápido

### Requisitos Previos

**Software:**

- Arduino IDE 2.x.
- Soporte de placas ESP32 para Arduino.
- Librería OneWire.
- Librería DallasTemperature.
- Soporte Preferences del ESP32.
- Fusion 360 para el diseño 3D.

**Hardware:**

- ESP32 Dev Module.
- Sensor DS18B20.
- Sensor de pH PH-4502C.
- Sensor de turbidez.
- Sensor TDS (pendiente).
- Pantalla de visualización (pendiente).
- LEDs rojo, amarillo y verde.
- Resistencias eléctricas.
- Protoboard.
- Cables Dupont.
- Alimentación mediante USB durante el desarrollo.

### Instalación

1. Descargar o clonar el repositorio de AquaSafe.
2. Abrir Arduino IDE.
3. Instalar el soporte para ESP32.
4. Instalar las librerías OneWire y DallasTemperature.
5. Abrir `software/AquaSafe_ESP32.ino`.
6. Conectar el ESP32 mediante USB.
7. Seleccionar la placa y el puerto correspondientes.
8. Cargar el programa.
9. Abrir el Monitor Serie a 115200 baudios.
10. Comprobar las lecturas de los sensores.

### Calibración de Sensores

El programa contempla comandos de calibración enviados mediante el Monitor Serie:

| Comando | Función |
|---|---|
| `7` | Registrar referencia de pH 7 |
| `4` | Registrar referencia de pH 4 |
| `L` | Registrar referencia de agua clara |
| `T` | Registrar referencia de muestra turbia |
| `R` | Borrar calibraciones almacenadas |

La calibración de pH requiere soluciones patrón conocidas.

La calibración relativa de turbidez permite comparar muestras, pero no entrega automáticamente valores en NTU.

---

## 6. Hardware

### Componentes Principales

| Componente | Función |
|---|---|
| ESP32 | Procesamiento y control |
| DS18B20 | Medición de temperatura |
| PH-4502C | Obtención de señal para estimar pH |
| Sensor de turbidez | Detección óptica de turbidez |
| Sensor TDS | Medición de sólidos disueltos |
| Pantalla | Visualización de parámetros |
| LEDs | Alertas visuales |
| Protoboard | Montaje del prototipo |

### Conexiones Actuales

| Componente | GPIO ESP32 |
|---|---|
| Temperatura DS18B20 | GPIO 21 |
| Sensor de pH | GPIO 32 |
| Turbidez | GPIO 34 |
| TDS (reservado) | GPIO 33 |
| LED verde | GPIO 16 |
| LED amarillo | GPIO 17 |
| LED rojo | GPIO 18 |
| Pantalla | Pendiente de definir |

### Alimentación

- DS18B20: 3,3 V.
- Módulo de pH: 5 V.
- Módulo de turbidez: 5 V.
- Tierra común (GND).
- Resistencia pull-up de 4,7 kΩ para el DS18B20.
- Divisores resistivos para las entradas analógicas de pH y turbidez.
- Resistencias limitadoras de corriente para los LEDs.

La documentación se encuentra en `hardware/`.

---

## 7. Usuarios Objetivo

AquaSafe está diseñado para personas que realizan actividades outdoor y están en contacto con fuentes naturales de agua.

Entre ellas se encuentran:

- Excursionistas.
- Campistas.
- Montañistas.
- Personas que practican trekking.
- Ciclistas de montaña.
- Personas que realizan actividades recreativas en entornos naturales.

### Necesidades del Usuario

El dispositivo busca ofrecer:

- Facilidad de transporte.
- Lectura directa de los parámetros.
- Pantalla legible en exteriores.
- Funcionamiento sencillo.
- Información organizada y comprensible.
- Alertas visuales complementarias.

La portabilidad, la autonomía y la legibilidad serán aspectos importantes durante el desarrollo y la validación del prototipo.

---

## 8. Testing y Validación

Se realizarán pruebas técnicas y sesiones de uso con personas que representen al público objetivo.

### Grupo Objetivo

- **Perfil:** Usuarios que realizan actividades outdoor.
- **Cantidad mínima:** 5 usuarios.
- **Contexto:** Condiciones representativas de actividades al aire libre.

### Aspectos a Evaluar

- Facilidad de utilización del dispositivo.
- Claridad de las mediciones mostradas.
- Legibilidad de la pantalla en exteriores.
- Tiempo necesario para interpretar los datos.
- Utilidad percibida en actividades outdoor.
- Estabilidad y respuesta de los sensores.
- Confiabilidad del sistema.

### Metodología

Las sesiones contemplarán:

1. Presentación del prototipo.
2. Interacción del usuario con AquaSafe.
3. Observación de su experiencia.
4. Entrevista posterior.
5. Encuesta cuantitativa.
6. Análisis de resultados.

Las evaluaciones de usabilidad utilizarán escalas de 1 a 5.

**Estado actual:** La metodología está documentada, pero las pruebas con usuarios todavía están pendientes.

Los protocolos, datos y evidencias se registrarán en `testing/`.

---

## 9. Objetivo de Desarrollo Sostenible

### ODS 6 – Agua Limpia y Saneamiento

AquaSafe se relaciona con el ODS 6 al proponer una herramienta tecnológica que facilite el monitoreo preliminar de parámetros del agua en entornos naturales.

### Objetivo del Proyecto

Desarrollar un dispositivo portátil que permita a los usuarios outdoor consultar e interpretar parámetros fisicoquímicos de fuentes naturales de agua mediante sensores y una pantalla de visualización.

### Indicadores de Evaluación

- Facilidad de interpretación de los parámetros.
- Legibilidad de la información presentada.
- Utilidad percibida por los usuarios.
- Funcionamiento y confiabilidad del prototipo.

La evaluación de usabilidad no demuestra por sí misma una mejora en la calidad del agua.

---

## 10. Checklist de Entrega

Las casillas indican el avance respecto de los requisitos establecidos para TEI201.

### Hardware

- [ ] Esquema eléctrico validado (Fritzing/Wokwi).
- [ ] BOM completo (lista de materiales).
- [ ] Fotografías de alta resolución según requisitos.
- [ ] Integración física completa de sensores.
- [ ] Integración de la pantalla.

### Software

- [x] Código Arduino desarrollado y comentado.
- [ ] Librerías completamente documentadas.
- [x] Instrucciones iniciales de instalación.
- [ ] Integración del sensor TDS.
- [ ] Programación de la pantalla.
- [ ] Calibraciones experimentales verificadas.
- [ ] Validación completa del sistema.

### Diseño 3D

- [ ] Archivos Fusion 360 (.f3d) con todos los componentes.
- [ ] Renders de alta calidad.
- [ ] Planos técnicos.
- [ ] Diseño de encapsulado portátil.

### Testing

- [ ] Reporte de testing con mínimo cinco usuarios.
- [ ] Evidencias fotográficas y videos.
- [ ] Datos cuantitativos recopilados.
- [ ] Evaluación de la pantalla.
- [ ] Pruebas en condiciones outdoor.
- [ ] Análisis de resultados.

### Documentación

- [ ] Reporte final (máximo 15 páginas).
- [ ] Presentación final (10 minutos).
- [ ] Archivo FUENTES.md completo.
- [ ] Documentación técnica final.
- [ ] Registro completo de iteraciones.

---

## 11. Próximos Objetivos

La siguiente etapa del proyecto se centrará en completar las funciones del prototipo y mejorar su facilidad de utilización.

### Prioridades de Desarrollo

1. Integrar el sensor TDS al sistema.
2. Incorporar una pantalla que muestre todos los parámetros.
3. Programar una interfaz clara y fácil de interpretar.
4. Calibrar los sensores.
5. Diseñar un encapsulado portátil y resistente para el uso previsto.
6. Realizar pruebas con muestras de agua natural.
7. Ejecutar pruebas con al menos cinco usuarios.
8. Analizar resultados e implementar mejoras.
9. Preparar los entregables finales del curso.

---

## 12. Limitaciones del Sistema

AquaSafe es un prototipo académico en desarrollo.

La medición de temperatura, pH, turbidez y TDS no permite identificar todos los contaminantes que pueden estar presentes en una fuente natural de agua.

Especialmente, el dispositivo no detecta por sí mismo microorganismos patógenos ni certifica la ausencia de sustancias peligrosas.

**Por lo tanto, AquaSafe no determina si el agua es segura para beber y no reemplaza análisis especializados de laboratorio.**

---

## 13. Licencia

Este proyecto se desarrolla como parte del curso **TEI201 – Taller de Diseño en Ingeniería**, de la Facultad de Ingeniería y Ciencias de la Universidad Adolfo Ibáñez.

**Licencia:** MIT.

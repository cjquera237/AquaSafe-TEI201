# Testing y Validación – AquaSafe

## Objetivo

Validar el funcionamiento técnico, la facilidad de uso y la utilidad del prototipo AquaSafe mediante pruebas experimentales y sesiones con usuarios reales.

AquaSafe está diseñado para personas que realizan actividades al aire libre y tienen contacto con fuentes naturales de agua.

El sistema utiliza un ESP32 y sensores para monitorear temperatura, pH, turbidez y, próximamente, sólidos disueltos totales (TDS). Las mediciones se visualizarán mediante una pantalla integrada, complementada con un semáforo de LEDs.

El proyecto se vincula con el **ODS 6: Agua limpia y saneamiento**.

---

## 📁 Carpetas

### `reportes/`
Informes de pruebas, análisis de resultados y conclusiones.

### `evidencias/`
Fotografías y videos de pruebas técnicas y sesiones con usuarios.

### `datos/`
Registros de mediciones, encuestas y datos cuantitativos.

---

## Metodología de Testing

### Grupo Objetivo

- **Perfil de usuarios:** Personas que realizan actividades outdoor, como trekking, camping, montañismo, ciclismo de montaña, excursiones y otras actividades en entornos naturales donde existe contacto con ríos, lagos, esteros u otras fuentes de agua.
- **Cantidad mínima:** 5 usuarios.
- **Ubicación:** Entornos naturales o espacios que reproduzcan las condiciones de utilización de AquaSafe durante actividades outdoor.
- **Estado:** Pruebas con usuarios pendientes de realización.

### Protocolo de Testing

**1. Briefing (5 minutos)**

- Explicar el propósito del prototipo AquaSafe.
- Presentar los sensores y los parámetros medidos.
- Explicar cómo utilizar la pantalla de visualización.
- Informar las limitaciones del dispositivo y de las mediciones.

**2. Sesión de Uso (15–20 minutos)**

- Presentar el prototipo en una situación representativa de una actividad outdoor.
- Realizar mediciones utilizando muestras de agua.
- Permitir que el usuario interactúe con el dispositivo.
- Evaluar si la pantalla facilita visualizar e interpretar los parámetros del agua.
- Observar la legibilidad de la pantalla bajo distintas condiciones de iluminación, especialmente en exteriores.
- Evaluar si la información presentada es clara, comprensible y útil.
- Registrar dificultades, errores y observaciones durante la interacción.

**3. Entrevista Post-Testing (10 minutos)**

- ¿Te resultó fácil utilizar AquaSafe?
- ¿La pantalla permitió visualizar claramente todos los parámetros?
- ¿Lograste comprender los valores presentados?
- ¿Consideras útil disponer de este dispositivo durante actividades outdoor?
- ¿Qué mejorarías de la pantalla o de la presentación de los datos?
- ¿En qué actividades o situaciones utilizarías AquaSafe?

**4. Encuesta Cuantitativa (5 minutos)**

Evaluación mediante escala Likert de 1 a 5:

1. Facilidad de uso del dispositivo.
2. Visibilidad y legibilidad de la pantalla.
3. Claridad de los parámetros presentados.
4. Utilidad del dispositivo durante actividades outdoor.
5. Satisfacción general con AquaSafe.

---

## Métricas de Evaluación

### Usabilidad

- Facilidad de uso: escala 1–5.
- Legibilidad de la pantalla: escala 1–5.
- Claridad de los datos: escala 1–5.
- Satisfacción general: escala 1–5.
- Porcentaje de usuarios que interpretan correctamente los parámetros mostrados.
- Tiempo necesario para encontrar e interpretar una medición.

### Impacto ODS

- Porcentaje de usuarios que reconoce la importancia del monitoreo de parámetros del agua.
- Porcentaje de usuarios que comprende correctamente los datos mostrados en pantalla.
- Utilidad percibida del sistema para actividades en entornos naturales.

Estos indicadores evalúan comprensión y utilidad potencial, no la potabilidad del agua.

### Desempeño Técnico

- Tiempo de respuesta de los sensores.
- Tiempo de actualización de la pantalla.
- Estabilidad de las lecturas.
- Tasa de errores.
- Confiabilidad del sistema.
- Visibilidad de la pantalla bajo iluminación exterior.
- Funcionamiento del semáforo como alerta complementaria.

---

## Registro de Pruebas Técnicas

| Componente | Resultado preliminar | Estado |
|---|---|---|
| ESP32 | Programación y comunicación establecidas | Probado |
| Temperatura DS18B20 | Lecturas en °C obtenidas | Probado |
| Sensor de pH | Lecturas analógicas obtenidas | Calibración pendiente |
| Sensor de turbidez | Lecturas analógicas obtenidas | Calibración pendiente |
| Semáforo de LEDs | Funcionamiento comprobado | Probado |
| Sensor TDS | Integración pendiente | Pendiente |
| Pantalla de visualización | Visualización de parámetros solicitada por el profesor | Pendiente |

### Observaciones técnicas

Durante el desarrollo inicial se corrigió un problema de tierra común (GND) que afectaba el funcionamiento de los LEDs.

Se obtuvieron lecturas de temperatura, pH y turbidez, pero las mediciones de pH y turbidez todavía requieren calibración.

La siguiente etapa contempla incorporar una pantalla que entregue información directamente al usuario, sin necesidad de utilizar un computador.

---

## Análisis de Resultados

### Cuantitativo

Una vez realizadas las pruebas, se calcularán:

- Promedio de las evaluaciones de usabilidad.
- Desviación estándar de las puntuaciones.
- Porcentaje de usuarios que comprende los parámetros.
- Tiempo promedio de interpretación de las mediciones.
- Evaluación de legibilidad en condiciones exteriores.
- Gráficos comparativos.
- Comparación con mediciones iniciales (baseline).

**Estado:** Pendiente de recolección de datos.

### Cualitativo

Se analizarán:

- Dificultades de utilización en entornos outdoor.
- Problemas de legibilidad de la pantalla.
- Comentarios sobre la presentación de los parámetros.
- Necesidades de portabilidad y facilidad de transporte.
- Sugerencias de mejora para el dispositivo.

**Estado:** Pendiente de entrevistas.

---

## Validación de Impacto ODS

**ODS seleccionado como propuesta:** ODS 6 – Agua limpia y saneamiento.

**Meta específica del proyecto:**

Desarrollar un dispositivo portátil que facilite el monitoreo y la interpretación de parámetros de fuentes naturales de agua para personas que realizan actividades outdoor.

### Indicadores de impacto

| Indicador | Baseline | Meta | Alcanzado |
|---|---|---|---|
| Comprensión de los parámetros | Por medir | Por definir | Pendiente |
| Facilidad de lectura en pantalla | Por medir | Por definir | Pendiente |
| Utilidad en actividades outdoor | Por medir | Por definir | Pendiente |

### Proyección de impacto a escala

AquaSafe podría facilitar el acceso a información preliminar sobre parámetros del agua durante actividades recreativas y deportivas en entornos naturales.

Su potencial dependerá de la precisión de las mediciones, la autonomía, la portabilidad y la facilidad de interpretación de la información en pantalla.

El dispositivo no certifica que el agua sea potable ni reemplaza análisis microbiológicos o de laboratorio.

---

## Próximas Actividades

- Calibrar los sensores de pH y turbidez.
- Integrar el sensor TDS.
- Incorporar y programar la pantalla.
- Diseñar una interfaz legible en condiciones exteriores.
- Mejorar la portabilidad del prototipo.
- Realizar mediciones en muestras de agua natural.
- Ejecutar pruebas con al menos 5 usuarios outdoor.
- Aplicar entrevistas y encuestas.
- Registrar resultados y evidencias.
- Elaborar el informe final de validación.

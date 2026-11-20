
## Descripción del proyecto

El hardware de AquaSafe esta diseñado para monitorear parámetros fisicoquímicos de fuentes naturales de agua mediante un sistema electrónico compacto y portátil basado en un microcontrolador ESP32.

El prototipo AquaSafe utiliza sensores de temperatura, pH, solidos totales disueltos (TDS) y turbidez, con la finalidad de obtener información que pueda ser interpretada por las personas que usen el dispositivo.

En esta carpeta se reúne toda la información y documentación técnica para comprender, reproducir, evaluar y mantener el sistema electrónico.

## Componentes

| Componente | Función | Jusificación técnica |
|---|---|---|
| ESP32-WROOM-32 | Procesamiento de señales | Microocontrolador que permite conectar los sensores y procesar los datos. |
| Sensor de temperatura DS18B20 | Medición de temperatura | Sensor digital que permite obtener mediciones y usa comunicación OneWire. |
| Sensor de pH con módulo PH-4502C | Medición de pH | Obtiene la señal de la sonda de pH. |
| Sensor de turbidez | Medición de turbidez | Detecta cambios en las propiedades ópticas del agua. |
| Sensor TDS | Medición de sólidos totales disueltos | Estima sólidos disueltos a partir de la conductividad del agua. |
| Pantalla de visualización | Presentación de las mediciones | Permite consultar los párametros monitoreados directamente desde el propio dispositivo. |
| Shield ESP para ESP32 | Conexión y distribución eléctrica | Facilita la conexión de sensores, ordena y organiza el montaje electrónico. | 
| Módulo Power bank con 2 pilas 18650 | Alimentación portátil del dispositivo | Permite suministrar energía al sistema mediante dos baterías recargables. |
| Resistencias de 4,7 kΩ, 10 kΩ y 220–330 Ω | Protección y acondicionamiento eléctrico | Se utilizan para limitar la corriente y adaptación de señales. |
| Cables Dupont | Conexión de componentes | Permite establecer conexiones eléctricas entre el microcontrolador, los sensores, la pantalla y los modulos electrónicos. |
| Placa de acrílico | Base estructural para el montaje | Permite fijar y ordenar los componentes electrónicos. |
| Caja estanca | Protección del sistema electrónico | Protege los componentes frente a la humedad, salpicaduras y polvo. |



## Conexiones del ESP32 ##

| Componente | Pin ESP32 |
|---|---|
| Temperatura DS18B20 | GPIO 21 |
| Sensor de pH | PIN 35 |
| Sensor de turbidez | PIN 34 |
| Pantalla | Por definir |

## Alimentación y conexiones

- DS18B20 alimentado con 3,3 V.
- Módulos de pH y turbidez alimentados con 5 V.
- Tierra GND compartida.
- Resistencia pull-up de 4,7 kΩ para el sensor DS18B20.
- Divisores resistivos para proteger las entradas analógicas del ESP32.

## Pantalla de visualización — Pendiente

Por recomendación del profesor, se integrará una pantalla al prototipo AquaSafe.

**Su objetivo es mostrar en tiempo real:**

- Temperatura del agua (°C).
- Nivel de pH.
- Turbidez del agua.
- Advertencias de sensores desconectados o pendientes de calibración.

El modelo de pantalla y sus conexiones eléctricas todavía están por definir.

## Estado actual del proyecto

**Implementado o probado:**
- Montaje del ESP32 en

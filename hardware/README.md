# Hardware – AquaSafe

## Descripción del proyecto

El hardware de AquaSafe esta diseñado para monitorear parámetros fisicoquímicos de fuentes naturales de agua mediante un sistema electrónico compacto y portátil basado en un microcontrolador ESP32.

El prototipo AquaSafe utiliza sensores de temperatura, pH, solidos totales disueltos (TDS) y turbidez, con la finalidad de obtener información que pueda ser interpretada por las personas que usen el dispositivo.

En esta carpeta se reúne toda la información y documentación técnica para comprender, reproducir, evaluar y mantener el sistema electrónico.

## Componentes

| Componentes | Función | Jusificación técnica |
|---|---|
| ESP32-WROOM-32 | Procesamiento de señales y control del sistema |
- Sensor de temperatura DS18B20.
- Sensor de pH con módulo PH-4502C.
- Sensor de turbidez.
- Sensor TDS.
- Pantalla de visualización.
- Shield ESP para ESP32.
- Resistencias de 4,7 kΩ, 10 kΩ y 220–330 Ω.
- Cables Dupont.
- Módulo Power bank con 2 pilas 18650.

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

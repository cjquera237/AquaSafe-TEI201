# Hardware – AquaSafe

## Descripción del proyecto

AquaSafe es un sistema electrónico de monitoreo de parámetros del agua, desarrollado mediante un microcontrolador ESP32.

El sistema utiliza sensores para obtener mediciones y un semáforo de LEDs para indicar el estado de los parámetros analizados.

Como mejora solicitada por el profesor, se incorporará una **pantalla de visualización que permita consultar las mediciones directamente desde el dispositivo**, sin necesidad de conectar un computador.

## Componentes

- ESP32-WROOM-32.
- Sensor de temperatura DS18B20.
- Sensor de pH con módulo PH-4502C.
- Sensor de turbidez.
- Pantalla de visualización.
- Shield ESP para ESP32.
- Resistencias de 4,7 kΩ, 10 kΩ y 220–330 Ω.
- Cables Dupont.
- Módulo Power bank con 2 pilas 18650.

## Conexiones del ESP32 ##

| Componente | Pin ESP32 |
|---|---|
| Temperatura DS18B20 | GPIO 21 |
| Sensor de pH | GPIO 32 |
| Sensor de turbidez | GPIO 34 |
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

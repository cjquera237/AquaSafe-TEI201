# Hardware – AquaSafe

## Descripción del proyecto

AquaSafe es un sistema electrónico de monitoreo de parámetros del agua, desarrollado mediante un microcontrolador ESP32.

El sistema utiliza sensores para obtener mediciones y un semáforo de LEDs para indicar el estado de los parámetros analizados.

Como mejora solicitada por el profesor, se incorporará una **pantalla de visualización que permita consultar las mediciones directamente desde el dispositivo**, sin necesidad de conectar un computador.

## Componentes

- ESP32 Dev Module.
- Sensor de temperatura DS18B20.
- Sensor de pH con módulo PH-4502C.
- Sensor de turbidez.
- Sensor TDS (integración pendiente).
- Tres LEDs: verde, amarillo y rojo.
- Pantalla de visualización (pendiente de integración).
- Protoboard.
- Resistencias de 4,7 kΩ, 10 kΩ y 220–330 Ω.
- Cables Dupont.

## Conexiones del ESP32

| Componente | Pin ESP32 |
|---|---|
| Temperatura DS18B20 | GPIO 21 |
| Sensor de pH | GPIO 32 |
| Sensor de turbidez | GPIO 34 |
| Sensor TDS (pendiente) | GPIO 33 |
| LED verde | GPIO 16 |
| LED amarillo | GPIO 17 |
| LED rojo | GPIO 18 |
| Pantalla | Por definir |

## Alimentación y conexiones

- DS18B20 alimentado con 3,3 V.
- Módulos de pH y turbidez alimentados con 5 V.
- Tierra GND compartida.
- Resistencia pull-up de 4,7 kΩ para el sensor DS18B20.
- Divisores resistivos para proteger las entradas analógicas del ESP32.
- Resistencias limitadoras de corriente para los LEDs.

## Pantalla de visualización — Pendiente

Por recomendación del profesor, se integrará una pantalla al prototipo AquaSafe.

**Su objetivo es mostrar en tiempo real:**

- Temperatura del agua (°C).
- Nivel de pH.
- Turbidez del agua.
- Concentración de sólidos disueltos (TDS), cuando se integre el sensor.
- Estado general del sistema, representado mediante el semáforo verde, amarillo o rojo.
- Advertencias de sensores desconectados o pendientes de calibración.

El modelo de pantalla y sus conexiones eléctricas todavía están por definir.

## Estado actual del proyecto

**Implementado o probado:**
- Montaje del ESP32 en

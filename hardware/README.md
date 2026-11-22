
## Descripción del proyecto

El hardware de AquaSafe esta diseñado para monitorear parámetros fisicoquímicos de fuentes naturales de agua mediante un sistema electrónico compacto y portátil basado en un microcontrolador ESP32.

El prototipo AquaSafe utiliza sensores de temperatura, pH, solidos totales disueltos (TDS) y turbidez, con la finalidad de obtener información que pueda ser interpretada por las personas que usen el dispositivo.

En esta carpeta se reúne toda la información y documentación técnica para comprender, reproducir, evaluar y mantener el sistema electrónico.

## Componentes

| Componente | Función | Justificación técnica |
|---|---|---|
| ESP32-WROOM-32 | Procesamiento de señales y control del sistema| Microcontrolador con entradas analógicas, digitales y conectividad que permite conectar los sensores y procesar las mediciones. |
| Sensor de temperatura DS18B20 | Medición de temperatura | Sensor digital que permite obtener mediciones y usa comunicación OneWire. |
| Sensor de pH con módulo PH-4502C | Medición del pH del agua | Permite obtener la señal de la sonda de pH para determinar la acidez o alcalinidad. |
| Sensor de turbidez | Medición de turbidez | Detecta cambios en las propiedades ópticas del agua asociadas a partículas en suspensión. |
| Sensor TDS | Estimación de sólidos disueltos totales | Estima sólidos disueltos a partir de mediciones relacionadas a la conductividad del agua. |
| Pantalla de visualización OLED | Presentación de mediciones | Permite consultar los parámetros monitoreados directamente desde el propio dispositivo. |
| Shield ESP para ESP32 | Conexión y distribución eléctrica | Facilita la conexión de sensores, ordena y organiza el montaje electrónico. | 
| Módulo Power bank con 2 pilas 18650 | Alimentación portátil del dispositivo | Permite suministrar energía al sistema mediante dos baterías recargables. |
| Resistencias de 4,7 kΩ, 10 kΩ y 220–330 Ω | Protección y acondicionamiento eléctrico | Se utilizan para limitar la corriente y adaptación de señales. |
| Cables Dupont | Conexión de componentes | Permite establecer conexiones eléctricas entre el microcontrolador, los sensores, la pantalla y los módulos electrónicos. |
| Cable con conector JST XH2.54 de 3 pines (macho y hembra, 20 cm) | Conexión de componentes | Permite conectar componentes de manera sencilla, facilitando el montaje del prototipo. |
| Placa de acrílico | Base estructural para el montaje | Permite fijar y ordenar los componentes electrónicos. |
| Caja estanca | Protección del sistema electrónico | Protege los componentes frente a la humedad, salpicaduras y polvo. |



## Conexiones electrónicas del ESP32 ##

| Componente | GPIO ESP32 | Descripción de la conexión |
|---|---|---|
| Sensor de temperatura DS18B20 | GPIO 27 | Se conecta al pin 27, con una resistencia de 4,7 kΩ entre DATA y 3V3. |
| Sensor de pH | GPIO 35 | Se conecta al pin 35 mediante un divisor de voltaje. |
| Sensor de turbidez | GPIO 34 | Se conecta al pin 34 mediante un divisor de voltaje. |
| Sensor de TDS | GPIO 32 | Se conecta al pin 32 mediante un divisor de voltaje formado por dos resistencias de 10 kΩ. | 
| Pantalla OLED| GPIO 21 y GPIO 22 | Se conecta mediante SDA al pin 21 y SCL al pin 22. |

## Alimentación y conexiones

- El sensor de temperatura DS18B20 se alimenta con 3,3 V.
- Módulos de pH y turbidez se alimentan con 5 V.
- Todos los componentes comparten una conexión a tierra (GND).
- Se usa una resistencia pull-up de 4,7 kΩ para el sensor de temperatura DS18B20.
- Los sensores de pH, turbidez y TDS emplean divisores de voltaje para conectar sus señales al ESP32.
- Se utilizan divisores resistivos para proteger las entradas analógicas del ESP32.

## Pantalla de visualización 
AquaSafe incorpora una pantalla OLED que permite visualizar directamente las mediciones obtenidas por los sensores conectados al microcontrolador ESP32. 

La pantalla se conecta mediante los pines GPIO 21 (SDA) y GPIO 22 (SCL). 

Su finalidad es facilitar la consulta de temperatura, pH, turbidez y TDS sin la necesidad de utilizar un computador.

**Su objetivo es mostrar en tiempo real:**

- Temperatura del agua (°C).
- Nivel de pH.
- Turbidez del agua.
- sólidos disueltos totales, TDS (ppm).
- Advertencias de sensores desconectados o pendientes de calibración.


## Lista de Materiales (BOM)

La BOM incluye los sensores, componentes electrónicos, materiales estructurales, proveedores y costos de AquaSafe.

El desembolso conocido del grupo es de **$106.759 CLP**.

- [Ver BOM en PDF](bom/bom_completo.pdf)
- [Consultar BOM en Excel](bom/bom_completo.xlsx)




# AquaSafe en Wokwi

Crea un proyecto **ESP32 DevKit V1** en Wokwi y copia `sketch.ino`, `diagram.json` y `libraries.txt` a sus pestañas. Inicia la simulación y abre el Monitor Serie a 115200 baudios.

Los potenciómetros representan las señales de pH, turbidez y TDS en las entradas del ESP32, después de los divisores del circuito físico. Sus valores son entradas de simulación y no calibraciones de sensores. La temperatura usa el DS18B20 y la salida visual usa la OLED I²C.

| Prueba | pH (0–1023) | Turbidez (0–1023) | Resultado esperado |
| --- | ---: | ---: | --- |
| Vaso de referencia | 459 | 930 | REF. POTABLE |
| Vaso con tierra y limón | 617 | 837 | MUESTRA TURBIA |
| Lecturas fuera de ambos perfiles | 530 | 930 | NO SE RECOMIENDA CONSUMIR |

Puedes mover los controles durante la simulación o asignar los valores iniciales en `attrs.value` de cada potenciómetro en `diagram.json`. El estado de la pantalla identifica perfiles de demostración. La potabilidad requiere verificación independiente.

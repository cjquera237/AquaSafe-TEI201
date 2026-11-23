# AquaSafe con dashboard

Esta versión del firmware agrega a `AquaSafe.ino`:
- una red WiFi propia;
- un registro de mediciones en la memoria del ESP32;
- un dashboard web con gráficos.

No necesita internet ni cuentas.

## Cargar el firmware

1. Abrir `AquaSafe_dashboard.ino` en Arduino IDE. Los tres archivos de esta carpeta tienen que quedar juntos.
2. Placa: **ESP32 Dev Module**. En *Herramientas › Partition Scheme* dejar uno que tenga SPIFFS/LittleFS, por ejemplo **Default 4MB with spiffs**.
3. Instalar las mismas librerías que ya usa AquaSafe: Adafruit SSD1306, Adafruit GFX, OneWire y DallasTemperature. WiFi, WebServer y LittleFS vienen con el soporte ESP32.
4. Subir el firmware. Al encender, la OLED muestra la red y la dirección durante 4 segundos.

## Abrir el dashboard

1. En el celular o el notebook, conectarse a la red WiFi **AquaSafe**, clave **aquasafe2026**. El aparato puede avisar que la red no tiene internet: elegir quedarse conectado.
2. Abrir **http://192.168.4.1** en el navegador.

El dashboard muestra:
- el resultado de la muestra;
- la lectura actual de pH, turbidez, TDS y temperatura, comparada con las referencias de la OMS;
- cuántas alertas hubo y qué porcentaje del tiempo estuvo el agua fuera de referencia;
- un gráfico por sensor para los últimos 15 minutos, la última hora o todo el registro.

La pantalla se actualiza cada 2 segundos.

## Datos

- Cada 10 segundos se guarda una fila en `datos.csv`, en la memoria flash del ESP32. El registro sobrevive a apagar el equipo.
- Columnas: `fecha, uptime_s, temp_c, ph, ph_v, tds_ppm, turb_ntu, turb_v, estado_oms, estado_muestra`.
- **Descargar CSV** baja el archivo completo. Sirve para la carpeta `testing/datos/` y para el análisis del informe.
- **Borrar registro** deja el historial vacío. Conviene usarlo antes de la demo.
- El ESP32 no tiene reloj: la fecha la toma del celular la primera vez que se abre el dashboard. Las filas guardadas antes de eso tienen solo `uptime_s`.
- Cuando el archivo pasa de ~600 kB (unas 19 horas de registro), se renombra a `datos_anterior.csv` y se empieza uno nuevo.

## Editar la página

La página está en `dashboard.html`. Al cambiarla hay que regenerar `dashboard_html.h`. Ese archivo contiene el mismo HTML dentro de:

`const char DASHBOARD_HTML[] PROGMEM = R"AQUASAFE( ... )AQUASAFE";`

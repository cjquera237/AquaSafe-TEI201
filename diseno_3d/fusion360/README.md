# Archivos Fusion 360

- **`encapsulado.f3d`**: ensamble completo del gemelo 3D de AquaSafe. Incluye la caja estanca, la tapa, 6 prensaestopas, la placa de acrílico, el ESP32 con su shield, la OLED, el power bank con sus 2 pilas 18650, los módulos de pH y turbidez, el adaptador del DS18B20 y las tres sondas. La vista explosionada está guardada en el espacio Animación.
- **`script_gemelo/GemeloAgua.py`**: script de Python (API de Fusion 360) que genera el ensamble paramétrico y revisa las interferencias. Las medidas son constantes con nombre al comienzo del script.
- **`script_gemelo/GemeloPost.py`**: exporta el `.f3d`, las capturas y los renders.
- **`script_gemelo/interferencias.txt`**: resultado del análisis de interferencias. Da 0 choques entre componentes distintos (844 cuerpos analizados).

## Abrir el ensamble

En Fusion 360: *Archivo › Abrir › Abrir desde mi equipo* y elegir `encapsulado.f3d`.

## Regenerar el ensamble desde el script

1. En Fusion 360, abrir *Utilidades › Complementos* (Shift+S) y entrar en la pestaña *Scripts*.
2. Pulsar *+* y elegir la carpeta `script_gemelo`.
3. Pulsar *Ejecutar*.

Los modelos STEP/STL descargados que importa el script no vienen en el repositorio, porque sus licencias no permiten redistribuirlos sueltos. Ya están incluidos dentro de `encapsulado.f3d`. Las fuentes de cada modelo están en `FUENTES.md`.

## Parámetros principales

| Parámetro | Valor |
|---|---|
| Largo de la caja (`LX`) | 200 mm |
| Ancho de la caja (`WY`) | 155 mm |
| Alto de la base (`HB`) | 74 mm |
| Espesor de la tapa (`LID_T`) | 4 mm |
| Pared (`WALL`) | 3 mm |
| Radio exterior de las esquinas (`R_EXT`) | 10 mm |
| Radio de los prensaestopas (`GLAND_R`) | 10 mm |
| Placa de acrílico | 175 × 116 × 3 mm |

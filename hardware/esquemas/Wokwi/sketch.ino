// AquaSafe para Wokwi. Los potenciómetros representan las señales en los GPIO.
#include <Arduino.h>
#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// AquaSafe | TEI201 | Taller de Diseno
// Prototipo de caracterizacion fisicoquimica de agua.
// Monitor Serie: 115200 baudios, Nueva linea. Enviar "ayuda" para calibrar.
// Calibrar cada canal analogico con dos patrones y el cableado definitivo.
// TDS: A0 -> 10 kOhm -> P32 -> 10 kOhm -> GND; P32 mide la union.
// Verificar el voltaje en cada GPIO antes de conectarlo.
// El estado de pantalla evalua solo los parametros medidos.
// ESP32 DevKit 30 pines
// Pines del shield rotulados Pxx = GPIOxx
constexpr uint8_t PIN_TEMP = 27;       // DS18B20 DATA
constexpr uint8_t PIN_PH   = 35;      // PH-4502C PO (ADC)
constexpr uint8_t PIN_TURB = 34;      // Turbidez OUT (ADC)
constexpr uint8_t PIN_TDS  = 32;      // Placa TDS A0 (ADC)
constexpr uint8_t PIN_SDA  = 21;      // OLED SDA
constexpr uint8_t PIN_SCL  = 22;      // OLED SCL

constexpr int OLED_W = 128;
constexpr int OLED_H = 64;
constexpr uint8_t OLED_ADDR = 0x3C;  // Si queda en blanco, probar 0x3D
Adafruit_SSD1306 display(OLED_W, OLED_H, &Wire, -1);

bool oledReady = false;

OneWire oneWire(PIN_TEMP);
DallasTemperature tempSensor(&oneWire);


Preferences storage;
bool storageReady = false;

struct Calibration {
  float signal[2];
  float reference[2];
};

// Declaracion explicita para el preprocesador de Arduino IDE.
bool hasCalibration(const Calibration &c);
float convertReading(const Calibration &c, float signal);

Calibration calibration[3];
float temperatureOffset = 0;
constexpr const char *keys[] = {"ph", "tds", "turb"};

float readVoltage(uint8_t pin, uint16_t *rawMean = nullptr);

float readVoltage(uint8_t pin, uint16_t *rawMean) {
  // Voltaje en el GPIO, despues del divisor. Calibrar con el cableado definitivo.
  uint32_t sum = 0;
  uint32_t rawSum = 0;
  uint8_t clipped = 0;
  for (int i = 0; i < 24; ++i) {
    int raw = analogRead(pin);
    rawSum += raw;
    if (raw == 0 || raw == 4095) ++clipped;
    sum += raw;
    delay(4);
  }
  if (rawMean) *rawMean = rawSum / 24;
  // Escala virtual para probar la interfaz. La placa real usa su ADC calibrado.
  return clipped > 2 ? NAN : sum * (3.3f / (4095.0f * 24.0f));
}

float readTemperature() {
  tempSensor.requestTemperatures();
  const float t = tempSensor.getTempCByIndex(0);
  return t >= -55 && t <= 125 ? t : NAN;
}

float tdsSignal(float volts, float temp) {
  // Aproximacion de 2% por grado cerca de 25 C. Para calibrar, usar patrones a 25 C.
  if (!isfinite(temp) || temp < 15 || temp > 35) return volts;
  return volts / (1.0f + 0.02f * (temp - 25));
}

bool hasCalibration(const Calibration &c) {
  return isfinite(c.signal[0]) && isfinite(c.signal[1]) &&
         isfinite(c.reference[0]) && isfinite(c.reference[1]) &&
         fabsf(c.signal[1] - c.signal[0]) >= 0.005f;
}

float convertReading(const Calibration &c, float signal) {
  if (!isfinite(signal) || !hasCalibration(c)) return NAN;
  const float fraction = (signal - c.signal[0]) / (c.signal[1] - c.signal[0]);
  if (fraction < 0 || fraction > 1) return NAN;
  return c.reference[0] + fraction * (c.reference[1] - c.reference[0]);
}

void printHelp() {
  Serial.println("AquaSafe | TEI201 | Lecturas de agua");
  Serial.println("115200 baudios, fin de linea Nueva linea. Usa punto decimal.");
  Serial.println("V = senal medida. ppm y UNT requieren calibracion. TDS* = sin compensar.");
  Serial.println("ph 1 4.01 / ph 2 9.18 (ejemplos: usa el pH exacto de tus sobres)");
  Serial.println("tds 1 VALOR / tds 2 VALOR (patrones TDS de la misma escala)");
  Serial.println("turb 1 VALOR / turb 2 VALOR (patrones UNT, por debajo y por encima de 1)");
  Serial.println("temp VALOR (termometro de referencia, opcional), ayuda");
  Serial.println("Calibra con lecturas estables. Cada comando captura la proxima lectura.");
}

void handleCommand(const char *line, const float signals[], float rawTemp) {
  if (!strcmp(line, "ayuda")) {
    printHelp();
    return;
  }
  char sensor[8], extra;
  float reference;
  int point;
  if (sscanf(line, "temp %f %c", &reference, &extra) == 1) {
    const float offset = reference - rawTemp;
    if (isfinite(offset) && fabsf(offset) <= 5 && storageReady &&
        storage.putFloat("temp", offset) == sizeof(float)) {
      temperatureOffset = offset;
      // Los puntos de TDS dependen de la correccion de temperatura.
      calibration[1] = {{NAN, NAN}, {NAN, NAN}};
      if (storage.putBytes("tds", &calibration[1], sizeof(Calibration)) != sizeof(Calibration)) {
        Serial.println("Error al limpiar TDS en memoria. Recalibra ambos puntos antes de reiniciar.");
      }
      Serial.println("Temperatura ajustada. Calibra nuevamente ambos puntos TDS.");
    } else {
      Serial.println("Error: temperatura invalida, diferencia > 5 C o error de memoria.");
    }
    return;
  }
  if (sscanf(line, "%7s %d %f %c", sensor, &point, &reference, &extra) != 3 ||
      point < 1 || point > 2 || !isfinite(reference) || reference < 0) {
    Serial.println("Comando invalido. Escribe ayuda.");
    return;
  }
  for (int i = 0; i < 3; ++i) {
    if (strcmp(sensor, keys[i])) continue;
    if (!isfinite(signals[i]) || (i == 0 && reference > 14)) {
      Serial.println("Senal o referencia invalida. Revisa el sensor y el patron.");
      return;
    }
    Calibration next = calibration[i];
    const int index = point - 1;
    const int other = 1 - index;
    if (isfinite(next.signal[other]) &&
        (fabsf(signals[i] - next.signal[other]) < 0.005f ||
         fabsf(reference - next.reference[other]) < 0.001f)) {
      Serial.println("Los puntos deben tener referencias y senales distintas.");
      return;
    }
    next.signal[index] = signals[i];
    next.reference[index] = reference;
    if (!storageReady || storage.putBytes(keys[i], &next, sizeof(next)) != sizeof(next)) {
      Serial.println("No se pudo guardar la calibracion.");
      return;
    }
    calibration[i] = next;
    Serial.printf("%s punto %d guardado: %.3f V, patron %.2f\n",
                  keys[i], point, signals[i], reference);
    return;
  }
  Serial.println("Sensor desconocido. Escribe ayuda.");
}

void readCommands(const float signals[], float temp) {
  static char line[64];
  static size_t length = 0;
  static bool overflow = false;
  while (Serial.available()) {
    int c = Serial.read();
    if (c == '\r' || c == '\n') {
      if (overflow) Serial.println("Comando demasiado largo.");
      else if (length) {
        line[length] = 0;
        handleCommand(line, signals, temp);
      }
      length = 0;
      overflow = false;
    } else if (length < sizeof(line) - 1) {
      line[length++] = static_cast<char>(c);
    } else {
      overflow = true;
    }
  }
}

static const uint8_t icon0[] PROGMEM = {0x38, 0x00, 0x44, 0x40, 0xd4, 0xa0, 0x54, 0x40, 0xd4, 0x1c, 0x54, 0x06, 0xd4, 0x02, 0x54, 0x02, 0x54, 0x06, 0x92, 0x1c, 0x39, 0x01, 0x75, 0x01, 0x7d, 0x01, 0x39, 0x01, 0x82, 0x00, 0x7c, 0x00};
static const uint8_t icon1[] PROGMEM = {0x20, 0x00, 0x20, 0x00, 0x30, 0x00, 0x50, 0x00, 0x48, 0x00, 0x88, 0x00, 0x04, 0x01, 0x04, 0x01, 0x82, 0x02, 0x02, 0x03, 0x01, 0x05, 0x01, 0x04, 0x02, 0x02, 0x02, 0x02, 0x0c, 0x01, 0xf0, 0x00};
static const uint8_t icon3[] PROGMEM = {0x80, 0x00, 0x84, 0x10, 0x08, 0x08, 0xc0, 0x01, 0x31, 0x46, 0x12, 0x24, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x12, 0x24, 0x31, 0x46, 0xc0, 0x01, 0x08, 0x08, 0x84, 0x10, 0x80, 0x00, 0x00, 0x00};
void drawReading(uint8_t x, uint8_t y, const uint8_t *icon,
                 const char *label, const char *value) {
  display.drawXBitmap(x, y + 3, icon, 16, 16, SSD1306_WHITE);
  display.setCursor(x + 19, y);
  display.print(label);
  display.setCursor(x + 19, y + 11);
  display.print(value);
}


const char *waterStatus(float ph, float tds, float turb) {
  // OMS: referencias operativas para pH y turbidez, y de sabor para TDS.
  constexpr float PH_MIN = 6.5f;
  constexpr float PH_MAX = 8.5f;
  constexpr float TURB_TARGET = 1.0f;    // UNT, meta para desinfeccion
  constexpr float TDS_TASTE_REF = 600.0f; // mg/L, referencia de sabor

  if (isfinite(ph) && (ph < PH_MIN || ph > PH_MAX)) return "pH FUERA DE REF.";
  if (isfinite(turb) && turb >= TURB_TARGET) return "TURB. SOBRE META";
  if (isfinite(tds) && tds >= TDS_TASTE_REF) return "TDS >= 600 mg/L";
  if (!isfinite(ph) || !isfinite(tds) || !isfinite(turb)) return "DATOS INCOMPLETOS";

  // Exigir que la calibracion cubra las referencias antes de mostrar coincidencia.
  const Calibration &phCal = calibration[0];
  const Calibration &tdsCal = calibration[1];
  const Calibration &turbCal = calibration[2];
  if (fminf(phCal.reference[0], phCal.reference[1]) > PH_MIN ||
      fmaxf(phCal.reference[0], phCal.reference[1]) < PH_MAX ||
      fmaxf(tdsCal.reference[0], tdsCal.reference[1]) < TDS_TASTE_REF ||
      fmaxf(turbCal.reference[0], turbCal.reference[1]) < TURB_TARGET) {
    return "REVISAR CALIBRACION";
  }
  return "EN REF. PARCIAL";
}

// Referencias empiricas de la prueba de dos vasos, con el mismo montaje y luz.
// Firmas medidas en la prueba de dos vasos con el montaje y luz de la demo.
const char *demoStatus(float phV, float turbV) {
  constexpr float PH_TAP_MIN = 1.45f;
  constexpr float PH_TAP_MAX = 1.52f;
  constexpr float PH_ALTERED_MIN = 1.95f;
  constexpr float PH_ALTERED_MAX = 2.03f;
  constexpr float TURB_ALTERED_MIN = 2.60f;
  constexpr float TURB_ALTERED_MAX = 2.85f;

  if (!isfinite(phV)) return "SIN LECTURA DE pH";
  const bool matchesTurbidity = turbV >= TURB_ALTERED_MIN && turbV <= TURB_ALTERED_MAX;
  if (matchesTurbidity && phV >= PH_ALTERED_MIN && phV <= PH_ALTERED_MAX) {
    return "MUESTRA TURBIA";
  }
  if (phV >= PH_TAP_MIN && phV <= PH_TAP_MAX && !matchesTurbidity) return "REF. POTABLE";
  return "NO SE RECOMIENDA\nCONSUMIR";
}

void showOLED(float temp, float phV, float tdsV, float turbV, const char *status) {
  if (!oledReady) return;
  char t[16] = "--", p[16] = "--", d[16] = "--", u[16] = "--";
  if (isfinite(temp)) snprintf(t, sizeof(t), "%.1f", temp);
  if (isfinite(phV)) snprintf(p, sizeof(p), "%.3f V", phV);
  if (isfinite(tdsV)) snprintf(d, sizeof(d), "%.3f V", tdsV);
  if (isfinite(turbV)) snprintf(u, sizeof(u), "%.3f V", turbV);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextWrap(false);
  display.fillRect(0, 0, OLED_W, 10, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(4, 1);
  display.print("AquaSafe | DEMO");
  display.setTextColor(SSD1306_WHITE);
  display.drawFastVLine(63, 10, 38, SSD1306_WHITE);
  display.drawFastHLine(0, 29, OLED_W, SSD1306_WHITE);
  drawReading(3, 10, icon0, "Temp C", t);
  drawReading(66, 10, icon1, "pH", p);
  display.setCursor(3, 30);
  display.print("TDS");
  display.setCursor(3, 41);
  display.print(d);
  drawReading(66, 30, icon3, "Turb.", u);
  display.drawFastHLine(0, 48, OLED_W, SSD1306_WHITE);
  // Adafruit GFX reinicia la posicion horizontal al imprimir el salto de linea.
  const bool twoLines = strchr(status, '\n') != nullptr;
  display.setCursor(twoLines ? 0 : 3, twoLines ? 49 : 53);
  display.print(status);
  display.display();
}

void setup() {
  Serial.begin(115200);
  storageReady = storage.begin("aquasafe", false);
  for (int i = 0; i < 3; ++i) {
    calibration[i] = {{NAN, NAN}, {NAN, NAN}};
    if (storageReady && storage.getBytesLength(keys[i]) == sizeof(Calibration)) {
      storage.getBytes(keys[i], &calibration[i], sizeof(Calibration));
    }
  }
  if (storageReady) temperatureOffset = storage.getFloat("temp", 0);
  else Serial.println("Memoria de calibracion no disponible.");

  analogReadResolution(12);
  analogSetPinAttenuation(PIN_PH, ADC_11db);
  analogSetPinAttenuation(PIN_TURB, ADC_11db);
  analogSetPinAttenuation(PIN_TDS, ADC_11db);
  Wire.begin(PIN_SDA, PIN_SCL);
  uint8_t address = OLED_ADDR;
  Wire.beginTransmission(address);
  if (Wire.endTransmission() != 0) address = 0x3D;
  Wire.beginTransmission(address);
  oledReady = Wire.endTransmission() == 0 &&
              display.begin(SSD1306_SWITCHCAPVCC, address, true, false);
  if (oledReady) Serial.printf("OLED detectada en 0x%02X\n", address);
  else Serial.println("OLED sin respuesta. Revisa 3V3, GND, SDA P21 y SCL P22.");
  tempSensor.begin();
  printHelp();
}

void loop() {
  const float rawTemp = readTemperature();
  const float phV = readVoltage(PIN_PH);
  uint16_t tdsRaw = 0;
  const float tdsV = readVoltage(PIN_TDS, &tdsRaw);
  const float turbV = readVoltage(PIN_TURB);
  float temp = rawTemp + temperatureOffset;
  float signals[] = {phV, tdsSignal(tdsV, temp), turbV};
  readCommands(signals, rawTemp);
  temp = rawTemp + temperatureOffset;
  signals[1] = tdsSignal(tdsV, temp);

  const float ph = convertReading(calibration[0], signals[0]);
  const float tds = convertReading(calibration[1], signals[1]);
  const float turb = convertReading(calibration[2], signals[2]);
  const bool tdsCompensated = isfinite(temp) && temp >= 15 && temp <= 35;
  const char *status = waterStatus(ph, tdsCompensated ? tds : NAN, turb);
  const char *sampleStatus = demoStatus(phV, turbV);
  Serial.printf("Temp %.2f C | pH %.2f (%.3f V) | TDS %.0f ppm%s (%.3f V) | Turb %.2f UNT (%.3f V) | %s\n",
                temp, ph, phV, tds, tdsCompensated ? "" : " sin compensar", tdsV, turb, turbV, status);
  Serial.printf("TDS ADC P32: %u/4095 | %s\n", static_cast<unsigned>(tdsRaw),
                !isfinite(tdsV) ? "lectura al limite" :
                !isfinite(tds) ? "revisar calibracion o rango" : "ppm disponibles");
  showOLED(temp, phV, tdsV, turbV, sampleStatus);
  delay(500);
}

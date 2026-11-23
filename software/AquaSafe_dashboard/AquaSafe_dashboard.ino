#include <Arduino.h>
#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <time.h>
#include <sys/time.h>
#include "dashboard_html.h"

// AquaSafe | TEI201 | Taller de Diseno
// Prototipo de caracterizacion fisicoquimica de agua.
// Dashboard: red WiFi AquaSafe (clave aquasafe2026), abrir http://192.168.4.1
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

// Una lectura completa, para el dashboard y el registro CSV.
struct Reading {
  float temp, ph, phV, tds, turb, turbV;
  const char *status;  // Resultado segun referencias OMS (waterStatus).
  const char *sample;  // Resultado de la muestra que se ve en la OLED (demoStatus).
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
    sum += analogReadMilliVolts(pin);
    delay(4);
  }
  if (rawMean) *rawMean = rawSum / 24;
  return clipped > 2 ? NAN : sum / 24000.0f;
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

// Curva nominal del Keyestudio KS0429, manual, pagina 5.
// A0 pasa por un divisor 10 kOhm / 10 kOhm antes de llegar a P32.
float keyestudioCurve(float compensatedAdcVolts) {
  const float v = compensatedAdcVolts * 2.0f;
  return ((133.42f * v - 255.86f) * v + 857.39f) * v * 0.5f;
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

float readTdsPpm(float adcVolts, float compensatedAdcVolts) {
  const float moduleVolts = adcVolts * 2.0f;
  if (!isfinite(moduleVolts) || moduleVolts < 0.0f || moduleVolts > 2.3f) return NAN;

  float ppm = keyestudioCurve(compensatedAdcVolts);
  const Calibration &c = calibration[1];
  if (hasCalibration(c)) {
    // Los puntos guardados son tensiones compensadas en P32.
    // Ajustar la curva del manual a los ppm medidos con los dos patrones.
    const Calibration correction = {
      {keyestudioCurve(c.signal[0]), keyestudioCurve(c.signal[1])},
      {c.reference[0], c.reference[1]}
    };
    ppm = convertReading(correction, ppm);
  }
  return ppm >= 0.0f && ppm <= 1000.0f ? ppm : NAN;
}

void printHelp() {
  Serial.println("AquaSafe | TEI201 | Lecturas de agua");
  Serial.println("115200 baudios, fin de linea Nueva linea. Usa punto decimal.");
  Serial.println("TDS: ppm estimados con curva KS0429 o calibrados con dos patrones.");
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

void showOLED(float temp, float phV, float tdsV, float tdsPpm, bool tdsCalibrated,
              bool tdsCompensated, float turbV, const char *status) {
  if (!oledReady) return;
  char t[16] = "--", p[16] = "--", d[16] = "--", u[16] = "--";
  if (isfinite(temp)) snprintf(t, sizeof(t), "%.1f", temp);
  if (isfinite(phV)) snprintf(p, sizeof(p), "%.3f V", phV);
  if (isfinite(tdsPpm)) snprintf(d, sizeof(d), "%.0f ppm", tdsPpm);
  else if (isfinite(tdsV)) snprintf(d, sizeof(d), "%.3f V", tdsV);
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
  if (!isfinite(tdsPpm)) display.print(isfinite(tdsV) ? "TDS rango" : "TDS");
  else if (!tdsCompensated) display.print("TDS s/temp");
  else display.print(tdsCalibrated ? "TDS" : "TDS est.");
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

// ===================== Red WiFi, registro CSV y dashboard =====================
// El ESP32 crea su propia red WiFi (no necesita internet ni cuentas en la nube).
// Cada LOG_INTERVAL_MS guarda una medicion en /datos.csv (memoria flash LittleFS),
// que sobrevive a reinicios. El dashboard se abre en http://192.168.4.1
const char *AP_SSID = "AquaSafe";
const char *AP_PASS = "aquasafe2026";  // Minimo 8 caracteres.
WebServer server(80);

const char *LOG_PATH = "/datos.csv";
const char *LOG_OLD = "/datos_anterior.csv";
const char *CSV_HEADER =
    "fecha,uptime_s,temp_c,ph,ph_v,tds_ppm,turb_ntu,turb_v,estado_oms,estado_muestra";
// 10 s: el pH tarda ~1 min en estabilizarse y la temperatura cambia lento,
// asi que 6 puntos por minuto bastan para ver tendencias sin llenar la memoria.
constexpr uint32_t LOG_INTERVAL_MS = 10000;
// ~600 kB son unas 7.000 filas (~19 h a 10 s). Al llenarse, se rota el archivo.
constexpr size_t LOG_MAX_BYTES = 600000;

bool fsReady = false;
bool clockSet = false;    // La hora la entrega el navegador al abrir el dashboard.
int32_t tzOffsetMin = 0;  // Minutos al oeste de UTC (igual que getTimezoneOffset).
uint32_t logRows = 0;
uint32_t lastLogMs = 0;

Reading current = {NAN, NAN, NAN, NAN, NAN, NAN, "SIN DATOS", "SIN DATOS"};
Reading lastLogged = current;
char lastLoggedDate[24] = "";
uint32_t lastLoggedUptime = 0;

void formatDate(char *out, size_t size) {
  if (!clockSet) {
    out[0] = 0;
    return;
  }
  const time_t local = time(nullptr) - static_cast<time_t>(tzOffsetMin) * 60;
  struct tm t;
  gmtime_r(&local, &t);
  strftime(out, size, "%Y-%m-%d %H:%M:%S", &t);
}

// Agrega un numero al CSV; si no hay lectura deja el campo vacio.
void csvNumber(String &line, float value, int decimals) {
  if (isfinite(value)) line += String(value, decimals);
  line += ',';
}

// Los estados pueden traer saltos de linea; en el CSV se cambian por espacios.
void csvText(String &line, const char *text) {
  for (const char *p = text; *p; ++p) line += (*p == '\n' || *p == ',' || *p == '"') ? ' ' : *p;
}

void ensureLogFile() {
  if (LittleFS.exists(LOG_PATH)) return;
  File f = LittleFS.open(LOG_PATH, "w");
  if (f) {
    f.println(CSV_HEADER);
    f.close();
  }
}

uint32_t countLogRows() {
  File f = LittleFS.open(LOG_PATH, "r");
  if (!f) return 0;
  uint32_t lines = 0;
  uint8_t buffer[256];
  while (f.available()) {
    const size_t n = f.read(buffer, sizeof(buffer));
    for (size_t i = 0; i < n; ++i) {
      if (buffer[i] == '\n') ++lines;
    }
  }
  f.close();
  return lines > 0 ? lines - 1 : 0;  // Sin contar el encabezado.
}

void logReading(const Reading &r) {
  if (!fsReady) return;
  File probe = LittleFS.open(LOG_PATH, "r");
  if (probe) {
    const size_t size = probe.size();
    probe.close();
    if (size > LOG_MAX_BYTES) {  // Rotar: se conserva el archivo anterior.
      LittleFS.remove(LOG_OLD);
      LittleFS.rename(LOG_PATH, LOG_OLD);
      logRows = 0;
    }
  }
  ensureLogFile();

  char date[24];
  formatDate(date, sizeof(date));
  const uint32_t uptime = millis() / 1000;
  String line;
  line.reserve(140);
  line += date;
  line += ',';
  line += String(uptime);
  line += ',';
  csvNumber(line, r.temp, 2);
  csvNumber(line, r.ph, 2);
  csvNumber(line, r.phV, 3);
  csvNumber(line, r.tds, 0);
  csvNumber(line, r.turb, 2);
  csvNumber(line, r.turbV, 3);
  csvText(line, r.status);
  line += ',';
  csvText(line, r.sample);

  File f = LittleFS.open(LOG_PATH, "a");
  if (!f) return;
  f.println(line);
  f.close();
  ++logRows;
  lastLogged = r;
  strncpy(lastLoggedDate, date, sizeof(lastLoggedDate));
  lastLoggedDate[sizeof(lastLoggedDate) - 1] = 0;
  lastLoggedUptime = uptime;
}

void jsonNumber(String &s, const char *key, float value, int decimals) {
  s += '"';
  s += key;
  s += "\":";
  if (isfinite(value)) s += String(value, decimals);
  else s += "null";
  s += ',';
}

void jsonText(String &s, const char *key, const char *value) {
  s += '"';
  s += key;
  s += "\":\"";
  for (const char *p = value; *p; ++p) {
    if (*p == '\n') {
      s += "\\n";
      continue;
    }
    if (*p == '"' || *p == '\\') s += '\\';
    s += *p;
  }
  s += "\",";
}

void jsonReading(String &s, const Reading &r) {
  jsonNumber(s, "temp", r.temp, 2);
  jsonNumber(s, "ph", r.ph, 2);
  jsonNumber(s, "ph_v", r.phV, 3);
  jsonNumber(s, "tds", r.tds, 0);
  jsonNumber(s, "turb", r.turb, 2);
  jsonNumber(s, "turb_v", r.turbV, 3);
  jsonText(s, "estado", r.status);
  jsonText(s, "muestra", r.sample);
}

void handleRoot() {
  server.send(200, "text/html; charset=utf-8", DASHBOARD_HTML);
}

// Lectura actual + ultima fila guardada (el dashboard la agrega a sus graficos).
void handleNow() {
  char date[24];
  formatDate(date, sizeof(date));
  String s;
  s.reserve(520);
  s += '{';
  jsonReading(s, current);
  jsonText(s, "fecha", date);
  s += "\"uptime\":";
  s += String(millis() / 1000);
  s += ",\"reloj\":";
  s += clockSet ? "true" : "false";
  s += ",\"rows\":";
  s += String(logRows);
  s += ",\"log\":{";
  jsonReading(s, lastLogged);
  jsonText(s, "fecha", lastLoggedDate);
  s += "\"up\":";
  s += String(lastLoggedUptime);
  s += "}}";
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", s);
}

void handleCsv() {
  server.sendHeader("Cache-Control", "no-store");
  File f = fsReady ? LittleFS.open(LOG_PATH, "r") : File();
  if (!f) {
    server.send(200, "text/csv", String(CSV_HEADER) + "\n");
    return;
  }
  server.streamFile(f, "text/csv");
  f.close();
}

void handleTime() {
  const long long epoch = atoll(server.arg("epoch").c_str());
  if (epoch < 1700000000LL) {
    server.send(400, "text/plain", "epoch invalido");
    return;
  }
  struct timeval tv;
  tv.tv_sec = static_cast<time_t>(epoch);
  tv.tv_usec = 0;
  settimeofday(&tv, nullptr);
  tzOffsetMin = server.hasArg("tz") ? server.arg("tz").toInt() : 0;
  clockSet = true;
  server.send(200, "text/plain", "ok");
}

void handleClear() {
  if (fsReady) {
    LittleFS.remove(LOG_PATH);
    LittleFS.remove(LOG_OLD);
    ensureLogFile();
  }
  logRows = 0;
  lastLoggedDate[0] = 0;
  server.send(200, "text/plain", "ok");
}

// Espera atendiendo el dashboard, en vez de un delay() que lo bloquea.
void serveFor(uint32_t ms) {
  const uint32_t start = millis();
  while (millis() - start < ms) {
    server.handleClient();
    delay(2);
  }
}

void setupDashboard() {
  fsReady = LittleFS.begin(true);  // true: formatea la particion la primera vez.
  if (fsReady) {
    ensureLogFile();
    logRows = countLogRows();
    Serial.printf("Registro: %u mediciones guardadas en %s\n",
                  static_cast<unsigned>(logRows), LOG_PATH);
  } else {
    Serial.println("LittleFS no disponible: las mediciones no se guardaran.");
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/now", HTTP_GET, handleNow);
  server.on("/datos.csv", HTTP_GET, handleCsv);
  server.on("/api/time", HTTP_GET, handleTime);
  server.on("/api/borrar", HTTP_POST, handleClear);
  server.onNotFound([]() { server.send(404, "text/plain", "No encontrado"); });
  server.begin();

  const String ip = WiFi.softAPIP().toString();
  Serial.printf("Dashboard: conectate a la red %s (clave %s) y abre http://%s\n",
                AP_SSID, AP_PASS, ip.c_str());
  if (oledReady) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("AquaSafe | Dashboard");
    display.println();
    display.print("WiFi: ");
    display.println(AP_SSID);
    display.print("Clave: ");
    display.println(AP_PASS);
    display.println();
    display.print("http://");
    display.println(ip);
    display.display();
    delay(4000);
  }
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
  setupDashboard();
  printHelp();
}

void loop() {
  server.handleClient();
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
  const bool tdsCalibrated = hasCalibration(calibration[1]);
  const float tds = readTdsPpm(tdsV, signals[1]);
  const float turb = convertReading(calibration[2], signals[2]);
  const bool tdsCompensated = isfinite(temp) && temp >= 15 && temp <= 35;
  const char *status = waterStatus(ph, tdsCalibrated && tdsCompensated ? tds : NAN, turb);
  const char *sampleStatus = demoStatus(phV, turbV);
  const char *tdsMethod = tdsCalibrated ? "calibrado" : "estimado KS0429";
  Serial.printf("Temp %.2f C | pH %.2f (%.3f V) | TDS %.0f ppm %s%s (P32 %.3f V, A0 aprox. %.3f V) | Turb %.2f UNT (%.3f V) | %s\n",
                temp, ph, phV, tds, tdsMethod, tdsCompensated ? "" : " sin compensar",
                tdsV, tdsV * 2.0f, turb, turbV, status);
  Serial.printf("TDS ADC P32: %u/4095 | %s\n", static_cast<unsigned>(tdsRaw),
                !isfinite(tdsV) ? "lectura al limite" :
                !isfinite(tds) ? "fuera del rango del modelo o de la calibracion" : "ppm disponibles");
  showOLED(temp, phV, tdsV, tds, tdsCalibrated, tdsCompensated, turbV, sampleStatus);

  // Datos para el dashboard y el registro CSV.
  current = {temp, ph, phV, tds, turb, turbV, status, sampleStatus};
  if (lastLogMs == 0 || millis() - lastLogMs >= LOG_INTERVAL_MS) {
    lastLogMs = millis();
    logReading(current);
  }
  serveFor(500);
}

#include <Arduino.h>
#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

// Completar solo en la copia local que se sube al ESP32.
constexpr char WIFI_CLEMENTE[] = "";
constexpr char WIFI_PASSWORD_CLEMENTE[] = "";
constexpr char WIFI_NICOLAS[] = "";
constexpr char WIFI_PASSWORD_NICOLAS[] = "";
constexpr char API_KEY[] = "";
constexpr char DEVICE_ID[] = "AquaSafe-01";
constexpr char READINGS_URL[] =
    "https://aquasafe-tei201-production.up.railway.app/api/lecturas";
constexpr uint32_t UPLOAD_INTERVAL_MS = 30000;

static const char RAILWAY_ROOT_CA[] = R"PEM(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIICGzCCAaGgAwIBAgIQQdKd0XLq7qeAwSxs6S+HUjAKBggqhkjOPQQDAzBPMQsw
CQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJuZXQgU2VjdXJpdHkgUmVzZWFyY2gg
R3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBYMjAeFw0yMDA5MDQwMDAwMDBaFw00
MDA5MTcxNjAwMDBaME8xCzAJBgNVBAYTAlVTMSkwJwYDVQQKEyBJbnRlcm5ldCBT
ZWN1cml0eSBSZXNlYXJjaCBHcm91cDEVMBMGA1UEAxMMSVNSRyBSb290IFgyMHYw
EAYHKoZIzj0CAQYFK4EEACIDYgAEzZvVn4CDCuwJSvMWSj5cz3es3mcFDR0HttwW
+1qLFNvicWDEukWVEYmO6gbf9yoWHKS5xcUy4APgHoIYOIvXRdgKam7mAHf7AlF9
ItgKbppbd9/w+kHsOdx1ymgHDB/qo0IwQDAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0T
AQH/BAUwAwEB/zAdBgNVHQ4EFgQUfEKWrt5LSDv6kviejM9ti6lyN5UwCgYIKoZI
zj0EAwMDaAAwZQIwe3lORlCEwkSHRhtFcP9Ymd70/aTSVaYgLXTWNLxBo1BfASdW
tL4ndQavEi51mI38AjEAi/V3bNTIZargCyzuFJ0nN6T5U6VR5CmD1/iQMVtCnwr1
/q4AaOeMSQ+2b1tbFfLn
-----END CERTIFICATE-----
)PEM";

struct DisplayReadings {
  char temperature[16] = "--";
  char ph[16] = "--";
  char turbidity[16] = "--";
  char tds[16] = "--";
  bool tdsInPpm;
  const char *status;
};

DisplayReadings makeDisplayReadings(float temp, float phV, float tdsV, float tdsPpm,
                                   float turbV, const char *status);

static QueueHandle_t dashboardQueue = nullptr;

static void postReading(WiFiClientSecure &client, const char *sensor, const char *reading, const char *unidad) {
  char *unit = nullptr;
  const float value = strtof(reading, &unit);
  if (unit == reading || !isfinite(value)) {
    Serial.printf("Dashboard %s: sin lectura valida\n", sensor);
    return;
  }

  // Conservar los digitos de la OLED e informar la unidad a la API.
  char json[144];
  const int length = snprintf(json, sizeof(json),
                              "{\"dispositivo\":\"%s\",\"sensor\":\"%s\",\"valor\":%.*s,\"unidad\":\"%s\"}",
                              DEVICE_ID, sensor, static_cast<int>(unit - reading), reading, unidad);
  if (length < 0 || static_cast<size_t>(length) >= sizeof(json)) return;

  HTTPClient http;
  http.setConnectTimeout(4000);
  http.setTimeout(4000);
  if (!http.begin(client, READINGS_URL)) {
    Serial.printf("Dashboard %s: no se pudo iniciar HTTPS\n", sensor);
    return;
  }
  http.addHeader("Content-Type", "application/json");
  http.addHeader("x-api-key", API_KEY);
  const int code = http.POST(reinterpret_cast<uint8_t *>(json), static_cast<size_t>(length));
  Serial.printf("Dashboard %s: %s (%d)\n", sensor,
                code >= 200 && code < 300 ? "enviado" : "error de envio", code);
  http.end();
}

static void dashboardTask(void *) {
  WiFiMulti networks;
  if (WIFI_PASSWORD_CLEMENTE[0]) networks.addAP(WIFI_CLEMENTE, WIFI_PASSWORD_CLEMENTE);
  if (WIFI_PASSWORD_NICOLAS[0]) networks.addAP(WIFI_NICOLAS, WIFI_PASSWORD_NICOLAS);
  WiFiClientSecure client;
  client.setCACert(RAILWAY_ROOT_CA);
  client.setHandshakeTimeout(5);
  DisplayReadings readings;
  for (;;) {
    if (xQueueReceive(dashboardQueue, &readings, portMAX_DELAY) != pdTRUE) continue;
    if (WiFi.status() != WL_CONNECTED && networks.run(5000) != WL_CONNECTED) {
      Serial.println("Dashboard: esperando Wi-Fi");
      continue;
    }
    if (time(nullptr) < 1700000000) {
      Serial.println("Dashboard: esperando hora NTP para HTTPS");
      continue;
    }

    Serial.printf("Dashboard OLED: Temp %s | pH %s | Turbidez %s | TDS %s | %s\n",
                  readings.temperature, readings.ph, readings.turbidity, readings.tds, readings.status);
    postReading(client, "temperatura", readings.temperature, "°C");
    postReading(client, "ph", readings.ph, "V");
    postReading(client, "turbidez", readings.turbidity, "V");
    postReading(client, "tds", readings.tds, readings.tdsInPpm ? "ppm" : "V");
    // estado_demo se muestra en OLED; no se envia porque la API no admite ese sensor.
  }
}

static void startDashboard() {
  if ((!WIFI_PASSWORD_CLEMENTE[0] && !WIFI_PASSWORD_NICOLAS[0]) || !API_KEY[0]) {
    Serial.println("Dashboard desactivado: completa al menos una contrasena Wi-Fi y API_KEY.");
    return;
  }
  dashboardQueue = xQueueCreate(1, sizeof(DisplayReadings));
  if (!dashboardQueue) {
    Serial.println("Dashboard: memoria insuficiente para la cola.");
    return;
  }
  if (xTaskCreate(dashboardTask, "dashboard", 12288, nullptr, 1, nullptr) != pdPASS) {
    vQueueDelete(dashboardQueue);
    dashboardQueue = nullptr;
    Serial.println("Dashboard: no se pudo iniciar el envio.");
    return;
  }
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(false);
  configTime(0, 0, "pool.ntp.org", "time.cloudflare.com");
}

static void queueDashboardReadings(const DisplayReadings &readings) {
  if (!dashboardQueue) return;
  static uint32_t lastUpload = millis() - UPLOAD_INTERVAL_MS;
  const uint32_t now = millis();
  if (now - lastUpload < UPLOAD_INTERVAL_MS) return;
  lastUpload = now;
  xQueueOverwrite(dashboardQueue, &readings);
}

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

// Comparacion con el vaso de agua clara de la demo. Usar el mismo montaje y luz.
const char *demoStatus(float phV, float turbV) {
  constexpr float PH_TAP_MIN = 1.45f;
  constexpr float PH_TAP_MAX = 1.52f;
  constexpr float TURB_ALTERED_MIN = 2.60f;
  constexpr float TURB_ALTERED_MAX = 2.85f;

  const bool matchesTurbidity = turbV >= TURB_ALTERED_MIN && turbV <= TURB_ALTERED_MAX;
  if (isfinite(phV) && phV >= PH_TAP_MIN && phV <= PH_TAP_MAX && !matchesTurbidity) {
    return "REF. POTABLE";
  }
  return "NO SE RECOMIENDA CONSUMIR";
}

DisplayReadings makeDisplayReadings(float temp, float phV, float tdsV, float tdsPpm,
                                   float turbV, const char *status) {
  DisplayReadings readings;
  readings.tdsInPpm = isfinite(tdsPpm);
  readings.status = status;
  if (isfinite(temp)) snprintf(readings.temperature, sizeof(readings.temperature), "%.1f", temp);
  if (isfinite(phV)) snprintf(readings.ph, sizeof(readings.ph), "%.3f V", phV);
  if (readings.tdsInPpm) snprintf(readings.tds, sizeof(readings.tds), "%.0f ppm", tdsPpm);
  else if (isfinite(tdsV)) snprintf(readings.tds, sizeof(readings.tds), "%.3f V", tdsV);
  if (isfinite(turbV)) snprintf(readings.turbidity, sizeof(readings.turbidity), "%.3f V", turbV);
  return readings;
}

void showOLED(const DisplayReadings &readings, bool tdsCalibrated, bool tdsCompensated) {
  if (!oledReady) return;
  const char *status = readings.status;

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
  drawReading(3, 10, icon0, "Temp C", readings.temperature);
  drawReading(66, 10, icon1, "pH", readings.ph);
  display.setCursor(3, 30);
  if (!readings.tdsInPpm) display.print(strcmp(readings.tds, "--") ? "TDS rango" : "TDS");
  else if (!tdsCompensated) display.print("TDS s/temp");
  else display.print(tdsCalibrated ? "TDS" : "TDS est.");
  display.setCursor(3, 41);
  display.print(readings.tds);
  drawReading(66, 30, icon3, "Turb.", readings.turbidity);
  display.drawFastHLine(0, 48, OLED_W, SSD1306_WHITE);
  const char *lastSpace = strrchr(status, ' ');
  if (lastSpace && strlen(status) * 6 > OLED_W) {
    // El aviso de consumo se divide antes de la ultima palabra.
    display.setCursor(3, 49);
    display.printf("%.*s", static_cast<int>(lastSpace - status), status);
    display.setCursor(3, 57);
    display.print(lastSpace + 1);
  } else {
    display.setCursor(3, 53);
    display.print(status);
  }
  display.display();
}

void setup() {
  Serial.begin(115200);
  Serial.println("AquaSafe FINAL OLED + Railway (campos compatibles)");
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
  startDashboard();
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
  const bool tdsCalibrated = hasCalibration(calibration[1]);
  const float tds = readTdsPpm(tdsV, signals[1]);
  const float turb = convertReading(calibration[2], signals[2]);
  const bool tdsCompensated = isfinite(temp) && temp >= 15 && temp <= 35;
  const char *status = waterStatus(ph, tdsCalibrated && tdsCompensated ? tds : NAN, turb);
  const char *sampleStatus = demoStatus(phV, turbV);
  const char *tdsMethod = tdsCalibrated ? "calibrado" : "estimado KS0429";
  Serial.printf("Temp %.2f C | pH %.2f (%.3f V) | TDS %.0f ppm %s%s (P32 %.3f V, A0 aprox. %.3f V) | Turb %.2f UNT (%.3f V) | Analisis: %s | Demo: %s%s\n",
                temp, ph, phV, tds, tdsMethod, tdsCompensated ? "" : " sin compensar",
                tdsV, tdsV * 2.0f, turb, turbV, status, sampleStatus,
                isfinite(phV) ? "" : " (sin lectura de pH en P35)");
  Serial.printf("TDS ADC P32: %u/4095 | %s\n", static_cast<unsigned>(tdsRaw),
                !isfinite(tdsV) ? "lectura al limite" :
                !isfinite(tds) ? "fuera del rango del modelo o de la calibracion" : "ppm disponibles");
  const DisplayReadings readings = makeDisplayReadings(temp, phV, tdsV, tds, turbV, sampleStatus);
  showOLED(readings, tdsCalibrated, tdsCompensated);
  queueDashboardReadings(readings);
  delay(500);
}

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Preferences.h>
#include <math.h>

// ========================================
//            AQUASAFE
// Sistema de monitoreo de calidad de agua
// ========================================

// ---------- SENSORES ----------
const int PIN_TEMPERATURA = 21;
const int PIN_PH = 32;
const int PIN_TURBIDEZ = 34;
const int PIN_TDS = 33;  // Reservado, no conectado

// ---------- SEMAFORO ----------
const int LED_VERDE = 16;
const int LED_AMARILLO = 17;
const int LED_ROJO = 18;

// ---------- OBJETOS ----------
OneWire oneWire(PIN_TEMPERATURA);
DallasTemperature sensorTemperatura(&oneWire);
Preferences memoria;

// ---------- CALIBRACIONES ----------
float ph7_mV = NAN;
float ph4_mV = NAN;
float clara_mV = NAN;
float turbia_mV = NAN;

// ========================================
// LECTURA ANALOGICA PROMEDIADA
// ========================================
float leerVoltaje(int pin) {
  float suma = 0;
  for (int i = 0; i < 30; i++) {
    suma += analogReadMilliVolts(pin);
    delay(5);
  }
  return suma / 30.0;
}

// ========================================
// VERIFICAR CALIBRACIONES
// ========================================
bool phCalibrado() {
  return isfinite(ph7_mV) &&
         isfinite(ph4_mV) &&
         fabs(ph7_mV - ph4_mV) > 30;
}

bool turbidezCalibrada() {
  return isfinite(clara_mV) &&
         isfinite(turbia_mV) &&
         fabs(clara_mV - turbia_mV) > 30;
}

// ========================================
// CALCULAR PH REAL
// ========================================
float calcularPH(float voltaje) {
  if (!phCalibrado()) {
    return NAN;
  }
  return 7.0 +
         (voltaje - ph7_mV) *
         (4.0 - 7.0) /
         (ph4_mV - ph7_mV);
}

// ========================================
// TURBIDEZ RELATIVA 0 - 100 %
// No equivale a NTU
// ========================================
float calcularTurbidez(float voltaje) {
  if (!turbidezCalibrada()) {
    return NAN;
  }
  float porcentaje =
      100.0 * (voltaje - clara_mV) /
      (turbia_mV - clara_mV);
  return constrain(porcentaje, 0.0f, 100.0f);
}

// ========================================
// CONTROL DEL SEMAFORO
// 0 = VERDE, 1 = AMARILLO, 2 = ROJO
// ========================================
void encenderSemaforo(int estado) {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_ROJO, LOW);

  if (estado == 0) {
    digitalWrite(LED_VERDE, HIGH);
  } else if (estado == 1) {
    digitalWrite(LED_AMARILLO, HIGH);
  } else {
    digitalWrite(LED_ROJO, HIGH);
  }
}

// ========================================
// COMANDOS DE CALIBRACION
// ========================================
void calibrar(char comando) {
  if (comando == '7') {
    ph7_mV = leerVoltaje(PIN_PH);
    memoria.putFloat("ph7", ph7_mV);
    Serial.println("Referencia pH 7 guardada");
  }

  if (comando == '4') {
    ph4_mV = leerVoltaje(PIN_PH);
    memoria.putFloat("ph4", ph4_mV);
    Serial.println("Referencia pH 4 guardada");
  }

  if (comando == 'L' || comando == 'l') {
    clara_mV = leerVoltaje(PIN_TURBIDEZ);
    memoria.putFloat("clara", clara_mV);
    Serial.println("Referencia agua clara guardada");
  }

  if (comando == 'T' || comando == 't') {
    turbia_mV = leerVoltaje(PIN_TURBIDEZ);
    memoria.putFloat("turbia", turbia_mV);
    Serial.println("Referencia agua turbia guardada");
  }

  if (comando == 'R' || comando == 'r') {
    memoria.clear();
    ph7_mV = NAN;
    ph4_mV = NAN;
    clara_mV = NAN;
    turbia_mV = NAN;
    Serial.println("Calibraciones borradas");
  }
}

// ========================================
// SETUP
// ========================================
void setup() {
  Serial.begin(115200);
  sensorTemperatura.begin();
  analogReadResolution(12);
  analogSetPinAttenuation(PIN_PH, ADC_11db);
  analogSetPinAttenuation(PIN_TURBIDEZ, ADC_11db);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);
  encenderSemaforo(1);

  memoria.begin("aquasafe", false);
  ph7_mV = memoria.getFloat("ph7", NAN);
  ph4_mV = memoria.getFloat("ph4", NAN);
  clara_mV = memoria.getFloat("clara", NAN);
  turbia_mV = memoria.getFloat("turbia", NAN);

  Serial.println();
  Serial.println("==========================");
  Serial.println("        AQUASAFE");
  Serial.println("   SISTEMA INICIADO");
  Serial.println("==========================");
  Serial.println("COMANDOS:");
  Serial.println("7 = Calibrar pH 7");
  Serial.println("4 = Calibrar pH 4");
  Serial.println("L = Agua clara");
  Serial.println("T = Agua turbia");
  Serial.println("R = Borrar calibraciones");
}

// ========================================
// LOOP
// ========================================
void loop() {
  while (Serial.available() > 0) {
    char comando = Serial.read();
    if (comando == '7' ||
        comando == '4' ||
        comando == 'L' ||
        comando == 'l' ||
        comando == 'T' ||
        comando == 't' ||
        comando == 'R' ||
        comando == 'r') {
      calibrar(comando);
    }
  }

  sensorTemperatura.requestTemperatures();
  float temperatura = sensorTemperatura.getTempCByIndex(0);
  bool temperaturaValida =
      temperatura != DEVICE_DISCONNECTED_C &&
      temperatura >= -55 &&
      temperatura <= 125;

  float voltajePH = leerVoltaje(PIN_PH);
  float voltajeTurbidez = leerVoltaje(PIN_TURBIDEZ);
  float ph = calcularPH(voltajePH);
  float turbidez = calcularTurbidez(voltajeTurbidez);

  // Rangos demostrativos; NO certifican potabilidad.
  int estado = 0;

  if (!temperaturaValida || temperatura < 0 || temperatura > 40) {
    estado = 2;
  } else if (temperatura < 5 || temperatura > 35) {
    estado = 1;
  }

  if (!phCalibrado()) {
    estado = max(estado, 1);
  } else if (!isfinite(ph) || ph < 0 || ph > 14 || ph < 6.0 || ph > 9.0) {
    estado = 2;
  } else if (ph < 6.5 || ph > 8.5) {
    estado = max(estado, 1);
  }

  if (!turbidezCalibrada()) {
    estado = max(estado, 1);
  } else if (!isfinite(turbidez) || turbidez > 50) {
    estado = 2;
  } else if (turbidez > 20) {
    estado = max(estado, 1);
  }

  encenderSemaforo(estado);

  Serial.println();
  Serial.println("================================");
  Serial.println("       AQUASAFE - LECTURAS");
  Serial.println("================================");

  Serial.print("TEMPERATURA: ");
  if (temperaturaValida) {
    Serial.print(temperatura, 2);
    Serial.println(" C");
  } else {
    Serial.println("SENSOR NO DETECTADO");
  }

  Serial.print("PH: ");
  if (phCalibrado()) {
    Serial.println(ph, 2);
  } else {
    Serial.println("SIN CALIBRAR");
  }
  Serial.print("PH SENAL: ");
  Serial.print(voltajePH, 0);
  Serial.println(" mV");

  Serial.print("TURBIDEZ: ");
  if (turbidezCalibrada()) {
    Serial.print(turbidez, 1);
    Serial.println(" % relativo");
  } else {
    Serial.println("SIN CALIBRAR");
  }
  Serial.print("TURBIDEZ SENAL: ");
  Serial.print(voltajeTurbidez, 0);
  Serial.println(" mV");

  Serial.println("TDS: NO CONECTADO");
  Serial.println("--------------------------------");

  if (estado == 0) {
    Serial.println("ESTADO: VERDE");
    Serial.println("Parametros dentro de rangos");
  } else if (estado == 1) {
    Serial.println("ESTADO: AMARILLO");
    Serial.println("Precaucion o falta calibracion");
  } else {
    Serial.println("ESTADO: ROJO");
    Serial.println("Parametro fuera de rango");
  }

  Serial.println("EVALUACION DEMOSTRATIVA");
  Serial.println("NO CERTIFICA POTABILIDAD");
  Serial.println("================================");
  delay(2000);
}

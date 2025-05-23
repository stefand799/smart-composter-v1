#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

const char* ssid = "name_of_network";
const char* password = "password_of_network";

WebServer server(80);

#define TEMP_PIN_1 4
#define TEMP_PIN_2 16

OneWire oneWireBus1(TEMP_PIN_1);
OneWire oneWireBus2(TEMP_PIN_2);

DallasTemperature tempSensors1(&oneWireBus1);
DallasTemperature tempSensors2(&oneWireBus2);

const int mqSensor1Pin       = 32;
const int mqSensor2Pin       = 33;
const int MQ135_RAW_MIN      = 100;
const int MQ135_RAW_MAX      = 3500;
const int MQ4_RAW_MIN        = 50;
const int MQ4_RAW_MAX        = 800;
const int humiditySensor1Pin = 34;
const int humiditySensor2Pin = 35;
const int HUMIDITY_RAW_DRY   = 4095;
const int HUMIDITY_RAW_WET   = 0;

const int onBoardLedPin = 2;

float mapToPercent(int rawValue, int rawMin, int rawMax) {
  rawValue = constrain(rawValue, min(rawMin, rawMax), max(rawMin, rawMax));
  return (float)(rawValue - rawMin) / (rawMax - rawMin) * 100.0;
}

void readAllSensorsAndRespond() {
  tempSensors1.requestTemperatures();
  tempSensors2.requestTemperatures();

  float tempC1 = tempSensors1.getTempCByIndex(0);
  float tempC2 = tempSensors2.getTempCByIndex(0);

  float avgTemperature = 0.0;
  int validTemps = 0;
  if (tempC1 != DEVICE_DISCONNECTED_C) { avgTemperature += tempC1; validTemps++; }
  if (tempC2 != DEVICE_DISCONNECTED_C) { avgTemperature += tempC2; validTemps++; }
  avgTemperature = validTemps ? avgTemperature / validTemps : -999.0;

  int mq1RawValue = analogRead(mqSensor1Pin);
  int mq2RawValue = analogRead(mqSensor2Pin);
  float mq1Percent = mapToPercent(mq1RawValue, MQ135_RAW_MIN, MQ135_RAW_MAX);
  float mq2Percent = mapToPercent(mq2RawValue, MQ4_RAW_MIN, MQ4_RAW_MAX);

  int humidity1RawValue = analogRead(humiditySensor1Pin);
  int humidity2RawValue = analogRead(humiditySensor2Pin);
  float humidity1Percent = mapToPercent(humidity1RawValue, HUMIDITY_RAW_DRY, HUMIDITY_RAW_WET);
  float humidity2Percent = mapToPercent(humidity2RawValue, HUMIDITY_RAW_DRY, HUMIDITY_RAW_WET);
  float avgHumidity = (humidity1Percent + humidity2Percent) / 2.0;

  const int capacity = JSON_OBJECT_SIZE(12) + 250;
  StaticJsonDocument<capacity> doc;

  doc["temp1_C"]           = (tempC1 != DEVICE_DISCONNECTED_C) ? tempC1 : nullptr;
  doc["temp2_C"]           = (tempC2 != DEVICE_DISCONNECTED_C) ? tempC2 : nullptr;
  doc["avg_temp_C"]        = avgTemperature;
  doc["mq1_raw"]           = mq1RawValue;
  doc["mq1_percent"]       = mq1Percent;
  doc["mq2_raw"]           = mq2RawValue;
  doc["mq2_percent"]       = mq2Percent;
  doc["humidity1_raw"]     = humidity1RawValue;
  doc["humidity1_percent"] = humidity1Percent;
  doc["humidity2_raw"]     = humidity2RawValue;
  doc["humidity2_percent"] = humidity2Percent;
  doc["avg_humidity"]      = avgHumidity;
  doc["timestamp_ms"]      = millis();

  String jsonResponse;
  serializeJson(doc, jsonResponse);
  server.send(200, "application/json", jsonResponse);

  digitalWrite(onBoardLedPin, HIGH);
  delay(50);
  digitalWrite(onBoardLedPin, LOW);
}

void setup() {
  Serial.begin(115200);
  tempSensors1.begin();
  tempSensors2.begin();
  pinMode(mqSensor1Pin, INPUT);
  pinMode(mqSensor2Pin, INPUT);
  pinMode(humiditySensor1Pin, INPUT);
  pinMode(humiditySensor2Pin, INPUT);
  pinMode(onBoardLedPin, OUTPUT);
  digitalWrite(onBoardLedPin, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  server.on("/data", HTTP_GET, readAllSensorsAndRespond);
  server.begin();
}

void loop() {
  server.handleClient();
}

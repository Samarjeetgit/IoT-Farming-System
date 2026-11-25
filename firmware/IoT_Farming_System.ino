
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SimpleDHT.h>
#include <time.h>


#define WIFI_SSID          "naughty"
#define WIFI_PASSWORD      "12345678"


#define FIREBASE_HOST      "soilmoisturedata-44dd2-default-rtdb.asia-southeast1.firebasedatabase.app"


#define DEVICE_ID          "ESP32-FARMING-01"

// ---- Sensors --------------------------------------------------------------
const int SOIL_PIN       = 32;   // capacitive soil moisture sensor (analog)
const int DHT11_PIN      = 4;    // DHT11 digital pin
SimpleDHT11 dht11(DHT11_PIN);

// ---- Calibration -----------------------------------------------------------
#define MOISTURE_WET_VALUE   800     // raw reading when soil is wettest
#define MOISTURE_DRY_VALUE  3500     // raw reading when soil is driest


const int  PUMP_RELAY_PIN = 25;          // digital pin driving the relay
const int  MOISTURE_DRY_THRESHOLD = 30;  // pump ON when moisture <= 30
const int  MOISTURE_WET_THRESHOLD = 45;  // pump OFF when moisture >= 45
const bool AUTO_PUMP              = true;


const unsigned long SEND_INTERVAL_MS  = 10000;
const unsigned long WIFI_RETRY_MS     = 30000;
const int           MOISTURE_SAMPLES  = 5;


static struct tm  timeinfo;
static bool       haveTime      = false;
static bool       pumpOn        = false;
static unsigned long lastSend   = 0;
static unsigned long lastWiFiRetry = 0;


static void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print("[WiFi] Connecting to '" WIFI_SSID "'");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\n[WiFi] Connected. IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WiFi] Connection failed; will retry...");
  }
}

static void syncTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
}

static void refreshTime() {
  time_t now = time(nullptr);
  if (now > 0 && getLocalTime(&timeinfo)) {
    haveTime = true;
  } else {
    syncTime();
  }
}

static const char* timestampStr() {
  static char buf[32];
  if (!haveTime) {
    refreshTime();
    if (getLocalTime(&timeinfo)) haveTime = true;
  }
  if (!haveTime) return "";
  snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02dZ",
           timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
           timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  return buf;
}

static int readMoisturePercent(int* rawOut) {
  long sum = 0;
  for (int i = 0; i < MOISTURE_SAMPLES; ++i) sum += analogRead(SOIL_PIN);
  int raw = (int)(sum / MOISTURE_SAMPLES);
  int moisture = map(raw, MOISTURE_WET_VALUE, MOISTURE_DRY_VALUE, 100, 0);
  if (rawOut) *rawOut = raw;
  return constrain(moisture, 0, 100);
}

static bool readEnvironment(byte* temp, byte* hum) {
  for (int attempt = 0; attempt < 3; ++attempt) {
    if (dht11.read(temp, hum, NULL) == SimpleDHTErrSuccess) return true;
    delay(1000);
  }
  if (temp) *temp = 0;
  if (hum)  *hum  = 0;
  return false;
}

static void startPump() {
  if (pumpOn) return;
  digitalWrite(PUMP_RELAY_PIN, HIGH);
  pumpOn = true;
  Serial.println("[Pump] Started (relay ON)");
}

static void stopPump() {
  if (!pumpOn) return;
  digitalWrite(PUMP_RELAY_PIN, LOW);
  pumpOn = false;
  Serial.println("[Pump] Stopped (relay OFF)");
}

static String buildPayload(int moisture, int raw, byte temp, byte hum) {
  String json = "{";
  json += "\"device\":\"" + String(DEVICE_ID) + "\",";
  json += "\"moisture\":" + String(moisture) + ",";
  json += "\"raw\":" + String(raw) + ",";
  json += "\"temperature\":" + String(temp) + ",";
  json += "\"humidity\":" + String(hum);
  if (haveTime) json += ",\"timestamp\":\"" + String(timestampStr()) + "\"";
  json += ",\"pump\":" + String(pumpOn ? 1 : 0);
  json += "}";
  return json;
}

static bool sendToFirebase(const String& json) {
  if (WiFi.status() != WL_CONNECTED) return false;
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  String url = "https://" + String(FIREBASE_HOST) + "/sensor.json";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  int code = http.PUT(json);
  if (code > 0) Serial.print("[Firebase] OK, code=");
  else          Serial.print("[Firebase] Error, code=");
  Serial.println(code);
  http.end();
  return code > 0;
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n[ESP32] IoT-Farming-System firmware starting...");
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);   // pump off at boot
  connectWiFi();
  syncTime();
}

void loop() {
  unsigned long now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWiFiRetry >= WIFI_RETRY_MS) {
      lastWiFiRetry = now;
      connectWiFi();
    }
  }


  int raw = 0;
  int moisture = readMoisturePercent(&raw);
  byte temp = 0, hum = 0;
  readEnvironment(&temp, &hum);

  
  if (AUTO_PUMP) {
    if (moisture <= MOISTURE_DRY_THRESHOLD) startPump();
    else if (moisture >= MOISTURE_WET_THRESHOLD) stopPump();
  }


  if (now - lastSend >= SEND_INTERVAL_MS) {
    lastSend = now;
    refreshTime();
    String json = buildPayload(moisture, raw, temp, hum);
    Serial.println("[Data] " + json);
    Serial.print("[Status] Moisture=");
    Serial.print(moisture);
    Serial.print("%  Pump=");
    Serial.println(pumpOn ? "ON" : "OFF");
    sendToFirebase(json);
  }
}

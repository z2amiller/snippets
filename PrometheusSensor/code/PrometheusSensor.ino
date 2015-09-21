/*
 * ESP8266 sensor module.
 * Polls a DHT22 and BMP180 sensor module and returns the results
 * to a prometheus PushGateway.
 * Goes in to deep sleep for 2 minutes between polling intervals, then
 * resets and starts in setup() again.  While asleep this should draw
 * O(100ua) and 4xAA batteries should last 4+ months.
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#define DHTTYPE DHT22
#define DHTPIN  4
#define DHTPWR  13

#define WIFI_SSID           "Embedded Pie"
#define WIFI_PASSWORD       "embedded"

#define SERVER_IP           "10.1.20.200"
#define SERVER_PORT         9091

#define METRICS_JOB         "env_sensor"
#define METRICS_INSTANCE    "LivingRoom"

const String metrics_url = "/metrics/job/" +
                           String(METRICS_JOB) +
                           "/instance/" +
                           String(METRICS_INSTANCE);

ADC_MODE(ADC_VCC);
 
// Initialize DHT sensor 
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

// Use the old BMP085 library - the newer ones requires the Unified Sensor
// library which does not yet support ESP8266.
Adafruit_BMP085 bmp;

unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor

// Note that with the ESP8266, waking up from deep sleep is actually done
// with a reset, so the code will start over in setup() after every loop.
void setup(void)
{
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v console cable

  // Power on the DHT22 sensor first.
  // This also supplies power to the bmp180 board.
  pinMode(DHTPWR, OUTPUT);
  digitalWrite(DHTPWR, HIGH);
  dht.begin();
  
  // Connect to WiFi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("\n\r \n\rWorking to connect");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  Wire.pins(12,  // SDA
            14)  // SCL
  if (!bmp.begin()) {
    Serial.println("BMP init error!");
    ESP.deepSleep(120000000, WAKE_RF_DEFAULT);
    // It can take a while for the ESP to actually go to sleep.
    // When it wakes up we start again in setup().
    delay(5000);
  }
  Serial.println("ESP8266 Sensor Board.");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Format the name of a sensor for prometheus.
String makeSensorVar(const String& name) {
  return "# TYPE env_sensor_" + name + " gauge\n" +
         "env_sensor_" + name +
         "{chipid=\"" + String(ESP.getChipId()) +
         "\",location=\"" + METRICS_INSTANCE + "\"}";
}

void sendMetrics(const String& metricString) {
  WiFiClient client;
  if (!client.connect(SERVER_IP, SERVER_PORT)) {
    Serial.println("Connection error.");
    return;
  }
  int content_length = metricString.length();
  client.print(
    "POST " + metrics_url + " HTTP/1.1\r\n" +
    "Host: " + String(SERVER_IP) + "\r\n" +
    "Connection: close\r\n" +
    "Content-Length: " + String(content_length) + "\r\n" +
    "\r\n" +
    metricString);
  delay(100);
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.println("Server response: " + line);
  }
  client.stop();
}

bool isValidHumidity(const float humidity) {
  return (!isnan(humidity) && humidity >= 0 && humidity <= 100);
}

bool isValidTemp(const float temp) {
  return (!isnan(temp) && temp >= -100 && temp <= 212);
}

float pascalsToInchesHg(const int64 pascals) {
  return pascals * 0.000295299830714;
}

float tempCtoTempF(const float tempC) {
  return (tempC + 32) * 9 / 5;
}

void loop(void)
{
  float temp, humidity;
  // DHT can be slow to return valid temperature and humidity.
  for (int i = 0; i < 4; i++) {
    temp = dht.readTemperature(true);
    if (!isValidTemp(temp)) {
      delay(250);
      continue;
    }
    break;
  }
  for (int i = 0; i < 4; i++) {
    humidity = dht.readHumidity();
    if (!isValidHumidity(humidity)) {
      delay(250);
      continue;
    }
    break;
  }
  
  // TODO(z2amiller): Use strcat or something more polite to build "out".
  String out;
  if (isValidHumidity(humidity)) {
    out += makeSensorVar("humidity") + " " + String(humidity) + "\n";
  }
  if (isValidTemp(temp)) {
    out +=  makeSensorVar("tempF") + " " + String(temp) + "\n";
  }
  // TODO(z2amiller): Poll the actual battery voltage not the LDO output.
  out += makeSensorVar("battery_millivolts") + " " + String(ESP.getVcc()) + "\n";
  out += makeSensorVar("free_heap") + " " + String(ESP.getFreeHeap()) + "\n";
  // TODO(z2amiller): Sanitize the bmp180 outputs.
  out += makeSensorVar("bmp_tempC") + " " + String(bmp.readTemperature()) + "\n";
  out += makeSensorVar("pressure") + " " + String(bmp.readPressure()) + "\n";
  out += makeSensorVar("altitude") + " " + String(bmp.readAltitude()) + "\n";
  Serial.println(out);
  sendMetrics(out);
  // Power off the sensor while the esp is in deep sleep.
  digitalWrite(DHTPWR, LOW);
  ESP.deepSleep(120000000, WAKE_RF_DEFAULT);
  // It can take a while for the ESP to actually go to sleep.
  // When it wakes up we start again in setup().
  delay(5000);
} 


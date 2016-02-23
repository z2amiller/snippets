#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <Math.h>
#include <Wire.h>
#include <prometheus.h>
#include <adc128d818.h>

#define SDAPIN 13
#define SCLPIN 12

#define SERVER_IP           "10.1.20.200"
#define SERVER_PORT         9091

// TODO(z2amiller):  Set up interrupts to call back when the temperature
//                   on one of the pins gets too hot.  Needs to be added to
//                   an options class passed to the ADC library.
#define ADC_INT 14

#define ADC_ADDR 0x37

#define WIFI_SSID           "Embedded Pie"
#define WIFI_PASSWORD       "embedded"

#define NUM_CHANNELS 4

const String channels[NUM_CHANNELS] = {
  "hot_water_out",
  "recirc_return",
  "cold_water_in",
  "ambient",
};

adc128d818 adc = adc128d818(ADC_ADDR, SDAPIN, SCLPIN);
PrometheusClient prom = PrometheusClient(SERVER_IP, SERVER_PORT,
                                         "OctoTemp", "WaterHeater");

void setup(void)
{
  Serial.begin(115200);
  adc.Init();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("\n\r \n\rConnecting to WiFi.");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

float TempC(const uint8_t channel) {
  const uint16_t t = adc.RawValue(channel);
  const int measured_resistance = 10000.0 * ((4096.0 / t) - 1); 
  const float mr_log = log(measured_resistance / 10000.0);
  // Steinhart-Hart coefficients for the Vishay NTCLG100E2103.
  // (10K @ 25C thermistor)
  const float tempK = 1 / (0.003354016 +
                          (0.0002569850 * mr_log) +
                          (0.000002620131 * pow(mr_log, 2)) +
                          (0.00000006383091 * pow(mr_log, 3)));
  return constrain(tempK - 273.15, 0, 100); // tempC
}

float TempF(const uint8_t channel) {
  return TempC(channel) * 1.8 + 32;
}

void loop(void)
{
  MapMetric met = MapMetric("octo_sensor_tempF", "location");
  for (int ch = 0; ch < NUM_CHANNELS; ch++) {
    met.Add(channels[ch], TempF(ch));
    Serial.println(channels[ch] + "(" + ch + ") = " + TempF(ch));
  }
  prom.AddMetric(met);
  prom.Send();
  delay(60000);
} 

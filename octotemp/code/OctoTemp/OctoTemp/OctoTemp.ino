#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <Math.h>
#include <Wire.h>
#include <prometheus.h>

#define SDAPIN 13
#define SCLPIN 12

#define SERVER_IP           "10.1.20.200"
#define SERVER_PORT         9091

// TODO(z2amiller):  Set up interrupts to call back when the temperature
//                   on one of the pins gets too hot.  Needs to be added to
//                   an options class passed to the ADC library.
#define ADC_INT 14

#define ADC_ADDR 0x37
#define NUM_CHANNELS 4


#define WIFI_SSID           "Embedded Pie"
#define WIFI_PASSWORD       "embedded"


const String channels[NUM_CHANNELS] = {
  "hot_water_out",
  "recirc_return",
  "cold_water_in",
  "ambient",
};

// TODO(z2amiller):  Cut this out into a separate library.
class TI128D818 {
 public:
  TI128D818();
  TI128D818(const uint8_t adc_addr);
  bool Init(const uint8_t adc_mode = 0);
  uint16_t RawValue(const uint8_t channel);
 private:
  uint8_t read8(const uint8_t addr);
  uint16_t read16(const uint8_t addr);
  void write8(const uint8_t addr, const uint8_t data);
  void pollUntilReady();
  uint8_t adc_addr_;
};

TI128D818::TI128D818() {
  adc_addr_ = ADC_ADDR;
}

TI128D818::TI128D818(const uint8_t adc_addr) {
    adc_addr_ = adc_addr;
}

// TODO(z2amiller):  Have a maximum allowed elapsed time?
void TI128D818::pollUntilReady() {
  int delay_time = 33;
  while (true) {
    delay(delay_time);
    const int r = read8(0x0C);
    if (r && 0x02) {
      delay_time *= 1.4;
      continue;
    }
    break;
  }
}

// Takes the channel number (0 - 8).  If given an invalid
// channel, returns 0.  The actual I2C channel ID is offset
// by 0x20.
uint16_t TI128D818::RawValue(const uint8_t channel) {
  if (channel < 0 || channel > 8) {
    return 0;
  }
  // This is a 12 bit ADC; bit shift this 16 bit value to get the
  // 12 bits we are interested in.
  return read16(channel + 0x20) >> 4;
}


// TODO(z2amiller):  Provide a configuration class that encapsulates
//                   all of the ADC configuration options.
//                   (pins, modes, etc).
bool TI128D818::Init(const uint8_t adc_mode) {
  Wire.begin(SDAPIN, SCLPIN);
  write8(0x00, 0x00); // disable all.
  pollUntilReady();
  write8(0x0B, 0x02 | 0x01); // Mode 1, external vref.
  write8(0x07, 0x01);        // continuous conversion.
  write8(0x00, 0x02 | 0x01); // interrupt enable, start ADC.
  return true;
}

void TI128D818::write8(const uint8_t addr, const uint8_t data) {
  Wire.beginTransmission(adc_addr_);
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t TI128D818::read8(const uint8_t addr) {
  Wire.beginTransmission(adc_addr_);
  Wire.write(addr);
  Wire.endTransmission();
  Wire.beginTransmission(adc_addr_);
  Wire.requestFrom(adc_addr_, (uint8_t)1);
  const uint8_t ret = Wire.read();
  Wire.endTransmission();
  return ret;
}

uint16_t TI128D818::read16(const uint8_t addr) {
  Wire.beginTransmission(adc_addr_);
  Wire.write(addr);
  Wire.endTransmission();
  Wire.beginTransmission(adc_addr_);
  Wire.requestFrom(adc_addr_, (uint8_t)2);
  uint16_t ret = Wire.read();
  ret <<= 8;
  ret |= Wire.read();
  Wire.endTransmission();
  return ret;
}

TI128D818 adc = TI128D818();
PrometheusClient prom = PrometheusClient(SERVER_IP, SERVER_PORT,
                                         "OctoTemp", "WaterHeater");

void setup(void)
{
  Serial.begin(115200);
  adc.Init(0x1);
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
  return tempK - 273.15; // tempC
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

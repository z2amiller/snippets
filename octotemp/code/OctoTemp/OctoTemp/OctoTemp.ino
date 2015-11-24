#include <Math.h>
#include <Wire.h>

#define SDAPIN 13
#define SCLPIN 12

// TODO(z2amiller):  Set up interrupts to call back when the temperature
//                   on one of the pins gets too hot.  Needs to be added to
//                   an options class passed to the ADC library.
#define ADC_INT 14

#define ADC_ADDR 0x37

// TODO(z2amiller):  Cut this out into a separate library.
class TI128D818 {
 public:
  TI128D818();
  TI128D818(const uint8_t add_addr);
  bool init(const uint8_t adc_mode = 0);
  float temp(const uint8_t channel = 0);
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
// TODO(z2amiller):  Can't dump to serial in a library.  #ifdef debug?
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


// TODO(z2amiller):  Have the TI128D818 library only return the raw
//                   temperature value, this temperature conversion
//                   is very application specific.
float TI128D818::temp(const uint8_t channel) {
  uint16_t t = read16(channel);
  // This is a 12 bit ADC; bit shift this 16 bit value to get the
  // 12 bits we are interested in.
  t >>= 4;
  // Calculate the actual temperature using the Steinhart-Hart
  // formula.
  const int measured_resistance = 10000.0 * ((4096.0 / t) - 1); 
  const float mr_log = log(measured_resistance / 10000.0);
  // Steinhart-hart coefficients for the Vishay NTCLG100E2103.
  // (10K @ 25C thermistor)
  const float tempK = 1 / (0.003354016 +
                          (0.0002569850 * mr_log) +
                          (0.000002620131 * pow(mr_log, 2)) +
                          (0.00000006383091 * pow(mr_log, 3)));
  return tempK - 273.15; // tempC
}

// TODO(z2amiller):  Provide a configuration class that encapsulates
//                   all of the ADC configuration options.
//                   (pins, modes, etc).
bool TI128D818::init(const uint8_t adc_mode) {
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
void setup(void)
{
  Serial.begin(115200);
  adc.init(0x1);
}

// TODO(z2amiller):  Do something interesting with this data.
//                   Post to prometheus or MQTT or something.
void loop(void)
{
  Serial.print("ch 0 = ");
  Serial.print(adc.temp(0x20));
  Serial.print("  ch 1 = ");
  Serial.print(adc.temp(0x21));
  Serial.println("");
  Serial.print("ch 2 = ");
  Serial.print(adc.temp(0x22));
  Serial.print("  ch 3 = ");
  Serial.print(adc.temp(0x23));
  Serial.println("");
  delay(5000);
} 

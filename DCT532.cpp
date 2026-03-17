/*
 * DCT_532.cpp
 *
 *  
 */
/*
 * DCT 532 I2C library
 * 
 *
 */

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


#include "DCT532.h"
#include "Wire.h"
#include <cstring>

/**************************************************************************/
/*!
    @brief Constructor
*/
/**************************************************************************/
DCT532::DCT532() {
  _i2caddr = DCT532_DEFAULT_ADDRESS;
  _pressureUnit = 0xFF; // unknown
}

/**************************************************************************/
/*!
    @brief Initialize and test connection
*/
/**************************************************************************/
bool DCT532::begin(uint8_t i2caddr) {
  _i2caddr = i2caddr;
  Wire.begin();

  // Simple connection test: just see if it ACKs
  Wire.beginTransmission(_i2caddr);
  if (Wire.endTransmission() != 0) {
    return false;
  }

  // Optional: reset config to defaults (float, little-endian, etc.)
  writeRegister(CONFIG, 0x00);

  // Set default unit (PSI if supported)
  setPressureUnits(DCT532_PRESSURE_UNITS_PSI);

  _temperatureUnit = readRegister(0x4D);

  return true;
}

/**************************************************************************/
/*!
    @brief Write 8-bit value to register
*/
/**************************************************************************/
bool DCT532::writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(reg);
  Wire.write(value);
  return (Wire.endTransmission() == 0);
}

/**************************************************************************/
/*!
    @brief Read single 8-bit register
*/
/**************************************************************************/
uint8_t DCT532::readRegister(uint8_t reg) {
  Wire.beginTransmission(_i2caddr);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(_i2caddr, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0xFF; // error
}

/**************************************************************************/
/*!
    @brief Get status byte (0x00)
*/
/**************************************************************************/
uint8_t DCT532::getStatus() {
  return readRegister(STATUS);
}

/**************************************************************************/
/*!
    @brief Check if new data is ready (bit 0 usually)
*/
/**************************************************************************/
bool DCT532::dataReady() {
  return (getStatus() & 0x01) != 0;
}

/**************************************************************************/
/*!
    @brief Read pressure as float (default format)
    Blocks/polls until data ready or timeout
*/
/**************************************************************************/
float DCT532::readPressure() {
  uint8_t buf[4];

  // Wait for ready (timeout ~500 ms)
  unsigned long start = millis();
  while (!dataReady()) {
    if (millis() - start > 500) return NAN;
    delay(1);
  }

  Wire.beginTransmission(_i2caddr);
  Wire.write(PRESSURE);
  Wire.endTransmission(false); // repeated start

  Wire.requestFrom(_i2caddr, (uint8_t)4);
  if (Wire.available() < 4) return NAN;

  for (int i = 0; i < 4; i++) {
    buf[i] = Wire.read();
  }

  // Default little-endian: buf[0]=LSB ... buf[3]=MSB (exponent/sign)
  return bytesToFloat(buf[0], buf[1], buf[2], buf[3]);
}

/**************************************************************************/
/*!
    @brief Read temperature as float
*/
/**************************************************************************/
float DCT532::readTemperature() {
  uint8_t buf[4];

  // Assume same ready flag for both pressure & temp
  unsigned long start = millis();
  while (!dataReady()) {
    if (millis() - start > 500) return NAN;
    delay(1);
  }

  Wire.beginTransmission(_i2caddr);
  Wire.write(TEMPERATURE);
  Wire.endTransmission(false);

  Wire.requestFrom(_i2caddr, (uint8_t)4);
  if (Wire.available() < 4) return NAN;

  for (int i = 0; i < 4; i++) {
    buf[i] = Wire.read();
  }

  return bytesToFloat(buf[0], buf[1], buf[2], buf[3]);
}

/**************************************************************************/
/*!
    @brief Convert 4 bytes to float (little-endian default)
    Call bytesToFloat(buf[0], buf[1], buf[2], buf[3])
*/
/**************************************************************************/
float DCT532::bytesToFloat(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) {
  union {
    float f;
    uint8_t bytes[4];
  } u;
  u.bytes[0] = b0; // LSB
  u.bytes[1] = b1;
  u.bytes[2] = b2;
  u.bytes[3] = b3; // MSB (sign/exponent)
  return u.f;
}

/**************************************************************************/
/*!
    @brief Set pressure unit
*/
/**************************************************************************/
void DCT532::setPressureUnits(dct532_pressure_unit units) {
  if(!writeRegister(PRESSUREUNIT, (uint8_t)units)){
     Serial.println("NACK on pressure unit set!")};
  _pressureUnit = (uint8_t)units;
}

/**************************************************************************/

/*!
    @brief Set temperature unit
*/
/**************************************************************************/
void DCT532::setTemperatureUnits(dct532_temperature_unit units) {
  if(!writeRegister(TEMPERATUREUNIT, (uint8_t)units)){
     Serial.println("NACK on tmeperature unit set!")};
  _temperatureUnit = (uint8_t)units;  //cache 
}

/**************************************************************************/

/*!
    @brief Get pressure unit string (based on last set/read)
*/
/**************************************************************************/
const char* DCT532::getPressureUnits() {
  if (_pressureUnit == 0xFF) {
    _pressureUnit = readRegister(PRESSUREUNIT);
  }
  switch (_pressureUnit) {
    case 0x05: return "psi";
    case 0x02: return "bar";
    case 0x0F: return "atm";
    case 0x0B: return "Pa";
    default: return "unknown";
  }
}

/*!
    @brief Get pressure unit string (based on last set/read)
*/
/**************************************************************************/
const char* DCT532::getTemperatureUnits() {
  if (_temperatureUnit == 0xFF) {	// 0xFF = uninitialized
    _temperatureUnit = readRegister(TEMPERATUREUNIT);
  }
  switch (_temperatureUnit) {
    case 0x20: return "°C";
    case 0x21: return "°F";
    case 0x22: return "°R";
    case 0x23: return "K";
    default: return "unknown";
  }
}

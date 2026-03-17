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
  //writeRegister(CONFIG, 0x00);
  setOutputType(false);  // 16-bit int
  setBigEndian(false);  // or true for big endian
  
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
    @brief Set output format: 16-bit integer or 32-bit float
    @param integerMode true = 16-bit signed int, false = float (default)
    @return true on success (ACK received)
*/
/**************************************************************************/
bool DCT532::setOutputType(bool integerMode) {
  uint8_t cfg = readRegister(CONFIG); // 0x40
  if (integerMode) {
    cfg |= 0x01; // set bit 0 (TYPE=1)
  } else {
    cfg &= ~0x01; // clear bit 0 (TYPE=0)
  }
  return writeRegister(CONFIG, cfg) == 0; // assume writeRegister returns endTransmission result
}

/**************************************************************************/
/*!
    @brief Set byte order (endianness)
    @param bigEndian true = high byte first, false = low byte first
    @return true on success
*/
/**************************************************************************/
bool DCT532::setBigEndian(bool bigEndian) {
  uint8_t cfg = readRegister(CONFIG);
  if (bigEndian) {
    cfg |= 0x02; // set bit 1 (ORDER=1)
  } else {
    cfg &= ~0x02; // clear bit 1
  }
  return writeRegister(CONFIG, cfg) == 0;
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
  u.bytes[0] = b3; // MSB (sign/exponent)
  u.bytes[1] = b2;
  u.bytes[2] = b1;
  u.bytes[3] = b0; // LSB
  return u.f;
}

/**************************************************************************/
/*!
    @brief Set pressure unit
*/
/**************************************************************************/
void DCT532::setPressureUnits(dct532_pressure_unit units) {
  if(!writeRegister(PRESSUREUNIT, (uint8_t)units)){
     Serial.println("NACK on pressure unit set!");
  }
  _pressureUnit = (uint8_t)units;
}

/**************************************************************************/

/*!
    @brief Set temperature unit
*/
/**************************************************************************/
void DCT532::setTemperatureUnits(dct532_temperature_unit units) {
  if(!writeRegister(TEMPERATUREUNIT, (uint8_t)units)){
     Serial.println("NACK on tmeperature unit set!");
  }
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

/**************************************************************************/
/*!
    @brief Read pressure as 16-bit signed integer (when in int16 mode)
    Scales to your unit (e.g. bar, psi) based on nominal range
*/
/**************************************************************************/
int16_t DCT532::readPressureInt16() {
  uint8_t buf[2];

  // Poll ready (same as before)
  unsigned long start = millis();
  while (!dataReady()) {
    if (millis() - start > 500) return 0; // or error value
    delay(1);
  }

  Wire.beginTransmission(_i2caddr);
  Wire.write(PRESSURE); // 0x01
  Wire.endTransmission(false);

  Wire.requestFrom(_i2caddr, (uint8_t)2);
  if (Wire.available() < 2) return 0;

  buf[0] = Wire.read();
  buf[1] = Wire.read();

  // Assemble signed 16-bit (little-endian default)
  int16_t val = (int16_t)((buf[1] << 8) | buf[0]); // big-endian on wire? swap to (buf[0] << 8) | buf[1]

  return val;
}

/**************************************************************************/
/*!
    @brief Read temperature as 16-bit signed integer (when in int16 mode)
*/
/**************************************************************************/
int16_t DCT532::readTemperatureInt16() {
  uint8_t buf[2];

  // same polling...

  Wire.beginTransmission(_i2caddr);
  Wire.write(0x03); // temp start in int16 mode
  Wire.endTransmission(false);

  Wire.requestFrom(_i2caddr, (uint8_t)2);
  if (Wire.available() < 2) return 0;

  buf[0] = Wire.read();
  buf[1] = Wire.read();

  int16_t val = (int16_t)((buf[1] << 8) | buf[0]); // adjust for endian if needed

  return val;
}

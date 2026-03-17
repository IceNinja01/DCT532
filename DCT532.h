/*
 * DCT532.h - Library for BD|SENSORS DCT532 I²C pressure/temperature sensor
 */

#ifndef DCT532_h
#define DCT532_h

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

/* I2C ADDRESS */
#define DCT532_DEFAULT_ADDRESS 0x28 // DEFAULT ON DATASHEET

/* Registers */
#define STATUS 0x00
#define PRESSURE 0x01 // 4 bytes float (or 2 bytes int16)
#define TEMPERATURE 0x03 // 4 bytes float (or 2 bytes int16)
#define CONFIG 0x40
#define SLAVEADDRESS 0x43
#define PRESSUREUNIT 0x44
#define TEMPERATUREUNIT 0x4D // often 0x4E or 0x4D - check your device

/* Pressure unit codes (from datasheet examples - adjust if needed) */
typedef enum {
  DCT532_PRESSURE_UNITS_PSI = 0x05, // PSI (common)
  DCT532_PRESSURE_UNITS_BAR = 0x02, // bar
  DCT532_PRESSURE_UNITS_MBAR = 0x06,
  DCT532_PRESSURE_UNITS_KPA = 0x0A,
  DCT532_PRESSURE_UNITS_ATM = 0x0F, // approx - verify
  DCT532_PRESSURE_UNITS_PA = 0x0B
} dct532_pressure_unit;

/* Temperature unit codes (from datasheet examples - adjust if needed) */
typedef enum {
	DCT532_TEMP_C		= 0X20,		// °C DEFAULT
	DCT532_TEMP_F		= 0X21,		// °F
	DCT532_TEMP_RANKINE	= 0X22,		// °R
	DCT532TEMP_KELVIN	= 0X23,		// K
} dct532_temperature_unit;

class DCT532 {
  public:
    DCT532();
    bool begin(uint8_t addr = DCT532_DEFAULT_ADDRESS);
    float readPressure();
    float readTemperature();
    uint8_t getStatus();
    bool dataReady();
    void setPressureUnits(dct532_pressure_unit units);
	void setTemperatureUnits(dct532_temperature_unit units);
    const char* getPressureUnits();
	const char* getTemperatureUnits();
    float bytesToFloat(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3); // little-endian default

    // Optional - for debug / advanced
    uint8_t readRegister(uint8_t reg);
    bool writeRegister(uint8_t reg, uint8_t value);

  private:
    uint8_t _i2caddr;
    uint8_t _pressureUnit;
 	uint8_t _temperatureUnit;
};

#endif

# DCT532 Arduino Library

Pressure Sensor Library for the DCT532 I2C pressure sensor made by BDsensors, which can be found here, https://www.bdsensors.de/en/evaluation/displays/details/produkt/dct-532/.

Supports BD|SENSORS DCT532 pressure transmitter with I²C interface.

**Note on Temperature**:  
On this hardware variant (DC0-1602-IC-3-N17-N40-1-000), temperature output is not functional/available via I²C (reads invalid float values like ovf or near-zero, even though status shows no error). Pressure works correctly.  
The sensor's internal temperature (electronics temp) may not be exposed in all configurations. Tmeprature reads should work for the 532i version.

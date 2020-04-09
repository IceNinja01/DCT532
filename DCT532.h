/*
 * DCT_532.h
 *
 */



#ifndef DCT532_h
#define DCT532_h

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


#include "Wire.h"

/*========================================================
    I2C ADDRESS/BITS
    ------------------------------------------------------*/
    #define DCT532_DEFAULT_ADDRESS                 (0x32)
/*========================================================*/


//Registry address
#define STATUS 			0X00
#define PRESSURE		0x01 // 0x01 to 0x04
#define CONFIG			0x40
#define SLAVEADDRESS	0x43
#define PRESSUREUNIT	0x44



typedef enum
{
	DCT532_PRESSURE_UNITS_PSI		= 0x06,	//Default-Pounds per square inch
	DCT532_PRESSURE_UNITS_BAR		= 0x07,	//Bar
	DCT532_PRESSURE_UNITS_ATM		= 0x0E,	//Atmosphere
	DCT532_PRESSURE_UNITS_PA		= 0x0B,	//Pascals

}dct532_pressure_unit;




class DCT532
{
	public:
		DCT532();
		bool begin(uint8_t addr = DCT532_DEFAULT_ADDRESS);
		float bytesToFloat(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0);
		uint8_t readRegistery(uint8_t reg);
		uint8_t* readRegistery(uint8_t reg, int byteNum);
		void writeRegistery(uint8_t reg, uint8_t value);
		void read();
		void setPressureUnits(dct532_pressure_unit units);
		char* getPressureUnits(void);
		uint8_t arry[5]; //array for bytes
		int32_t pres;
		float pressure; //data from sensor
		int status;		//status from sensor new reading (1) or old (0)
	private:
		int8_t  _i2caddr;
		uint8_t pres_Hex;

};

#endif

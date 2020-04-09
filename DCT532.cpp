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



DCT532::DCT532(){

}


/**************************************************************************/
/*!
    @brief  starts and configures the device
*/
bool DCT532::begin(uint8_t i2caddr) {
  Wire.begin();
  _i2caddr = i2caddr;
  /* Check connection */
    uint8_t deviceid = readRegistery(SLAVEADDRESS);
    if (deviceid != _i2caddr)
    {
      /* No DCT detected ... return false */
      //Serial.println(deviceid, HEX);
      return false;
    }
    //Reset Sensor
    writeRegistery(CONFIG, 0x00);

    //Set unit to PSI--Default
    setPressureUnits(DCT532_PRESSURE_UNITS_PSI);

    return true;

}
/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register
*/
/**************************************************************************/
void DCT532::writeRegistery(uint8_t reg, uint8_t value){
  Wire.beginTransmission(_i2caddr);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t DCT532::readRegistery(uint8_t reg) {
	Wire.beginTransmission(_i2caddr);
	Wire.write(reg);
	Wire.endTransmission();

	Wire.requestFrom(_i2caddr,1);
	if (! Wire.available()) return -1;
	return (Wire.read());
}

uint8_t* DCT532::readRegistery(uint8_t reg, int byteNum) {
	uint8_t* byteArray = new uint8_t[byteNum];
	Wire.beginTransmission(_i2caddr);
	Wire.write(reg);
	Wire.endTransmission();

	Wire.requestFrom(_i2caddr,byteNum);
	if (! Wire.available()){
	}
	for (int i =0; i<byteNum; i++) {
		byteArray[i] = Wire.read();
	}
	return byteArray;
}

void DCT532::read(){
	Wire.beginTransmission(_i2caddr); // transmit to device #112 (0x70)
//	  Wire.write(0);      // sets Configuration Resgistry
	  Wire.endTransmission();      // stop transmitting
	    //  request reading from sensor
	  Wire.requestFrom(0x32, 5);    // request 9 bytes from slave device #32

	if(5 <= Wire.available()) {
	    for (int i =0; i<5; i++) {
	        arry[i] = Wire.read();  // receive high byte (overwrites previous reading)
	    }
	  }


}

/*
 * Sets the Pressure sensor units
 */

void DCT532::setPressureUnits(dct532_pressure_unit units){
	writeRegistery(PRESSUREUNIT, units);

}

char* DCT532::getPressureUnits(void){
	pres_Hex = readRegistery(PRESSUREUNIT);
	if(pres_Hex== 0x06){
		return "psi";
	}
	if(pres_Hex== 0x07){
		return "bar";
	}
	if(pres_Hex== 0x0E){
		return "atm";
	}
	if(pres_Hex== 0x0B){
		return "pa";
	}
	else{
		return "NaN";
	}
}

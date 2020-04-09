#include <DCT532.h>
DCT532 dct = DCT532();
byte reading[5];

void setup() {
  // put your setup code here, to run once:
    delay(5000);

  Serial.begin(9600);
  delay(2000);
  Serial.println("DCT532 Sensor Test");
//  if(!dct.begin()){
//    Serial.println("Could not start");
//    while(1);
//  }
  Serial.println(dct.begin());
  Serial.println("DCT532 Found");
  dct.setPressureUnits(DCT532_PRESSURE_UNITS_PSI);
  Serial.print("Pressure Units = "); Serial.print(dct.getPressureUnits());  
  Serial.println("");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  dct.read();
  for (int i =0; i<5; i++) {    
      reading[i]=dct.arry[i];
//      Serial.print(reading[i]);   // print the reading Bytes  
//      Serial.print("  ");
    }
  Serial.print("Pressure is = "); 
  float newpres = bytes_to_float(reading[1],reading[2],reading[3],reading[4]);
  Serial.print("   ");Serial.print(newpres);Serial.print(" ");
  Serial.println(dct.getPressureUnits());
  delay(500);
}

float bytes_to_float(byte b3,byte b2,byte b1,byte b0)
{
    byte b[] = {b3,b2,b1,b0};
    float f;
    memcpy(&f, &b, sizeof(f));
    return f;
}

#include <Arduino.h>
#include "function.h"
#include "ICM_20948.h"


#define SERIAL_PORT Serial
#define WIRE_PORT Wire
#define AD0_VAL 1

ICM_20948_I2C myICM;

float accX, accY, accZ;
float gyrX, gyrY, gyrZ;
float magX, magY, magZ;


void setup() {
  SERIAL_PORT.begin(115200);
  while (!SERIAL_PORT) {};
  
  WIRE_PORT.begin();
  WIRE_PORT.setClock(400000);
  
  bool initialized = false;
  while (!initialized) {
    
    myICM.begin(WIRE_PORT, AD0_VAL);
    SERIAL_PORT.print(F("Init status: "));
    SERIAL_PORT.println(myICM.statusString());
    if (myICM.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println("Retrying...");
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }
  while (!myICM.dataReady()) {}
  myICM.getAGMT();
  int8_t temp = myICM.temp();
  SERIAL_PORT.print("Temperature: ");
  SERIAL_PORT.println(temp);
  
}

void loop() {
  
  if (myICM.dataReady())
  {
    myICM.getAGMT();         
    
    accX = myICM.accX(); accY = myICM.accY(); accZ = myICM.accZ();
    gyrX = myICM.gyrX(); gyrY = myICM.gyrY(); gyrZ = myICM.gyrZ();
    magX = myICM.magX(); magY = myICM.magY(); magZ = myICM.magZ();
    
    SERIAL_PORT.print(accX); SERIAL_PORT.print(", ");
    SERIAL_PORT.print(accY); SERIAL_PORT.print(", ");
    SERIAL_PORT.print(accZ); SERIAL_PORT.print(", ");
    SERIAL_PORT.print(gyrX); SERIAL_PORT.print(", ");
    SERIAL_PORT.print(gyrY); SERIAL_PORT.print(", ");
    SERIAL_PORT.print(gyrZ); SERIAL_PORT.print(", ");
    SERIAL_PORT.print(magX); SERIAL_PORT.print(", ");
    SERIAL_PORT.print(magY); SERIAL_PORT.print(", ");
    SERIAL_PORT.print(magZ); SERIAL_PORT.println();
    
  }
  delay(100);
}

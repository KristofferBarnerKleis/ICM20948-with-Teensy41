#include <Arduino.h>
#include "function.h"
#include "ICM_20948.h"

#define SERIAL_PORT Serial
#define WIRE_PORT Wire
#define AD0_VAL 1

ICM_20948_I2C myICM;

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

  int8_t temp = myICM.agmt.tmp.val();
  SERIAL_PORT.print("Temperature: ");
  SERIAL_PORT.print(temp);
}

void loop() {
  
  if (myICM.dataReady())
  {
    myICM.getAGMT();         
    
  }
  delay(100);
}

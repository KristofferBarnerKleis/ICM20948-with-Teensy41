#include <Arduino.h>
//#include "function.h"
#include "ICM_20948.h"
#include <typeinfo>

#define SERIAL_PORT Serial
#define WIRE_PORT Wire
#define AD0_VAL 1

ICM_20948_I2C myICM;

float accX, accY, accZ;
float gyrX, gyrY, gyrZ;
float magX, magY, magZ;

uint8_t gyroX, gyroY, gyroZ, accelX, accelY, accelZ, mgX, mgY, mgZ = 0;

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
  
  bool success = true;
  
  success &= (myICM.initializeDMP() == ICM_20948_Stat_Ok);
  
  // DMP sensor options are defined in ICM_20948_DMP.h
  //    INV_ICM20948_SENSOR_ACCELEROMETER               (16-bit accel)
  //    INV_ICM20948_SENSOR_GYROSCOPE                   (16-bit gyro + 32-bit calibrated gyro)
  //    INV_ICM20948_SENSOR_RAW_ACCELEROMETER           (16-bit accel)
  //    INV_ICM20948_SENSOR_RAW_GYROSCOPE               (16-bit gyro + 32-bit calibrated gyro)
  //    INV_ICM20948_SENSOR_MAGNETIC_FIELD_UNCALIBRATED (16-bit compass)
  //    INV_ICM20948_SENSOR_GYROSCOPE_UNCALIBRATED      (16-bit gyro)
  //    INV_ICM20948_SENSOR_STEP_DETECTOR               (Pedometer Step Detector)
  //    INV_ICM20948_SENSOR_STEP_COUNTER                (Pedometer Step Detector)
  //    INV_ICM20948_SENSOR_GAME_ROTATION_VECTOR        (32-bit 6-axis quaternion)
  //    INV_ICM20948_SENSOR_ROTATION_VECTOR             (32-bit 9-axis quaternion + heading accuracy)
  //    INV_ICM20948_SENSOR_GEOMAGNETIC_ROTATION_VECTOR (32-bit Geomag RV + heading accuracy)
  //    INV_ICM20948_SENSOR_GEOMAGNETIC_FIELD           (32-bit calibrated compass)
  //    INV_ICM20948_SENSOR_GRAVITY                     (32-bit 6-axis quaternion)
  //    INV_ICM20948_SENSOR_LINEAR_ACCELERATION         (16-bit accel + 32-bit 6-axis quaternion)
  //    INV_ICM20948_SENSOR_ORIENTATION                 (32-bit 9-axis quaternion + heading accuracy)
  
  success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_ORIENTATION) == ICM_20948_Stat_Ok);
  success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_ACCELEROMETER) == ICM_20948_Stat_Ok);
  success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_GYROSCOPE) == ICM_20948_Stat_Ok);
  success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_GEOMAGNETIC_FIELD) == ICM_20948_Stat_Ok);
  
  success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Quat9, 0) == ICM_20948_Stat_Ok); 
  success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Accel, 0) == ICM_20948_Stat_Ok); // Accel
  success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Gyro, 0) == ICM_20948_Stat_Ok);  // Gyro
  success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Geomag, 0) == ICM_20948_Stat_Ok); // Compass
  
  
  // Enable the FIFO
  success &= (myICM.enableFIFO() == ICM_20948_Stat_Ok);
  
  // Enable the DMP
  success &= (myICM.enableDMP() == ICM_20948_Stat_Ok);
  
  // Reset DMP
  success &= (myICM.resetDMP() == ICM_20948_Stat_Ok);
  
  // Reset FIFO
  success &= (myICM.resetFIFO() == ICM_20948_Stat_Ok);
  
  if (success)
  {
    SERIAL_PORT.println(F("DMP enabled!"));
  }
  else
  {
    SERIAL_PORT.println(F("Enable DMP failed!"));
    while (1) {}
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
    icm_20948_DMP_data_t data;
    myICM.readDMPdataFromFIFO(&data);
    if ((myICM.status == ICM_20948_Stat_Ok) || (myICM.status == ICM_20948_Stat_FIFOMoreDataAvail)) // Was valid data available?
    {
      myICM.getAGMT();
      accX = myICM.accX(); accY = myICM.accY(); accZ = myICM.accZ();
      
      SERIAL_PORT.print(accX); SERIAL_PORT.print(", ");
      SERIAL_PORT.print(accY); SERIAL_PORT.print(", ");
      SERIAL_PORT.print(accZ); SERIAL_PORT.print(", ");
      SERIAL_PORT.print(data.Accel_Accuracy); SERIAL_PORT.print(", ");
      SERIAL_PORT.print(data.Gyro_Accuracy); SERIAL_PORT.print(", ");
      SERIAL_PORT.print(data.Compass_Accuracy); SERIAL_PORT.println();
      
      if ((myICM.status == ICM_20948_Stat_Ok) || (myICM.status == ICM_20948_Stat_FIFOMoreDataAvail)) // Was valid data available?
      {
        
        if ((data.header & DMP_header_bitmap_Quat9) > 0) // We have asked for orientation data so we should receive Quat9
        {
          // Scale to +/- 1
          double q1 = ((double)data.Quat9.Data.Q1) / 1073741824.0; // Convert to double. Divide by 2^30
          double q2 = ((double)data.Quat9.Data.Q2) / 1073741824.0; // Convert to double. Divide by 2^30
          double q3 = ((double)data.Quat9.Data.Q3) / 1073741824.0; // Convert to double. Divide by 2^30
          
          double q0 = sqrt(1.0 - ((q1 * q1) + (q2 * q2) + (q3 * q3)));
          
          double qw = q0; // See issue #145 - thank you @Gord1
          double qx = q2;
          double qy = q1;
          double qz = -q3;
          
          // roll (x-axis rotation)
          double t0 = +2.0 * (qw * qx + qy * qz);
          double t1 = +1.0 - 2.0 * (qx * qx + qy * qy);
          double roll = atan2(t0, t1) * 180.0 / PI;
          
          // pitch (y-axis rotation)
          double t2 = +2.0 * (qw * qy - qx * qz);
          t2 = t2 > 1.0 ? 1.0 : t2;
          t2 = t2 < -1.0 ? -1.0 : t2;
          double pitch = asin(t2) * 180.0 / PI;
          
          // yaw (z-axis rotation)
          double t3 = +2.0 * (qw * qz + qx * qy);
          double t4 = +1.0 - 2.0 * (qy * qy + qz * qz);
          double yaw = atan2(t3, t4) * 180.0 / PI;
          
          SERIAL_PORT.print(F("Roll: "));
          SERIAL_PORT.print(roll, 1);
          SERIAL_PORT.print(F("\tPitch: "));
          SERIAL_PORT.print(pitch, 1);
          SERIAL_PORT.print(F("\tYaw: "));
          SERIAL_PORT.println(yaw, 1);
        }
      }
    }
    if (myICM.status != ICM_20948_Stat_FIFOMoreDataAvail)
    {
      delay(10);
    }
  }
}


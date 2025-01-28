#ifndef Sensors_h
#define Sensors_h
#include "Arduino.h" 
#include <Wire.h>
#include <AS5600.h> 

#define TCAADDR 0x70

class Sensors {
public:
Sensors();
  void init(int);
  double getAngle(int);
private:
  AS5600 ams5600;
  //double amsOffsets[6] = {154,254,287,215,219,322};
  //double amsOffsets[6] = {125.46, 100.52, 74.30, 146.04, 117.77, 37.57};
  //double amsOffsets[6] = {9.40, 134.33, 298.06, 297.80, 238.03, 157.30};
  double amsOffsets[6] = {307.28, 131.11, 300.93, 308.59, 200.71, 153.29};
  void tcaselect(uint8_t);
};
#endif
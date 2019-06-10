// SRF02_address
// use to change address

#include "Wire.h"
#include "SRF02.h"

void setup() {
  Wire.begin();
  SRF02::configureDeviceId(0x70, 0x79);
}

void loop() {
}

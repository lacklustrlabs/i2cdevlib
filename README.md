## This is an experimental templated version of Arduino i2cdevlib
This implementation has these advantages:
* allows i2c register addresses larger that 8bits
* makes it easy to use alternative i2c backbone implementations simultaneously
* can use more than one i2c bus at the same time (eg. stm32duino)
* Added am i2cdev driver for AT24C32 (12bit register address)

Example:
```c++
#include "I2Cdev.h"
#include "AT24C32.h"
#include "DS1307.h"

// An I2Cdev template specialization with 16 bit register addresses
I2CdevT<TwoWire,uint16_t> i2cdev16(Wire);

// A I2Cdev template specialization with  8 bit register addresses
// I2Cdev is now shorthand for I2CdevT<TwoWire,uint8_t>
I2Cdev i2cdev8(Wire);

AT24C32<TwoWire> at24c32(i2cdev16); // AT24C32 uses 12 bit register addresses
DS1307<TwoWire> rtc(i2cdev8);       // DS1307 uses the normal 8 bit register addresses

setup() {
....
```


## Note:
Only a handful of the drivers have (as of yet) been converter to the template API.

* ADS1115
* DS1307
* AT24C32 (brand new)
* HTU21D

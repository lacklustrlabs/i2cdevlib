// I2Cdev library collection - HTU21D I2C device class header file
// Based on MEAS HTU21D HPC199_2 HTU321(F) datasheet, October 2013
// 2016-03-24 by https://github.com/eadf
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2016-03-24 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2016 Eadf, Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#ifndef _HTU21D_H_
#define _HTU21D_H_

#include "I2Cdev.h"

#define HTU21D_DEFAULT_ADDRESS     0x40

#define HTU21D_RA_TEMPERATURE      0xE3
#define HTU21D_RA_HUMIDITY         0xE5
#define HTU21D_RESET               0xFE
#define HTU21D_WRITE_USER_REGISTER 0xE6
#define HTU21D_READ_USER_REGISTER  0xE7

template <typename WIRE>
class HTU21D {
    public:

        HTU21D(I2CdevT<WIRE, uint8_t>& i2cdev):_i2cdev(i2cdev),
                                               devAddr(HTU21D_DEFAULT_ADDRESS) {

        }

        HTU21D() = delete;
        HTU21D(const HTU21D<WIRE>& other) = delete; // non construction-copyable
        HTU21D& operator=(const HTU21D<WIRE>&) = delete; // non copyable

        /** Power on and prepare for general usage.
         * This operation calls reset() on the HTU21D device and it takes at least 15milliseconds.
         */
        void initialize() {
          reset();
        }

        /** Verify the I2C connection.
         * Make sure the device is connected and responds as expected.
         * This operation calls reset() on the HTU21D device and it takes at least 15milliseconds.
         * @return True if connection is valid, false otherwise
         */
        bool testConnection() {
            reset();
            buffer[0] = 0;
            _i2cdev.readByte(devAddr, HTU21D_READ_USER_REGISTER, buffer);
            return buffer[0] == 0x2;
        }

        float getTemperature();
        float getHumidity();

        void reset();

    private:
        I2CdevT<WIRE, uint8_t>& _i2cdev;
        uint8_t devAddr;
        uint8_t buffer[2];
};

/** Reads and returns the temperature, ignores the CRC field.
 * @return The measured temperature, or NaN if the operation failed.
 */
template <typename WIRE>
float HTU21D<WIRE>::getTemperature() {
    // Ignore the CRC byte
    uint16_t t = 0;
    if (1!=_i2cdev.readWord(devAddr, HTU21D_RA_TEMPERATURE, &t)){
        return NAN;
    }
    // clear the status bits (bit0 & bit1) and calculate the temperature
    // as per the formula in the datasheet
    return ((float)(t&0xFFFC))*175.72/65536.0-46.85;
}

/** Reads and returns the humidity, ignores the CRC field
 * @return The measured humidity, or NaN if the operation failed.
 */
template <typename WIRE>
float HTU21D<WIRE>::getHumidity() {
    // Ignore the CRC byte
    uint16_t t = 0;
    if (1!=_i2cdev.readWord(devAddr, HTU21D_RA_HUMIDITY, &t)){
        return NAN;
    }
    // clear the status bits (bit0 & bit1) and calculate the humidity
    // as per the formula in the datasheet
    return ((float)(t&0xFFFC))*125.0/65536.0-6.0;
}

/** Does a soft reset of the HTU21D
 * This operation takes at least 15milliseconds.
 */
template <typename WIRE>
void HTU21D<WIRE>::reset() {
    _i2cdev.writeByte(devAddr, HTU21D_RESET, 0);
    delay(15);
}

#endif /* _HTU21D_H_ */

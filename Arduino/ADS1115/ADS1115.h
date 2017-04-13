// I2Cdev library collection - ADS1115 I2C device class header file
// Based on Texas Instruments ADS1113/4/5 datasheet, May 2009 (SBAS444B, revised October 2009)
// Note that the ADS1115 uses 16-bit registers, not 8-bit registers.
// 8/2/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2013-05-05 - Add debug information.  Clean up Single Shot implementation
//     2011-10-29 - added getDifferentialx() methods, F. Farzanegan
//     2011-08-02 - initial release


/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

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

#ifndef _ADS1115_H_
#define _ADS1115_H_

#include "I2Cdev.h"

// -----------------------------------------------------------------------------
// Arduino-style "Serial.print" debug constant (uncomment to enable)
// -----------------------------------------------------------------------------
//#define ADS1115_SERIAL_DEBUG

#define ADS1115_ADDRESS_ADDR_GND    ((uint8_t)0x48) // address pin low (GND)
#define ADS1115_ADDRESS_ADDR_VDD    ((uint8_t)0x49) // address pin high (VCC)
#define ADS1115_ADDRESS_ADDR_SDA    ((uint8_t)0x4A) // address pin tied to SDA pin
#define ADS1115_ADDRESS_ADDR_SCL    ((uint8_t)0x4B) // address pin tied to SCL pin
#define ADS1115_DEFAULT_ADDRESS     ADS1115_ADDRESS_ADDR_GND

#define ADS1115_RA_CONVERSION       0x00
#define ADS1115_RA_CONFIG           0x01
#define ADS1115_RA_LO_THRESH        0x02
#define ADS1115_RA_HI_THRESH        0x03

#define ADS1115_CFG_OS_BIT          15
#define ADS1115_CFG_MUX_BIT         14
#define ADS1115_CFG_MUX_LENGTH      3
#define ADS1115_CFG_PGA_BIT         11
#define ADS1115_CFG_PGA_LENGTH      3
#define ADS1115_CFG_MODE_BIT        8
#define ADS1115_CFG_DR_BIT          7
#define ADS1115_CFG_DR_LENGTH       3
#define ADS1115_CFG_COMP_MODE_BIT   4
#define ADS1115_CFG_COMP_POL_BIT    3
#define ADS1115_CFG_COMP_LAT_BIT    2
#define ADS1115_CFG_COMP_QUE_BIT    1
#define ADS1115_CFG_COMP_QUE_LENGTH 2


#define ADS1115_MUX_P0_N1           0x00 // default
#define ADS1115_MUX_P0_N3           0x01
#define ADS1115_MUX_P1_N3           0x02
#define ADS1115_MUX_P2_N3           0x03
#define ADS1115_MUX_P0_NG           0x04
#define ADS1115_MUX_P1_NG           0x05
#define ADS1115_MUX_P2_NG           0x06
#define ADS1115_MUX_P3_NG           0x07

#define ADS1115_PGA_6P144           0x00
#define ADS1115_PGA_4P096           0x01
#define ADS1115_PGA_2P048           0x02 // default
#define ADS1115_PGA_1P024           0x03
#define ADS1115_PGA_0P512           0x04
#define ADS1115_PGA_0P256           0x05
#define ADS1115_PGA_0P256B          0x06
#define ADS1115_PGA_0P256C          0x07

#define ADS1115_MV_6P144            0.187500
#define ADS1115_MV_4P096            0.125000
#define ADS1115_MV_2P048            0.062500 // default
#define ADS1115_MV_1P024            0.031250
#define ADS1115_MV_0P512            0.015625
#define ADS1115_MV_0P256            0.007813
#define ADS1115_MV_0P256B           0.007813 
#define ADS1115_MV_0P256C           0.007813

#define ADS1115_MODE_CONTINUOUS     0x00
#define ADS1115_MODE_SINGLESHOT     0x01 // default

#define ADS1115_RATE_8              0x00
#define ADS1115_RATE_16             0x01
#define ADS1115_RATE_32             0x02
#define ADS1115_RATE_64             0x03
#define ADS1115_RATE_128            0x04 // default
#define ADS1115_RATE_250            0x05
#define ADS1115_RATE_475            0x06
#define ADS1115_RATE_860            0x07

#define ADS1115_COMP_MODE_HYSTERESIS    0x00 // default
#define ADS1115_COMP_MODE_WINDOW        0x01

#define ADS1115_COMP_POL_ACTIVE_LOW     0x00 // default
#define ADS1115_COMP_POL_ACTIVE_HIGH    0x01

#define ADS1115_COMP_LAT_NON_LATCHING   0x00 // default
#define ADS1115_COMP_LAT_LATCHING       0x01

#define ADS1115_COMP_QUE_ASSERT1    0x00
#define ADS1115_COMP_QUE_ASSERT2    0x01
#define ADS1115_COMP_QUE_ASSERT4    0x02
#define ADS1115_COMP_QUE_DISABLE    0x03 // default

// -----------------------------------------------------------------------------
// Arduino-style "Serial.print" debug constant (uncomment to enable)
// -----------------------------------------------------------------------------
//#define ADS1115_SERIAL_DEBUG

template <typename WIRE>
class ADS1115 {

  public:
        ADS1115() = delete;
        ADS1115( const ADS1115& other ) = delete; // non construction-copyable
        ADS1115 & operator=( const ADS1115& ) = delete; // non copyable

        /** Specific address constructor.
         * @param address I2C address
         * @see ADS1115_DEFAULT_ADDRESS
         * @see ADS1115_ADDRESS_ADDR_GND
         * @see ADS1115_ADDRESS_ADDR_VDD
         * @see ADS1115_ADDRESS_ADDR_SDA
         * @see ADS1115_ADDRESS_ADDR_SDL
         */
        ADS1115(I2CdevT<WIRE, uint8_t>& i2cdev, uint8_t address=ADS1115_DEFAULT_ADDRESS) : _i2cdev(i2cdev),
                                                                                     devAddr(address),
                                                                                     devMode(false),
                                                                                     muxMode(0),
                                                                                     pgaMode(0)
                                                                                     {
        }
/*
        ADS1115(I2CdevT<uint8_t>& i2cdev) : ADS1115(i2cdev,ADS1115_DEFAULT_ADDRESS) {
        }
        */

        void initialize();
        bool testConnection();

        // SINGLE SHOT utilities
        bool pollConversion(uint16_t max_retries);
        void triggerConversion();

        // Read the current CONVERSION register
        int16_t getConversion(bool triggerAndPoll=true);

        // Differential
        int16_t getConversionP0N1();
        int16_t getConversionP0N3();
        int16_t getConversionP1N3();
        int16_t getConversionP2N3();

        // Single-ended
        int16_t getConversionP0GND();
        int16_t getConversionP1GND();
        int16_t getConversionP2GND();
        int16_t getConversionP3GND();

        // Utility
        float getMilliVolts(bool triggerAndPoll=true);
        float getMvPerCount();

        // CONFIG register
        bool isConversionReady();
        uint8_t getMultiplexer();
        void setMultiplexer(uint8_t mux);
        uint8_t getGain();
        void setGain(uint8_t gain);
        bool getMode();
        void setMode(bool mode);
        uint8_t getRate();
        void setRate(uint8_t rate);
        bool getComparatorMode();
        void setComparatorMode(bool mode);
        bool getComparatorPolarity();
        void setComparatorPolarity(bool polarity);
        bool getComparatorLatchEnabled();
        void setComparatorLatchEnabled(bool enabled);
        uint8_t getComparatorQueueMode();
        void setComparatorQueueMode(uint8_t mode);
        void setConversionReadyPinMode();

        // *_THRESH registers
        int16_t getLowThreshold();
        void setLowThreshold(int16_t threshold);
        int16_t getHighThreshold();
        void setHighThreshold(int16_t threshold);

        // DEBUG
        void showConfigRegister();
  private:
        I2CdevT<WIRE, uint8_t>& _i2cdev;
        uint8_t devAddr;
        uint16_t buffer[2];
        bool    devMode;
        uint8_t muxMode;
        uint8_t pgaMode;
};

/** Power on and prepare for general usage.
 * This device is ready to use automatically upon power-up. It defaults to
 * single-shot read mode, P0/N1 mux, 2.048v gain, 128 samples/sec, default
 * comparator with hysterysis, active-low polarity, non-latching comparator,
 * and comparater-disabled operation.
 */
template <typename WIRE>
void ADS1115<WIRE>::initialize() {
  setMultiplexer(ADS1115_MUX_P0_N1);
  setGain(ADS1115_PGA_2P048);
  setMode(ADS1115_MODE_SINGLESHOT);
  setRate(ADS1115_RATE_128);
  setComparatorMode(ADS1115_COMP_MODE_HYSTERESIS);
  setComparatorPolarity(ADS1115_COMP_POL_ACTIVE_LOW);
  setComparatorLatchEnabled(ADS1115_COMP_LAT_NON_LATCHING);
  setComparatorQueueMode(ADS1115_COMP_QUE_DISABLE);
}

/** Verify the I2C connection.
 * Make sure the device is connected and responds as expected.
 * @return True if connection is valid, false otherwise
 */
template <typename WIRE>
bool ADS1115<WIRE>::testConnection() {
    return _i2cdev.readWord(devAddr, ADS1115_RA_CONVERSION, buffer) == 1;
}

/** Poll the operational status bit until the conversion is finished
 * Retry at most 'max_retries' times
 * conversion is finished, then return true;
 * @see ADS1115_CFG_OS_BIT
 * @return True if data is available, false otherwise
 */
template <typename WIRE>
bool ADS1115<WIRE>::pollConversion(uint16_t max_retries) {
  for(uint16_t i = 0; i < max_retries; i++) {
    if (isConversionReady()) return true;
  }
  return false;
}

/** Read differential value based on current MUX configuration.
 * The default MUX setting sets the device to get the differential between the
 * AIN0 and AIN1 pins. There are 8 possible MUX settings, but if you are using
 * all four input pins as single-end voltage sensors, then the default option is
 * not what you want; instead you will need to set the MUX to compare the
 * desired AIN* pin with GND. There are shortcut methods (getConversion*) to do
 * this conveniently, but you can also do it manually with setMultiplexer()
 * followed by this method.
 *
 * In single-shot mode, this register may not have fresh data. You need to write
 * a 1 bit to the MSB of the CONFIG register to trigger a single read/conversion
 * before this will be populated with fresh data. This technique is not as
 * effortless, but it has enormous potential to save power by only running the
 * comparison circuitry when needed.
 *
 * @param triggerAndPoll If true (and only in singleshot mode) the conversion trigger
 *        will be executed and the conversion results will be polled.
 * @return 16-bit signed differential value
 * @see getConversionP0N1();
 * @see getConversionPON3();
 * @see getConversionP1N3();
 * @see getConversionP2N3();
 * @see getConversionP0GND();
 * @see getConversionP1GND();
 * @see getConversionP2GND();
 * @see getConversionP3GND);
 * @see setMultiplexer();
 * @see ADS1115_RA_CONVERSION
 * @see ADS1115_MUX_P0_N1
 * @see ADS1115_MUX_P0_N3
 * @see ADS1115_MUX_P1_N3
 * @see ADS1115_MUX_P2_N3
 * @see ADS1115_MUX_P0_NG
 * @see ADS1115_MUX_P1_NG
 * @see ADS1115_MUX_P2_NG
 * @see ADS1115_MUX_P3_NG
 */
template <typename WIRE>
int16_t ADS1115<WIRE>::getConversion(bool triggerAndPoll) {
    if (triggerAndPoll && devMode == ADS1115_MODE_SINGLESHOT) {
      triggerConversion();
      pollConversion(I2CDEV_DEFAULT_READ_TIMEOUT);
    }
    _i2cdev.readWord(devAddr, ADS1115_RA_CONVERSION, buffer);
    return buffer[0];
}

/** Get AIN0/N1 differential.
 * This changes the MUX setting to AIN0/N1 if necessary, triggers a new
 * measurement (also only if necessary), then gets the differential value
 * currently in the CONVERSION register.
 * @return 16-bit signed differential value
 * @see getConversion()
 */
template <typename WIRE>
int16_t ADS1115<WIRE>::getConversionP0N1() {
    if (muxMode != ADS1115_MUX_P0_N1) setMultiplexer(ADS1115_MUX_P0_N1);
    return getConversion();
}

/** Get AIN0/N3 differential.
 * This changes the MUX setting to AIN0/N3 if necessary, triggers a new
 * measurement (also only if necessary), then gets the differential value
 * currently in the CONVERSION register.
 * @return 16-bit signed differential value
 * @see getConversion()
 */
template <typename WIRE>
int16_t ADS1115<WIRE>::getConversionP0N3() {
    if (muxMode != ADS1115_MUX_P0_N3) setMultiplexer(ADS1115_MUX_P0_N3);
    return getConversion();
}

/** Get AIN1/N3 differential.
 * This changes the MUX setting to AIN1/N3 if necessary, triggers a new
 * measurement (also only if necessary), then gets the differential value
 * currently in the CONVERSION register.
 * @return 16-bit signed differential value
 * @see getConversion()
 */
template <typename WIRE>
int16_t ADS1115<WIRE>::getConversionP1N3() {
    if (muxMode != ADS1115_MUX_P1_N3) setMultiplexer(ADS1115_MUX_P1_N3);
    return getConversion();
}

/** Get AIN2/N3 differential.
 * This changes the MUX setting to AIN2/N3 if necessary, triggers a new
 * measurement (also only if necessary), then gets the differential value
 * currently in the CONVERSION register.
 * @return 16-bit signed differential value
 * @see getConversion()
 */
template <typename WIRE>
int16_t ADS1115<WIRE>::getConversionP2N3() {
    if (muxMode != ADS1115_MUX_P2_N3) setMultiplexer(ADS1115_MUX_P2_N3);
    return getConversion();
}

/** Get AIN0/GND differential.
 * This changes the MUX setting to AIN0/GND if necessary, triggers a new
 * measurement (also only if necessary), then gets the differential value
 * currently in the CONVERSION register.
 * @return 16-bit signed differential value
 * @see getConversion()
 */
template <typename WIRE>
int16_t ADS1115<WIRE>::getConversionP0GND() {
    if (muxMode != ADS1115_MUX_P0_NG) setMultiplexer(ADS1115_MUX_P0_NG);
    return getConversion();
}

/** Get AIN1/GND differential.
 * This changes the MUX setting to AIN1/GND if necessary, triggers a new
 * measurement (also only if necessary), then gets the differential value
 * currently in the CONVERSION register.
 * @return 16-bit signed differential value
 * @see getConversion()
 */
template <typename WIRE>
int16_t ADS1115<WIRE>::getConversionP1GND() {
    if (muxMode != ADS1115_MUX_P1_NG) setMultiplexer(ADS1115_MUX_P1_NG);
    return getConversion();
}

/** Get AIN2/GND differential.
 * This changes the MUX setting to AIN2/GND if necessary, triggers a new
 * measurement (also only if necessary), then gets the differential value
 * currently in the CONVERSION register.
 * @return 16-bit signed differential value
 * @see getConversion()
 */
template <typename WIRE>
int16_t ADS1115<WIRE>::getConversionP2GND() {
    if (muxMode != ADS1115_MUX_P2_NG) setMultiplexer(ADS1115_MUX_P2_NG);
    return getConversion();
}

/** Get AIN3/GND differential.
 * This changes the MUX setting to AIN3/GND if necessary, triggers a new
 * measurement (also only if necessary), then gets the differential value
 * currently in the CONVERSION register.
 * @return 16-bit signed differential value
 * @see getConversion()
 */
template <typename WIRE>
int16_t ADS1115<WIRE>::getConversionP3GND() {
    if (muxMode != ADS1115_MUX_P3_NG) setMultiplexer(ADS1115_MUX_P3_NG);
    return getConversion();
}

/** Get the current voltage reading
 * Read the current differential and return it multiplied
 * by the constant for the current gain.  mV is returned to
 * increase the precision of the voltage
 * @param triggerAndPoll If true (and only in singleshot mode) the conversion trigger
 *        will be executed and the conversion results will be polled.
 */
template <typename WIRE>
float ADS1115<WIRE>::getMilliVolts(bool triggerAndPoll) {
  switch (pgaMode) {
    case ADS1115_PGA_6P144:
      return (getConversion(triggerAndPoll) * ADS1115_MV_6P144);
      break;
    case ADS1115_PGA_4P096:
      return (getConversion(triggerAndPoll) * ADS1115_MV_4P096);
      break;
    case ADS1115_PGA_2P048:
      return (getConversion(triggerAndPoll) * ADS1115_MV_2P048);
      break;
    case ADS1115_PGA_1P024:
      return (getConversion(triggerAndPoll) * ADS1115_MV_1P024);
      break;
    case ADS1115_PGA_0P512:
      return (getConversion(triggerAndPoll) * ADS1115_MV_0P512);
      break;
    case ADS1115_PGA_0P256:
    case ADS1115_PGA_0P256B:
    case ADS1115_PGA_0P256C:
      return (getConversion(triggerAndPoll) * ADS1115_MV_0P256);
      break;
    default:
      return 0; // should not really happen
  }
}

/**
 * Return the current multiplier for the PGA setting.
 *
 * This may be directly retreived by using getMilliVolts(),
 * but this causes an independent read.  This function could
 * be used to average a number of reads from the getConversion()
 * getConversionx() functions and cut downon the number of
 * floating-point calculations needed.
 *
 */
template <typename WIRE>
float ADS1115<WIRE>::getMvPerCount() {
  switch (pgaMode) {
    case ADS1115_PGA_6P144:
      return ADS1115_MV_6P144;
      break;
    case ADS1115_PGA_4P096:
      return  ADS1115_MV_4P096;
      break;
    case ADS1115_PGA_2P048:
      return ADS1115_MV_2P048;
      break;
    case ADS1115_PGA_1P024:
      return ADS1115_MV_1P024;
      break;
    case ADS1115_PGA_0P512:
      return ADS1115_MV_0P512;
      break;
    case ADS1115_PGA_0P256:
    case ADS1115_PGA_0P256B:
    case ADS1115_PGA_0P256C:
      return ADS1115_MV_0P256;
      break;
    default:
      return 0; // should not really happen
  }
}

// CONFIG register

/** Get operational status.
 * @return Current operational status (false for active conversion, true for inactive)
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_OS_BIT
 */
template <typename WIRE>
bool ADS1115<WIRE>::isConversionReady() {
    _i2cdev.readBitW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_OS_BIT, buffer);
    return buffer[0];
}

/** Trigger a new conversion.
 * Writing to this bit will only have effect while in power-down mode (no conversions active).
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_OS_BIT
 */
template <typename WIRE>
void ADS1115<WIRE>::triggerConversion() {
    _i2cdev.writeBitW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_OS_BIT, 1);
}

/** Get multiplexer connection.
 * @return Current multiplexer connection setting
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_MUX_BIT
 * @see ADS1115_CFG_MUX_LENGTH
 */
template <typename WIRE>
uint8_t ADS1115<WIRE>::getMultiplexer() {
    _i2cdev.readBitsW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_MUX_BIT, ADS1115_CFG_MUX_LENGTH, buffer);
    muxMode = (uint8_t)buffer[0];
    return muxMode;
}

/** Set multiplexer connection.  Continous mode may fill the conversion register
 * with data before the MUX setting has taken effect.  A stop/start of the conversion
 * is done to reset the values.
 * @param mux New multiplexer connection setting
 * @see ADS1115_MUX_P0_N1
 * @see ADS1115_MUX_P0_N3
 * @see ADS1115_MUX_P1_N3
 * @see ADS1115_MUX_P2_N3
 * @see ADS1115_MUX_P0_NG
 * @see ADS1115_MUX_P1_NG
 * @see ADS1115_MUX_P2_NG
 * @see ADS1115_MUX_P3_NG
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_MUX_BIT
 * @see ADS1115_CFG_MUX_LENGTH
 */
template <typename WIRE>
void ADS1115<WIRE>::setMultiplexer(uint8_t mux) {
    if (_i2cdev.writeBitsW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_MUX_BIT, ADS1115_CFG_MUX_LENGTH, mux)) {
        muxMode = mux;
        if (devMode == ADS1115_MODE_CONTINUOUS) {
          // Force a stop/start
          setMode(ADS1115_MODE_SINGLESHOT);
          getConversion();
          setMode(ADS1115_MODE_CONTINUOUS);
        }
    }
}

/** Get programmable gain amplifier level.
 * @return Current programmable gain amplifier level
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_PGA_BIT
 * @see ADS1115_CFG_PGA_LENGTH
 */
template <typename WIRE>
uint8_t ADS1115<WIRE>::getGain() {
    _i2cdev.readBitsW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_PGA_BIT, ADS1115_CFG_PGA_LENGTH, buffer);
    pgaMode=(uint8_t)buffer[0];
    return pgaMode;
}

/** Set programmable gain amplifier level.
 * Continous mode may fill the conversion register
 * with data before the gain setting has taken effect.  A stop/start of the conversion
 * is done to reset the values.
 * @param gain New programmable gain amplifier level
 * @see ADS1115_PGA_6P144
 * @see ADS1115_PGA_4P096
 * @see ADS1115_PGA_2P048
 * @see ADS1115_PGA_1P024
 * @see ADS1115_PGA_0P512
 * @see ADS1115_PGA_0P256
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_PGA_BIT
 * @see ADS1115_CFG_PGA_LENGTH
 */
template <typename WIRE>
void ADS1115<WIRE>::setGain(uint8_t gain) {
    if (_i2cdev.writeBitsW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_PGA_BIT, ADS1115_CFG_PGA_LENGTH, gain)) {
      pgaMode = gain;
         if (devMode == ADS1115_MODE_CONTINUOUS) {
            // Force a stop/start
            setMode(ADS1115_MODE_SINGLESHOT);
            getConversion();
            setMode(ADS1115_MODE_CONTINUOUS);
         }
    }
}

/** Get device mode.
 * @return Current device mode
 * @see ADS1115_MODE_CONTINUOUS
 * @see ADS1115_MODE_SINGLESHOT
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_MODE_BIT
 */
template <typename WIRE>
bool ADS1115<WIRE>::getMode() {
    _i2cdev.readBitW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_MODE_BIT, buffer);
    devMode = buffer[0];
    return devMode;
}

/** Set device mode.
 * @param mode New device mode
 * @see ADS1115_MODE_CONTINUOUS
 * @see ADS1115_MODE_SINGLESHOT
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_MODE_BIT
 */
template <typename WIRE>
void ADS1115<WIRE>::setMode(bool mode) {
    if (_i2cdev.writeBitW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_MODE_BIT, mode)) {
        devMode = mode;
    }
}

/** Get data rate.
 * @return Current data rate
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_DR_BIT
 * @see ADS1115_CFG_DR_LENGTH
 */
template <typename WIRE>
uint8_t ADS1115<WIRE>::getRate() {
    _i2cdev.readBitsW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_DR_BIT, ADS1115_CFG_DR_LENGTH, buffer);
    return (uint8_t)buffer[0];
}

/** Set data rate.
 * @param rate New data rate
 * @see ADS1115_RATE_8
 * @see ADS1115_RATE_16
 * @see ADS1115_RATE_32
 * @see ADS1115_RATE_64
 * @see ADS1115_RATE_128
 * @see ADS1115_RATE_250
 * @see ADS1115_RATE_475
 * @see ADS1115_RATE_860
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_DR_BIT
 * @see ADS1115_CFG_DR_LENGTH
 */
template <typename WIRE>
void ADS1115<WIRE>::setRate(uint8_t rate) {
    _i2cdev.writeBitsW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_DR_BIT, ADS1115_CFG_DR_LENGTH, rate);
}

/** Get comparator mode.
 * @return Current comparator mode
 * @see ADS1115_COMP_MODE_HYSTERESIS
 * @see ADS1115_COMP_MODE_WINDOW
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_MODE_BIT
 */
template <typename WIRE>
bool ADS1115<WIRE>::getComparatorMode() {
    _i2cdev.readBitW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_COMP_MODE_BIT, buffer);
    return buffer[0];
}

/** Set comparator mode.
 * @param mode New comparator mode
 * @see ADS1115_COMP_MODE_HYSTERESIS
 * @see ADS1115_COMP_MODE_WINDOW
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_MODE_BIT
 */
template <typename WIRE>
void ADS1115<WIRE>::setComparatorMode(bool mode) {
    _i2cdev.writeBitW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_COMP_MODE_BIT, mode);
}

/** Get comparator polarity setting.
 * @return Current comparator polarity setting
 * @see ADS1115_COMP_POL_ACTIVE_LOW
 * @see ADS1115_COMP_POL_ACTIVE_HIGH
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_POL_BIT
 */
template <typename WIRE>
bool ADS1115<WIRE>::getComparatorPolarity() {
    _i2cdev.readBitW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_COMP_POL_BIT, buffer);
    return buffer[0];
}

/** Set comparator polarity setting.
 * @param polarity New comparator polarity setting
 * @see ADS1115_COMP_POL_ACTIVE_LOW
 * @see ADS1115_COMP_POL_ACTIVE_HIGH
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_POL_BIT
 */
template <typename WIRE>
void ADS1115<WIRE>::setComparatorPolarity(bool polarity) {
    _i2cdev.writeBitW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_COMP_POL_BIT, polarity);
}

/** Get comparator latch enabled value.
 * @return Current comparator latch enabled value
 * @see ADS1115_COMP_LAT_NON_LATCHING
 * @see ADS1115_COMP_LAT_LATCHING
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_LAT_BIT
 */
template <typename WIRE>
bool ADS1115<WIRE>::getComparatorLatchEnabled() {
    _i2cdev.readBitW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_COMP_LAT_BIT, buffer);
    return buffer[0];
}

/** Set comparator latch enabled value.
 * @param enabled New comparator latch enabled value
 * @see ADS1115_COMP_LAT_NON_LATCHING
 * @see ADS1115_COMP_LAT_LATCHING
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_LAT_BIT
 */
template <typename WIRE>
void ADS1115<WIRE>::setComparatorLatchEnabled(bool enabled) {
    _i2cdev.writeBitW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_COMP_LAT_BIT, enabled);
}

/** Get comparator queue mode.
 * @return Current comparator queue mode
 * @see ADS1115_COMP_QUE_ASSERT1
 * @see ADS1115_COMP_QUE_ASSERT2
 * @see ADS1115_COMP_QUE_ASSERT4
 * @see ADS1115_COMP_QUE_DISABLE
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_QUE_BIT
 * @see ADS1115_CFG_COMP_QUE_LENGTH
 */
template <typename WIRE>
uint8_t ADS1115<WIRE>::getComparatorQueueMode() {
    _i2cdev.readBitsW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_COMP_QUE_BIT, ADS1115_CFG_COMP_QUE_LENGTH, buffer);
    return (uint8_t)buffer[0];
}

/** Set comparator queue mode.
 * @param mode New comparator queue mode
 * @see ADS1115_COMP_QUE_ASSERT1
 * @see ADS1115_COMP_QUE_ASSERT2
 * @see ADS1115_COMP_QUE_ASSERT4
 * @see ADS1115_COMP_QUE_DISABLE
 * @see ADS1115_RA_CONFIG
 * @see ADS1115_CFG_COMP_QUE_BIT
 * @see ADS1115_CFG_COMP_QUE_LENGTH
 */
template <typename WIRE>
void ADS1115<WIRE>::setComparatorQueueMode(uint8_t mode) {
    _i2cdev.writeBitsW(devAddr, ADS1115_RA_CONFIG, ADS1115_CFG_COMP_QUE_BIT, ADS1115_CFG_COMP_QUE_LENGTH, mode);
}

// *_THRESH registers

/** Get low threshold value.
 * @return Current low threshold value
 * @see ADS1115_RA_LO_THRESH
 */
template <typename WIRE>
int16_t ADS1115<WIRE>::getLowThreshold() {
    _i2cdev.readWord(devAddr, ADS1115_RA_LO_THRESH, buffer);
    return buffer[0];
}

/** Set low threshold value.
 * @param threshold New low threshold value
 * @see ADS1115_RA_LO_THRESH
 */
template <typename WIRE>
void ADS1115<WIRE>::setLowThreshold(int16_t threshold) {
    _i2cdev.writeWord(devAddr, ADS1115_RA_LO_THRESH, threshold);
}

/** Get high threshold value.
 * @return Current high threshold value
 * @see ADS1115_RA_HI_THRESH
 */
template <typename WIRE>
int16_t ADS1115<WIRE>::getHighThreshold() {
    _i2cdev.readWord(devAddr, ADS1115_RA_HI_THRESH, buffer);
    return buffer[0];
}

/** Set high threshold value.
 * @param threshold New high threshold value
 * @see ADS1115_RA_HI_THRESH
 */
template <typename WIRE>
void ADS1115<WIRE>::setHighThreshold(int16_t threshold) {
    _i2cdev.writeWord(devAddr, ADS1115_RA_HI_THRESH, threshold);
}

/** Configures ALERT/RDY pin as a conversion ready pin.
 *  It does this by setting the MSB of the high threshold register to '1' and the MSB
 *  of the low threshold register to '0'. COMP_POL and COMP_QUE bits will be set to '0'.
 *  Note: ALERT/RDY pin requires a pull up resistor.
 */
template <typename WIRE>
void ADS1115<WIRE>::setConversionReadyPinMode() {
    _i2cdev.writeBitW(devAddr, ADS1115_RA_HI_THRESH, 15, 1);
    _i2cdev.writeBitW(devAddr, ADS1115_RA_LO_THRESH, 15, 0);
    setComparatorPolarity(0);
    setComparatorQueueMode(0);
}

// Create a mask between two bits
unsigned createMask(unsigned a, unsigned b) {
   unsigned mask = 0;
   for (unsigned i=a; i<=b; i++)
       mask |= 1 << i;
   return mask;
}

uint16_t shiftDown(uint16_t extractFrom, int places) {
  return (extractFrom >> places);
}


uint16_t getValueFromBits(uint16_t extractFrom, int high, int length) {
   int low= high-length +1;
   uint16_t mask = createMask(low ,high);
   return shiftDown(extractFrom & mask, low);
}

/** Show all the config register settings
 */
template <typename WIRE>
void ADS1115<WIRE>::showConfigRegister() {
    _i2cdev.readWord(devAddr, ADS1115_RA_CONFIG, buffer);

    #ifdef ADS1115_SERIAL_DEBUG
      uint16_t configRegister =buffer[0];

      I2CDEV_DEBUG_OUTPUT.print("Register is:");
      I2CDEV_DEBUG_OUTPUT.println(configRegister,BIN);

      I2CDEV_DEBUG_OUTPUT.print("OS:\t");
      I2CDEV_DEBUG_OUTPUT.println(getValueFromBits(configRegister,
        ADS1115_CFG_OS_BIT,1), BIN);
      I2CDEV_DEBUG_OUTPUT.print("MUX:\t");
      I2CDEV_DEBUG_OUTPUT.println(getValueFromBits(configRegister,
        ADS1115_CFG_MUX_BIT,ADS1115_CFG_MUX_LENGTH), BIN);

      I2CDEV_DEBUG_OUTPUT.print("PGA:\t");
      I2CDEV_DEBUG_OUTPUT.println(getValueFromBits(configRegister,
        ADS1115_CFG_PGA_BIT,ADS1115_CFG_PGA_LENGTH), BIN);

      I2CDEV_DEBUG_OUTPUT.print("MODE:\t");
      I2CDEV_DEBUG_OUTPUT.println(getValueFromBits(configRegister,
        ADS1115_CFG_MODE_BIT,1), BIN);

      I2CDEV_DEBUG_OUTPUT.print("DR:\t");
      I2CDEV_DEBUG_OUTPUT.println(getValueFromBits(configRegister,
        ADS1115_CFG_DR_BIT,ADS1115_CFG_DR_LENGTH), BIN);

      I2CDEV_DEBUG_OUTPUT.print("CMP_MODE:\t");
      I2CDEV_DEBUG_OUTPUT.println(getValueFromBits(configRegister,
        ADS1115_CFG_COMP_MODE_BIT,1), BIN);

      I2CDEV_DEBUG_OUTPUT.print("CMP_POL:\t");
      I2CDEV_DEBUG_OUTPUT.println(getValueFromBits(configRegister,
        ADS1115_CFG_COMP_POL_BIT,1), BIN);

      I2CDEV_DEBUG_OUTPUT.print("CMP_LAT:\t");
      I2CDEV_DEBUG_OUTPUT.println(getValueFromBits(configRegister,
        ADS1115_CFG_COMP_LAT_BIT,1), BIN);

      I2CDEV_DEBUG_OUTPUT.print("CMP_QUE:\t");
      I2CDEV_DEBUG_OUTPUT.println(getValueFromBits(configRegister,
        ADS1115_CFG_COMP_QUE_BIT,ADS1115_CFG_COMP_QUE_LENGTH), BIN);
    #endif // ADS1115_SERIAL_DEBUG
};
#endif /* _ADS1115_H_ */

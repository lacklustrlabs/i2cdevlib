// I2Cdev library collection - DS1307 I2C device class
// Based on Maxim DS1307 datasheet, 2008
// 11/13/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
// I2C Device Library hosted at http://www.i2cdevlib.com
//
// Changelog:
//     2011-11-13 - initial release
//     2012-03-29 - alain.spineux@gmail.com: bug in getHours24() 
//                  am/pm is bit 0x20 instead of 0x80
//

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

#include "DS1307.h"

#ifdef DS1307_INCLUDE_DATETIME_CLASS
    // DateTime class courtesy of public domain JeeLabs code
    #include <avr/pgmspace.h>
    #define SECONDS_PER_DAY 86400L
    #define SECONDS_FROM_1970_TO_2000 946684800

    ////////////////////////////////////////////////////////////////////////////////
    // utility code, some of this could be exposed in the DateTime API if needed

    static const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    // number of days since 2000/01/01, valid for 2001..2099
    static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
        if (y >= 2000)
            y -= 2000;
        uint16_t days = d;
        for (uint8_t i = 1; i < m; ++i)
            days += pgm_read_byte(daysInMonth + i - 1);
        if (m > 2 && y % 4 == 0)
            ++days;
        return days + 365 * y + (y + 3) / 4 - 1;
    }

    static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
        return ((days * 24L + h) * 60 + m) * 60 + s;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // DateTime implementation - ignores time zones and DST changes
    // NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second

    DateTime::DateTime (uint32_t t) {
      t -= SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970

        ss = t % 60;
        t /= 60;
        mm = t % 60;
        t /= 60;
        hh = t % 24;
        uint16_t days = t / 24;
        uint8_t leap;
        for (yOff = 0; ; ++yOff) {
            leap = yOff % 4 == 0;
            if (days < 365 + leap)
                break;
            days -= 365 + leap;
        }
        for (m = 1; ; ++m) {
            uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
            if (leap && m == 2)
                ++daysPerMonth;
            if (days < daysPerMonth)
                break;
            days -= daysPerMonth;
        }
        d = days + 1;
    }

    DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
        if (year >= 2000)
            year -= 2000;
        yOff = year;
        m = month;
        d = day;
        hh = hour;
        mm = min;
        ss = sec;
    }

    static uint8_t conv2d(const char* p) {
        uint8_t v = 0;
        if ('0' <= *p && *p <= '9')
            v = *p - '0';
        return 10 * v + *++p - '0';
    }

    // A convenient constructor for using "the compiler's time":
    //   DateTime now (__DATE__, __TIME__);
    // NOTE: using PSTR would further reduce the RAM footprint
    DateTime::DateTime (const char* date, const char* time) {
        // sample input: date = "Dec 26 2009", time = "12:34:56"
        yOff = conv2d(date + 9);
        // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
        switch (date[0]) {
            case 'J': m = (date[1] == 'a' ? 1 : (date[2] == 'n' ? 6 : 7)); break;
            case 'F': m = 2; break;
            case 'A': m = date[2] == 'r' ? 4 : 8; break;
            case 'M': m = date[2] == 'r' ? 3 : 5; break;
            case 'S': m = 9; break;
            case 'O': m = 10; break;
            case 'N': m = 11; break;
            case 'D': m = 12; break;
        }
        d = conv2d(date + 4);
        hh = conv2d(time);
        mm = conv2d(time + 3);
        ss = conv2d(time + 6);
    }

    uint8_t DateTime::dayOfWeek() const {
        uint16_t day = secondstime() / SECONDS_PER_DAY;
        return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
    }

    uint32_t DateTime::unixtime(void) const {
        return secondstime() + SECONDS_FROM_1970_TO_2000;
    }

    long DateTime::secondstime(void) const {
        uint32_t t;
        uint16_t days = date2days(yOff, m, d);
        t = time2long(days, hh, mm, ss);
        return t;
    }
#endif

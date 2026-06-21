#pragma once

#include "I2C_eeprom.h"

static constexpr long STANDARD_EEPROM_SIZE_65536 = 65536;
static constexpr long STANDARD_EEPROM_SIZE_32768 = 32768;
static constexpr long STANDARD_EEPROM_SIZE_16384 = 16384;
static constexpr long STANDARD_EEPROM_SIZE_8192 = 8192;
static constexpr long STANDARD_EEPROM_SIZE_4096 = 4096;
static constexpr long STANDARD_EEPROM_SIZE_2048 = 2048;
static constexpr long STANDARD_EEPROM_SIZE_1024 = 1024;
static constexpr long STANDARD_EEPROM_SIZE_512 = 512;
static constexpr long STANDARD_EEPROM_SIZE_256 = 256;
static constexpr long STANDARD_EEPROM_SIZE_128 = 128;

class EepromI2cDriver {
    I2C_eeprom _eeprom;

    long _eeprom_size;

public:
    EepromI2cDriver(const int address, const long eeprom_size) : _eeprom(address, eeprom_size),
                                                                 _eeprom_size(eeprom_size) {
    }

    bool setup() {
        Wire.begin();

        return _eeprom.isConnected();
    }

    int read(const int start, uint8_t *buffer, const int length) {
        return _eeprom.readBlock(start, buffer, length);
    }

    int write(const int start, const uint8_t *buffer, const int length) {
        return _eeprom.writeBlock(start, buffer, length);
    }

    long getSize() const {
        return _eeprom_size;
    }
};

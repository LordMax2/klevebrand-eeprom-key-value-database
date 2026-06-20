//
// Created by maxkl on 2026-06-19.
//

#ifndef KLEVEBRAND_EEPROM_KEY_VALUE_STORE_EEPROM_I2C_DRIVER_H
#define KLEVEBRAND_EEPROM_KEY_VALUE_STORE_EEPROM_I2C_DRIVER_H
#include "I2C_eeprom.h"

class EepromI2cDriver {
    I2C_eeprom _eeprom;

    long _eeprom_size;

public:
    EepromI2cDriver(const int address, const long eeprom_size) : _eeprom(address, eeprom_size), _eeprom_size(eeprom_size) {
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

    long getSize() const
    {
        return _eeprom_size;
    }
};

#endif //KLEVEBRAND_EEPROM_KEY_VALUE_STORE_EEPROM_I2C_DRIVER_H

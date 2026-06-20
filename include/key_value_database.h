#ifndef KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H
#define KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H

#include <Arduino.h>

template <class EepromDriver, int SlotSize, int KeyLength>
class KeyValueDatabase
{
    EepromDriver _driver;

    long _eeprom_size;

    int hashCode(const char* str) const;

    int getAlignedStartPosition(const char* key) const;

public:
    explicit KeyValueDatabase(EepromDriver driver) : _driver(driver), _eeprom_size(driver.getSize())
    {
    }

    bool insert(char key[KeyLength], const long* data);

    bool insert(char key[KeyLength], long data);

    template <int size>
    bool insert(char key[KeyLength], const byte* data);

    template <typename T>
    T get(char key[KeyLength]);
};

#include "key_value_database.ipp"

#endif //KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H

#pragma once

template<class EepromDriver, long SlotSize, long KeyLength>
class KeyValueDatabase {
    EepromDriver _driver;

    long _eeprom_size;

    long hash1(const char *str) const;

    long hash2(const char *str) const;

    static_assert((SlotSize > 0 && SlotSize & (SlotSize - 1)) == 0, "SlotSize must be a power of 2");

    long findPosition(const char *key);

public:
    explicit KeyValueDatabase(EepromDriver driver) : _driver(driver), _eeprom_size(driver.getSize()) {
    }

    bool insert(const char *key, const long *data);

    bool insert(const char *key, long data);

    template<long DataSize>
    bool insert(const char *key, const byte *data);

    template<typename T>
    T get(const char *key);
};

#include "key_value_database.ipp"

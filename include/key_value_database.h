#ifndef KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H
#define KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H

template<class EepromDriver, long SlotSize, long KeyLength>
class KeyValueDatabase {
    EepromDriver _driver;

    long _eeprom_size;

protected:
    long hashCode(const char *str) const;

    long getStartPosition(const char *key);

public:
    explicit KeyValueDatabase(EepromDriver driver) : _driver(driver), _eeprom_size(driver.getSize()) {
    }

    long hash1(const char *str) const;

    long hash2(const char *str) const;

    bool insert(const char *key, const long *data);

    bool insert(const char *key, long data);

    template<long size>
    bool insert(const char *key, const byte *data);

    template<typename T>
    T get(const char *key);
};

#include "key_value_database.ipp"

#endif //KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H

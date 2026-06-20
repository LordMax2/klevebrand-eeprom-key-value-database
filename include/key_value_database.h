#ifndef KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H
#define KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H

struct CuckooPositionResult {
    bool is_occupied;
    long existing_bucket_new_start_position;
    long start_position;

    bool cycleDetected() const {
        return is_occupied && existing_bucket_new_start_position == -1;
    }
};

template<class EepromDriver, long SlotSize, long KeyLength>
class KeyValueDatabase {
    EepromDriver _driver;

    long _eeprom_size;

protected:
    long hashCode(const char *str) const;

    CuckooPositionResult getInsertPosition(const char *key);

    long findPosition(const char *key);

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

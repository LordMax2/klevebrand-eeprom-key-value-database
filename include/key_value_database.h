//
// Created by maxkl on 2026-06-19.
//

#ifndef KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H
#define KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <string.h>

#include "bucket.h"

template<class EepromDriver>
class KeyValueDatabase {
    EepromDriver _driver;

    long _eeprom_size;

    static constexpr int SLOT_SIZE = 128;
    static constexpr int KEY_LENGTH = 16;

    int hashCode(const char *str) {
        int hash = 0;
        for (int i = 0; str[i] != '\0'; i++) {
            hash = (hash * 31 + str[i]) % _eeprom_size;
        }
        return hash;
    }

    int getAlignedStartPosition(const char *key) {
        int hash = hashCode(key);

        hash = hash % _eeprom_size;

        const int start = hash - hash % SLOT_SIZE;

        return start;
    }

public:
    explicit KeyValueDatabase(EepromDriver driver, const long eeprom_size) : _driver(driver),
                                                                             _eeprom_size(eeprom_size) {
    }

    template<int size>
    bool insert(char key[KEY_LENGTH], const byte *data) {
        const int start = getAlignedStartPosition(key);

        Serial.println(start);

        Bucket<KEY_LENGTH, size, SLOT_SIZE> bucket;

        _driver.read(start, (byte *) &bucket, SLOT_SIZE);

        bool is_occupied = false;

        for (int i = 0; i < 16; i++) {
            if (bucket.key[i] == '\0') {
                is_occupied = true;
            }
        }

        if (is_occupied && false) {
            // Rehash and find new position
            Serial.println("Is occupied");

            return true;
        }

        Bucket<16, size, SLOT_SIZE> bucket_to_insert(key, data);

        _driver.write(start, (byte *) &bucket_to_insert, SLOT_SIZE);

        return true;
    }

    template<typename T>
    T get(char key[KEY_LENGTH]) {
        const int start = getAlignedStartPosition(key);

        Bucket<KEY_LENGTH, sizeof(T), SLOT_SIZE> bucket;
        _driver.read(start, (byte *) &bucket, SLOT_SIZE);

        for (int i = 0; i < 16; i++) {
            if (bucket.key[i] == '\0') break;

            Serial.print(bucket.key[i]);
        }

        Serial.println();

        for (int i = 0; i < sizeof(T); i++) {
            Serial.print(bucket.data[i]);
        }

        Serial.println();

        T result;

        memcpy(&result, bucket.data, sizeof(T));

        return result;
    }
};

#endif //KLEVEBRAND_EEPROM_KEY_VALUE_STORE_KEY_VALUE_DATABASE_H

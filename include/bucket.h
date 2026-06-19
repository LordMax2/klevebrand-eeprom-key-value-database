//
// Created by maxkl on 2026-06-19.
//

#ifndef KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H
#define KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H

template <int key_length, int data_size, int slot_size>
struct Bucket {
    Bucket() : next(nullptr), previous(nullptr) {}
    Bucket(const char key[key_length], const byte* data) : next(nullptr), previous(nullptr) {
        memcpy(this->key, key, key_length);
        memcpy(this->data, data, data_size);
    }

    char key[key_length] = {};
    Bucket* next;
    Bucket* previous;
    byte data[data_size] = {};
    byte padding[slot_size - sizeof(Bucket*) - sizeof(char[key_length]) - sizeof(byte[data_size])];
};


#endif //KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H
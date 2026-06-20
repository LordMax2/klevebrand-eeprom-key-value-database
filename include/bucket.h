#ifndef KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H
#define KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H

template <int key_length, int data_size, int slot_size>
struct Bucket
{
    Bucket() = default;

    Bucket(const char key[key_length], const byte* data)
    {
        memcpy(this->key, key, key_length);
        memcpy(this->data, data, data_size);
    }

    char key[key_length] = {};
    byte data[data_size] = {};
    byte padding[slot_size - sizeof(char[key_length]) - sizeof(byte[data_size])] = {};
};


#endif //KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H

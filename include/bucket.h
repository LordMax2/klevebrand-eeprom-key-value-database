#ifndef KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H
#define KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H

template <int key_length, int data_size, int slot_size>
struct Bucket {
    Bucket() : next_start_position(0), previous_start_position(0)
    {}
    Bucket(const char key[key_length], const byte* data) : next_start_position(0), previous_start_position(0)
    {
        memcpy(this->key, key, key_length);
        memcpy(this->data, data, data_size);
    }

    char key[key_length] = {};
    size_t next_start_position;
    size_t previous_start_position;
    byte data[data_size] = {};
    byte padding[slot_size - sizeof(next_start_position) - sizeof(previous_start_position) - sizeof(char[key_length]) - sizeof(byte[data_size])] = {};
};


#endif //KLEVEBRAND_EEPROM_KEY_VALUE_STORE_BUCKET_H
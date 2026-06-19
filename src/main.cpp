#include <Arduino.h>

#include "I2C_eeprom.h"

I2C_eeprom eeprom(0x50, I2C_DEVICESIZE_24LC512);

static constexpr int SLOT_SIZE = 128;
static constexpr int KEY_LENGTH = 16;

template <int key_length, int size>
struct Bucket {
    Bucket() : next(nullptr), previous(nullptr) {}
    Bucket(const char key[key_length], const byte* data) : next(nullptr), previous(nullptr) {
        memcpy(this->key, key, key_length);
        memcpy(this->data, data, size);
    }

    char key[key_length] = {};
    Bucket* next;
    Bucket* previous;
    byte data[size] = {};
    byte padding[SLOT_SIZE - sizeof(Bucket*) - sizeof(char[key_length]) - sizeof(byte[size])];
};

int hashCode(const char* str) {
    int hash = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        hash = (hash * 31 + str[i]) % I2C_DEVICESIZE_24LC512;
    }
    return hash;
}

int getAlignedStartPosition(const char* key) {
    int hash = hashCode(key);

    hash = hash % I2C_DEVICESIZE_24LC512;

    const int start = hash - hash % SLOT_SIZE;

    return start;
}

template <int size>
bool insert(char key[KEY_LENGTH], const byte* data) {
    const int start = getAlignedStartPosition(key);

    Serial.println(start);

    Bucket<KEY_LENGTH, size> bucket;

    eeprom.readBlock(start, (byte*) &bucket, SLOT_SIZE);

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

    Bucket<16, size> bucket_to_insert(key, data);

    eeprom.writeBlock(start, (byte*) &bucket_to_insert, SLOT_SIZE);

    return true;
}

template <typename T>
T get(char key[KEY_LENGTH]) {
    const int start = getAlignedStartPosition(key);

    Bucket<KEY_LENGTH, sizeof(T)> bucket;
    eeprom.readBlock(start, (byte*) &bucket, SLOT_SIZE);

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

void setup() {
    Serial.begin(115200);

    Wire.begin();

    if (!eeprom.isConnected()) {
        Serial.println("EEPROM not connected");
        while (true);
    }

    Serial.println("EEPROM connected");

    const long start = 123875103l;

    Serial.print("Inserting data: ");
    Serial.println(start);

    char key[KEY_LENGTH] = "test\0";

    insert<sizeof(long)>(key, (const byte*) &start);

    const auto read = get<long>(key);

    Serial.print("Reading data: ");

    Serial.println(read);

    Serial.println("Done.");
}

void loop() {
    delay(1000);
}
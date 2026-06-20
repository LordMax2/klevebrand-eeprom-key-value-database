#include "bucket.h"
#include "hashtable_values.h"

template<class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::hashCode(const char *str) const {
    long hash = 0;
    for (long i = 0; str[i] != '\0'; i++) {
        hash = (hash * 31 + str[i]) % (_eeprom_size / SlotSize);
    }
    return hash;
}

template<class EepromDriver, long SlotSize, long KeyLength>
CuckooPositionResult KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::getInsertPosition(const char *key) {
    const long existing_position = findPosition(key);

    if (existing_position != -1) {
        return {false, -1, existing_position};
    }

    const long hash = hash1(key);

    const long start_position = hash * SlotSize;

    Bucket<KeyLength, 0, SlotSize> existing_bucket;

    _driver.read(start_position, (byte *) &existing_bucket, SlotSize);

    const bool is_empty = !existing_bucket.hasNullTerminator();
    const bool occupied_by_the_same_key = existing_bucket.keyEquals(key);

    // Is empty, just return the start position, no moving needed.
    if (is_empty || occupied_by_the_same_key) {
        return {false, -1, start_position};
    }

    const long relocation_start_position = hash2(existing_bucket.key) * SlotSize;

    Bucket<KeyLength, 0, SlotSize> existing_bucket_for_relocating_bucket;

    _driver.read(relocation_start_position, (byte *) &existing_bucket_for_relocating_bucket, SlotSize);

    const bool relocation_is_empty = !existing_bucket_for_relocating_bucket.hasNullTerminator();
    const bool relocation_occupied_by_the_same_key = existing_bucket_for_relocating_bucket.keyEquals(existing_bucket.key);

    // Relocation is empty, return the relocation start position
    if (relocation_is_empty || relocation_occupied_by_the_same_key) {
        return {true, relocation_start_position, start_position};
    }

    // If we get here, we unfortunately have a cycle, PANIC
    return {true, -1, start_position};
}

template<class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::findPosition(const char *key) {
    const long start_position = hash1(key) * SlotSize;

    Bucket<KeyLength, 0, SlotSize> bucket;

    _driver.read(start_position, (byte *) &bucket, SlotSize);

    if (!bucket.hasNullTerminator()) {
        return -1;
    }

    if (bucket.keyEquals(key)) {
        return start_position;
    }

    const long alternative_position = hash2(key) * SlotSize;

    _driver.read(alternative_position, (byte *) &bucket, SlotSize);

    if (!bucket.hasNullTerminator()) {
        return -1;
    }

    if (bucket.keyEquals(key)) {
        return alternative_position;
    }

    return -1;
}

template<class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::hash1(const char *str) const {
    uint16_t h = 0x811C;

    for (long i = 0; i < KeyLength; i++) {
        if (str[i] == '\0') {
            break;
        }

        uint8_t byte_val = (uint8_t) str[i];
        h = (h << 5) + h;

        if (i % 2 == 0) {
            h ^= pgm_read_word(&(Table_H1_Byte0[byte_val]));
        } else {
            h ^= pgm_read_word(&(Table_H1_Byte1[byte_val]));
        }
    }

    return h & ((_eeprom_size / SlotSize) - 1);
}

template<class EepromDriver, long SlotSize, long KeyLength>
long KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::hash2(const char *str) const {
    uint16_t h = 0xCBF2;

    for (long i = 0; i < KeyLength; i++) {
        if (str[i] == '\0') {
            break;
        }

        uint8_t byte_val = (uint8_t) str[i];
        h = (h << 5) + h;

        if (i % 2 == 0) {
            h ^= pgm_read_word(&(Table_H2_Byte0[byte_val]));
        } else {
            h ^= pgm_read_word(&(Table_H2_Byte1[byte_val]));
        }
    }

    return h & ((_eeprom_size / SlotSize) - 1);
}

template<class EepromDriver, long SlotSize, long KeyLength>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(const char *key, const long *data) {
    return insert<sizeof(long)>(key, (const byte *) data);
}

template<class EepromDriver, long SlotSize, long KeyLength>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(const char *key, const long data) {
    return insert<sizeof(long)>(key, (const byte *) &data);
}

template<class EepromDriver, long SlotSize, long KeyLength>
template<long size>
bool KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::insert(const char *key, const byte *data) {
    // Prevent insert for now if size is too large, somehow maybe spread the object out in multiple slots in the future.
    if (size > SlotSize) {
        Serial.println("Size is too large for this EEPROM.");

        return false;
    }

    const CuckooPositionResult position_result = getInsertPosition(key);

    if (position_result.cycleDetected()) {
        Serial.print(key);
        Serial.println(": Cycle detected.");

        return false;
    }

    if (position_result.is_occupied) {
        byte existing_bucket[SlotSize];

        _driver.read(position_result.start_position, existing_bucket, SlotSize);

        _driver.write(position_result.existing_bucket_new_start_position, existing_bucket, SlotSize);
    }

    Bucket<KeyLength, size, SlotSize> bucket_to_insert(key, data);

    _driver.write(position_result.start_position, (byte *) &bucket_to_insert, SlotSize);

    return true;
}

template<class EepromDriver, long SlotSize, long KeyLength>
template<typename T>
T KeyValueDatabase<EepromDriver, SlotSize, KeyLength>::get(const char *key) {
    const long position = findPosition(key);

    if (position == -1) {
        Serial.println("Key not found.");

        return T{};
    }

    Bucket<KeyLength, sizeof(T), SlotSize> bucket;
    _driver.read(position, (byte *) &bucket, SlotSize);

    T result;

    memcpy(&result, bucket.data, sizeof(T));

    return result;
}

#pragma once

template<int KeyLength, int DataSize, int SlotSize>
struct Bucket {
    Bucket() = default;

    Bucket(const char *key, const byte *data) {
        copyKey(key);
        memcpy(this->data, data, DataSize);
    }

    char key[KeyLength] = {};
    byte data[DataSize] = {};
    byte padding[SlotSize - sizeof(char[KeyLength]) - sizeof(byte[DataSize])] = {};

    bool hasNullTerminator() const {
        for (int i = 0; i < KeyLength - 1; i++) {
            if (key[i] == '\0') {
                return true;
            }
        }

        return false;
    }

    void copyKey(const char *other_key) {
        for (int i = 0; i < KeyLength; i++) {
            key[i] = other_key[i];

            if (other_key[i] == '\0') {
                return;
            }
        }

        key[KeyLength - 1] = '\0';
    }

    bool keyEquals(const char *other_key) const {
        for (int i = 0; i < KeyLength; i++) {
            if (key[i] != other_key[i]) {
                return false;
            }

            if (key[i] == '\0') {
                return true;
            }
        }

        return true;
    }
};

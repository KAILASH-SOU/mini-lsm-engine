#include<iostream>
#include "wal.h"

#include "memtable.h"
void WAL::replay(MemTable& memtable) {
    std::ifstream in(filename_, std::ios::binary);
    if (!in) return;

    while (true) {
        uint32_t key_size, value_size;

        in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
        in.read(reinterpret_cast<char*>(&value_size), sizeof(value_size));
        if (!in) break;

        std::string key(key_size, '\0');
        std::string value(value_size, '\0');

        in.read(&key[0], key_size);
        in.read(&value[0], value_size);
        if (!in) break;

        memtable.put(key, value);
    }
}


#include "wal.h"
#include <iostream>

WAL::WAL(const std::string& filename)
    : filename_(filename) {
    open();
}

void WAL::open() {
    wal_file_.open(filename_, std::ios::binary | std::ios::app);
    if (!wal_file_) {
        throw std::runtime_error("Failed to open WAL file");
    }
}

void WAL::append(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);

    uint32_t key_size = key.size();
    uint32_t value_size = value.size();

    wal_file_.write(reinterpret_cast<char*>(&key_size), sizeof(key_size));
    wal_file_.write(reinterpret_cast<char*>(&value_size), sizeof(value_size));
    wal_file_.write(key.data(), key_size);
    wal_file_.write(value.data(), value_size);

    wal_file_.flush();   
}

void WAL::replay(std::unordered_map<std::string, std::string>& memtable) {
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

        memtable[key] = value;
    }
}

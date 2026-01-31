#include "sstable.h"
#include <fstream>

void SSTable::write(
    const std::string& filename,
    const std::map<std::string, std::string>& data
) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Failed to create SSTable");
    }

    for (const auto& [key, value] : data) {
        uint32_t key_size = key.size();
        uint32_t value_size = value.size();

        out.write(reinterpret_cast<char*>(&key_size), sizeof(key_size));
        out.write(reinterpret_cast<char*>(&value_size), sizeof(value_size));
        out.write(key.data(), key_size);
        out.write(value.data(), value_size);
    }

    out.flush(); // fsync later
}

bool SSTable::get(
    const std::string& filename,
    const std::string& key,
    std::string& value
) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) return false;

    while (true) {
        uint32_t key_size, value_size;

        in.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
        in.read(reinterpret_cast<char*>(&value_size), sizeof(value_size));
        if (!in) break;

        std::string file_key(key_size, '\0');
        std::string file_value(value_size, '\0');

        in.read(&file_key[0], key_size);
        in.read(&file_value[0], value_size);
        if (!in) break;

        if (file_key == key) {
            value = file_value;
            return true;
        }
    }

    return false;
}

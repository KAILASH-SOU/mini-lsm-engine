#include "engine.h"
#include "sstable.h"
#include <filesystem>


Engine::Engine(const std::string& wal_path)
    : wal_(wal_path) {
    wal_.replay(memtable_);
}



void Engine::put(const std::string& key, const std::string& value) {
    wal_.append(key, value);
    memtable_.put(key, value);

    const size_t MEMTABLE_FLUSH_LIMIT = 3;
    if (memtable_.size() >= MEMTABLE_FLUSH_LIMIT) {
        flush_memtable();
    }
}


std::optional<std::string> Engine::get(const std::string& key) {
    
    auto val = memtable_.get(key);
    if (val) return val;

    
    for (auto it = sstables_.rbegin(); it != sstables_.rend(); ++it) {
        std::string value;
        if (SSTable::get(*it, key, value)) {
            return value;
        }
    }

    return std::nullopt;
}


void Engine::flush_memtable() {
    if (memtable_.size() == 0) return;

    std::filesystem::create_directories("data/sstables");

    std::string filename =
        "data/sstables/sstable_" + std::to_string(sstables_.size()) + ".dat";

    SSTable::write(filename, memtable_.data());

    sstables_.push_back(filename);

    memtable_.clear();
}

#include "engine.h"

Engine::Engine(const std::string& wal_path)
    : wal_(wal_path) {
    wal_.replay(memtable_);
}


void Engine::put(const std::string& key, const std::string& value) {
    
    wal_.append(key, value);

    memtable_.put(key, value);
}

std::optional<std::string> Engine::get(const std::string& key) {
    return memtable_.get(key);
}

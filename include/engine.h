#pragma once

#include "wal.h"
#include "memtable.h"
#include <string>

class Engine {
public:
    explicit Engine(const std::string& wal_path);

    // User API
    void put(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);

private:
    WAL wal_;
    MemTable memtable_;
};

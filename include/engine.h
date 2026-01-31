#pragma once

#include "wal.h"
#include "memtable.h"
#include <string>
#include <vector>
#include <optional>

class Engine {
public:
    explicit Engine(const std::string& wal_path);

    void put(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key);

private:
    WAL wal_;
    MemTable memtable_;

    
    std::vector<std::string> sstables_;

    void flush_memtable();
    void compact();

};

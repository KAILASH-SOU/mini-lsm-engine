#include "engine.h"
#include "sstable.h"
#include <filesystem>
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

Engine::Engine(const std::string& wal_path)
    : wal_(wal_path) {
    // 1. Load existing SSTables from disk
    if (fs::exists("data/sstables")) {
        for (const auto& entry : fs::directory_iterator("data/sstables")) {
            if (entry.path().extension() == ".dat") {
                sstables_.push_back(entry.path().string());
            }
        }
        // Sort SSTables by name/time to ensure correct read order (newest last)
        std::sort(sstables_.begin(), sstables_.end());
    }

    // 2. Replay WAL into MemTable
    wal_.replay(memtable_);
}

void Engine::put(const std::string& key, const std::string& value) {
    wal_.append(key, value);
    memtable_.put(key, value);

    const size_t MEMTABLE_FLUSH_LIMIT = 5; // Increased for better demo
    if (memtable_.size() >= MEMTABLE_FLUSH_LIMIT) {
        flush_memtable();
    }
}

std::optional<std::string> Engine::get(const std::string& key) {
    // Check MemTable first
    auto val = memtable_.get(key);
    if (val) return val;

    // Check SSTables from newest to oldest
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

    fs::create_directories("data/sstables");

    std::string filename =
        "data/sstables/sstable_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(sstables_.size()) + ".dat";

    SSTable::write(filename, memtable_.data());
    sstables_.push_back(filename);

    memtable_.clear();
    
    // Simple compaction trigger: merge if more than 4 SSTables
    if (sstables_.size() >= 4) {
        compact();
    }
}

void Engine::compact() {
    if (sstables_.size() < 2) return;

    std::cout << "[Compaction] Merging SSTables..." << std::endl;

    // Merge all SSTables into one for simplicity in this version
    std::map<std::string, std::string> merged;

    auto read_all = [&](const std::string& file) {
        std::ifstream in(file, std::ios::binary);
        if (!in) return;
        while (true) {
            uint32_t ks, vs;
            in.read(reinterpret_cast<char*>(&ks), sizeof(ks));
            in.read(reinterpret_cast<char*>(&vs), sizeof(vs));
            if (!in) break;

            std::string k(ks, '\0'), v(vs, '\0');
            in.read(&k[0], ks);
            in.read(&v[0], vs);
            if (!in) break;

            merged[k] = v; // Overwrite older values with newer ones
        }
    };

    // Read from oldest to newest so newest values win
    for (const auto& sst : sstables_) {
        read_all(sst);
    }

    std::string out = "data/sstables/compacted_" + std::to_string(std::time(nullptr)) + ".dat";
    SSTable::write(out, merged);

    // Remove old files
    for (const auto& sst : sstables_) {
        fs::remove(sst);
    }

    sstables_.clear();
    sstables_.push_back(out);
    
    std::cout << "[Compaction] Done. Result: " << out << std::endl;
}

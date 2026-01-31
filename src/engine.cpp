#include "engine.h"
#include "sstable.h"
#include <filesystem>
#include <map>

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
    if (sstables_.size() >= 3) {
    compact();
}

}

void Engine::compact() {
    if (sstables_.size() < 2) return;

    
    std::string sst1 = sstables_[0];
    std::string sst2 = sstables_[1];

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

            merged[k] = v; 
        }
    };

    read_all(sst1);
    read_all(sst2);

    
    std::string out =
        "data/sstables/sstable_compacted_" + std::to_string(sstables_.size()) + ".dat";
    SSTable::write(out, merged);

    
    sstables_.erase(sstables_.begin(), sstables_.begin() + 2);
    sstables_.push_back(out);

    std::filesystem::remove(sst1);
    std::filesystem::remove(sst2);
}

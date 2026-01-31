#include "memtable.h"

void MemTable::put(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    table_[key] = value;
}

std::optional<std::string> MemTable::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = table_.find(key);
    if (it == table_.end()) {
        return std::nullopt;
    }
    return it->second;
}

size_t MemTable::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return table_.size();
}

void MemTable::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    table_.clear();
}

const std::map<std::string, std::string>& MemTable::data() const {
    return table_;
}

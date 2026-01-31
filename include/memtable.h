#pragma once

#include <map>
#include <string>
#include <mutex>
#include <optional>

class MemTable {
public:
    
    void put(const std::string& key, const std::string& value);

    std::optional<std::string> get(const std::string& key);

    size_t size() const;

    void clear();

    const std::map<std::string, std::string>& data() const;

private:
    std::map<std::string, std::string> table_;
    mutable std::mutex mutex_;
};

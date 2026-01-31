#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <unordered_map>

class WAL {
public:
    explicit WAL(const std::string& filename);
    void append(const std::string& key, const std::string& value);

    void replay(std::unordered_map<std::string, std::string>& memtable);

private:
    std::string filename_;
    std::ofstream wal_file_;
    std::mutex mutex_;

    void open();
};

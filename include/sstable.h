#pragma once

#include <string>
#include <map>

class SSTable {
public:
    
    static void write(
        const std::string& filename,
        const std::map<std::string, std::string>& data
    );

    static bool get(
        const std::string& filename,
        const std::string& key,
        std::string& value
    );
};

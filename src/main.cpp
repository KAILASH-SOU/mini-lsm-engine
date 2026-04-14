#include "engine.h"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

void print_header(const std::string& title) {
    std::cout << "\n" << std::setfill('=') << std::setw(50) << "" << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::setfill('=') << std::setw(50) << "" << std::endl;
}

int main() {
    print_header("Mini LSM Engine Demo");

    // Initialize engine with a WAL file
    Engine engine("data/wal.log");

    std::cout << "[Info] Initializing engine and loading data..." << std::endl;

    // 1. Basic Put & Get
    print_header("1. Basic Operations");
    engine.put("user:1", "Alice");
    engine.put("user:2", "Bob");
    engine.put("user:3", "Charlie");

    auto val1 = engine.get("user:1");
    auto val2 = engine.get("user:2");
    
    std::cout << "Fetched user:1 -> " << (val1 ? *val1 : "NOT FOUND") << std::endl;
    std::cout << "Fetched user:2 -> " << (val2 ? *val2 : "NOT FOUND") << std::endl;

    // 2. Demonstration of MemTable Flush and Compaction
    print_header("2. Flush & Compaction Demo");
    std::cout << "Inserting more keys to trigger MemTable flush..." << std::endl;
    for (int i = 0; i < 15; ++i) {
        engine.put("key_" + std::to_string(i), "value_" + std::to_string(i));
    }
    
    std::cout << "Checking a key from an older SSTable: key_0 -> " 
              << (engine.get("key_0") ? *engine.get("key_0") : "NOT FOUND") << std::endl;

    // 3. Update existing key
    print_header("3. Key Updates");
    std::cout << "Updating user:1 Alice -> Alice Smith" << std::endl;
    engine.put("user:1", "Alice Smith");
    std::cout << "New user:1 -> " << (engine.get("user:1") ? *engine.get("user:1") : "NOT FOUND") << std::endl;

    // 4. Persistence Test
    print_header("4. Persistence Information");
    std::cout << "The data is now stored in 'data/' directory." << std::endl;
    std::cout << "Try restarting this program to see persistence in action!" << std::endl;

    print_header("Demo Complete");
    return 0;
}

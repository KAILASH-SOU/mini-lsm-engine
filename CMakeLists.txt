cmake_minimum_required(VERSION 3.12)
project(mini_lsm_engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/engine.cpp
    src/memtable.cpp
    src/sstable.cpp
    src/wal.cpp
    src/main.cpp
)

# Header search paths
include_directories(include)

# Executable
add_executable(lsm_engine ${SOURCES})

# Installation options (optional)
install(TARGETS lsm_engine DESTINATION bin)

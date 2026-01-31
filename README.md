# Mini Log-Structured Merge Tree (LSM) Storage Engine

## Overview
This project is a **single-node, disk-backed key–value storage engine**
inspired by **Log-Structured Merge Tree (LSM)** design principles.

The goal of this project is **not** to build a production database,
but to deeply understand and implement the **core mechanisms used in real
storage systems** such as RocksDB and LevelDB.

The engine prioritizes:
- Correctness
- Crash safety
- Write efficiency
- Clear separation of responsibilities

---

## Key Features

- **Write-Ahead Logging (WAL)** for durability
- **In-memory MemTable** for fast reads and writes
- **Immutable SSTables** for persistent disk storage
- **Safe background compaction**
- **Crash recovery via WAL replay**
- Clean, modular C++17 codebase

---

## High-Level Architecture

### Write Path
PUT(key, value)
↓
Append to WAL (disk, sequential)
↓
Insert into MemTable (RAM)
↓
Flush MemTable → SSTable (when threshold reached)


### Read Path
GET(key)
↓
Check MemTable
↓
Check newest SSTables first
↓
Return first matching value


### Background Tasks
- MemTable flushing to SSTables
- SSTable compaction (merge old tables safely)

---

## Core Components

### 1. Write-Ahead Log (WAL)
- Append-only, sequential disk writes
- Guarantees durability before in-memory updates
- Used only for crash recovery (not for reads)

### 2. MemTable
- In-memory, sorted key–value store
- Acts as the primary working database
- Flushed to disk when size threshold is reached

### 3. SSTables
- Immutable, sorted disk files
- Written sequentially
- Never modified after creation
- Read-only during normal operation

### 4. Compaction
- Merges older SSTables into a new one
- Retains only the latest value for each key
- Old files are deleted **only after successful merge**
- Ensures crash safety and correctness

---

## Crash Recovery

On startup:
1. Existing SSTables are loaded
2. WAL is replayed sequentially
3. MemTable is rebuilt from WAL entries
4. Engine resumes normal operation

This guarantees that all acknowledged writes
are recoverable after a crash.

---

## Design Principles

- **Sequential disk writes over random writes**
- **Immutability for safety**
- **Write optimization over read simplicity**
- **Explicit durability vs performance tradeoffs**
- **Correctness before optimization**





## Build & Run

### Build
```bash
g++ -std=c++17 src/*.cpp -Iinclude -o lsm

### Run 
    ./lsm


# Redis Clone in C

A Redis-inspired database built from scratch in C (and no AI) to learn database internals, networking, persistence, and systems programming concepts.

This project is not intended to be a production-ready Redis replacement. The goal is to understand how databases work internally by implementing the core building blocks ourselves without using AI. Yup, no claude, copilot or any other ai agent :)

## Why This Project?

Most developers use databases as black boxes.

This project explores the internals behind systems like Redis by implementing:

- TCP networking
- Event-driven I/O
- Redis Serialization Protocol (RESP)
- Command parsing and execution
- In-memory storage
- Key expiration (TTL)
- Persistence through Append Only Files (AOF)
- Replication (planned)

The emphasis is on learning and understanding rather than feature completeness.

---

## Features

### Networking

- TCP server written in C
- Single-threaded architecture
- Event-driven networking using kqueue
- Multiple simultaneous client connections

### RESP Protocol

Implemented support for Redis Serialization Protocol (RESP):

- Arrays
- Bulk Strings
- Simple Strings
- Error Responses
- Integer Responses

Compatible with redis-cli for supported commands.

### Supported Commands

| Command | Status |
| ------- | ------ |
| PING    | ✅     |
| ECHO    | ✅     |
| SET     | ✅     |
| GET     | ✅     |
| DEL     | ✅     |
| EXPIRE  | ✅     |
| TTL     | ✅     |

### In-Memory Storage

- Hash map based key-value store
- String keys
- String values

### Expiration

- TTL support
- Lazy expiration
- Automatic cleanup of expired keys on access

### Persistence

Append Only File (AOF) implementation:

- Write operations appended to disk
- Database state reconstructed on startup
- Uses the same RESP parser and command execution path as live requests

---

## Architecture

```text
                  +----------------+
                  |    redis-cli   |
                  +-------+--------+
                          |
                          v
                  +----------------+
                  |   TCP Server   |
                  +-------+--------+
                          |
                          v
                  +----------------+
                  |     kqueue     |
                  +-------+--------+
                          |
                          v
                  +----------------+
                  | RESP Parser    |
                  +-------+--------+
                          |
                          v
                  +----------------+
                  | Command Router |
                  +-------+--------+
                          |
                          v
                  +----------------+
                  |   Executor     |
                  +-------+--------+
                          |
                          v
                  +----------------+
                  |   Hash Map     |
                  +----------------+

```

### AOF Recovery Path

```text
AOF File
    |
    v
RESP Parser
    |
    v
Executor
    |
    v
Hash Map
```

The same parser and executor are used for both live requests and recovery, ensuring consistent behavior.

---

## Running

```bash
git clone https://github.com/Rishabh-Singh-01/redis-clone.git
cd redis-clone
```

```bash
make run
```

Connect using:

```bash
redis-cli -p 9999
```

Example:

```bash
PING
PONG

SET name john
OK

GET name
john

DEL name
(integer) 1
```

---

## What We Learned

This project explores:

### Networking

- TCP sockets
- Blocking vs non-blocking I/O
- Event loops
- kqueue

### Protocol Design

- RESP
- Message framing
- Serialization
- Parsing streamed data

### Database Internals

- In-memory storage
- Hash tables
- Key expiration
- Durability
- Recovery

### Systems Programming

- Memory management
- Resource ownership
- State machines
- Event-driven architectures

---

## Roadmap

### Completed

- [x] TCP Server
- [x] RESP Parser
- [x] PING
- [x] ECHO
- [x] SET / GET
- [x] DEL
- [x] EXPIRE
- [x] TTL
- [x] Lazy Expiration
- [x] AOF Persistence
- [x] AOF Recovery
- [x] Event Loop (kqueue)
- [x] Multiple Client Connections

### Planned

- [ ] Replication
- [ ] Full Synchronization
- [ ] Replica Mode
- [ ] Hash Table Resizing
- [ ] Additional Redis Data Structures
- [ ] RDB Snapshots
- [ ] Transactions
- [ ] Pub/Sub
- [ ] Partial TCP Read/Writes
- [ ] AOF Optimizations
- [ ] Networking Optimizations
- [ ] Error Handling Improvement

---

## Disclaimer

This project is a learning exercise focused on understanding database internals and systems programming concepts.

It intentionally prioritizes clarity and learning over performance, feature completeness, and production readiness.

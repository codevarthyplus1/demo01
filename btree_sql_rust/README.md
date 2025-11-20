# B-Tree SQL Server (Rust)

A lightweight SQL database server implemented in Rust from scratch, featuring a B-tree index structure for efficient data retrieval. This is a Rust reimplementation of the C version with improved memory safety and error handling.

## Features

- **B-Tree Indexing**: Efficient O(log n) search and insert operations using B-tree data structure
- **SQL Parser**: Custom SQL parser supporting basic SQL statements
- **REPL Interface**: Interactive command-line interface for executing SQL queries
- **Persistence**: Save and load tables to/from disk using bincode serialization
- **Data Types**: Support for INTEGER and TEXT data types
- **Memory Safety**: Leverages Rust's ownership system for safe memory management
- **Error Handling**: Comprehensive Result-based error handling throughout
- **Core SQL Operations**:
  - CREATE TABLE
  - INSERT INTO
  - SELECT (with optional WHERE clause)

## Architecture

### Modules

1. **btree.rs**
   - Generic B-tree implementation with configurable order
   - Supports insert and search operations
   - Automatic node splitting for self-balancing
   - Uses Box<T> for heap allocation and ownership

2. **storage.rs**
   - Type-safe column definitions using enums
   - Row-oriented storage format
   - Serializable tables using serde
   - Persistence layer with bincode

3. **parser.rs**
   - SQL tokenizer and parser
   - Returns strongly-typed Statement enum
   - Supports CREATE TABLE, INSERT, and SELECT statements

4. **executor.rs**
   - Query execution engine
   - Database catalog management using HashMap
   - Coordinates between storage and B-tree index
   - Result-based error propagation

5. **main.rs**
   - REPL (Read-Eval-Print Loop) interface
   - Meta-commands for database management
   - Input/output handling

## Building and Running

### Prerequisites

- Rust 1.70+ (2021 edition)
- Cargo

### Build

```bash
cargo build --release
```

### Run

```bash
cargo run --release
# or
./target/release/btree_sql_rust
```

### Run Tests

```bash
cargo test
```

## Usage Examples

### Create a table
```sql
CREATE TABLE users (id INT, name TEXT, age INT)
```

### Insert data
```sql
INSERT INTO users VALUES (1, 'Alice', 30)
INSERT INTO users VALUES (2, 'Bob', 25)
INSERT INTO users VALUES (3, 'Charlie', 35)
```

### Query data
```sql
SELECT * FROM users
SELECT * FROM users WHERE id = 2
```

### Meta Commands

- `.help` - Show help information
- `.tables` - List all tables in the database
- `.schema` - Show schema for all tables
- `.btree <table>` - Display B-tree structure for a specific table
- `.save` - Save all tables to disk
- `.load <table>` - Load a table from disk
- `.exit` - Exit the program

## Rust-Specific Advantages

### Memory Safety
- No manual memory management
- Compile-time checks prevent memory leaks
- No null pointer dereferences
- Automatic cleanup with Drop trait

### Type Safety
- Strong type system with enums for SQL types
- Pattern matching for exhaustive case handling
- Result<T, E> for explicit error handling
- No implicit type conversions

### Performance
- Zero-cost abstractions
- Efficient memory layout
- No garbage collection overhead
- Optimized release builds

## Implementation Details

### B-Tree Properties
- Order: 4 (minimum degree)
- Minimum keys per node: 3
- Maximum keys per node: 7
- Self-balancing through node splitting
- Uses Box<BTreeNode> for heap allocation

### Storage Format
- Serialized using bincode for compact binary format
- Row-oriented storage with Vec<Row>
- Integer: i64 (64-bit signed)
- Text: String (UTF-8, heap-allocated)

### Error Handling
- All operations return Result<T, String>
- Parser errors include descriptive messages
- Type checking at insert time
- No panics in normal operation

### Differences from C Version

| Feature | C Version | Rust Version |
|---------|-----------|--------------|
| Memory Management | Manual (malloc/free) | Automatic (ownership) |
| Error Handling | Return codes | Result<T, E> |
| Type System | Weak | Strong with enums |
| Safety | Runtime checks | Compile-time checks |
| Serialization | Binary fread/fwrite | Serde + bincode |
| String Handling | Fixed-size arrays | Heap-allocated String |
| Collections | Manual arrays | Vec, HashMap |

## Limitations

- Simple WHERE clause (equality on any column)
- No JOIN operations
- No UPDATE or DELETE statements
- No transactions or concurrency control
- Limited data types (INT and TEXT only)
- No NULL values
- No constraints (PRIMARY KEY, FOREIGN KEY, etc.)

## Future Enhancements

- [ ] Implement UPDATE and DELETE operations
- [ ] Add support for more data types (f64, bool, etc.)
- [ ] Implement JOIN operations
- [ ] Add transaction support with ACID properties
- [ ] Implement query optimization
- [ ] Add support for more complex WHERE clauses
- [ ] Implement aggregate functions (COUNT, SUM, AVG, etc.)
- [ ] Add support for indexes on multiple columns
- [ ] Implement concurrent access with Arc and Mutex
- [ ] Add support for async I/O

## Dependencies

- `serde` (1.0) - Serialization framework
- `bincode` (1.3) - Binary encoding for serde

## Testing

Run the test suite:
```bash
cargo test
```

Run with example queries:
```bash
./target/release/btree_sql_rust < test_queries.sql
```

## Performance Comparison

The Rust version offers several performance benefits:
- Faster compilation-time optimizations
- Better cache locality with Vec
- Zero-cost abstractions
- LLVM optimizations

Benchmark results show similar O(log n) performance for B-tree operations with improved safety guarantees.

## License

MIT License - feel free to use this for learning purposes.

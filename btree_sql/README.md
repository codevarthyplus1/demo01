# B-Tree SQL Server

A lightweight SQL database server implemented in C from scratch, featuring a B-tree index structure for efficient data retrieval.

## Features

- **B-Tree Indexing**: Efficient O(log n) search, insert operations using B-tree data structure
- **SQL Parser**: Custom SQL parser supporting basic SQL statements
- **REPL Interface**: Interactive command-line interface for executing SQL queries
- **Persistence**: Save and load tables to/from disk
- **Data Types**: Support for INTEGER and TEXT data types
- **Core SQL Operations**:
  - CREATE TABLE
  - INSERT INTO
  - SELECT (with optional WHERE clause)

## Architecture

### Components

1. **B-Tree Module** (`btree.c/h`)
   - Implements a self-balancing B-tree with configurable order
   - Supports insert and search operations
   - Maintains sorted keys for efficient range queries

2. **Storage Module** (`storage.c/h`)
   - Page-based storage management
   - Row-oriented storage format
   - Table schema management
   - Persistence layer for saving/loading tables

3. **Parser Module** (`parser.c/h`)
   - SQL tokenizer and parser
   - Converts SQL text into executable statements
   - Supports CREATE TABLE, INSERT, and SELECT statements

4. **Executor Module** (`executor.c/h`)
   - Query execution engine
   - Manages database catalog (tables and indexes)
   - Coordinates between storage and B-tree index

5. **Main Module** (`main.c`)
   - REPL (Read-Eval-Print Loop) interface
   - Meta-commands for database management

## Building

```bash
make
```

## Running

```bash
make run
# or
./btree_sql
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
- `.btree` - Display B-tree structure (for debugging)
- `.save` - Save all tables to disk
- `.load` - Load a table from disk
- `.exit` - Exit the program

## Implementation Details

### B-Tree Properties
- Order: 4 (configurable via BTREE_ORDER)
- Minimum keys per node: 3
- Maximum keys per node: 7
- Self-balancing through node splitting

### Storage Format
- Row-oriented storage
- Fixed-size column values
- Integer: 64-bit signed
- Text: Variable length up to 256 characters

### Limitations
- Simple WHERE clause (equality on first column only)
- No JOIN operations
- No UPDATE or DELETE statements (DELETE is stubbed)
- No transactions or concurrency control
- Limited data types (INT and TEXT only)
- No NULL values
- No constraints (PRIMARY KEY, FOREIGN KEY, etc.)

## Future Enhancements

- [ ] Implement UPDATE and DELETE operations
- [ ] Add support for more data types (FLOAT, DATE, etc.)
- [ ] Implement JOIN operations
- [ ] Add transaction support with ACID properties
- [ ] Implement query optimization
- [ ] Add support for more complex WHERE clauses
- [ ] Implement aggregate functions (COUNT, SUM, AVG, etc.)
- [ ] Add support for indexes on multiple columns
- [ ] Implement a buffer pool manager
- [ ] Add support for concurrent access

## License

MIT License - feel free to use this for learning purposes.

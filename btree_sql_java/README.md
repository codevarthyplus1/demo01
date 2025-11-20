# B-Tree SQL Server (Java)

A lightweight SQL database server implemented in Java from scratch, featuring a B-tree index structure for efficient data retrieval. This is a Java reimplementation showcasing object-oriented design and Java's built-in features.

## Features

- **B-Tree Indexing**: Efficient O(log n) search and insert operations using B-tree data structure
- **SQL Parser**: Custom SQL parser supporting basic SQL statements
- **REPL Interface**: Interactive command-line interface for executing SQL queries
- **Persistence**: Save and load tables to/from disk using Java serialization
- **Data Types**: Support for INTEGER and TEXT data types
- **Object-Oriented Design**: Clean separation of concerns with interfaces and classes
- **Core SQL Operations**:
  - CREATE TABLE
  - INSERT INTO
  - SELECT (with optional WHERE clause)

## Architecture

### Packages and Classes

1. **BTree.java**
   - Self-balancing B-tree implementation
   - Inner BTreeNode class for tree structure
   - ArrayList-based storage for keys and children
   - Automatic node splitting

2. **Storage Classes**
   - **ColumnType.java**: Enum for column types (INT, TEXT)
   - **ColumnDef.java**: Column definition with name and type
   - **ColumnValue.java**: Type-safe value wrapper
   - **Row.java**: Database row with row ID and values
   - **Table.java**: Table management with serialization

3. **Statement Classes**
   - **Statement.java**: Interface for SQL statements
   - **CreateTableStatement.java**: CREATE TABLE representation
   - **InsertStatement.java**: INSERT representation
   - **SelectStatement.java**: SELECT with WHERE support

4. **Parser.java**
   - SQL tokenizer and parser
   - Converts SQL strings to Statement objects
   - Error handling with exceptions

5. **Database.java**
   - Query execution engine
   - HashMap-based table catalog
   - Coordinates storage and indexing
   - Comprehensive error handling

6. **Main.java**
   - REPL (Read-Eval-Print Loop) interface
   - Meta-commands for database management
   - Scanner-based input handling

## Building and Running

### Prerequisites

- Java 11 or higher
- No external dependencies (uses Java built-in serialization)

### Compile

```bash
mkdir -p target/classes
javac -d target/classes src/main/java/com/btreesql/*.java
```

### Run

```bash
java -cp target/classes com.btreesql.Main
# or use the convenience script
./run.sh
```

### Build with Maven (if available)

```bash
mvn clean package
java -jar target/btree-sql-server-1.0.0.jar
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

## Java-Specific Features

### Object-Oriented Design
- **Interfaces**: Statement interface for polymorphism
- **Enums**: Type-safe ColumnType enumeration
- **Encapsulation**: Private fields with public accessors
- **Inheritance**: Statement implementations

### Collections Framework
- **ArrayList**: Dynamic arrays for keys, values, rows
- **HashMap**: O(1) table lookup by name
- **List Interface**: Flexible collection handling

### Exception Handling
- Checked exceptions for error handling
- Try-with-resources for automatic resource management
- Descriptive error messages

### Serialization
- Java built-in serialization (no external dependencies)
- Automatic serialization with Serializable interface
- Binary format for efficient storage

## Implementation Details

### B-Tree Properties
- Order: 4 (minimum degree)
- Minimum keys per node: 3
- Maximum keys per node: 7
- Self-balancing through node splitting
- ArrayList-based storage

### Storage Format
- Java ObjectOutputStream/ObjectInputStream
- Binary serialization with .ser extension
- Integer: long (64-bit signed)
- Text: String (UTF-16 internally)

### Type System
- Enum-based column types
- Static factory methods for ColumnValue
- Type checking at insert time
- Exception-based error handling

### Design Patterns Used

| Pattern | Usage |
|---------|-------|
| Factory Method | ColumnValue.ofInt(), ColumnValue.ofText() |
| Strategy | Statement interface with implementations |
| Singleton-like | Database manages global state |
| Builder-like | CreateTableStatement.addColumn() |
| Iterator | Enhanced for-loops with Collections |

## Advantages of Java Version

1. **Object-Oriented**: Clean class hierarchy and interfaces
2. **Type Safety**: Strong typing with enums and generics
3. **Collections**: Built-in ArrayList, HashMap for dynamic data
4. **Garbage Collection**: Automatic memory management
5. **Exception Handling**: Structured error handling
6. **Portability**: Write once, run anywhere (JVM)
7. **IDE Support**: Excellent tooling and debugging
8. **Reflection**: Serialization without boilerplate

## Comparison with Other Implementations

| Feature | C Version | Rust Version | Java Version |
|---------|-----------|--------------|--------------|
| Memory Management | Manual | Ownership | Garbage Collection |
| Type System | Weak | Strong | Strong with OOP |
| Error Handling | Return codes | Result<T, E> | Exceptions |
| Collections | Arrays | Vec, HashMap | ArrayList, HashMap |
| Serialization | fwrite | Serde | Java Serialization |
| Build System | Makefile | Cargo | Maven/Gradle |
| Null Safety | Pointers | Option<T> | Nullable references |

## Limitations

- Simple WHERE clause (equality on any column)
- No JOIN operations
- No UPDATE or DELETE statements
- No transactions or concurrency control
- Limited data types (INT and TEXT only)
- No NULL values
- No constraints (PRIMARY KEY, FOREIGN KEY, etc.)
- Single-threaded execution

## Future Enhancements

- [ ] Implement UPDATE and DELETE operations
- [ ] Add support for more data types (Double, Boolean, Date)
- [ ] Implement JOIN operations
- [ ] Add transaction support with ACID properties
- [ ] Implement query optimization
- [ ] Add support for more complex WHERE clauses
- [ ] Implement aggregate functions (COUNT, SUM, AVG, etc.)
- [ ] Add support for indexes on multiple columns
- [ ] Implement concurrent access with synchronized blocks
- [ ] Add connection pooling for multi-client support
- [ ] Implement prepared statements
- [ ] Add support for stored procedures

## Project Structure

```
btree_sql_java/
├── src/
│   └── main/
│       └── java/
│           └── com/
│               └── btreesql/
│                   ├── BTree.java (160 lines)
│                   ├── ColumnDef.java (30 lines)
│                   ├── ColumnType.java (23 lines)
│                   ├── ColumnValue.java (53 lines)
│                   ├── CreateTableStatement.java (35 lines)
│                   ├── Database.java (251 lines)
│                   ├── InsertStatement.java (32 lines)
│                   ├── Main.java (115 lines)
│                   ├── Parser.java (168 lines)
│                   ├── Row.java (31 lines)
│                   ├── SelectStatement.java (51 lines)
│                   ├── Statement.java (12 lines)
│                   └── Table.java (115 lines)
├── data/
├── pom.xml
├── run.sh
└── README.md
```

**Total Lines of Code**: ~1,076

## Testing

Run with test queries:
```bash
java -cp target/classes com.btreesql.Main < test_queries.sql
```

Run with advanced test (B-tree splitting):
```bash
java -cp target/classes com.btreesql.Main < test_advanced.sql
```

## Performance Characteristics

- **Compilation**: Fast (~1-2s)
- **Startup**: Moderate (JVM initialization)
- **Runtime**: Good performance with JIT compilation
- **Memory**: Higher overhead due to JVM and garbage collection
- **Scalability**: Good for moderate datasets

## Why Java?

- **Enterprise-Ready**: Industry standard for backend systems
- **Cross-Platform**: Runs on any OS with JVM
- **Rich Ecosystem**: Vast libraries and frameworks
- **Mature Tooling**: IDEs, debuggers, profilers
- **Community**: Large developer community
- **Hiring**: Easy to find Java developers
- **Stability**: Backward compatibility and long-term support

## License

MIT License - feel free to use this for learning purposes.

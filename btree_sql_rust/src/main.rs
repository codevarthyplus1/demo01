mod btree;
mod executor;
mod parser;
mod storage;

use executor::Database;
use parser::Parser;
use std::io::{self, Write};

fn print_welcome() {
    println!("========================================");
    println!("  B-Tree SQL Server v1.0 (Rust)");
    println!("========================================");
    println!("Type SQL commands or '.help' for help");
    println!("Type '.exit' to quit\n");
}

fn print_help() {
    println!("\nSupported SQL Commands:");
    println!("  CREATE TABLE table_name (col1 INT, col2 TEXT, ...)");
    println!("  INSERT INTO table_name VALUES (val1, val2, ...)");
    println!("  SELECT * FROM table_name");
    println!("  SELECT * FROM table_name WHERE col = value");
    println!("\nMeta Commands:");
    println!("  .help     - Show this help");
    println!("  .tables   - List all tables");
    println!("  .schema   - Show schema for all tables");
    println!("  .btree <table> - Show B-tree structure for table");
    println!("  .save     - Save all tables to disk");
    println!("  .load <table> - Load a table from disk");
    println!("  .exit     - Exit the program\n");
}

fn handle_meta_command(db: &mut Database, command: &str) -> Result<bool, String> {
    let parts: Vec<&str> = command.split_whitespace().collect();

    match parts[0] {
        ".exit" | ".quit" => {
            println!("Goodbye!");
            return Ok(true); // Signal to exit
        }
        ".help" => {
            print_help();
        }
        ".tables" => {
            print!("{}", db.list_tables());
        }
        ".schema" => {
            print!("{}", db.show_schema());
        }
        ".btree" => {
            if parts.len() < 2 {
                println!("Usage: .btree <table_name>");
            } else {
                db.print_btree(parts[1]);
            }
        }
        ".save" => match db.save_all_tables() {
            Ok(msg) => print!("{}", msg),
            Err(e) => println!("Error: {}", e),
        },
        ".load" => {
            if parts.len() < 2 {
                println!("Usage: .load <table_name>");
            } else {
                match db.load_table(parts[1]) {
                    Ok(msg) => println!("{}", msg),
                    Err(e) => println!("Error: {}", e),
                }
            }
        }
        _ => {
            println!("Unknown command: {}", parts[0]);
            println!("Type '.help' for help");
        }
    }

    Ok(false) // Don't exit
}

fn main() {
    let mut db = Database::new();
    let parser = Parser::new();

    print_welcome();

    loop {
        print!("sql> ");
        io::stdout().flush().unwrap();

        let mut input = String::new();
        match io::stdin().read_line(&mut input) {
            Ok(0) => break, // EOF
            Ok(_) => {}
            Err(e) => {
                eprintln!("Error reading input: {}", e);
                continue;
            }
        }

        let input = input.trim();

        // Skip empty input
        if input.is_empty() {
            continue;
        }

        // Handle meta commands
        if input.starts_with('.') {
            match handle_meta_command(&mut db, input) {
                Ok(should_exit) => {
                    if should_exit {
                        break;
                    }
                }
                Err(e) => println!("Error: {}", e),
            }
            continue;
        }

        // Parse and execute SQL
        match parser.parse(input) {
            Ok(statement) => match db.execute(statement) {
                Ok(result) => print!("{}", result),
                Err(e) => println!("Error: {}", e),
            },
            Err(e) => {
                println!("Parse error: {}", e);
                println!("Type '.help' for supported commands");
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_workflow() {
        let mut db = Database::new();
        let parser = Parser::new();

        // Create table
        let create_stmt = parser
            .parse("CREATE TABLE test (id INT, name TEXT)")
            .unwrap();
        assert!(db.execute(create_stmt).is_ok());

        // Insert data
        let insert_stmt = parser.parse("INSERT INTO test VALUES (1, 'Alice')").unwrap();
        assert!(db.execute(insert_stmt).is_ok());

        // Select data
        let select_stmt = parser.parse("SELECT * FROM test").unwrap();
        assert!(db.execute(select_stmt).is_ok());
    }
}

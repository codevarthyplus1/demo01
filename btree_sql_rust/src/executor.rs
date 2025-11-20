use crate::btree::BTree;
use crate::parser::Statement;
use crate::storage::{ColumnType, Table};
use std::collections::HashMap;

pub struct Database {
    tables: HashMap<String, Table>,
    indexes: HashMap<String, BTree>,
}

impl Database {
    pub fn new() -> Self {
        Database {
            tables: HashMap::new(),
            indexes: HashMap::new(),
        }
    }

    pub fn execute(&mut self, statement: Statement) -> Result<String, String> {
        match statement {
            Statement::CreateTable {
                table_name,
                columns,
            } => self.execute_create_table(table_name, columns),
            Statement::Insert { table_name, values } => self.execute_insert(table_name, values),
            Statement::Select {
                table_name,
                where_clause,
            } => self.execute_select(table_name, where_clause),
        }
    }

    fn execute_create_table(
        &mut self,
        table_name: String,
        columns: Vec<crate::storage::ColumnDef>,
    ) -> Result<String, String> {
        if self.tables.contains_key(&table_name) {
            return Err(format!("Table '{}' already exists", table_name));
        }

        let mut table = Table::new(table_name.clone());
        for col in columns {
            table.add_column(col.name, col.col_type);
        }

        // Create B-tree index for this table
        let index = BTree::new();

        self.tables.insert(table_name.clone(), table);
        self.indexes.insert(table_name.clone(), index);

        Ok(format!("Table '{}' created successfully", table_name))
    }

    fn execute_insert(
        &mut self,
        table_name: String,
        values: Vec<crate::storage::ColumnValue>,
    ) -> Result<String, String> {
        let table = self
            .tables
            .get_mut(&table_name)
            .ok_or(format!("Table '{}' not found", table_name))?;

        let row_id = table.insert_row(values.clone())?;

        // Insert into B-tree index (using first column as key)
        let key = if let Some(first_value) = values.first() {
            match first_value {
                crate::storage::ColumnValue::Int(v) => *v,
                crate::storage::ColumnValue::Text(_) => row_id, // Use row_id for text keys
            }
        } else {
            row_id
        };

        if let Some(index) = self.indexes.get_mut(&table_name) {
            index.insert(key, row_id);
        }

        Ok(format!("Row inserted successfully (row_id={})", row_id))
    }

    fn execute_select(
        &mut self,
        table_name: String,
        where_clause: Option<(String, i64)>,
    ) -> Result<String, String> {
        let table = self
            .tables
            .get(&table_name)
            .ok_or(format!("Table '{}' not found", table_name))?;

        let mut output = String::new();
        output.push('\n');

        // Print header
        for col in &table.columns {
            output.push_str(&format!("{:<15}", col.name));
        }
        output.push('\n');

        for _ in 0..table.columns.len() {
            output.push_str(&format!("{:<15}", "---------------"));
        }
        output.push('\n');

        // Print rows
        let mut count = 0;

        if let Some((col_name, value)) = where_clause {
            // Find column index
            let col_idx = table
                .columns
                .iter()
                .position(|c| c.name.to_uppercase() == col_name.to_uppercase())
                .ok_or(format!("Column '{}' not found", col_name))?;

            // For first column, use B-tree index
            if col_idx == 0 && table.columns[0].col_type == ColumnType::Int {
                if let Some(index) = self.indexes.get(&table_name) {
                    if let Some(row_id) = index.search(value) {
                        if let Some(row) = table.get_row(row_id) {
                            for val in &row.values {
                                output.push_str(&format!("{:<15}", val.to_string()));
                            }
                            output.push('\n');
                            count += 1;
                        }
                    }
                }
            } else {
                // Linear scan for other columns
                for row in &table.rows {
                    if let Some(val) = row.values.get(col_idx) {
                        if let Some(int_val) = val.as_int() {
                            if int_val == value {
                                for v in &row.values {
                                    output.push_str(&format!("{:<15}", v.to_string()));
                                }
                                output.push('\n');
                                count += 1;
                            }
                        }
                    }
                }
            }
        } else {
            // Select all
            for row in &table.rows {
                for val in &row.values {
                    output.push_str(&format!("{:<15}", val.to_string()));
                }
                output.push('\n');
                count += 1;
            }
        }

        output.push('\n');
        output.push_str(&format!("{} row(s) selected\n", count));

        Ok(output)
    }

    pub fn list_tables(&self) -> String {
        if self.tables.is_empty() {
            return "No tables in database\n".to_string();
        }

        let mut output = String::from("\nTables:\n");
        for (name, table) in &self.tables {
            output.push_str(&format!("  - {} ({} rows)\n", name, table.num_rows()));
        }
        output.push('\n');
        output
    }

    pub fn show_schema(&self) -> String {
        if self.tables.is_empty() {
            return "No tables in database\n".to_string();
        }

        let mut output = String::new();
        for (name, table) in &self.tables {
            output.push_str(&format!("\nTable: {}\n", name));
            output.push_str("Columns:\n");
            for col in &table.columns {
                output.push_str(&format!("  {}: {}\n", col.name, col.col_type.as_str()));
            }
        }
        output.push('\n');
        output
    }

    pub fn show_btrees(&self) -> String {
        if self.indexes.is_empty() {
            return "No indexes in database\n".to_string();
        }

        let mut output = String::new();
        for (name, index) in &self.indexes {
            output.push_str(&format!("\nB-Tree for table '{}':\n", name));
            // Note: We can't easily get the string representation, so we'll just print a message
            output.push_str("B-Tree structure (use print method for details)\n");
        }
        output.push('\n');
        output
    }

    pub fn print_btree(&self, table_name: &str) {
        if let Some(index) = self.indexes.get(table_name) {
            println!("\nB-Tree for table '{}':", table_name);
            index.print();
        } else {
            println!("Table '{}' not found", table_name);
        }
    }

    pub fn save_table(&self, table_name: &str) -> Result<String, String> {
        let table = self
            .tables
            .get(table_name)
            .ok_or(format!("Table '{}' not found", table_name))?;

        let filename = format!("data/{}.tbl", table_name);
        table
            .save_to_file(&filename)
            .map_err(|e| format!("Failed to save table: {}", e))?;

        Ok(format!("Table '{}' saved to {}", table_name, filename))
    }

    pub fn save_all_tables(&self) -> Result<String, String> {
        let mut output = String::new();
        for (name, table) in &self.tables {
            let filename = format!("data/{}.tbl", name);
            table
                .save_to_file(&filename)
                .map_err(|e| format!("Failed to save table '{}': {}", name, e))?;
            output.push_str(&format!("Table '{}' saved to {}\n", name, filename));
        }
        Ok(output)
    }

    pub fn load_table(&mut self, table_name: &str) -> Result<String, String> {
        if self.tables.contains_key(table_name) {
            return Err(format!("Table '{}' already exists in database", table_name));
        }

        let filename = format!("data/{}.tbl", table_name);
        let table = Table::load_from_file(&filename)
            .map_err(|e| format!("Failed to load table: {}", e))?;

        // Rebuild B-tree index
        let mut index = BTree::new();
        for row in &table.rows {
            let key = if !table.columns.is_empty() && table.columns[0].col_type == ColumnType::Int {
                row.values[0].as_int().unwrap_or(row.row_id)
            } else {
                row.row_id
            };
            index.insert(key, row.row_id);
        }

        self.tables.insert(table_name.to_string(), table);
        self.indexes.insert(table_name.to_string(), index);

        Ok(format!("Table '{}' loaded successfully", table_name))
    }

    pub fn get_table_names(&self) -> Vec<String> {
        self.tables.keys().cloned().collect()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::parser::Parser;

    #[test]
    fn test_create_and_insert() {
        let mut db = Database::new();
        let parser = Parser::new();

        let create_sql = "CREATE TABLE users (id INT, name TEXT)";
        let create_stmt = parser.parse(create_sql).unwrap();
        let result = db.execute(create_stmt);
        assert!(result.is_ok());

        let insert_sql = "INSERT INTO users VALUES (1, 'Alice')";
        let insert_stmt = parser.parse(insert_sql).unwrap();
        let result = db.execute(insert_stmt);
        assert!(result.is_ok());
    }

    #[test]
    fn test_select() {
        let mut db = Database::new();
        let parser = Parser::new();

        // Create and populate table
        db.execute(parser.parse("CREATE TABLE users (id INT, name TEXT)").unwrap())
            .unwrap();
        db.execute(parser.parse("INSERT INTO users VALUES (1, 'Alice')").unwrap())
            .unwrap();

        // Select all
        let select_stmt = parser.parse("SELECT * FROM users").unwrap();
        let result = db.execute(select_stmt);
        assert!(result.is_ok());
    }
}

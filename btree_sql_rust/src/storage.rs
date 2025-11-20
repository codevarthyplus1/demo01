use serde::{Deserialize, Serialize};
use std::fs::File;
use std::io::{self, Read, Write};

#[derive(Debug, Clone, PartialEq, Serialize, Deserialize)]
pub enum ColumnType {
    Int,
    Text,
}

impl ColumnType {
    pub fn from_str(s: &str) -> Option<Self> {
        match s.to_uppercase().as_str() {
            "INT" | "INTEGER" => Some(ColumnType::Int),
            "TEXT" | "VARCHAR" => Some(ColumnType::Text),
            _ => None,
        }
    }

    pub fn as_str(&self) -> &str {
        match self {
            ColumnType::Int => "INT",
            ColumnType::Text => "TEXT",
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ColumnDef {
    pub name: String,
    pub col_type: ColumnType,
}

impl ColumnDef {
    pub fn new(name: String, col_type: ColumnType) -> Self {
        ColumnDef { name, col_type }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ColumnValue {
    Int(i64),
    Text(String),
}

impl ColumnValue {
    pub fn get_type(&self) -> ColumnType {
        match self {
            ColumnValue::Int(_) => ColumnType::Int,
            ColumnValue::Text(_) => ColumnType::Text,
        }
    }

    pub fn as_int(&self) -> Option<i64> {
        match self {
            ColumnValue::Int(v) => Some(*v),
            _ => None,
        }
    }

    pub fn as_text(&self) -> Option<&str> {
        match self {
            ColumnValue::Text(v) => Some(v),
            _ => None,
        }
    }

    pub fn to_string(&self) -> String {
        match self {
            ColumnValue::Int(v) => v.to_string(),
            ColumnValue::Text(v) => v.clone(),
        }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Row {
    pub row_id: i64,
    pub values: Vec<ColumnValue>,
}

impl Row {
    pub fn new(row_id: i64, values: Vec<ColumnValue>) -> Self {
        Row { row_id, values }
    }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Table {
    pub name: String,
    pub columns: Vec<ColumnDef>,
    pub rows: Vec<Row>,
    next_row_id: i64,
}

impl Table {
    pub fn new(name: String) -> Self {
        Table {
            name,
            columns: Vec::new(),
            rows: Vec::new(),
            next_row_id: 1,
        }
    }

    pub fn add_column(&mut self, name: String, col_type: ColumnType) {
        self.columns.push(ColumnDef::new(name, col_type));
    }

    pub fn insert_row(&mut self, values: Vec<ColumnValue>) -> Result<i64, String> {
        if values.len() != self.columns.len() {
            return Err(format!(
                "Column count mismatch: expected {}, got {}",
                self.columns.len(),
                values.len()
            ));
        }

        // Type checking
        for (i, value) in values.iter().enumerate() {
            if value.get_type() != self.columns[i].col_type {
                return Err(format!(
                    "Type mismatch for column '{}': expected {}, got {}",
                    self.columns[i].name,
                    self.columns[i].col_type.as_str(),
                    value.get_type().as_str()
                ));
            }
        }

        let row_id = self.next_row_id;
        self.next_row_id += 1;

        self.rows.push(Row::new(row_id, values));
        Ok(row_id)
    }

    pub fn get_row(&self, row_id: i64) -> Option<&Row> {
        self.rows.iter().find(|r| r.row_id == row_id)
    }

    pub fn print(&self) {
        println!("\nTable: {}", self.name);

        // Print header
        for col in &self.columns {
            print!("{:<15}", col.name);
        }
        println!();

        for _ in 0..self.columns.len() {
            print!("{:<15}", "---------------");
        }
        println!();

        // Print rows
        for row in &self.rows {
            for value in &row.values {
                print!("{:<15}", value.to_string());
            }
            println!();
        }

        println!("\n{} row(s) in table", self.rows.len());
    }

    pub fn save_to_file(&self, filename: &str) -> io::Result<()> {
        let encoded = bincode::serialize(self)
            .map_err(|e| io::Error::new(io::ErrorKind::Other, e))?;

        let mut file = File::create(filename)?;
        file.write_all(&encoded)?;
        Ok(())
    }

    pub fn load_from_file(filename: &str) -> io::Result<Self> {
        let mut file = File::open(filename)?;
        let mut buffer = Vec::new();
        file.read_to_end(&mut buffer)?;

        let table = bincode::deserialize(&buffer)
            .map_err(|e| io::Error::new(io::ErrorKind::Other, e))?;

        Ok(table)
    }

    pub fn num_rows(&self) -> usize {
        self.rows.len()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_table_creation() {
        let mut table = Table::new("users".to_string());
        table.add_column("id".to_string(), ColumnType::Int);
        table.add_column("name".to_string(), ColumnType::Text);

        assert_eq!(table.columns.len(), 2);
        assert_eq!(table.name, "users");
    }

    #[test]
    fn test_insert_row() {
        let mut table = Table::new("users".to_string());
        table.add_column("id".to_string(), ColumnType::Int);
        table.add_column("name".to_string(), ColumnType::Text);

        let values = vec![
            ColumnValue::Int(1),
            ColumnValue::Text("Alice".to_string()),
        ];

        let result = table.insert_row(values);
        assert!(result.is_ok());
        assert_eq!(table.rows.len(), 1);
    }

    #[test]
    fn test_type_mismatch() {
        let mut table = Table::new("users".to_string());
        table.add_column("id".to_string(), ColumnType::Int);

        let values = vec![ColumnValue::Text("not_an_int".to_string())];
        let result = table.insert_row(values);
        assert!(result.is_err());
    }
}

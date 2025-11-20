use crate::storage::{ColumnDef, ColumnType, ColumnValue};

#[derive(Debug, Clone)]
pub enum Statement {
    CreateTable {
        table_name: String,
        columns: Vec<ColumnDef>,
    },
    Insert {
        table_name: String,
        values: Vec<ColumnValue>,
    },
    Select {
        table_name: String,
        where_clause: Option<(String, i64)>, // (column_name, value)
    },
}

pub struct Parser;

impl Parser {
    pub fn new() -> Self {
        Parser
    }

    pub fn parse(&self, sql: &str) -> Result<Statement, String> {
        let sql = sql.trim();
        let tokens: Vec<&str> = sql.split_whitespace().collect();

        if tokens.is_empty() {
            return Err("Empty SQL statement".to_string());
        }

        match tokens[0].to_uppercase().as_str() {
            "CREATE" => self.parse_create_table(sql),
            "INSERT" => self.parse_insert(sql),
            "SELECT" => self.parse_select(sql),
            _ => Err(format!("Unknown statement type: {}", tokens[0])),
        }
    }

    fn parse_create_table(&self, sql: &str) -> Result<Statement, String> {
        let sql = sql.to_uppercase();
        if !sql.starts_with("CREATE TABLE") {
            return Err("Invalid CREATE TABLE statement".to_string());
        }

        // Find table name
        let after_table = sql.strip_prefix("CREATE TABLE").unwrap().trim();
        let paren_pos = after_table
            .find('(')
            .ok_or("Missing opening parenthesis")?;
        let table_name = after_table[..paren_pos].trim().to_string();

        // Extract column definitions
        let close_paren = after_table
            .rfind(')')
            .ok_or("Missing closing parenthesis")?;
        let col_def_str = &after_table[paren_pos + 1..close_paren];

        let mut columns = Vec::new();
        for col_def in col_def_str.split(',') {
            let parts: Vec<&str> = col_def.trim().split_whitespace().collect();
            if parts.len() < 2 {
                return Err("Invalid column definition".to_string());
            }

            let col_name = parts[0].to_string();
            let col_type_str = parts[1];

            // Handle VARCHAR(N) case
            let col_type_base = if col_type_str.starts_with("VARCHAR") {
                "VARCHAR"
            } else {
                col_type_str
            };

            let col_type = ColumnType::from_str(col_type_base)
                .ok_or(format!("Unknown column type: {}", col_type_str))?;

            columns.push(ColumnDef::new(col_name, col_type));
        }

        Ok(Statement::CreateTable {
            table_name: table_name.to_lowercase(),
            columns,
        })
    }

    fn parse_insert(&self, sql: &str) -> Result<Statement, String> {
        let sql_upper = sql.to_uppercase();
        if !sql_upper.starts_with("INSERT INTO") {
            return Err("Invalid INSERT statement".to_string());
        }

        // Find table name
        let after_into = sql_upper.strip_prefix("INSERT INTO").unwrap().trim();
        let values_pos = after_into
            .find("VALUES")
            .ok_or("Missing VALUES clause")?;
        let table_name = after_into[..values_pos].trim().to_string();

        // Extract values
        let values_part = &sql[sql_upper.find("VALUES").unwrap() + 6..];
        let paren_start = values_part
            .find('(')
            .ok_or("Missing opening parenthesis in VALUES")?;
        let paren_end = values_part
            .rfind(')')
            .ok_or("Missing closing parenthesis in VALUES")?;

        let values_str = &values_part[paren_start + 1..paren_end];
        let mut values = Vec::new();

        let mut in_string = false;
        let mut current_value = String::new();

        for ch in values_str.chars() {
            match ch {
                '\'' => {
                    if in_string {
                        // End of string
                        values.push(ColumnValue::Text(current_value.clone()));
                        current_value.clear();
                        in_string = false;
                    } else {
                        // Start of string
                        in_string = true;
                    }
                }
                ',' if !in_string => {
                    // Value separator
                    let trimmed = current_value.trim();
                    if !trimmed.is_empty() {
                        // Try to parse as integer
                        if let Ok(num) = trimmed.parse::<i64>() {
                            values.push(ColumnValue::Int(num));
                        }
                    }
                    current_value.clear();
                }
                _ => {
                    current_value.push(ch);
                }
            }
        }

        // Handle last value
        let trimmed = current_value.trim();
        if !trimmed.is_empty() {
            if let Ok(num) = trimmed.parse::<i64>() {
                values.push(ColumnValue::Int(num));
            }
        }

        Ok(Statement::Insert {
            table_name: table_name.to_lowercase(),
            values,
        })
    }

    fn parse_select(&self, sql: &str) -> Result<Statement, String> {
        let sql_upper = sql.to_uppercase();
        if !sql_upper.starts_with("SELECT") {
            return Err("Invalid SELECT statement".to_string());
        }

        // Find FROM clause
        let from_pos = sql_upper.find("FROM").ok_or("Missing FROM clause")?;
        let after_from = sql_upper[from_pos + 4..].trim();

        // Check for WHERE clause
        let where_clause = if let Some(where_pos) = after_from.find("WHERE") {
            let table_name = after_from[..where_pos].trim().to_string();
            let where_part = &sql[sql_upper.find("WHERE").unwrap() + 5..];

            // Parse simple WHERE col = value
            let parts: Vec<&str> = where_part.split('=').collect();
            if parts.len() != 2 {
                return Err("Invalid WHERE clause".to_string());
            }

            let col_name = parts[0].trim().to_string();
            let value = parts[1]
                .trim()
                .parse::<i64>()
                .map_err(|_| "WHERE value must be an integer")?;

            (table_name.to_lowercase(), Some((col_name, value)))
        } else {
            (after_from.trim().to_string().to_lowercase(), None)
        };

        Ok(Statement::Select {
            table_name: where_clause.0,
            where_clause: where_clause.1,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_create_table() {
        let parser = Parser::new();
        let sql = "CREATE TABLE users (id INT, name TEXT)";
        let result = parser.parse(sql);

        assert!(result.is_ok());
        match result.unwrap() {
            Statement::CreateTable { table_name, columns } => {
                assert_eq!(table_name, "users");
                assert_eq!(columns.len(), 2);
            }
            _ => panic!("Expected CreateTable statement"),
        }
    }

    #[test]
    fn test_parse_insert() {
        let parser = Parser::new();
        let sql = "INSERT INTO users VALUES (1, 'Alice')";
        let result = parser.parse(sql);

        assert!(result.is_ok());
        match result.unwrap() {
            Statement::Insert { table_name, values } => {
                assert_eq!(table_name, "users");
                assert_eq!(values.len(), 2);
            }
            _ => panic!("Expected Insert statement"),
        }
    }

    #[test]
    fn test_parse_select() {
        let parser = Parser::new();
        let sql = "SELECT * FROM users";
        let result = parser.parse(sql);

        assert!(result.is_ok());
        match result.unwrap() {
            Statement::Select {
                table_name,
                where_clause,
            } => {
                assert_eq!(table_name, "users");
                assert!(where_clause.is_none());
            }
            _ => panic!("Expected Select statement"),
        }
    }

    #[test]
    fn test_parse_select_with_where() {
        let parser = Parser::new();
        let sql = "SELECT * FROM users WHERE id = 1";
        let result = parser.parse(sql);

        assert!(result.is_ok());
        match result.unwrap() {
            Statement::Select {
                table_name,
                where_clause,
            } => {
                assert_eq!(table_name, "users");
                assert!(where_clause.is_some());
            }
            _ => panic!("Expected Select statement"),
        }
    }
}

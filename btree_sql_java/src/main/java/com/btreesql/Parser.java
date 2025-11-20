package com.btreesql;

/**
 * SQL Parser
 */
public class Parser {

    public Statement parse(String sql) throws Exception {
        sql = sql.trim();
        String sqlUpper = sql.toUpperCase();

        if (sqlUpper.startsWith("CREATE")) {
            return parseCreateTable(sql);
        } else if (sqlUpper.startsWith("INSERT")) {
            return parseInsert(sql);
        } else if (sqlUpper.startsWith("SELECT")) {
            return parseSelect(sql);
        } else {
            throw new Exception("Unknown statement type");
        }
    }

    private Statement parseCreateTable(String sql) throws Exception {
        String sqlUpper = sql.toUpperCase();
        if (!sqlUpper.startsWith("CREATE TABLE")) {
            throw new Exception("Invalid CREATE TABLE statement");
        }

        // Find table name
        String afterTable = sql.substring("CREATE TABLE".length()).trim();
        int parenPos = afterTable.indexOf('(');
        if (parenPos == -1) {
            throw new Exception("Missing opening parenthesis");
        }

        String tableName = afterTable.substring(0, parenPos).trim().toLowerCase();
        CreateTableStatement stmt = new CreateTableStatement(tableName);

        // Extract column definitions
        int closeParenPos = afterTable.lastIndexOf(')');
        if (closeParenPos == -1) {
            throw new Exception("Missing closing parenthesis");
        }

        String colDefStr = afterTable.substring(parenPos + 1, closeParenPos);
        String[] colDefs = colDefStr.split(",");

        for (String colDef : colDefs) {
            String[] parts = colDef.trim().split("\\s+");
            if (parts.length < 2) {
                throw new Exception("Invalid column definition");
            }

            String colName = parts[0].toUpperCase();
            String colTypeStr = parts[1];

            ColumnType colType = ColumnType.fromString(colTypeStr);
            if (colType == null) {
                throw new Exception("Unknown column type: " + colTypeStr);
            }

            stmt.addColumn(new ColumnDef(colName, colType));
        }

        return stmt;
    }

    private Statement parseInsert(String sql) throws Exception {
        String sqlUpper = sql.toUpperCase();
        if (!sqlUpper.startsWith("INSERT INTO")) {
            throw new Exception("Invalid INSERT statement");
        }

        // Find table name
        String afterInto = sql.substring("INSERT INTO".length()).trim();
        int valuesPos = afterInto.toUpperCase().indexOf("VALUES");
        if (valuesPos == -1) {
            throw new Exception("Missing VALUES clause");
        }

        String tableName = afterInto.substring(0, valuesPos).trim().toLowerCase();
        InsertStatement stmt = new InsertStatement(tableName);

        // Extract values
        String valuesPart = afterInto.substring(valuesPos + 6).trim();
        int parenStart = valuesPart.indexOf('(');
        int parenEnd = valuesPart.lastIndexOf(')');

        if (parenStart == -1 || parenEnd == -1) {
            throw new Exception("Missing parentheses in VALUES");
        }

        String valuesStr = valuesPart.substring(parenStart + 1, parenEnd);

        // Parse values (handle quoted strings)
        boolean inString = false;
        StringBuilder currentValue = new StringBuilder();

        for (char ch : valuesStr.toCharArray()) {
            if (ch == '\'') {
                if (inString) {
                    // End of string
                    stmt.addValue(ColumnValue.ofText(currentValue.toString()));
                    currentValue = new StringBuilder();
                    inString = false;
                } else {
                    // Start of string
                    inString = true;
                }
            } else if (ch == ',' && !inString) {
                // Value separator
                String trimmed = currentValue.toString().trim();
                if (!trimmed.isEmpty()) {
                    try {
                        long num = Long.parseLong(trimmed);
                        stmt.addValue(ColumnValue.ofInt(num));
                    } catch (NumberFormatException e) {
                        // Skip if not a number
                    }
                }
                currentValue = new StringBuilder();
            } else {
                currentValue.append(ch);
            }
        }

        // Handle last value
        String trimmed = currentValue.toString().trim();
        if (!trimmed.isEmpty()) {
            try {
                long num = Long.parseLong(trimmed);
                stmt.addValue(ColumnValue.ofInt(num));
            } catch (NumberFormatException e) {
                // Already handled as string
            }
        }

        return stmt;
    }

    private Statement parseSelect(String sql) throws Exception {
        String sqlUpper = sql.toUpperCase();
        if (!sqlUpper.startsWith("SELECT")) {
            throw new Exception("Invalid SELECT statement");
        }

        // Find FROM clause
        int fromPos = sqlUpper.indexOf("FROM");
        if (fromPos == -1) {
            throw new Exception("Missing FROM clause");
        }

        String afterFrom = sql.substring(fromPos + 4).trim();

        // Check for WHERE clause
        int wherePos = afterFrom.toUpperCase().indexOf("WHERE");
        String tableName;

        if (wherePos != -1) {
            tableName = afterFrom.substring(0, wherePos).trim().toLowerCase();
            SelectStatement stmt = new SelectStatement(tableName);

            String wherePart = afterFrom.substring(wherePos + 5).trim();
            String[] parts = wherePart.split("=");

            if (parts.length == 2) {
                String column = parts[0].trim().toUpperCase();
                long value = Long.parseLong(parts[1].trim());
                stmt.setWhere(column, value);
            }

            return stmt;
        } else {
            tableName = afterFrom.trim().toLowerCase();
            return new SelectStatement(tableName);
        }
    }
}

package com.btreesql;

import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Database engine
 */
public class Database {
    private Map<String, Table> tables;
    private Map<String, BTree> indexes;

    public Database() {
        this.tables = new HashMap<>();
        this.indexes = new HashMap<>();
    }

    public String execute(Statement statement) throws Exception {
        switch (statement.getType()) {
            case CREATE_TABLE:
                return executeCreateTable((CreateTableStatement) statement);
            case INSERT:
                return executeInsert((InsertStatement) statement);
            case SELECT:
                return executeSelect((SelectStatement) statement);
            default:
                throw new Exception("Unknown statement type");
        }
    }

    private String executeCreateTable(CreateTableStatement stmt) throws Exception {
        String tableName = stmt.getTableName();

        if (tables.containsKey(tableName)) {
            throw new Exception("Table '" + tableName + "' already exists");
        }

        Table table = new Table(tableName);
        for (ColumnDef col : stmt.getColumns()) {
            table.addColumn(col.getName(), col.getType());
        }

        // Create B-tree index
        BTree index = new BTree();

        tables.put(tableName, table);
        indexes.put(tableName, index);

        return "Table '" + tableName + "' created successfully";
    }

    private String executeInsert(InsertStatement stmt) throws Exception {
        String tableName = stmt.getTableName();
        Table table = tables.get(tableName);

        if (table == null) {
            throw new Exception("Table '" + tableName + "' not found");
        }

        long rowId = table.insertRow(stmt.getValues());

        // Insert into B-tree index (using first column as key)
        List<ColumnValue> values = stmt.getValues();
        long key;

        if (!values.isEmpty() && values.get(0).getType() == ColumnType.INT) {
            key = values.get(0).asInt();
        } else {
            key = rowId;
        }

        BTree index = indexes.get(tableName);
        if (index != null) {
            index.insert(key, rowId);
        }

        return "Row inserted successfully (row_id=" + rowId + ")";
    }

    private String executeSelect(SelectStatement stmt) throws Exception {
        String tableName = stmt.getTableName();
        Table table = tables.get(tableName);

        if (table == null) {
            throw new Exception("Table '" + tableName + "' not found");
        }

        StringBuilder output = new StringBuilder("\n");

        // Print header
        for (ColumnDef col : table.getColumns()) {
            output.append(String.format("%-15s", col.getName()));
        }
        output.append("\n");

        for (int i = 0; i < table.getColumns().size(); i++) {
            output.append(String.format("%-15s", "---------------"));
        }
        output.append("\n");

        int count = 0;

        if (stmt.hasWhere()) {
            // Find column index
            String whereColumn = stmt.getWhereColumn();
            int colIdx = -1;

            for (int i = 0; i < table.getColumns().size(); i++) {
                if (table.getColumns().get(i).getName().equalsIgnoreCase(whereColumn)) {
                    colIdx = i;
                    break;
                }
            }

            if (colIdx == -1) {
                throw new Exception("Column '" + whereColumn + "' not found");
            }

            // For first column, use B-tree index
            if (colIdx == 0 && table.getColumns().get(0).getType() == ColumnType.INT) {
                BTree index = indexes.get(tableName);
                Long rowId = index.search(stmt.getWhereValue());

                if (rowId != null) {
                    Row row = table.getRow(rowId);
                    if (row != null) {
                        for (ColumnValue value : row.getValues()) {
                            output.append(String.format("%-15s", value.toString()));
                        }
                        output.append("\n");
                        count++;
                    }
                }
            } else {
                // Linear scan for other columns
                for (Row row : table.getRows()) {
                    ColumnValue value = row.getValue(colIdx);
                    if (value.getType() == ColumnType.INT &&
                        value.asInt().equals(stmt.getWhereValue())) {
                        for (ColumnValue v : row.getValues()) {
                            output.append(String.format("%-15s", v.toString()));
                        }
                        output.append("\n");
                        count++;
                    }
                }
            }
        } else {
            // Select all rows
            for (Row row : table.getRows()) {
                for (ColumnValue value : row.getValues()) {
                    output.append(String.format("%-15s", value.toString()));
                }
                output.append("\n");
                count++;
            }
        }

        output.append("\n").append(count).append(" row(s) selected\n");
        return output.toString();
    }

    public String listTables() {
        if (tables.isEmpty()) {
            return "No tables in database\n";
        }

        StringBuilder output = new StringBuilder("\nTables:\n");
        for (Map.Entry<String, Table> entry : tables.entrySet()) {
            output.append(String.format("  - %s (%d rows)\n",
                entry.getKey(), entry.getValue().getRowCount()));
        }
        output.append("\n");
        return output.toString();
    }

    public String showSchema() {
        if (tables.isEmpty()) {
            return "No tables in database\n";
        }

        StringBuilder output = new StringBuilder();
        for (Map.Entry<String, Table> entry : tables.entrySet()) {
            output.append("\nTable: ").append(entry.getKey()).append("\n");
            output.append("Columns:\n");
            for (ColumnDef col : entry.getValue().getColumns()) {
                output.append("  ").append(col.toString()).append("\n");
            }
        }
        output.append("\n");
        return output.toString();
    }

    public void printBTree(String tableName) {
        BTree index = indexes.get(tableName);
        if (index != null) {
            System.out.println("\nB-Tree for table '" + tableName + "':");
            index.print();
        } else {
            System.out.println("Table '" + tableName + "' not found");
        }
    }

    public String saveAllTables() {
        StringBuilder output = new StringBuilder();
        for (Map.Entry<String, Table> entry : tables.entrySet()) {
            String filename = "data/" + entry.getKey() + ".ser";
            try {
                entry.getValue().saveToFile(filename);
                output.append("Table '").append(entry.getKey())
                      .append("' saved to ").append(filename).append("\n");
            } catch (IOException e) {
                output.append("Failed to save table '").append(entry.getKey())
                      .append("': ").append(e.getMessage()).append("\n");
            }
        }
        return output.toString();
    }

    public String loadTable(String tableName) {
        if (tables.containsKey(tableName)) {
            return "Table '" + tableName + "' already exists in database";
        }

        String filename = "data/" + tableName + ".ser";
        try {
            Table table = Table.loadFromFile(filename);

            // Rebuild B-tree index
            BTree index = new BTree();
            for (Row row : table.getRows()) {
                long key;
                if (!table.getColumns().isEmpty() &&
                    table.getColumns().get(0).getType() == ColumnType.INT) {
                    key = row.getValue(0).asInt();
                } else {
                    key = row.getRowId();
                }
                index.insert(key, row.getRowId());
            }

            tables.put(tableName, table);
            indexes.put(tableName, index);

            return "Table '" + tableName + "' loaded successfully";
        } catch (IOException | ClassNotFoundException e) {
            return "Failed to load table '" + tableName + "': " + e.getMessage();
        }
    }
}

package com.btreesql;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

/**
 * Database table
 */
public class Table implements Serializable {
    private static final long serialVersionUID = 1L;

    private String name;
    private List<ColumnDef> columns;
    private List<Row> rows;
    private long nextRowId;

    public Table(String name) {
        this.name = name;
        this.columns = new ArrayList<>();
        this.rows = new ArrayList<>();
        this.nextRowId = 1;
    }

    public String getName() {
        return name;
    }

    public List<ColumnDef> getColumns() {
        return columns;
    }

    public List<Row> getRows() {
        return rows;
    }

    public void addColumn(String name, ColumnType type) {
        columns.add(new ColumnDef(name, type));
    }

    public long insertRow(List<ColumnValue> values) throws Exception {
        if (values.size() != columns.size()) {
            throw new Exception(String.format(
                "Column count mismatch: expected %d, got %d",
                columns.size(), values.size()
            ));
        }

        // Type checking
        for (int i = 0; i < values.size(); i++) {
            if (values.get(i).getType() != columns.get(i).getType()) {
                throw new Exception(String.format(
                    "Type mismatch for column '%s': expected %s, got %s",
                    columns.get(i).getName(),
                    columns.get(i).getType(),
                    values.get(i).getType()
                ));
            }
        }

        long rowId = nextRowId++;
        rows.add(new Row(rowId, values));
        return rowId;
    }

    public Row getRow(long rowId) {
        for (Row row : rows) {
            if (row.getRowId() == rowId) {
                return row;
            }
        }
        return null;
    }

    public int getRowCount() {
        return rows.size();
    }

    public void print() {
        System.out.println("\nTable: " + name);

        // Print header
        for (ColumnDef col : columns) {
            System.out.printf("%-15s", col.getName());
        }
        System.out.println();

        for (int i = 0; i < columns.size(); i++) {
            System.out.printf("%-15s", "---------------");
        }
        System.out.println();

        // Print rows
        for (Row row : rows) {
            for (ColumnValue value : row.getValues()) {
                System.out.printf("%-15s", value.toString());
            }
            System.out.println();
        }

        System.out.println("\n" + rows.size() + " row(s) in table\n");
    }

    public void saveToFile(String filename) throws IOException {
        try (ObjectOutputStream oos = new ObjectOutputStream(new FileOutputStream(filename))) {
            oos.writeObject(this);
        }
    }

    public static Table loadFromFile(String filename) throws IOException, ClassNotFoundException {
        try (ObjectInputStream ois = new ObjectInputStream(new FileInputStream(filename))) {
            return (Table) ois.readObject();
        }
    }
}

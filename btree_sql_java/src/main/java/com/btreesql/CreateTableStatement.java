package com.btreesql;

import java.util.ArrayList;
import java.util.List;

/**
 * CREATE TABLE statement
 */
public class CreateTableStatement implements Statement {
    private String tableName;
    private List<ColumnDef> columns;

    public CreateTableStatement(String tableName) {
        this.tableName = tableName;
        this.columns = new ArrayList<>();
    }

    @Override
    public StatementType getType() {
        return StatementType.CREATE_TABLE;
    }

    public String getTableName() {
        return tableName;
    }

    public List<ColumnDef> getColumns() {
        return columns;
    }

    public void addColumn(ColumnDef column) {
        columns.add(column);
    }
}

package com.btreesql;

import java.util.ArrayList;
import java.util.List;

/**
 * INSERT statement
 */
public class InsertStatement implements Statement {
    private String tableName;
    private List<ColumnValue> values;

    public InsertStatement(String tableName) {
        this.tableName = tableName;
        this.values = new ArrayList<>();
    }

    @Override
    public StatementType getType() {
        return StatementType.INSERT;
    }

    public String getTableName() {
        return tableName;
    }

    public List<ColumnValue> getValues() {
        return values;
    }

    public void addValue(ColumnValue value) {
        values.add(value);
    }
}

package com.btreesql;

/**
 * SELECT statement
 */
public class SelectStatement implements Statement {
    private String tableName;
    private boolean selectAll;
    private String whereColumn;
    private Long whereValue;

    public SelectStatement(String tableName) {
        this.tableName = tableName;
        this.selectAll = true;
        this.whereColumn = null;
        this.whereValue = null;
    }

    @Override
    public StatementType getType() {
        return StatementType.SELECT;
    }

    public String getTableName() {
        return tableName;
    }

    public boolean isSelectAll() {
        return selectAll;
    }

    public boolean hasWhere() {
        return whereColumn != null;
    }

    public String getWhereColumn() {
        return whereColumn;
    }

    public Long getWhereValue() {
        return whereValue;
    }

    public void setWhere(String column, long value) {
        this.whereColumn = column;
        this.whereValue = value;
    }
}

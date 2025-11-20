package com.btreesql;

import java.io.Serializable;

/**
 * Column definition
 */
public class ColumnDef implements Serializable {
    private static final long serialVersionUID = 1L;

    private String name;
    private ColumnType type;

    public ColumnDef(String name, ColumnType type) {
        this.name = name;
        this.type = type;
    }

    public String getName() {
        return name;
    }

    public ColumnType getType() {
        return type;
    }

    @Override
    public String toString() {
        return name + ": " + type;
    }
}

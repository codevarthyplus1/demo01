package com.btreesql;

/**
 * Enumeration of supported column types
 */
public enum ColumnType {
    INT,
    TEXT;

    public static ColumnType fromString(String s) {
        String upper = s.toUpperCase();
        if (upper.equals("INT") || upper.equals("INTEGER")) {
            return INT;
        } else if (upper.equals("TEXT") || upper.startsWith("VARCHAR")) {
            return TEXT;
        }
        return null;
    }

    @Override
    public String toString() {
        return this.name();
    }
}

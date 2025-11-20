package com.btreesql;

import java.io.Serializable;

/**
 * Column value that can hold INT or TEXT
 */
public class ColumnValue implements Serializable {
    private static final long serialVersionUID = 1L;

    private ColumnType type;
    private Long intValue;
    private String textValue;

    private ColumnValue(ColumnType type, Long intValue, String textValue) {
        this.type = type;
        this.intValue = intValue;
        this.textValue = textValue;
    }

    public static ColumnValue ofInt(long value) {
        return new ColumnValue(ColumnType.INT, value, null);
    }

    public static ColumnValue ofText(String value) {
        return new ColumnValue(ColumnType.TEXT, null, value);
    }

    public ColumnType getType() {
        return type;
    }

    public Long asInt() {
        return intValue;
    }

    public String asText() {
        return textValue;
    }

    @Override
    public String toString() {
        if (type == ColumnType.INT) {
            return String.valueOf(intValue);
        } else {
            return textValue;
        }
    }
}

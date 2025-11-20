package com.btreesql;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

/**
 * Database row
 */
public class Row implements Serializable {
    private static final long serialVersionUID = 1L;

    private long rowId;
    private List<ColumnValue> values;

    public Row(long rowId, List<ColumnValue> values) {
        this.rowId = rowId;
        this.values = new ArrayList<>(values);
    }

    public long getRowId() {
        return rowId;
    }

    public List<ColumnValue> getValues() {
        return values;
    }

    public ColumnValue getValue(int index) {
        return values.get(index);
    }
}

package com.btreesql;

/**
 * Base interface for SQL statements
 */
public interface Statement {
    StatementType getType();

    enum StatementType {
        CREATE_TABLE,
        INSERT,
        SELECT
    }
}

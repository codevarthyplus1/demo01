#include "../include/parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Helper function to skip whitespace
static const char* skip_whitespace(const char *str) {
    while (*str && isspace(*str)) str++;
    return str;
}

// Helper function to parse identifier (table/column names)
static const char* parse_identifier(const char *str, char *out, int max_len) {
    str = skip_whitespace(str);
    int i = 0;
    while (*str && (isalnum(*str) || *str == '_') && i < max_len - 1) {
        out[i++] = *str++;
    }
    out[i] = '\0';
    return str;
}

// Helper function to parse string literal
static const char* parse_string(const char *str, char *out, int max_len) {
    str = skip_whitespace(str);
    if (*str != '\'') return str;
    str++; // skip opening quote

    int i = 0;
    while (*str && *str != '\'' && i < max_len - 1) {
        out[i++] = *str++;
    }
    out[i] = '\0';

    if (*str == '\'') str++; // skip closing quote
    return str;
}

// Helper function to parse integer
static const char* parse_integer(const char *str, int64_t *out) {
    str = skip_whitespace(str);
    char *end;
    *out = strtoll(str, &end, 10);
    return end;
}

// Parse CREATE TABLE statement
static bool parse_create_table(const char *sql, ParsedStatement *stmt) {
    sql = skip_whitespace(sql);

    // Skip "CREATE TABLE"
    if (strncasecmp(sql, "CREATE", 6) != 0) return false;
    sql += 6;
    sql = skip_whitespace(sql);
    if (strncasecmp(sql, "TABLE", 5) != 0) return false;
    sql += 5;

    // Parse table name
    sql = parse_identifier(sql, stmt->statement.create_table.table_name, MAX_COLUMN_NAME_LEN);
    stmt->statement.create_table.num_columns = 0;

    // Expect opening parenthesis
    sql = skip_whitespace(sql);
    if (*sql != '(') return false;
    sql++;

    // Parse columns
    while (1) {
        sql = skip_whitespace(sql);
        if (*sql == ')') break;

        int col_idx = stmt->statement.create_table.num_columns;
        if (col_idx >= MAX_COLUMNS) return false;

        // Parse column name
        sql = parse_identifier(sql, stmt->statement.create_table.columns[col_idx].name, MAX_COLUMN_NAME_LEN);

        // Parse column type
        sql = skip_whitespace(sql);
        if (strncasecmp(sql, "INT", 3) == 0 || strncasecmp(sql, "INTEGER", 7) == 0) {
            stmt->statement.create_table.columns[col_idx].type = COL_TYPE_INT;
            sql += (strncasecmp(sql, "INTEGER", 7) == 0) ? 7 : 3;
        } else if (strncasecmp(sql, "TEXT", 4) == 0 || strncasecmp(sql, "VARCHAR", 7) == 0) {
            stmt->statement.create_table.columns[col_idx].type = COL_TYPE_TEXT;
            sql += (strncasecmp(sql, "VARCHAR", 7) == 0) ? 7 : 4;
            // Skip optional (N) for VARCHAR
            sql = skip_whitespace(sql);
            if (*sql == '(') {
                while (*sql && *sql != ')') sql++;
                if (*sql == ')') sql++;
            }
        } else {
            return false;
        }

        stmt->statement.create_table.num_columns++;

        sql = skip_whitespace(sql);
        if (*sql == ',') {
            sql++;
        } else if (*sql == ')') {
            break;
        } else {
            return false;
        }
    }

    return true;
}

// Parse INSERT statement
static bool parse_insert(const char *sql, ParsedStatement *stmt) {
    sql = skip_whitespace(sql);

    // Skip "INSERT INTO"
    if (strncasecmp(sql, "INSERT", 6) != 0) return false;
    sql += 6;
    sql = skip_whitespace(sql);
    if (strncasecmp(sql, "INTO", 4) != 0) return false;
    sql += 4;

    // Parse table name
    sql = parse_identifier(sql, stmt->statement.insert.table_name, MAX_COLUMN_NAME_LEN);

    // Skip "VALUES"
    sql = skip_whitespace(sql);
    if (strncasecmp(sql, "VALUES", 6) != 0) return false;
    sql += 6;

    // Expect opening parenthesis
    sql = skip_whitespace(sql);
    if (*sql != '(') return false;
    sql++;

    // Parse values
    stmt->statement.insert.num_values = 0;
    while (1) {
        sql = skip_whitespace(sql);
        if (*sql == ')') break;

        int val_idx = stmt->statement.insert.num_values;
        if (val_idx >= MAX_COLUMNS) return false;

        // Check if it's a string or integer
        if (*sql == '\'') {
            stmt->statement.insert.values[val_idx].type = COL_TYPE_TEXT;
            sql = parse_string(sql, stmt->statement.insert.values[val_idx].data.text_val, MAX_STRING_LEN);
        } else if (isdigit(*sql) || *sql == '-') {
            stmt->statement.insert.values[val_idx].type = COL_TYPE_INT;
            sql = parse_integer(sql, &stmt->statement.insert.values[val_idx].data.int_val);
        } else {
            return false;
        }

        stmt->statement.insert.num_values++;

        sql = skip_whitespace(sql);
        if (*sql == ',') {
            sql++;
        } else if (*sql == ')') {
            break;
        } else {
            return false;
        }
    }

    return true;
}

// Parse SELECT statement
static bool parse_select(const char *sql, ParsedStatement *stmt) {
    sql = skip_whitespace(sql);

    // Skip "SELECT"
    if (strncasecmp(sql, "SELECT", 6) != 0) return false;
    sql += 6;

    // Check for *
    sql = skip_whitespace(sql);
    if (*sql == '*') {
        stmt->statement.select.select_all = true;
        sql++;
    } else {
        stmt->statement.select.select_all = true; // For now, always select all
    }

    // Skip "FROM"
    sql = skip_whitespace(sql);
    if (strncasecmp(sql, "FROM", 4) != 0) return false;
    sql += 4;

    // Parse table name
    sql = parse_identifier(sql, stmt->statement.select.table_name, MAX_COLUMN_NAME_LEN);

    // Check for WHERE clause (simple: WHERE id = value)
    sql = skip_whitespace(sql);
    stmt->statement.select.has_where = false;
    if (strncasecmp(sql, "WHERE", 5) == 0) {
        sql += 5;
        sql = skip_whitespace(sql);

        // Skip column name (assume it's the first column/id)
        char col_name[MAX_COLUMN_NAME_LEN];
        sql = parse_identifier(sql, col_name, MAX_COLUMN_NAME_LEN);

        // Skip '='
        sql = skip_whitespace(sql);
        if (*sql == '=') {
            sql++;
            sql = parse_integer(sql, &stmt->statement.select.where_key);
            stmt->statement.select.has_where = true;
        }
    }

    return true;
}

ParsedStatement* parse_sql(const char *sql) {
    if (!sql) return NULL;

    ParsedStatement *stmt = (ParsedStatement*)calloc(1, sizeof(ParsedStatement));

    const char *trimmed = skip_whitespace(sql);

    if (strncasecmp(trimmed, "CREATE", 6) == 0) {
        stmt->type = STMT_CREATE_TABLE;
        if (!parse_create_table(trimmed, stmt)) {
            free(stmt);
            return NULL;
        }
    } else if (strncasecmp(trimmed, "INSERT", 6) == 0) {
        stmt->type = STMT_INSERT;
        if (!parse_insert(trimmed, stmt)) {
            free(stmt);
            return NULL;
        }
    } else if (strncasecmp(trimmed, "SELECT", 6) == 0) {
        stmt->type = STMT_SELECT;
        if (!parse_select(trimmed, stmt)) {
            free(stmt);
            return NULL;
        }
    } else {
        stmt->type = STMT_UNKNOWN;
        free(stmt);
        return NULL;
    }

    return stmt;
}

void free_parsed_statement(ParsedStatement *stmt) {
    if (stmt) {
        free(stmt);
    }
}

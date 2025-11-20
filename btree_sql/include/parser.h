#ifndef PARSER_H
#define PARSER_H

#include "storage.h"
#include <stdbool.h>

typedef enum {
    STMT_CREATE_TABLE,
    STMT_INSERT,
    STMT_SELECT,
    STMT_UNKNOWN
} StatementType;

typedef struct {
    char table_name[MAX_COLUMN_NAME_LEN];
    int num_columns;
    ColumnDef columns[MAX_COLUMNS];
} CreateTableStatement;

typedef struct {
    char table_name[MAX_COLUMN_NAME_LEN];
    int num_values;
    ColumnValue values[MAX_COLUMNS];
} InsertStatement;

typedef struct {
    char table_name[MAX_COLUMN_NAME_LEN];
    bool select_all;
    int64_t where_key;  // Simple WHERE key = value
    bool has_where;
} SelectStatement;

typedef struct {
    StatementType type;
    union {
        CreateTableStatement create_table;
        InsertStatement insert;
        SelectStatement select;
    } statement;
} ParsedStatement;

// Parser functions
ParsedStatement* parse_sql(const char *sql);
void free_parsed_statement(ParsedStatement *stmt);

#endif // PARSER_H

#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include <stdbool.h>

#define PAGE_SIZE 4096
#define MAX_COLUMNS 16
#define MAX_COLUMN_NAME_LEN 64
#define MAX_STRING_LEN 256

typedef enum {
    COL_TYPE_INT,
    COL_TYPE_TEXT
} ColumnType;

typedef struct {
    char name[MAX_COLUMN_NAME_LEN];
    ColumnType type;
} ColumnDef;

typedef struct {
    ColumnType type;
    union {
        int64_t int_val;
        char text_val[MAX_STRING_LEN];
    } data;
} ColumnValue;

typedef struct {
    int64_t row_id;
    ColumnValue values[MAX_COLUMNS];
} Row;

typedef struct {
    char table_name[MAX_COLUMN_NAME_LEN];
    int num_columns;
    ColumnDef columns[MAX_COLUMNS];
    Row *rows;
    int num_rows;
    int capacity;
    int64_t next_row_id;
} Table;

// Storage operations
Table* table_create(const char *name);
void table_destroy(Table *table);
void table_add_column(Table *table, const char *name, ColumnType type);
int64_t table_insert_row(Table *table, ColumnValue *values, int num_values);
Row* table_get_row(Table *table, int64_t row_id);
void table_print(Table *table);

// Persistence
bool table_save_to_file(Table *table, const char *filename);
Table* table_load_from_file(const char *filename);

#endif // STORAGE_H

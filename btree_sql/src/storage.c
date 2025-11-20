#include "../include/storage.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Table* table_create(const char *name) {
    Table *table = (Table*)malloc(sizeof(Table));
    strncpy(table->table_name, name, MAX_COLUMN_NAME_LEN - 1);
    table->table_name[MAX_COLUMN_NAME_LEN - 1] = '\0';
    table->num_columns = 0;
    table->num_rows = 0;
    table->capacity = 100;
    table->next_row_id = 1;
    table->rows = (Row*)malloc(table->capacity * sizeof(Row));
    return table;
}

void table_destroy(Table *table) {
    if (table == NULL) return;
    free(table->rows);
    free(table);
}

void table_add_column(Table *table, const char *name, ColumnType type) {
    if (table->num_columns >= MAX_COLUMNS) {
        fprintf(stderr, "Maximum number of columns reached\n");
        return;
    }

    strncpy(table->columns[table->num_columns].name, name, MAX_COLUMN_NAME_LEN - 1);
    table->columns[table->num_columns].name[MAX_COLUMN_NAME_LEN - 1] = '\0';
    table->columns[table->num_columns].type = type;
    table->num_columns++;
}

int64_t table_insert_row(Table *table, ColumnValue *values, int num_values) {
    if (num_values != table->num_columns) {
        fprintf(stderr, "Column count mismatch\n");
        return -1;
    }

    // Resize if needed
    if (table->num_rows >= table->capacity) {
        table->capacity *= 2;
        table->rows = (Row*)realloc(table->rows, table->capacity * sizeof(Row));
    }

    Row *row = &table->rows[table->num_rows];
    row->row_id = table->next_row_id++;

    for (int i = 0; i < num_values; i++) {
        row->values[i] = values[i];
    }

    table->num_rows++;
    return row->row_id;
}

Row* table_get_row(Table *table, int64_t row_id) {
    for (int i = 0; i < table->num_rows; i++) {
        if (table->rows[i].row_id == row_id) {
            return &table->rows[i];
        }
    }
    return NULL;
}

void table_print(Table *table) {
    printf("\nTable: %s\n", table->table_name);
    printf("Columns: ");
    for (int i = 0; i < table->num_columns; i++) {
        printf("%s (%s)", table->columns[i].name,
               table->columns[i].type == COL_TYPE_INT ? "INT" : "TEXT");
        if (i < table->num_columns - 1) printf(", ");
    }
    printf("\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < table->num_rows; i++) {
        Row *row = &table->rows[i];
        printf("Row %ld: ", row->row_id);
        for (int j = 0; j < table->num_columns; j++) {
            if (table->columns[j].type == COL_TYPE_INT) {
                printf("%ld", row->values[j].data.int_val);
            } else {
                printf("'%s'", row->values[j].data.text_val);
            }
            if (j < table->num_columns - 1) printf(", ");
        }
        printf("\n");
    }
    printf("----------------------------------------\n");
    printf("Total rows: %d\n\n", table->num_rows);
}

bool table_save_to_file(Table *table, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        return false;
    }

    // Write table metadata
    fwrite(table->table_name, sizeof(char), MAX_COLUMN_NAME_LEN, file);
    fwrite(&table->num_columns, sizeof(int), 1, file);
    fwrite(&table->num_rows, sizeof(int), 1, file);
    fwrite(&table->next_row_id, sizeof(int64_t), 1, file);

    // Write column definitions
    for (int i = 0; i < table->num_columns; i++) {
        fwrite(&table->columns[i], sizeof(ColumnDef), 1, file);
    }

    // Write rows
    for (int i = 0; i < table->num_rows; i++) {
        fwrite(&table->rows[i], sizeof(Row), 1, file);
    }

    fclose(file);
    return true;
}

Table* table_load_from_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return NULL;
    }

    Table *table = (Table*)malloc(sizeof(Table));

    // Read table metadata
    fread(table->table_name, sizeof(char), MAX_COLUMN_NAME_LEN, file);
    fread(&table->num_columns, sizeof(int), 1, file);
    fread(&table->num_rows, sizeof(int), 1, file);
    fread(&table->next_row_id, sizeof(int64_t), 1, file);

    // Read column definitions
    for (int i = 0; i < table->num_columns; i++) {
        fread(&table->columns[i], sizeof(ColumnDef), 1, file);
    }

    // Allocate and read rows
    table->capacity = table->num_rows > 0 ? table->num_rows * 2 : 100;
    table->rows = (Row*)malloc(table->capacity * sizeof(Row));
    for (int i = 0; i < table->num_rows; i++) {
        fread(&table->rows[i], sizeof(Row), 1, file);
    }

    fclose(file);
    return table;
}

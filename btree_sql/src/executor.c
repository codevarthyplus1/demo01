#include "../include/executor.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Database* database_create() {
    Database *db = (Database*)malloc(sizeof(Database));
    db->num_tables = 0;
    for (int i = 0; i < MAX_TABLES; i++) {
        db->tables[i] = NULL;
        db->indexes[i] = NULL;
    }
    return db;
}

void database_destroy(Database *db) {
    if (!db) return;

    for (int i = 0; i < db->num_tables; i++) {
        if (db->tables[i]) {
            table_destroy(db->tables[i]);
        }
        if (db->indexes[i]) {
            btree_destroy(db->indexes[i]);
        }
    }
    free(db);
}

Table* database_get_table(Database *db, const char *name) {
    for (int i = 0; i < db->num_tables; i++) {
        if (strcmp(db->tables[i]->table_name, name) == 0) {
            return db->tables[i];
        }
    }
    return NULL;
}

int database_get_table_index(Database *db, const char *name) {
    for (int i = 0; i < db->num_tables; i++) {
        if (strcmp(db->tables[i]->table_name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static bool execute_create_table(Database *db, CreateTableStatement *stmt) {
    // Check if table already exists
    if (database_get_table(db, stmt->table_name) != NULL) {
        printf("Error: Table '%s' already exists\n", stmt->table_name);
        return false;
    }

    if (db->num_tables >= MAX_TABLES) {
        printf("Error: Maximum number of tables reached\n");
        return false;
    }

    // Create table
    Table *table = table_create(stmt->table_name);

    // Add columns
    for (int i = 0; i < stmt->num_columns; i++) {
        table_add_column(table, stmt->columns[i].name, stmt->columns[i].type);
    }

    // Create B-tree index for this table
    BTree *index = btree_create(BTREE_ORDER);

    // Add to database
    db->tables[db->num_tables] = table;
    db->indexes[db->num_tables] = index;
    db->num_tables++;

    printf("Table '%s' created successfully\n", stmt->table_name);
    return true;
}

static bool execute_insert(Database *db, InsertStatement *stmt) {
    Table *table = database_get_table(db, stmt->table_name);
    if (!table) {
        printf("Error: Table '%s' not found\n", stmt->table_name);
        return false;
    }

    int table_idx = database_get_table_index(db, stmt->table_name);
    if (table_idx < 0) {
        return false;
    }

    // Type checking
    if (stmt->num_values != table->num_columns) {
        printf("Error: Column count mismatch (expected %d, got %d)\n",
               table->num_columns, stmt->num_values);
        return false;
    }

    for (int i = 0; i < stmt->num_values; i++) {
        if (stmt->values[i].type != table->columns[i].type) {
            printf("Error: Type mismatch for column '%s'\n", table->columns[i].name);
            return false;
        }
    }

    // Insert row
    int64_t row_id = table_insert_row(table, stmt->values, stmt->num_values);
    if (row_id < 0) {
        printf("Error: Failed to insert row\n");
        return false;
    }

    // Insert into B-tree index (using first column as key for simplicity)
    int64_t key;
    if (table->columns[0].type == COL_TYPE_INT) {
        key = stmt->values[0].data.int_val;
    } else {
        // For text, use row_id as key
        key = row_id;
    }
    btree_insert(db->indexes[table_idx], key, row_id);

    printf("Row inserted successfully (row_id=%ld)\n", row_id);
    return true;
}

static bool execute_select(Database *db, SelectStatement *stmt) {
    Table *table = database_get_table(db, stmt->table_name);
    if (!table) {
        printf("Error: Table '%s' not found\n", stmt->table_name);
        return false;
    }

    int table_idx = database_get_table_index(db, stmt->table_name);
    if (table_idx < 0) {
        return false;
    }

    if (stmt->has_where) {
        // Use B-tree to find specific row
        int64_t row_id = btree_search(db->indexes[table_idx], stmt->where_key);
        if (row_id < 0) {
            printf("No rows found\n");
            return true;
        }

        Row *row = table_get_row(table, row_id);
        if (!row) {
            printf("Error: Row not found\n");
            return false;
        }

        // Print header
        printf("\n");
        for (int i = 0; i < table->num_columns; i++) {
            printf("%-15s", table->columns[i].name);
        }
        printf("\n");
        for (int i = 0; i < table->num_columns; i++) {
            printf("%-15s", "---------------");
        }
        printf("\n");

        // Print row
        for (int i = 0; i < table->num_columns; i++) {
            if (table->columns[i].type == COL_TYPE_INT) {
                printf("%-15ld", row->values[i].data.int_val);
            } else {
                printf("%-15s", row->values[i].data.text_val);
            }
        }
        printf("\n\n1 row selected\n");
    } else {
        // Print all rows
        if (table->num_rows == 0) {
            printf("No rows found\n");
            return true;
        }

        // Print header
        printf("\n");
        for (int i = 0; i < table->num_columns; i++) {
            printf("%-15s", table->columns[i].name);
        }
        printf("\n");
        for (int i = 0; i < table->num_columns; i++) {
            printf("%-15s", "---------------");
        }
        printf("\n");

        // Print all rows
        for (int r = 0; r < table->num_rows; r++) {
            Row *row = &table->rows[r];
            for (int i = 0; i < table->num_columns; i++) {
                if (table->columns[i].type == COL_TYPE_INT) {
                    printf("%-15ld", row->values[i].data.int_val);
                } else {
                    printf("%-15s", row->values[i].data.text_val);
                }
            }
            printf("\n");
        }
        printf("\n%d row(s) selected\n", table->num_rows);
    }

    return true;
}

bool execute_statement(Database *db, ParsedStatement *stmt) {
    if (!db || !stmt) return false;

    switch (stmt->type) {
        case STMT_CREATE_TABLE:
            return execute_create_table(db, &stmt->statement.create_table);
        case STMT_INSERT:
            return execute_insert(db, &stmt->statement.insert);
        case STMT_SELECT:
            return execute_select(db, &stmt->statement.select);
        default:
            printf("Error: Unknown statement type\n");
            return false;
    }
}

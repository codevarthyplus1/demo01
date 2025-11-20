#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"
#include "storage.h"
#include "btree.h"
#include <stdbool.h>

#define MAX_TABLES 32

typedef struct {
    Table *tables[MAX_TABLES];
    BTree *indexes[MAX_TABLES];  // B-tree index for each table (on row_id)
    int num_tables;
} Database;

// Database operations
Database* database_create();
void database_destroy(Database *db);
bool execute_statement(Database *db, ParsedStatement *stmt);

// Helper functions
Table* database_get_table(Database *db, const char *name);
int database_get_table_index(Database *db, const char *name);

#endif // EXECUTOR_H

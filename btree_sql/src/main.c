#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/btree.h"
#include "../include/storage.h"
#include "../include/parser.h"
#include "../include/executor.h"

#define MAX_INPUT_SIZE 1024

void print_welcome() {
    printf("========================================\n");
    printf("  B-Tree SQL Server v1.0\n");
    printf("========================================\n");
    printf("Type SQL commands or '.help' for help\n");
    printf("Type '.exit' to quit\n\n");
}

void print_help() {
    printf("\nSupported SQL Commands:\n");
    printf("  CREATE TABLE table_name (col1 INT, col2 TEXT, ...)\n");
    printf("  INSERT INTO table_name VALUES (val1, val2, ...)\n");
    printf("  SELECT * FROM table_name\n");
    printf("  SELECT * FROM table_name WHERE col = value\n");
    printf("\nMeta Commands:\n");
    printf("  .help     - Show this help\n");
    printf("  .tables   - List all tables\n");
    printf("  .schema   - Show schema for all tables\n");
    printf("  .btree    - Show B-tree structure\n");
    printf("  .save     - Save database to disk\n");
    printf("  .load     - Load database from disk\n");
    printf("  .exit     - Exit the program\n\n");
}

void list_tables(Database *db) {
    if (db->num_tables == 0) {
        printf("No tables in database\n");
        return;
    }

    printf("\nTables:\n");
    for (int i = 0; i < db->num_tables; i++) {
        printf("  - %s (%d rows)\n", db->tables[i]->table_name, db->tables[i]->num_rows);
    }
    printf("\n");
}

void show_schema(Database *db) {
    if (db->num_tables == 0) {
        printf("No tables in database\n");
        return;
    }

    for (int i = 0; i < db->num_tables; i++) {
        Table *table = db->tables[i];
        printf("\nTable: %s\n", table->table_name);
        printf("Columns:\n");
        for (int j = 0; j < table->num_columns; j++) {
            printf("  %s: %s\n", table->columns[j].name,
                   table->columns[j].type == COL_TYPE_INT ? "INT" : "TEXT");
        }
    }
    printf("\n");
}

void show_btrees(Database *db) {
    if (db->num_tables == 0) {
        printf("No tables in database\n");
        return;
    }

    for (int i = 0; i < db->num_tables; i++) {
        printf("\nB-Tree for table '%s':\n", db->tables[i]->table_name);
        btree_print(db->indexes[i]);
    }
    printf("\n");
}

void save_database(Database *db) {
    char filename[256];
    for (int i = 0; i < db->num_tables; i++) {
        snprintf(filename, sizeof(filename), "data/%s.tbl", db->tables[i]->table_name);
        if (table_save_to_file(db->tables[i], filename)) {
            printf("Table '%s' saved to %s\n", db->tables[i]->table_name, filename);
        } else {
            printf("Failed to save table '%s'\n", db->tables[i]->table_name);
        }
    }
}

void load_database(Database *db) {
    printf("Enter table name to load: ");
    char table_name[MAX_COLUMN_NAME_LEN];
    if (fgets(table_name, sizeof(table_name), stdin)) {
        // Remove newline
        table_name[strcspn(table_name, "\n")] = 0;

        char filename[256];
        snprintf(filename, sizeof(filename), "data/%s.tbl", table_name);

        Table *table = table_load_from_file(filename);
        if (table) {
            // Check if table already exists
            if (database_get_table(db, table_name) != NULL) {
                printf("Table '%s' already exists in database\n", table_name);
                table_destroy(table);
                return;
            }

            if (db->num_tables >= MAX_TABLES) {
                printf("Error: Maximum number of tables reached\n");
                table_destroy(table);
                return;
            }

            // Rebuild B-tree index
            BTree *index = btree_create(BTREE_ORDER);
            for (int i = 0; i < table->num_rows; i++) {
                int64_t key;
                if (table->columns[0].type == COL_TYPE_INT) {
                    key = table->rows[i].values[0].data.int_val;
                } else {
                    key = table->rows[i].row_id;
                }
                btree_insert(index, key, table->rows[i].row_id);
            }

            db->tables[db->num_tables] = table;
            db->indexes[db->num_tables] = index;
            db->num_tables++;

            printf("Table '%s' loaded successfully\n", table_name);
        } else {
            printf("Failed to load table '%s'\n", table_name);
        }
    }
}

int main() {
    Database *db = database_create();
    char input[MAX_INPUT_SIZE];

    print_welcome();

    while (true) {
        printf("sql> ");
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;

        // Skip empty input
        if (strlen(input) == 0) {
            continue;
        }

        // Handle meta commands
        if (input[0] == '.') {
            if (strcmp(input, ".exit") == 0 || strcmp(input, ".quit") == 0) {
                printf("Goodbye!\n");
                break;
            } else if (strcmp(input, ".help") == 0) {
                print_help();
            } else if (strcmp(input, ".tables") == 0) {
                list_tables(db);
            } else if (strcmp(input, ".schema") == 0) {
                show_schema(db);
            } else if (strcmp(input, ".btree") == 0) {
                show_btrees(db);
            } else if (strcmp(input, ".save") == 0) {
                save_database(db);
            } else if (strcmp(input, ".load") == 0) {
                load_database(db);
            } else {
                printf("Unknown command: %s\n", input);
                printf("Type '.help' for help\n");
            }
            continue;
        }

        // Parse and execute SQL
        ParsedStatement *stmt = parse_sql(input);
        if (!stmt) {
            printf("Error: Failed to parse SQL statement\n");
            printf("Type '.help' for supported commands\n");
            continue;
        }

        execute_statement(db, stmt);
        free_parsed_statement(stmt);
    }

    database_destroy(db);
    return 0;
}

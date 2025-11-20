package com.btreesql;

import java.util.Scanner;

/**
 * Main entry point with REPL interface
 */
public class Main {

    public static void main(String[] args) {
        Database db = new Database();
        Parser parser = new Parser();
        Scanner scanner = new Scanner(System.in);

        printWelcome();

        while (true) {
            System.out.print("sql> ");
            String input = scanner.nextLine().trim();

            if (input.isEmpty()) {
                continue;
            }

            // Handle meta commands
            if (input.startsWith(".")) {
                if (handleMetaCommand(db, input, scanner)) {
                    break; // Exit
                }
                continue;
            }

            // Parse and execute SQL
            try {
                Statement stmt = parser.parse(input);
                String result = db.execute(stmt);
                System.out.print(result);
            } catch (Exception e) {
                System.out.println("Error: " + e.getMessage());
            }
        }

        scanner.close();
    }

    private static void printWelcome() {
        System.out.println("========================================");
        System.out.println("  B-Tree SQL Server v1.0 (Java)");
        System.out.println("========================================");
        System.out.println("Type SQL commands or '.help' for help");
        System.out.println("Type '.exit' to quit\n");
    }

    private static void printHelp() {
        System.out.println("\nSupported SQL Commands:");
        System.out.println("  CREATE TABLE table_name (col1 INT, col2 TEXT, ...)");
        System.out.println("  INSERT INTO table_name VALUES (val1, val2, ...)");
        System.out.println("  SELECT * FROM table_name");
        System.out.println("  SELECT * FROM table_name WHERE col = value");
        System.out.println("\nMeta Commands:");
        System.out.println("  .help     - Show this help");
        System.out.println("  .tables   - List all tables");
        System.out.println("  .schema   - Show schema for all tables");
        System.out.println("  .btree <table> - Show B-tree structure for table");
        System.out.println("  .save     - Save all tables to disk");
        System.out.println("  .load <table> - Load a table from disk");
        System.out.println("  .exit     - Exit the program\n");
    }

    private static boolean handleMetaCommand(Database db, String command, Scanner scanner) {
        String[] parts = command.split("\\s+");

        switch (parts[0]) {
            case ".exit":
            case ".quit":
                System.out.println("Goodbye!");
                return true;

            case ".help":
                printHelp();
                break;

            case ".tables":
                System.out.print(db.listTables());
                break;

            case ".schema":
                System.out.print(db.showSchema());
                break;

            case ".btree":
                if (parts.length < 2) {
                    System.out.println("Usage: .btree <table_name>");
                } else {
                    db.printBTree(parts[1]);
                }
                break;

            case ".save":
                System.out.print(db.saveAllTables());
                break;

            case ".load":
                if (parts.length < 2) {
                    System.out.println("Usage: .load <table_name>");
                } else {
                    System.out.println(db.loadTable(parts[1]));
                }
                break;

            default:
                System.out.println("Unknown command: " + parts[0]);
                System.out.println("Type '.help' for help");
                break;
        }

        return false;
    }
}

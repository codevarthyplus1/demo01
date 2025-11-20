CC = gcc
CFLAGS = -Wall -Wextra -g -I./include
LDFLAGS =

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TARGET = btree_sql

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean run

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all    - Build the project (default)"
	@echo "  clean  - Remove build files"
	@echo "  run    - Build and run the SQL server"
	@echo "  help   - Show this help message"

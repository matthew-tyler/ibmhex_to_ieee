CC=gcc
CFLAGS=-I.
DEPS =
OBJ = floating.o
BIN_DIR = bin
SRC_DIR = src
INPUT_DIR = testdata
INPUT_FILE = single.bin
PRECISION = -s
DEBUG_FLAGS = -g -O0 -Wall

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	mkdir -p $(BIN_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN_DIR)/floating: $(BIN_DIR)/$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean run debug valgrind

clean:
	rm -rf $(BIN_DIR)

run: $(BIN_DIR)/floating
	./$(BIN_DIR)/floating $(INPUT_DIR)/$(INPUT_FILE) $(PRECISION)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(BIN_DIR)/floating

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/floating $(INPUT_DIR)/$(INPUT_FILE) $(PRECISION)

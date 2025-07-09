CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = src
TESTS = tests
MUNIT = munit
BUILD = build

SRCS = $(SRC)/snekobject.c $(SRC)/bootmem.c
TEST_FILES = $(TESTS)/test_snekobject.c $(TESTS)/tests_runner.c
MUNIT_SRC = $(MUNIT)/munit.c

TARGET = $(BUILD)/all_tests

all: $(BUILD) $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(TARGET): $(SRCS) $(TEST_FILES) $(MUNIT_SRC)
	$(CC) $(CFLAGS) -I$(SRC) -I$(MUNIT) $(SRCS) $(TEST_FILES) $(MUNIT_SRC) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD)

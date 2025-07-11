CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = src
TESTS = tests
MUNIT = munit
BUILD = build

SRCS = $(SRC)/snekobject.c $(SRC)/bootmem.c $(SRC)/stack.c $(SRC)/vm.c
TEST_SRCS = $(TESTS)/test_snekobject.c $(TESTS)/test_stack.c $(TESTS)/test_vm.c $(TESTS)/tests_runner.c
MUNIT_SRC = $(MUNIT)/munit.c

OBJS = $(SRCS:.c=.o) $(TEST_SRCS:.c=.o) $(MUNIT_SRC:.c=.o)
TARGET = $(BUILD)/all_tests

.PHONY: all run clean

all: $(BUILD) $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -I$(SRC) -I$(MUNIT) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD) $(SRC)/*.o $(TESTS)/*.o $(MUNIT)/*.o

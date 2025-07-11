CC      := gcc
CFLAGS  := -Wall -Wextra -g -MMD -MP
INCLUDES := -I$(SRC_DIR) -I$(MUNIT_DIR)

SRC_DIR    := src
TESTS_DIR  := tests
MUNIT_DIR  := munit
BUILD_DIR  := build
OBJ_DIR    := $(BUILD_DIR)/obj
BIN        := $(BUILD_DIR)/all_tests

SRC_FILES      := $(wildcard $(SRC_DIR)/*.c)
TEST_SRC_FILES := $(wildcard $(TESTS_DIR)/*.c)
MUNIT_FILES    := $(wildcard $(MUNIT_DIR)/*.c)

ALL_SRC        := $(SRC_FILES) $(TEST_SRC_FILES) $(MUNIT_FILES)
OBJ_FILES      := $(patsubst %.c, $(OBJ_DIR)/%.o, $(ALL_SRC))
DEP_FILES      := $(OBJ_FILES:.o=.d)

# Targets
.PHONY: all run clean

all: $(BIN)

$(BIN): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

# Pattern rule for compiling .c files to .o in build/obj/
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

run: all
	./$(BIN)

clean:
	rm -rf $(BUILD_DIR)

# Include auto-generated dependency files, ignore if they don't exist
-include $(DEP_FILES)

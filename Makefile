CC := gcc

MODE ?= debug

ifeq ($(MODE),release)
    CFLAGS    := -std=c11 -pedantic -W -Wall -Wextra -Werror -O2
    BUILD_DIR := build/release
else
    CFLAGS    := -g -ggdb -std=c11 -pedantic -W -Wall -Wextra
    BUILD_DIR := build/debug
endif

TARGET_PARENT := $(BUILD_DIR)/parent
TARGET_CHILD  := $(BUILD_DIR)/child

.PHONY: all clean

all: $(TARGET_PARENT) $(TARGET_CHILD)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)  # ТАБУЛЯЦИЯ

$(TARGET_PARENT): src/parent.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@  # ТАБУЛЯЦИЯ

$(TARGET_CHILD): src/child.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@  # ТАБУЛЯЦИЯ

clean:
	@[ -d build/debug ] && find build/debug -mindepth 1 ! -name .gitignore -exec rm -rf {} + || true
	@[ -d build/release ] && find build/release -mindepth 1 ! -name .gitignore -exec rm -rf {} + || true
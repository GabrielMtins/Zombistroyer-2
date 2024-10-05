CC = gcc
EXEC ?= main

CCFLAGS := -O2 -std=c99 -Wall -Wextra -pedantic

LIBFLAGS := -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm

SRC_DIR := src

BUILD_DIR := ./build

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(addprefix $(BUILD_DIR)/, $(SRC))
OBJECTS := $(addsuffix .o, $(OBJECTS))

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(BUILD_DIR)/$(SRC_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@ $(CCFLAGS)

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(CCFLAGS) $(LIBFLAGS) -o $(EXEC) $(CCFLAGS)

clean:
	rm -r build
	rm $(EXEC)

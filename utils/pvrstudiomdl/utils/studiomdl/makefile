CC = gcc
CFLAGS = -Wall -O2
LDFLAGS =

# Source directory
SRCDIR =./
INCLUDES =./ ../common

# Get all .c files
SOURCES = $(wildcard $(SRCDIR)*.c) ../common/scriplib.c ../common/mathlib.c ../common/cmdlib.c ../common/lbmlib.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = pvrstudiomdl

# Include directories
CFLAGS += $(addprefix -I,$(INCLUDES))

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compilation
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
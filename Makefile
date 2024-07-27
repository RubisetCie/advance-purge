# Compilers
CC = gcc

# Compiler flags
override CFLAGS += -ansi -D_DEFAULT_SOURCE

# Linker flags
override LDFLAGS +=

# Source files
SRC_FILES = \
	src/main.c \
	src/config.c \
	src/purge.c

# Installation prefix
PREFIX = /usr/local

TARGET = advancepurge

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install:
	mkdir -p $(PREFIX)/bin
	cp $(TARGET) $(PREFIX)/bin/

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(TARGET)

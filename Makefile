#Makefile
CC=gcc
CPPFLAGS= -MMD -D_XOPEN_SOURCE=500 `pkg-config --cflags sdl gtk+-3.0`
CFLAGS= -Wall -Wextra -std=c99 -g
LDFLAGS=
LDLIBS= `pkg-config --libs sdl gtk+-3.0` -lSDL_image -lm -export-dynamic

SRCDIR := .
BUILDDIR := ./build

SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%.o,$(basename $(SOURCES)))
DEPENDENCIES := $(OBJECTS:.o=.d)

TARGET := main

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
		@mkdir -p $(@D)
		$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
		rm -rf $(BUILDDIR) $(TARGET)

-include $(DEPENDENCIES)

.PHONY: all clean

#End
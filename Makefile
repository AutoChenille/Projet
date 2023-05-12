#Makefile
CC := gcc
CFLAGS := -Wall -Wextra -g
CPPFLAGS := -MMD
LDLIBS = pkg-config --libs sdl2 SDL2_image gtk+-3.0 -lm

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

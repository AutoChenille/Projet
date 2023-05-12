CC := gcc
CFLAGS := -Wall -Wextra -g
CPPFLAGS := -MMD `pkg-config --cflags gtk+-3.0 sdl2 SDL2_image`
LDFLAGS := `pkg-config --libs gtk+-3.0 sdl2 SDL2_image`
LDLIBS := -lm

SRCDIR := .
BUILDDIR := ./build

SOURCES := $(shell find $(SRCDIR) -type f -name '*.c')
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))
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

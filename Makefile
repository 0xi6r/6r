# Cross-platform Makefile for 6r Text Editor
# Supports Windows (MinGW), Linux, and macOS

# Detect operating system
UNAME_S := $(shell uname -s)
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(UNAME_S)
endif

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

# Platform-specific settings
ifeq ($(DETECTED_OS),Windows)
    # Windows settings (MinGW)
    EXE_EXT = .exe
    RM = del /Q
    MKDIR = mkdir
    PLATFORM_CFLAGS = -DPLATFORM_WINDOWS
    PLATFORM_LIBS = 
else
    # Unix-like settings (Linux, macOS)
    EXE_EXT = 
    RM = rm -rf
    MKDIR = mkdir -p
    PLATFORM_CFLAGS = -DPLATFORM_UNIX
    PLATFORM_LIBS = 
endif

# Project settings
TARGET = 6r$(EXE_EXT)
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Include directories
INCLUDES = -I$(INCDIR)

# All flags
ALL_CFLAGS = $(CFLAGS) $(PLATFORM_CFLAGS) $(INCLUDES)
ALL_LIBS = $(PLATFORM_LIBS)

# Default target
.PHONY: all
all: $(BINDIR)/$(TARGET)

# Create directories
$(OBJDIR):
	$(MKDIR) $(OBJDIR)

$(BINDIR):
	$(MKDIR) $(BINDIR)

# Link the executable
$(BINDIR)/$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@ $(ALL_LIBS)
	@echo "Built: $@"

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(ALL_CFLAGS) -c $< -o $@

# Clean build artifacts
.PHONY: clean
clean:
ifeq ($(DETECTED_OS),Windows)
	-if exist $(OBJDIR) $(RM) $(OBJDIR)\* 2>nul
	-if exist $(BINDIR) $(RM) $(BINDIR)\* 2>nul
else
	$(RM) $(OBJDIR) $(BINDIR)
endif
	@echo "Cleaned build artifacts"

# Rebuild everything
.PHONY: rebuild
rebuild: clean all

# Run the editor
.PHONY: run
run: $(BINDIR)/$(TARGET)
	./$(BINDIR)/$(TARGET)

# Debug build
.PHONY: debug
debug: CFLAGS += -g -DDEBUG
debug: all

# Install (optional - for Unix-like systems)
.PHONY: install
install: $(BINDIR)/$(TARGET)
ifeq ($(DETECTED_OS),Windows)
	@echo "On Windows, copy $(BINDIR)/$(TARGET) to your desired location"
else
	cp $(BINDIR)/$(TARGET) /usr/local/bin/
	@echo "Installed $(TARGET) to /usr/local/bin/"
endif

# Uninstall (optional - for Unix-like systems)
.PHONY: uninstall
uninstall:
ifeq ($(DETECTED_OS),Windows)
	@echo "On Windows, manually delete $(TARGET) from where you installed it"
else
	rm -f /usr/local/bin/$(TARGET)
	@echo "Removed $(TARGET) from /usr/local/bin/"
endif

# Help
.PHONY: help
help:
	@echo "6r Text Editor - Cross-platform Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all      - Build the editor (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  rebuild  - Clean and build"
	@echo "  run      - Build and run the editor"
	@echo "  debug    - Build with debug symbols"
	@echo "  install  - Install to system path (Unix-like only)"
	@echo "  uninstall- Remove from system path (Unix-like only)"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Detected OS: $(DETECTED_OS)"
	@echo "Target: $(TARGET)"

# Show configuration
.PHONY: show-config
show-config:
	@echo "Configuration:"
	@echo "  OS: $(DETECTED_OS)"
	@echo "  Compiler: $(CC)"
	@echo "  Flags: $(ALL_CFLAGS)"
	@echo "  Libraries: $(ALL_LIBS)"
	@echo "  Target: $(TARGET)"
	@echo "  Sources: $(SOURCES)"

# Dependency tracking
-include $(OBJECTS:.o=.d)

$(OBJDIR)/%.d: $(SRCDIR)/%.c | $(OBJDIR)
	@$(CC) $(ALL_CFLAGS) -MM $< > $@
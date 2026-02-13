# Makefile for libgraffik
# Smart build system with interactive backend and example selection

# Colors for output
COLOR_RESET = \033[0m
COLOR_BOLD = \033[1m
COLOR_GREEN = \033[32m
COLOR_BLUE = \033[34m
COLOR_YELLOW = \033[33m
COLOR_RED = \033[31m

# Compiler
CXX ?= g++

# Directories
LIB_DIR = lib
EXAMPLES_DIR = examples
BUILD_DIR = build

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -I$(LIB_DIR)

# Library source
LIB_SOURCE = $(LIB_DIR)/graphics.cpp

# Detect platform
ifeq ($(OS),Windows_NT)
    PLATFORM = Windows
    EXE_EXT = .exe
    RM = del /Q
    RMDIR = rmdir /S /Q
    MKDIR = mkdir
    PATH_SEP = \\
else
    PLATFORM = Linux
    EXE_EXT =
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
    PATH_SEP = /
endif

# Available backends
AVAILABLE_BACKENDS = sdl win32 x11

# Backend-specific settings
ifeq ($(BACKEND),sdl)
    BACKEND_DEFINE = -DUSE_SDL
    ifeq ($(PLATFORM),Windows)
        SDL2_PATH ?= C:/SDL2
        INCLUDES = -I$(SDL2_PATH)/include -I$(SDL2_PATH)/include/SDL2
        LDFLAGS = -L$(SDL2_PATH)/lib
        LIBS = -lmingw32 -lSDL2main -lSDL2 -mwindows
    else
        INCLUDES = $(shell sdl2-config --cflags 2>/dev/null || echo "-I/usr/include/SDL2")
        LDFLAGS = $(shell sdl2-config --libs 2>/dev/null || echo "-lSDL2")
        LIBS = $(LDFLAGS)
    endif
else ifeq ($(BACKEND),win32)
    BACKEND_DEFINE = -DUSE_WIN32
    INCLUDES =
    LDFLAGS =
    LIBS = -lgdi32 -luser32
else ifeq ($(BACKEND),x11)
    BACKEND_DEFINE = -DUSE_X11
    INCLUDES = -I/usr/include
    LDFLAGS = -L/usr/lib
    LIBS = -lX11
endif

# Find all example files
EXAMPLE_FILES = $(wildcard $(EXAMPLES_DIR)/*.cpp)
EXAMPLE_NAMES = $(basename $(notdir $(EXAMPLE_FILES)))

# Default target - show menu
.PHONY: all
all: menu

# Interactive menu
.PHONY: menu
menu:
	@echo "$(COLOR_BOLD)$(COLOR_BLUE)╔════════════════════════════════════════╗$(COLOR_RESET)"
	@echo "$(COLOR_BOLD)$(COLOR_BLUE)║        libgraffik Build System         ║$(COLOR_RESET)"
	@echo "$(COLOR_BOLD)$(COLOR_BLUE)╚════════════════════════════════════════╝$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_BOLD)Platform:$(COLOR_RESET) $(COLOR_GREEN)$(PLATFORM)$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_BOLD)Usage:$(COLOR_RESET)"
	@echo "  make build BACKEND=<backend> EXAMPLE=<example>"
	@echo ""
	@echo "$(COLOR_BOLD)Available Backends:$(COLOR_RESET)"
	@echo "  $(COLOR_GREEN)sdl$(COLOR_RESET)    - SDL2 (cross-platform)"
	@echo "  $(COLOR_YELLOW)win32$(COLOR_RESET)  - Win32 API (Windows only)"
	@echo "  $(COLOR_YELLOW)x11$(COLOR_RESET)    - X11 (Linux only)"
	@echo ""
	@echo "$(COLOR_BOLD)Available Examples:$(COLOR_RESET)"
	@for example in $(EXAMPLE_NAMES); do \
		echo "  $(COLOR_BLUE)$$example$(COLOR_RESET)"; \
	done
	@echo ""
	@echo "$(COLOR_BOLD)Examples:$(COLOR_RESET)"
	@echo "  make build BACKEND=sdl EXAMPLE=sample1"
	@echo "  make build BACKEND=x11 EXAMPLE=sample2"
	@echo ""
	@echo "$(COLOR_BOLD)Quick Build:$(COLOR_RESET)"
	@echo "  make quick              - Interactive mode"
	@echo "  make clean              - Clean build files"
	@echo ""

# Interactive quick build
.PHONY: quick
quick:
	@echo "$(COLOR_BOLD)$(COLOR_GREEN)Select Backend:$(COLOR_RESET)"
	@echo "1) SDL2 (recommended, cross-platform)"
ifeq ($(PLATFORM),Windows)
	@echo "2) Win32 (Windows native)"
else
	@echo "2) X11 (Linux native)"
endif
	@read -p "Enter choice [1-2]: " choice; \
	if [ "$$choice" = "1" ]; then \
		BACKEND_CHOICE="sdl"; \
	elif [ "$$choice" = "2" ]; then \
		if [ "$(PLATFORM)" = "Windows" ]; then \
			BACKEND_CHOICE="win32"; \
		else \
			BACKEND_CHOICE="x11"; \
		fi; \
	else \
		echo "$(COLOR_RED)Invalid choice$(COLOR_RESET)"; \
		exit 1; \
	fi; \
	echo ""; \
	echo "$(COLOR_BOLD)$(COLOR_GREEN)Select Example:$(COLOR_RESET)"; \
	i=1; \
	for example in $(EXAMPLE_NAMES); do \
		echo "$$i) $$example"; \
		i=$$((i+1)); \
	done; \
	read -p "Enter choice [1-$(words $(EXAMPLE_NAMES))]: " choice; \
	EXAMPLE_CHOICE=$$(echo "$(EXAMPLE_NAMES)" | cut -d' ' -f$$choice); \
	if [ -z "$$EXAMPLE_CHOICE" ]; then \
		echo "$(COLOR_RED)Invalid choice$(COLOR_RESET)"; \
		exit 1; \
	fi; \
	echo ""; \
	echo "$(COLOR_BOLD)Building $$EXAMPLE_CHOICE with $$BACKEND_CHOICE backend...$(COLOR_RESET)"; \
	$(MAKE) build BACKEND=$$BACKEND_CHOICE EXAMPLE=$$EXAMPLE_CHOICE

# Build target
.PHONY: build
build: validate
	@echo "$(COLOR_BOLD)$(COLOR_GREEN)Building $(EXAMPLE) with $(BACKEND) backend...$(COLOR_RESET)"
	@$(MKDIR) $(BUILD_DIR) 2>/dev/null || true
	$(CXX) $(CXXFLAGS) $(BACKEND_DEFINE) $(INCLUDES) \
		$(EXAMPLES_DIR)/$(EXAMPLE).cpp $(LIB_SOURCE) \
		-o $(BUILD_DIR)/$(EXAMPLE)$(EXE_EXT) \
		$(LDFLAGS) $(LIBS)
	@echo "$(COLOR_BOLD)$(COLOR_GREEN)✓ Build successful!$(COLOR_RESET)"
	@echo "$(COLOR_BOLD)Output:$(COLOR_RESET) $(BUILD_DIR)/$(EXAMPLE)$(EXE_EXT)"
ifeq ($(BACKEND),sdl)
ifeq ($(PLATFORM),Windows)
	@if [ -f "$(LIB_DIR)/SDL2.dll" ]; then \
		cp $(LIB_DIR)/SDL2.dll $(BUILD_DIR)/ 2>/dev/null || copy $(LIB_DIR)$(PATH_SEP)SDL2.dll $(BUILD_DIR)$(PATH_SEP) 2>nul || true; \
		echo "$(COLOR_BLUE)✓ SDL2.dll copied to build directory$(COLOR_RESET)"; \
	elif [ -f "$(SDL2_PATH)/bin/SDL2.dll" ]; then \
		cp $(SDL2_PATH)/bin/SDL2.dll $(BUILD_DIR)/ 2>/dev/null || copy $(SDL2_PATH)$(PATH_SEP)bin$(PATH_SEP)SDL2.dll $(BUILD_DIR)$(PATH_SEP) 2>nul || true; \
		echo "$(COLOR_BLUE)✓ SDL2.dll copied from SDL2 installation$(COLOR_RESET)"; \
	fi
endif
endif
	@echo ""
	@echo "$(COLOR_BOLD)Run with:$(COLOR_RESET)"
	@echo "  ./$(BUILD_DIR)/$(EXAMPLE)$(EXE_EXT)"

# Validate inputs
.PHONY: validate
validate:
ifndef BACKEND
	@echo "$(COLOR_RED)Error: BACKEND not specified$(COLOR_RESET)"
	@echo "Run 'make' or 'make quick' for interactive mode"
	@exit 1
endif
ifndef EXAMPLE
	@echo "$(COLOR_RED)Error: EXAMPLE not specified$(COLOR_RESET)"
	@echo "Run 'make' or 'make quick' for interactive mode"
	@exit 1
endif
	@if ! echo "$(AVAILABLE_BACKENDS)" | grep -w "$(BACKEND)" > /dev/null; then \
		echo "$(COLOR_RED)Error: Invalid backend '$(BACKEND)'$(COLOR_RESET)"; \
		echo "Available: $(AVAILABLE_BACKENDS)"; \
		exit 1; \
	fi
	@if [ ! -f "$(EXAMPLES_DIR)/$(EXAMPLE).cpp" ]; then \
		echo "$(COLOR_RED)Error: Example '$(EXAMPLE)' not found$(COLOR_RESET)"; \
		echo "Available examples:"; \
		for ex in $(EXAMPLE_NAMES); do echo "  $$ex"; done; \
		exit 1; \
	fi
ifeq ($(BACKEND),win32)
ifneq ($(PLATFORM),Windows)
	@echo "$(COLOR_RED)Error: win32 backend only available on Windows$(COLOR_RESET)"
	@exit 1
endif
endif
ifeq ($(BACKEND),x11)
ifeq ($(PLATFORM),Windows)
	@echo "$(COLOR_RED)Error: x11 backend only available on Linux$(COLOR_RESET)"
	@exit 1
endif
endif

# Build all examples with specified backend
.PHONY: build-all
build-all:
ifndef BACKEND
	@echo "$(COLOR_RED)Error: BACKEND not specified$(COLOR_RESET)"
	@echo "Usage: make build-all BACKEND=<backend>"
	@exit 1
endif
	@echo "$(COLOR_BOLD)$(COLOR_GREEN)Building all examples with $(BACKEND) backend...$(COLOR_RESET)"
	@for example in $(EXAMPLE_NAMES); do \
		echo ""; \
		$(MAKE) build BACKEND=$(BACKEND) EXAMPLE=$$example; \
	done
	@echo ""
	@echo "$(COLOR_BOLD)$(COLOR_GREEN)✓ All examples built successfully!$(COLOR_RESET)"

# Run an example
.PHONY: run
run: build
	@echo "$(COLOR_BOLD)$(COLOR_GREEN)Running $(EXAMPLE)...$(COLOR_RESET)"
	@cd $(BUILD_DIR) && ./$(EXAMPLE)$(EXE_EXT)

# Clean build files
.PHONY: clean
clean:
	@echo "$(COLOR_BOLD)Cleaning build files...$(COLOR_RESET)"
ifeq ($(PLATFORM),Windows)
	@if exist $(BUILD_DIR) $(RMDIR) $(BUILD_DIR) 2>nul || true
else
	@$(RMDIR) $(BUILD_DIR) 2>/dev/null || true
endif
	@echo "$(COLOR_GREEN)✓ Clean complete$(COLOR_RESET)"

# Help
.PHONY: help
help: menu

# Show configuration
.PHONY: config
config:
	@echo "$(COLOR_BOLD)Current Configuration:$(COLOR_RESET)"
	@echo "  Platform:    $(PLATFORM)"
	@echo "  Compiler:    $(CXX)"
	@echo "  Lib Dir:     $(LIB_DIR)"
	@echo "  Examples:    $(EXAMPLES_DIR)"
	@echo "  Build Dir:   $(BUILD_DIR)"
	@echo ""
	@echo "$(COLOR_BOLD)Available Examples:$(COLOR_RESET)"
	@for example in $(EXAMPLE_NAMES); do \
		echo "  - $$example"; \
	done
	@echo ""
	@echo "$(COLOR_BOLD)Available Backends:$(COLOR_RESET)"
	@echo "  - sdl (SDL2)"
ifeq ($(PLATFORM),Windows)
	@echo "  - win32 (Win32 API)"
else
	@echo "  - x11 (X11)"
endif

.DEFAULT_GOAL := menu

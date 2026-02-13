# Makefile for libgraffik
# Windows-compatible build system

# Compiler
CXX = g++

# Directories
LIB_DIR = lib
EXAMPLES_DIR = examples
BUILD_DIR = build

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -I$(LIB_DIR)

# Library source
LIB_SOURCE = $(LIB_DIR)/graphics.cpp

# Detect platform
ifdef OS
    ifeq ($(OS),Windows_NT)
        PLATFORM = Windows
        EXE_EXT = .exe
        RM = del /Q
        RMDIR = rmdir /S /Q
        MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    endif
else
    PLATFORM = Linux
    EXE_EXT =
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p $(BUILD_DIR)
endif

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
EXAMPLE_FILES := $(wildcard $(EXAMPLES_DIR)/*.cpp)
EXAMPLE_NAMES := $(basename $(notdir $(EXAMPLE_FILES)))

# Default target - show help
.PHONY: all
all: help

# Help menu
.PHONY: help
help:
	@echo ========================================
	@echo    libgraffik Build System
	@echo ========================================
	@echo.
	@echo Platform: $(PLATFORM)
	@echo.
	@echo Usage:
	@echo   make build BACKEND=^<backend^> EXAMPLE=^<example^>
	@echo.
	@echo Available Backends:
	@echo   sdl    - SDL2 (cross-platform)
	@echo   win32  - Win32 API (Windows only)
	@echo   x11    - X11 (Linux only)
	@echo.
	@echo Available Examples:
	@for %%f in ($(EXAMPLES_DIR)\*.cpp) do @echo   %%~nf
	@echo.
	@echo Example Commands:
	@echo   make build BACKEND=sdl EXAMPLE=sample1
	@echo   make build BACKEND=win32 EXAMPLE=sample2
	@echo   make run BACKEND=sdl EXAMPLE=sample1
	@echo   make build-all BACKEND=sdl
	@echo   make clean
	@echo.

# Build target
.PHONY: build
build: validate
	@echo Building $(EXAMPLE) with $(BACKEND) backend...
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $(BACKEND_DEFINE) $(INCLUDES) \
		$(EXAMPLES_DIR)/$(EXAMPLE).cpp $(LIB_SOURCE) \
		-o $(BUILD_DIR)/$(EXAMPLE)$(EXE_EXT) \
		$(LDFLAGS) $(LIBS)
	@echo Build successful!
	@echo Output: $(BUILD_DIR)/$(EXAMPLE)$(EXE_EXT)
ifeq ($(BACKEND),sdl)
ifeq ($(PLATFORM),Windows)
	@if exist $(LIB_DIR)\SDL2.dll copy $(LIB_DIR)\SDL2.dll $(BUILD_DIR)\ >nul 2>&1
	@if exist $(SDL2_PATH)\bin\SDL2.dll if not exist $(BUILD_DIR)\SDL2.dll copy $(SDL2_PATH)\bin\SDL2.dll $(BUILD_DIR)\ >nul 2>&1
endif
endif
	@echo.
	@echo Run with: $(BUILD_DIR)\$(EXAMPLE)$(EXE_EXT)

# Validate inputs
.PHONY: validate
validate:
ifndef BACKEND
	@echo Error: BACKEND not specified
	@echo Usage: make build BACKEND=^<backend^> EXAMPLE=^<example^>
	@echo Run 'make help' for more information
	@exit 1
endif
ifndef EXAMPLE
	@echo Error: EXAMPLE not specified
	@echo Usage: make build BACKEND=^<backend^> EXAMPLE=^<example^>
	@echo Run 'make help' for more information
	@exit 1
endif
	@if not exist $(EXAMPLES_DIR)\$(EXAMPLE).cpp (echo Error: Example '$(EXAMPLE)' not found & exit 1)

# Build all examples with specified backend
.PHONY: build-all
build-all:
ifndef BACKEND
	@echo Error: BACKEND not specified
	@echo Usage: make build-all BACKEND=^<backend^>
	@exit 1
endif
	@echo Building all examples with $(BACKEND) backend...
	@for %%f in ($(EXAMPLES_DIR)\*.cpp) do @$(MAKE) --no-print-directory build BACKEND=$(BACKEND) EXAMPLE=%%~nf
	@echo.
	@echo All examples built successfully!

# Run an example
.PHONY: run
run: build
	@echo Running $(EXAMPLE)...
	@cd $(BUILD_DIR) && $(EXAMPLE)$(EXE_EXT)

# Clean build files
.PHONY: clean
clean:
	@echo Cleaning build files...
ifeq ($(PLATFORM),Windows)
	@if exist $(BUILD_DIR) $(RMDIR) $(BUILD_DIR) 2>nul
else
	@$(RMDIR) $(BUILD_DIR) 2>/dev/null || true
endif
	@echo Clean complete

# Show configuration
.PHONY: config
config:
	@echo Current Configuration:
	@echo   Platform:    $(PLATFORM)
	@echo   Compiler:    $(CXX)
	@echo   Lib Dir:     $(LIB_DIR)
	@echo   Examples:    $(EXAMPLES_DIR)
	@echo   Build Dir:   $(BUILD_DIR)
	@echo.
	@echo Available Examples:
	@for %%f in ($(EXAMPLES_DIR)\*.cpp) do @echo   - %%~nf
	@echo.
	@echo Available Backends:
	@echo   - sdl (SDL2)
ifeq ($(PLATFORM),Windows)
	@echo   - win32 (Win32 API)
else
	@echo   - x11 (X11)
endif

.DEFAULT_GOAL := help

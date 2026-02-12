# Makefile for Graphics Demo (MinGW/Windows)
# Usage: make
# Make sure to set SDL2_PATH to your SDL2 installation directory

# Detect SDL2 path - adjust this to where you installed SDL2
SDL2_PATH ?= C:/SDL2

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -DUSE_SDL

# Include directories
INCLUDES = -I$(SDL2_PATH)/include -I$(SDL2_PATH)/include/SDL2

# Library directories and libraries
LDFLAGS = -L$(SDL2_PATH)/lib
LIBS = -lmingw32 -lSDL2main -lSDL2 -mwindows

# Source files
SOURCES = sample1.cpp graphics.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Output executable
TARGET = sample.exe

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LIBS)
	@echo "Build complete! Run with: ./$(TARGET)"
	@echo "Note: Make sure SDL2.dll is in the same directory or in your PATH"

# Compile source files
%.o: %.cpp graphics.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Help
help:
	@echo "Makefile for Graphics Demo"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build the project"
	@echo "  make clean    - Remove build files"
	@echo "  make run      - Build and run the program"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  Set SDL2_PATH environment variable or edit the Makefile"
	@echo "  Current SDL2_PATH: $(SDL2_PATH)"
	@echo ""
	@echo "Example:"
	@echo "  make SDL2_PATH=D:/SDL2-2.28.0"

.PHONY: all clean run help
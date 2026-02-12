cmake_minimum_required(VERSION 3.10)
project(GraphicsDemo)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define USE_SDL for compilation
add_definitions(-DUSE_SDL)

# Find SDL2
find_package(SDL2 REQUIRED)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})

# Add executable
add_executable(GraphicsDemo
    main.cpp
    graphics.cpp
    graphics.h
)

# Link SDL2
target_link_libraries(GraphicsDemo ${SDL2_LIBRARIES})

# For Windows, add necessary flags
if(WIN32)
    # This ensures we get a console window for debugging
    # Remove this line to create a GUI-only application
    set_target_properties(GraphicsDemo PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

# Platform-specific configurations
if(MSVC)
    # MSVC specific flags
    target_compile_options(GraphicsDemo PRIVATE /W4)
else()
    # GCC/Clang flags
    target_compile_options(GraphicsDemo PRIVATE -Wall -Wextra)
endif()
cmake_minimum_required(VERSION 3.10)

# Project Name and Version
p34r(P34R VERSION 1.0)

# Set C Standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add source files
file(GLOB SOURCES "src/*.c")

# Create executable target
add_executable(P34R ${SOURCES})

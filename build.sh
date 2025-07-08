#!/bin/bash

# Build script for C++ Pong

echo "Building C++ Pong..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Generate build files
echo "Generating build files..."
cmake ..

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Compile the project
echo "Compiling..."
cmake --build .

if [ $? -eq 0 ]; then
    echo "Build successful! Run './CppPong' to play the game."
else
    echo "Build failed!"
    exit 1
fi 
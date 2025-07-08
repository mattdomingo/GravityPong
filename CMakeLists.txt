cmake_minimum_required(VERSION 3.10)
project(CppPong)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set pkg-config path for Homebrew on macOS
if(APPLE)
    set(ENV{PKG_CONFIG_PATH} "/opt/homebrew/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif()

# Find required packages
find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)

# Fallback to pkg-config if CMake packages not found
if(NOT SDL3_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL3 REQUIRED sdl3)
    pkg_check_modules(SDL3_TTF REQUIRED sdl3-ttf)
endif()

# Include directories
include_directories(include)
if(SDL3_INCLUDE_DIRS)
    include_directories(${SDL3_INCLUDE_DIRS})
endif()
if(SDL3_TTF_INCLUDE_DIRS)
    include_directories(${SDL3_TTF_INCLUDE_DIRS})
endif()

# Add executable
add_executable(CppPong
    src/main.cpp
    src/Game.cpp
    src/Paddle.cpp
    src/Ball.cpp
    src/PowerUp.cpp
    src/Constants.cpp
)

# Link libraries
if(TARGET SDL3::SDL3)
    # Use CMake targets if available
    target_link_libraries(CppPong SDL3::SDL3)
    if(TARGET SDL3_ttf::SDL3_ttf)
        target_link_libraries(CppPong SDL3_ttf::SDL3_ttf)
    endif()
else()
    # Fallback to pkg-config variables
    if(SDL3_LIBRARIES)
        target_link_libraries(CppPong ${SDL3_LIBRARIES})
    endif()
    if(SDL3_TTF_LIBRARIES)
        target_link_libraries(CppPong ${SDL3_TTF_LIBRARIES})
    endif()
endif()

# Copy any required DLLs on Windows
if(WIN32)
    add_custom_command(TARGET CppPong POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:CppPong>)
endif() 
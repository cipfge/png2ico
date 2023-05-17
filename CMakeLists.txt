cmake_minimum_required(VERSION 3.20)

project(png2ico VERSION 1.0.0 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to Debug as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

add_subdirectory(src)

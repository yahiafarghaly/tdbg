cmake_minimum_required (VERSION 2.8.11)
project (tiny-debugger)

SET(execName tdbg)

SET(LINE_NOISE_SRC ext-libs/linenoise/linenoise.c)


include_directories(ext-libs/libelfin ext-libs/linenoise)

FILE(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(${execName} ${SRC_FILES} ${LINE_NOISE_SRC})

add_definitions(-std=c++17) # or -std=c++11 if u don't support 17

add_subdirectory(debugging-examples)
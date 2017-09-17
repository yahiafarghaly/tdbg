cmake_minimum_required (VERSION 2.8.11)
project (tiny-debugger)

SET(execName tdbg)

SET(LINE_NOISE_SRC ext-libs/linenoise/linenoise.c)


include_directories(ext-libs/libelfin ext-libs/linenoise)

FILE(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(${execName} ${SRC_FILES} ${LINE_NOISE_SRC})

add_definitions(-std=c++1y) #For GCC < 4.9.3 ,otherwise,  use -std=c++14


add_subdirectory(debugging-examples)
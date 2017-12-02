cmake_minimum_required (VERSION 3.0)
project (tiny-debugger)

SET(execName tdbg)

SET(LINE_NOISE_SRC ext-libs/linenoise/linenoise.c)


include_directories(ext-libs/libelfin/dwarf 
                    ext-libs/libelfin/elf 
                    ext-libs/linenoise)

FILE(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_executable(${execName} ${SRC_FILES} ${LINE_NOISE_SRC})

add_definitions(-std=c++17) # or -std=c++11 if u don't support 17


LINK_DIRECTORIES( ${CMAKE_BINARY_DIR}/ext-libs/libelfin/dwarf/
                ${CMAKE_BINARY_DIR}/ext-libs/libelfin/elf/
)

target_link_libraries(${execName} libdwarf++.so libelf++.so)

add_subdirectory(debugging-examples)

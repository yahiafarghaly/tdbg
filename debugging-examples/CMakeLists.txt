

add_executable(helloworld helloworld.cpp)
add_executable(calculator calculator.cpp)
add_executable(single-step single-step.cpp)
add_executable(loop loop.c)

set_target_properties(helloworld PROPERTIES COMPILE_FLAGS "-g")
set_target_properties(calculator PROPERTIES COMPILE_FLAGS "-g")
set_target_properties(single-step PROPERTIES COMPILE_FLAGS "-g")
set_target_properties(loop PROPERTIES COMPILE_FLAGS "-g")
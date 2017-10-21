

add_executable(helloworld helloworld.cpp)
add_executable(calculator calculator.cpp)

set_target_properties(helloworld PROPERTIES COMPILE_FLAGS "-g")
set_target_properties(calculator PROPERTIES COMPILE_FLAGS "-g")
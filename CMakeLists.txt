cmake_minimum_required(VERSION 3.16)
project(cpp_thread_pool CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic)
find_package(Threads REQUIRED)

add_executable(primes_demo src/main.cpp)
target_link_libraries(primes_demo Threads::Threads)

enable_testing()
add_executable(test_thread_pool tests/test_thread_pool.cpp)
target_link_libraries(test_thread_pool Threads::Threads)
add_test(NAME test_thread_pool COMMAND test_thread_pool)
add_test(NAME demo COMMAND primes_demo 200000)

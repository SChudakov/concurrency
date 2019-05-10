cmake_minimum_required(VERSION 3.12)

project(concurrency CXX)
set(CMAKE_CXX_STANDARD 17)

add_executable(concurrency cpu_cache.cpp AtomicInteger.cpp AtomicInteger.h NaiveAtomicInteger.cpp NaiveAtomicInteger.h benchmark.hpp benchmark.cpp disable_cache.c)

#project(concurrency C)
#set(CMAKE_C_STANDARD 11)

#add_executable(concurrency dinning_philosophers.c threads_intercommunication.c
#        concurrent_queue.c concurrent_queue.h
#        transfer_queue.c transfer_queue.h process.c memory_read.c keystroke_capture.c matrix_multiplication.cpp)

target_link_libraries(concurrency -lpthread)

cmake_minimum_required(VERSION 3.12)
project(concurrency C)

set(CMAKE_C_STANDARD 11)

add_executable(concurrency dinning_philosophers.c threads_intercommunication.c
        concurrent_queue.c concurrent_queue.h
        transfer_queue.c transfer_queue.h process.c memory_read.c keystroke_capture.c)

target_link_libraries(concurrency -lpthread)

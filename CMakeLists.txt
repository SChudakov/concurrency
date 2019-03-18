cmake_minimum_required(VERSION 3.12)
project(concurrency C)

set(CMAKE_C_STANDARD 11)

add_executable(concurrency main.c dinning_philosophers.c threads_intercommunication.c concurrent_queue.c concurrent_queue.h)
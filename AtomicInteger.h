//
// Created by semen on 24.04.19.
//

#ifndef CONCURRENCY_ATOMICINTEGER_H
#define CONCURRENCY_ATOMICINTEGER_H

#include <mutex>
#include <chrono>
#include <thread>

class AtomicInteger {
    int value;
    std::mutex mutex;
public:
    AtomicInteger();

    explicit AtomicInteger(int value);

    void increment();

    int getValue();
};


#endif //CONCURRENCY_ATOMICINTEGER_H

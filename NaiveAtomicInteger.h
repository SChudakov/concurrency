//
// Created by semen on 24.04.19.
//

#ifndef CONCURRENCY_NAIVEATOMICINTEGER_H
#define CONCURRENCY_NAIVEATOMICINTEGER_H

#include <mutex>
#include <thread>
#include <chrono>

class NaiveAtomicInteger {
    int value;
    std::mutex mutex;
public:
    NaiveAtomicInteger();

    explicit NaiveAtomicInteger(int value);

    void increment();

    int getValue();
};


#endif //CONCURRENCY_NAIVEATOMICINTEGER_H

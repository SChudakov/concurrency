//
// Created by semen on 24.04.19.
//

#include "AtomicInteger.h"


AtomicInteger::AtomicInteger() : AtomicInteger(0) {
}

AtomicInteger::AtomicInteger(int value) : value(value) {

}

void AtomicInteger::increment() {
    std::unique_lock<std::mutex> lock(mutex);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    value++;
}

int AtomicInteger::getValue() {
    return value;
}

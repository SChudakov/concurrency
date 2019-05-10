//
// Created by semen on 24.04.19.
//

#include "NaiveAtomicInteger.h"

NaiveAtomicInteger::NaiveAtomicInteger() : NaiveAtomicInteger(0) {

}

NaiveAtomicInteger::NaiveAtomicInteger(int value) : value(value) {

}

void NaiveAtomicInteger::increment() {
//    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    ++value;
}

int NaiveAtomicInteger::getValue() {
    return value;
}

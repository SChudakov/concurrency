//
// Created by semen on 24.04.19.
//

#include <iostream>
#include <vector>
#include <functional>
#include <random>

#include "AtomicInteger.h"
#include "NaiveAtomicInteger.h"
#include "benchmark.hpp"
#include "disable_cache.c"

namespace {
    AtomicInteger atomicInteger;
    NaiveAtomicInteger naiveAtomicInteger;

//    void incrementer() {
//        for (int i = 0; i < 10000000; i++) {
//            atomicInteger.increment();
//        }
//    }

    void naive_incrementer() {
        for (int i = 0; i < 100000000; i++) {
            naiveAtomicInteger.increment();
        }
    }
}

void measure_time(const std::function<void()> &f, const std::string &descriptor) {
    long start = benchmark::current_time();
    f();
    long end = benchmark::current_time();
    std::cout << std::endl << descriptor << " execution time: " << (end - start) << std::endl << std::endl;
}

void increments() {
    const int numOfWorkers = 12;

    std::vector<std::thread> incremeterThreads;
    std::vector<std::thread> naiveIncremeterThreads;

//    for (int i = 0; i < numOfWorkers; i++) {
//        std::thread incrementer(::incrementer);
//        incremeterThreads.emplace_back(std::move(incrementer));
//    }
    for (int i = 0; i < numOfWorkers; i++) {
        std::thread naiveIncremeter(::naive_incrementer);
        naiveIncremeterThreads.emplace_back(std::move(naiveIncremeter));
    }

    for (std::thread &worker: incremeterThreads) {
        worker.join();
    }
    for (std::thread &worker: naiveIncremeterThreads) {
        worker.join();
    }

    std::cout << "atomic integer value: " << atomicInteger.getValue() << std::endl;
    std::cout << "naive atomic integer value: " << naiveAtomicInteger.getValue() << std::endl;
}


void generate(std::vector<std::vector<int>> &matrix, int m, int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 10);
    for (int i = 0; i < m; i++) {

        std::vector<int> v;
        v.reserve(static_cast<unsigned long>(n));

        for (int j = 0; j < n; j++) {
            v.push_back(dis(gen));
        }
        matrix.push_back(std::move(v));
    }
}

struct MultiplierData {
    int m;
    int n;
    int k;
    std::vector<std::vector<int>> &a;
    std::vector<std::vector<int>> &b;
    std::vector<std::vector<int>> &result;

    MultiplierData(int m, int n, int k, std::vector<std::vector<int>> &a, std::vector<std::vector<int>> &b,
                   std::vector<std::vector<int>> &result) : m(m), n(n), k(k), a(a), b(b), result(result) {}
};

void worker(MultiplierData data) {
    std::cout << "thread (" << data.m << "," << data.k << ") has started" << std::endl;

    int result = 0;
    for (int i = 0; i < data.n; i++) {
        result += data.a[data.m][i] * data.b[i][data.k];
    }
    data.result[data.m][data.k] = result;

    std::cout << "thread (" << data.m << "," << data.k << ") has ended" << std::endl;
}

void output(const std::vector<std::vector<int>> &v, const std::string &label) {
    std::cout << "matrix " << label << std::endl;
    for (const auto &i : v) {
        for (int j : i) {
            std::cout << j << "\t";
        }
        std::cout << std::endl;
    }
}

void matrix_multiplication() {
    srand(static_cast<unsigned int>(time(nullptr)));

    int m = 10;
    int n = 10;
    int k = 10;

    std::vector<std::vector<int>> a;
    std::vector<std::vector<int>> b;
    std::vector<std::vector<int>> result(m, std::vector<int>(k, 0));
    generate(a, m, n);
    generate(b, n, k);

    std::cout << "matrices generated" << std::endl;

    std::vector<std::thread> workers;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            MultiplierData data(i, n, j, a, b, result);
            std::thread w(worker, data);
            workers.emplace_back(std::move(w));
        }
    }

    for (std::thread &w : workers) {
        w.join();
    }

    output(a, "a");
    output(b, "b");
    output(result, "result");
}


int main() {
    auto matrix_multiplication_lb = []() { matrix_multiplication(); };
    auto increments_lb = []() { increments(); };


    measure_time(matrix_multiplication_lb, "matrix multiplication");
    measure_time(increments_lb, "increments with critical region");

    return 0;
}
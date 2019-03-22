//
// Created by Semen on 2/18/2019.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "concurrent_queue.h"

/*
 * Solution to the https://en.wikipedia.org/wiki/Dining_philosophers_problem
 * problem using resources hierarchy technique.
 */
int num_of_meals[5] = {};


/*
 * A chop stick is identified by its index.
 */
struct ChopSick {
    int chop_stick_index;
};

/*
 * Constructor for ChopSick
 */
struct ChopSick *ChopSick_new(int index) {
    struct ChopSick *stick = (struct ChopSick *) malloc(sizeof(struct ChopSick));
    stick->chop_stick_index = index;
    return stick;
}

struct ChopStickMutexAndQueue {
    struct ChopSick *chopSick;
    pthread_mutex_t lock;
    struct concurrent_queue *threads_queue;
};

/*
 * Constructor for ChopStickMutexAndQueue
 */
struct ChopStickMutexAndQueue *ChopStickMutexAndQueue_new(struct ChopSick *stick, struct concurrent_queue *queue) {
    struct ChopStickMutexAndQueue *s = (struct ChopStickMutexAndQueue *) malloc(sizeof(struct ChopStickMutexAndQueue));
    s->chopSick = stick;
    s->threads_queue = queue;
    return s;
}

struct ChopSticksMonitor {
    int monitor_size;
    struct ChopStickMutexAndQueue *chop_sticks_and_mutexes[];
};

/*
 * Constructor for ChopSticksMonitor
 */
struct ChopSticksMonitor *ChopSticksMonitor_new(int size) {
    struct ChopSticksMonitor *monitor = (struct ChopSticksMonitor *) malloc(
            sizeof(struct ChopSticksMonitor) + size * sizeof(struct ChopStickMutexAndQueue));
    monitor->monitor_size = size;
    for (int i = 0; i < size; i++) {
        monitor->chop_sticks_and_mutexes[i] = ChopStickMutexAndQueue_new(ChopSick_new(i), concurrent_queue_new());
        if (pthread_mutex_init(&(monitor->chop_sticks_and_mutexes[i]->lock), NULL) != 0) {
            printf("init failed for mutex %d\n", i);
            printf("return NULL\n");
            return NULL;
        }
    }
    return monitor;
}

/*
 * Obtains a chops stick from the provided
 * monitor in a concurrently-safe fashion by
 * blocking the mutex, which corresponds to
 * the chopstick
 */
struct ChopSick *get_chop_stick(struct ChopSticksMonitor *monitor, int stick_index, const pthread_t *thread_id) {
//    printf("[%d] get_chop_stick\n", index);
    if (stick_index >= monitor->monitor_size) {
        printf("return NULL\n");
        return NULL;
    }
    pthread_t *first_thread_id = (pthread_t *) concurrent_queue_first(
            monitor->chop_sticks_and_mutexes[stick_index]->threads_queue);
//    printf("first scc\n");
    if (*(thread_id) == *(first_thread_id)) {
        pthread_mutex_lock(&monitor->chop_sticks_and_mutexes[stick_index]->lock);
        concurrent_queue_remove(monitor->chop_sticks_and_mutexes[stick_index]->threads_queue);
//        printf("remove scc\n");
        return monitor->chop_sticks_and_mutexes[stick_index]->chopSick;
    } else {
        printf("return NULL\n");
        return NULL;
    }
}

/*
 * Returns a chops stick to the provided
 * monitor in a concurrently-safe fashion
 * by unlocking the mutex, which corresponds
 * to the chop stick
 */
void return_chop_stick(struct ChopSticksMonitor *monitor, int stick_index) {
//    printf("[%d] return_chop_stick\n", stick_index);
    if (stick_index >= monitor->monitor_size) {
        printf("illegal index in return_chop_stick function");
    }
    pthread_mutex_unlock(&monitor->chop_sticks_and_mutexes[stick_index]->lock);
}

/*
 * Structure which is provided to a philosopher
 * function while starting a new thread
 */
struct PhilosopherData {
    int id;
    pthread_t *thread_id;
    int left_chop_stick_index;
    int right_chop_stick_index;
    struct ChopSticksMonitor *monitor;
};

/*
 * Constructor for PhilosopherData
 */
struct PhilosopherData *PhilosopherData_new(int id, pthread_t *philosopher_index,
                                            int left, int right, struct ChopSticksMonitor *monitor) {
    struct PhilosopherData *data = (struct PhilosopherData *) malloc(sizeof(struct PhilosopherData));
    data->id = id;
    data->thread_id = philosopher_index;
    data->left_chop_stick_index = left;
    data->right_chop_stick_index = right;
    data->monitor = monitor;
    return data;
}

/*
 * Routine that mimics the behavior if a philosopher.
 * First the the function tries to get both forks. Once
 * this is done the thread "has a dinner" for 5 seconds.
 * After that it releases both forks and "thinks" for 5
 * seconds.
 */
void *philosopher(void *arg) {
    struct PhilosopherData *data = (struct PhilosopherData *) arg;
    printf("[%d] philosopher started\n", data->id);
    for (int i = 0; i < 10; i++) {
        concurrent_queue_add(data->monitor->chop_sticks_and_mutexes[data->left_chop_stick_index]->threads_queue,
                             data->thread_id);
//        printf("[%lu] left stick queue state: ", *data->thread_id);
//        concurrent_queue_print(data->monitor->chop_sticks_and_mutexes[data->left_chop_stick_index]->threads_queue);
        struct ChopSick *left_stick = NULL;
        while (left_stick == NULL) {
            left_stick = get_chop_stick(data->monitor, data->left_chop_stick_index, data->thread_id);
        }
//        printf("[%lu] got left chop stick\n", *data->thread_id);

        concurrent_queue_add(data->monitor->chop_sticks_and_mutexes[data->right_chop_stick_index]->threads_queue,
                             data->thread_id);
//        printf("[%lu] right stick queue state: ", *data->thread_id);
//        concurrent_queue_print(data->monitor->chop_sticks_and_mutexes[data->right_chop_stick_index]->threads_queue);
        struct ChopSick *right_stick = NULL;
        while (right_stick == NULL) {
            right_stick = get_chop_stick(data->monitor, data->right_chop_stick_index, data->thread_id);
        }
//        printf("[%llu] got right chop stick\n", *data->thread_id);

        for (int j = 0; j < 5; j++) {
            printf("[%d] philosopher is dining\n", data->id);
            sleep(1);
        }
//        printf("id: %d", data->id);
        num_of_meals[data->id]++;

        return_chop_stick(data->monitor, data->left_chop_stick_index);
        return_chop_stick(data->monitor, data->right_chop_stick_index);

        for (int j = 0; j < 5; j++) {
            printf("[%d] philosopher is thinking\n", data->id);
            sleep(1);
        }
    }
    return 0;
}

int min(int a, int b) {
    return a > b ? b : a;
}

int max(int a, int b) {
    return a > b ? a : b;
}


/*
 * Entry point to the program.
 * Creates the monitor with the
 * chop sticks and starts the
 * philosophers threads.
 */
int main() {
    printf("main started\n");
    const int num_of_philosophers = 5;
    pthread_t thread_ids[num_of_philosophers];
    for (int i = 0; i < num_of_philosophers; i++) { thread_ids[i] = 0; }
    struct ChopSticksMonitor *monitor = ChopSticksMonitor_new(num_of_philosophers);
    int error = 0;
    for (int i = 0; i < num_of_philosophers; i++) {
        struct PhilosopherData *data = PhilosopherData_new(i, &(thread_ids[i]),
                                                           min(i, (i + 1) % num_of_philosophers),
                                                           max(i, (i + 1) % num_of_philosophers),
                                                           monitor);
        error = pthread_create(&(thread_ids[i]), NULL, &philosopher, (void *) data);
        if (error != 0) {
            printf("\nThread %d can't be created :[%s]\n", i, strerror(error));
        }
    }
    sleep(60);
    for (int i = 0; i < num_of_philosophers; i++) {
        pthread_cancel(thread_ids[i]);
    }

    for (int i = 0; i < num_of_philosophers; i++) {
        printf("%d ", num_of_meals[i]);
    }
    printf("\n");

    return 0;
}

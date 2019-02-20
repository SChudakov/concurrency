//
// Created by Semen on 2/18/2019.
//
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

/*
 * Solution to the https://en.wikipedia.org/wiki/Dining_philosophers_problem
 * problem using resources hierarchy technique.
 */


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

struct ChopStickAndMutex {
    struct ChopSick *chopSick;
    pthread_mutex_t lock;
};

/*
 * Constructor for ChopStickAndMutex
 */
struct ChopStickAndMutex *ChopStickAndMutex_new(struct ChopSick *stick) {
    struct ChopStickAndMutex *s = (struct ChopStickAndMutex *) malloc(sizeof(struct ChopStickAndMutex));
    s->chopSick = stick;
    return s;
}

struct ChopSticksMonitor {
    int monitor_size;
    struct ChopStickAndMutex *chop_sticks_and_mutexes[];
};

/*
 * Constructor for ChopSticksMonitor
 */
struct ChopSticksMonitor *ChopSticksMonitor_new(int size) {
    struct ChopSticksMonitor *monitor = (struct ChopSticksMonitor *) malloc(
            sizeof(struct ChopSticksMonitor) + size * sizeof(struct ChopStickAndMutex));
    monitor->monitor_size = size;
    for (int i = 0; i < size; i++) {
        monitor->chop_sticks_and_mutexes[i] = ChopStickAndMutex_new(ChopSick_new(i));
        if (pthread_mutex_init(&(monitor->chop_sticks_and_mutexes[i]->lock), NULL) != 0) {
            printf("init failed for mutex %d\n", i);
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
struct ChopSick *get_chop_stick(struct ChopSticksMonitor *monitor, int index) {
    printf("[%d] get_chop_stick\n", index);
    if (index >= monitor->monitor_size) {
        printf("return NULL");
        return NULL;
    }
    pthread_mutex_lock(&monitor->chop_sticks_and_mutexes[index]->lock);
    return monitor->chop_sticks_and_mutexes[index]->chopSick;
}

/*
 * Returns a chops stick to the provided
 * monitor in a concurrently-safe fashion
 * by unlocking the mutex, which corresponds
 * to the chop stick
 */
struct ChopSick *return_chop_stick(struct ChopSticksMonitor *monitor, int index) {
    printf("[%d] return_chop_stick\n", index);
    if (index >= monitor->monitor_size) {
        printf("return NULL");
        return NULL;
    }
    pthread_mutex_unlock(&monitor->chop_sticks_and_mutexes[index]->lock);
    return monitor->chop_sticks_and_mutexes[index]->chopSick;
}

/*
 * Structure which is provided to a philosopher
 * function while starting a new thread
 */
struct PhilosopherData {
    int philosopher_index;
    int left_chop_stick;
    int right_chop_stick;
    struct ChopSticksMonitor *monitor;
};

/*
 * Constructor for PhilosopherData
 */
struct PhilosopherData *PhilosopherData_new(int philosopher_index,
                                            int left, int right, struct ChopSticksMonitor *monitor) {
    struct PhilosopherData *data = (struct PhilosopherData *) malloc(sizeof(struct PhilosopherData));
    data->philosopher_index = philosopher_index;
    data->left_chop_stick = left;
    data->right_chop_stick = right;
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
    printf("[%d] philosopher started\n", data->philosopher_index);
    while (1) {
        struct ChopSick *left_stick = get_chop_stick(data->monitor, data->left_chop_stick);
        struct ChopSick *right_stick = get_chop_stick(data->monitor, data->right_chop_stick);

        for (int i = 0; i < 5; i++) {
            printf("[%d] philosopher is dining\n", data->philosopher_index);
            sleep(1);
        }
        return_chop_stick(data->monitor, data->left_chop_stick);
        return_chop_stick(data->monitor, data->right_chop_stick);

        for (int i = 0; i < 5; i++) {
            printf("[%d] philosopher is thinking\n", data->philosopher_index);
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
        struct PhilosopherData *data = PhilosopherData_new(i,
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
    return 0;
}

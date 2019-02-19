//
// Created by Semen on 2/18/2019.
//
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

struct ChopSick {
    int chop_stick_index;
};

struct ChopSick *ChopSick_new(int index) {
    printf("ChopSick_new\n");
    struct ChopSick *stick = (struct ChopSick *) malloc(sizeof(struct ChopSick));
    stick->chop_stick_index = index;
    return stick;
}


struct ChopStickAndMutex {
    struct ChopSick *chopSick;
    pthread_mutex_t lock;
};

struct ChopStickAndMutex *ChopStickAndMutex_new(struct ChopSick *stick) {
    printf("ChopStickAndMutex_new\n");
    struct ChopStickAndMutex *s = (struct ChopStickAndMutex *) malloc(sizeof(struct ChopStickAndMutex));
    s->chopSick = stick;
    return s;
}

struct ChopSticksMonitor {
    int monitor_size;
    struct ChopStickAndMutex *chop_sticks_and_mutexes[];
};

struct ChopSticksMonitor *ChopSticksMonitor_new(int size) {
    printf("ChopSticksMonitor_new\n");
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

struct ChopSick *get_chop_stick(struct ChopSticksMonitor *monitor, int index) {
    printf("get_chop_stick to thread %s\n", index);
    if (index >= monitor->monitor_size) {
        printf("return NULL");
        return NULL;
    }
    pthread_mutex_lock(monitor->chop_sticks_and_mutexes[index]->lock);
    return monitor->chop_sticks_and_mutexes[index]->chopSick;
}

struct ChopSick *return_chop_stick(struct ChopSticksMonitor *monitor, int index) {
    printf("return_chop_stick from thread %s\n", index);
    if (index >= monitor->monitor_size) {
        printf("return NULL");
        return NULL;
    }
    pthread_mutex_unlock(monitor->chop_sticks_and_mutexes[index]->lock);
    return monitor->chop_sticks_and_mutexes[index]->chopSick;
}

struct PhilosopherData {
    int philosopher_index;
    int left_chop_stick;
    int right_chop_stick;
    struct ChopSticksMonitor *monitor;
};

struct PhilosopherData *PhilosopherData_new(int philosopher_index,
                                            int left, int right, struct ChopSticksMonitor *monitor) {
    printf("PhilosopherData_new\n");
    struct PhilosopherData *data = (struct PhilosopherData *) malloc(sizeof(struct PhilosopherData));
    data->philosopher_index = philosopher_index;
    data->left_chop_stick = left;
    data->right_chop_stick = right;
    data->monitor = monitor;
    return data;
}

void *philosopher(void *arg) {
    struct PhilosopherData *data = (struct PhilosopherData *) arg;
    printf("philosopher %d started\n", data->philosopher_index);
    while (1) {
        struct ChopSick *left_stick = get_chop_stick(data->monitor, data->left_chop_stick);
        struct ChopSick *right_stick = get_chop_stick(data->monitor, data->right_chop_stick);
        for (int i = 0; i < 5; i++) {
            printf("%d-th philosopher is dining\n", data->philosopher_index);
            sleep(1);
        }
        return_chop_stick(data->monitor, data->left_chop_stick);
        return_chop_stick(data->monitor, data->right_chop_stick);
    }
    return 0;
}

int main() {
    printf("main started\n");
    const int num_of_philosophers = 5;
    pthread_t thread_ids[num_of_philosophers];
    for (int i = 0; i < num_of_philosophers; i++) { thread_ids[i] = 0; }
    struct ChopSticksMonitor *monitor = ChopSticksMonitor_new(num_of_philosophers);

    int error = 0;
    for (int i = 0; i < num_of_philosophers; i++) {
        struct PhilosopherData *data = PhilosopherData_new(i, i, (i + 1) % num_of_philosophers, monitor);
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


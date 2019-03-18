//
// Created by Semen on 3/5/2019.
//

#include <stdbool.h>
#include <pthread.h>

#ifndef CONCURRENCY_CONCURRENT_QUEUE_H
#define CONCURRENCY_CONCURRENT_QUEUE_H
struct node {
    void *data;
    struct node *next;
    struct node *previous;
};

extern struct node *node_new(void *data);

extern void node_delete(struct node *node);

struct concurrent_queue {
    struct node *first;
    struct node *last;
    int size;
    pthread_mutex_t queue_mutex;
};

extern struct concurrent_queue *concurrent_queue_new();

extern void concurrent_queue_delete(struct concurrent_queue *queue);

extern void concurrent_queue_add(struct concurrent_queue *queue, void *data);

extern void *concurrent_queue_remove(struct concurrent_queue *queue);

extern void *concurrent_queue_first(struct concurrent_queue *queue);

extern void concurrent_queue_print(struct concurrent_queue *queue);

extern bool is_empty(struct concurrent_queue *queue);

#endif //CONCURRENCY_CONCURRENT_QUEUE_H

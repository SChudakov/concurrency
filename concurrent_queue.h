//
// Created by Semen on 3/5/2019.
//

#include <stdbool.h>
#include <pthread.h>

#ifndef CONCURRENCY_CONCURRENT_QUEUE_H
#define CONCURRENCY_CONCURRENT_QUEUE_H
struct cq_node {
    void *data;
    struct cq_node *next;
    struct cq_node *previous;
};

extern struct cq_node *node_new(void *data);

extern void node_delete(struct cq_node *node);

struct concurrent_queue {
    struct cq_node *first;
    struct cq_node *last;
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

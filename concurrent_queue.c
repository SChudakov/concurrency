//
// Created by Semen on 3/5/2019.
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "concurrent_queue.h"

struct cq_node *node_new(void *data) {
    struct cq_node *result = (struct cq_node *) malloc(sizeof(struct cq_node));
    result->data = data;
    result->next = NULL;
    result->previous = NULL;
    return result;
}

extern void node_delete(struct cq_node *node) {
    if (node->previous != NULL) {
        node_delete(node->previous);
        node->previous = NULL;
    }
    if (node->next != NULL) {
        node_delete(node->next);
        node->next = NULL;
    }
    free((void *) node);
}


struct concurrent_queue *concurrent_queue_new() {
    struct concurrent_queue *result = (struct concurrent_queue *) malloc(sizeof(struct concurrent_queue));
    result->size = 0;
    result->first = NULL;
    result->last = NULL;
    int err = pthread_mutex_init(&(result->queue_mutex), NULL);
    if (err != 0) {
        printf("unable to initialize mutex for concurrent queue: %s", strerror(err));
    }
    return result;
}

void concurrent_queue_add(struct concurrent_queue *queue, void *data) {
    pthread_mutex_lock(&(queue->queue_mutex));
    struct cq_node *node = node_new(data);
    if (queue->first == NULL && queue->last == NULL) {
        queue->first = node;
        queue->last = node;
    } else if (queue->first == queue->last) {
        queue->last = node;
        queue->first->previous = node;
    } else {
        struct cq_node *tmp_last = queue->last;
        node->next = tmp_last;
        tmp_last->previous = node;
        queue->last = node;
    }
    queue->size++;
    pthread_mutex_unlock(&(queue->queue_mutex));
}

void *concurrent_queue_remove(struct concurrent_queue *queue) {
    pthread_mutex_lock(&(queue->queue_mutex));
    void *result = NULL;
    if (queue->first == NULL && queue->last == NULL) { // empty queue
        result = NULL;
    } else if (queue->first == queue->last) {
        result = queue->first->data;
        node_delete(queue->first);
        queue->first = NULL;
        queue->last = NULL;
    } else {
        struct cq_node *tmp_last = queue->last;
        queue->last = queue->last->next;

        queue->last->previous = NULL;
        tmp_last->next = NULL;

        result = tmp_last->data;
        node_delete(tmp_last);
    }
    pthread_mutex_unlock(&(queue->queue_mutex));
    return result;
}

void *concurrent_queue_first(struct concurrent_queue *queue) {
    pthread_mutex_lock(&(queue->queue_mutex));
    struct cq_node *result = queue->first;
    pthread_mutex_unlock(&(queue->queue_mutex));
    return result->data;
}

void concurrent_queue_print(struct concurrent_queue *queue) {
    pthread_mutex_lock(&(queue->queue_mutex));
    struct cq_node *it = queue->first;
    while (it != NULL) {
        printf("%llu->", *((pthread_t *) it->data));
        it = it->next;
    }
    printf("\n");
    pthread_mutex_unlock(&(queue->queue_mutex));
}

bool is_empty(struct concurrent_queue *queue) {
    return queue->size == 0;
}


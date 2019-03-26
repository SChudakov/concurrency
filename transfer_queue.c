//
// Created by Semen on 3/26/2019.
//


#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>


#include "transfer_queue.h"


struct transfer_queue *transfer_queue_new() {
    struct transfer_queue *result = (struct transfer_queue *) malloc(sizeof(struct transfer_queue));
    result->message = NULL;

    int put_err = sem_init(&(result->put_sem), false, 1);
    if (put_err != 0) {
        printf("unable to initialize put semaphore for transfer queue: %s", strerror(put_err));
    }

    int get_err = sem_init(&(result->get_sem), false, 0);
    if (get_err != 0) {
        printf("unable to initialize get semaphore for transfer queue: %s", strerror(get_err));
    }
    return result;
}

void transfer_queue_delete(struct transfer_queue *queue) {
    if (queue->message != NULL) {
        free(queue->message);
        queue->message = NULL;
    }
    int put_err = sem_destroy(&(queue->put_sem));
    if (put_err != 0) {
        printf("unable to destroy put semaphore for transfer queue: %s", strerror(put_err));
    }

    int get_err = sem_destroy(&(queue->get_sem));
    if (get_err != 0) {
        printf("unable to destroy get semaphore for transfer queue: %s", strerror(get_err));
    }
}


void tq_put(struct transfer_queue *queue, void *message) {
    sem_wait(&(queue->put_sem));
    queue->message = message;
    sem_post(&(queue->get_sem));
    sem_wait(&(queue->put_sem));
}

void *tq_get(struct transfer_queue *queue) {
    sem_wait(&(queue->get_sem));
    void *result = queue->message;
    sem_post(&(queue->put_sem));
    return result;
}
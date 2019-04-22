//
// Created by Semen on 3/26/2019.
//

#ifndef CONCURRENCY_TRANSFER_QUEUE_H
#define CONCURRENCY_TRANSFER_QUEUE_H

#include <semaphore.h>

struct transfer_queue {
    sem_t put_sem;
    sem_t get_sem;
    void *message;
};

extern struct transfer_queue *transfer_queue_new();

extern void transfer_queue_delete(struct transfer_queue* queue);


extern void tq_put(struct transfer_queue *queue, void *message);

extern void *tq_get(struct transfer_queue *queue);


#endif //CONCURRENCY_TRANSFER_QUEUE_H

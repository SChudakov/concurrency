//
// Created by Semen on 3/1/2019.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "transfer_queue.h"


void print_bool(bool value) {
    if (value) {
        printf("true");
    } else {
        printf("false");
    }
}

char read_char() {
    char prev = 0;
    char ch = 0;
    while (1) {
        ch = (char) getchar();

        if (ch == '\n') {
            // double return pressed!
            break;
        }

        prev = ch;
    }
    return prev;
}


struct Result {
    bool value;
};

struct Result *Result_new(bool value) {
    struct Result *result = (struct Result *) malloc(sizeof(struct Result));
    result->value = value;
    return result;
}

struct Result *Result_delete(struct Result *result) {
    free(result);
}

struct ResultContainer {
    struct Result *result;
};

struct ResultContainer *ResultContainer_new(struct Result *result) {
    struct ResultContainer *container = (struct ResultContainer *) malloc(sizeof(struct ResultContainer));
    container->result = result;
    return container;
}

void ResultContainer_delete(struct ResultContainer *resultContainer) {
    free(resultContainer->result);
    resultContainer->result = NULL;
    free(resultContainer);
}

struct Message {
    char *value;
};

struct Message *Message_new(char *value) {
    struct Message *message = (struct Message *) malloc(sizeof(struct Message));
    message->value = value;
    return message;
}

void Message_delete(struct Message *m) {
    free(m->value);
    m->value = NULL;
    free(m);
}

struct ThreadArg {
    struct Result *result;
    struct ResultContainer *result_container;
    struct Message *message;
    struct transfer_queue *queue;
    bool perform_message_action;
};

struct ThreadArg *ThreadArg_new(struct Result *result, struct ResultContainer *container,
                                struct Message *message, struct transfer_queue *queue,
                                bool perform_message_action) {
    struct ThreadArg *arg = (struct ThreadArg *) malloc(sizeof(struct ThreadArg));
    arg->result = result;
    arg->result_container = container;
    arg->message = message;
    arg->queue = queue;
    arg->perform_message_action = perform_message_action;
    return arg;
}

void ThreadArg_delete(struct ThreadArg *arg) {
    if (arg->result_container->result == NULL) {
        ResultContainer_delete(arg->result_container);
        arg->result_container = NULL;
        Result_delete(arg->result);
        arg->result = NULL;
    } else {
        ResultContainer_delete(arg->result_container);
        arg->result_container = NULL;
        arg->result = NULL;
    }

    Message_delete(arg->message);
    arg->message = NULL;

    transfer_queue_delete(arg->queue);
    arg->queue = NULL;
}


void *f(void *arg) {
    printf("f() is started\n");
    struct ThreadArg *casted_arg = (struct ThreadArg *) arg;

    if (casted_arg->perform_message_action) {
        printf("f() tries to put message in the exchanger\n");
        tq_put(casted_arg->queue, casted_arg->message);
        printf("f() has put message into transfer queue");
    }
    casted_arg->result_container->result = casted_arg->result;
    pthread_exit(0);
}

void *g(void *arg) {
    printf("g() is started\n");
    struct ThreadArg *casted_arg = (struct ThreadArg *) arg;


    if (casted_arg->perform_message_action) {
        printf("g() tries to get message from the exchanger\n");
        struct Message *m = (struct Message *) tq_get(casted_arg->queue);
        printf("g() received message from transfer queue: %s \n", m->value);
    }

    casted_arg->result_container->result = casted_arg->result;
    pthread_exit(0);
}

int main() {
    printf("main started\n");
    pthread_t thread_ids[2];

    struct transfer_queue *queue = transfer_queue_new();

    struct Result *f_result = Result_new(true);
    struct ResultContainer *f_container = ResultContainer_new(NULL);
    struct Message *f_message = Message_new("this is from f");
    bool f_perform_message_action = false;
    struct ThreadArg *f_arg = ThreadArg_new(
            f_result,
            f_container,
            f_message,
            queue,
            f_perform_message_action
    );
//    printf("f_arg constructed\n");

    struct Result *g_result = Result_new(false);
    struct ResultContainer *g_container = ResultContainer_new(NULL);
    struct Message *g_message = Message_new("this is from g");
    bool g_perform_message_action = true;
    struct ThreadArg *g_arg = ThreadArg_new(
            g_result,
            g_container,
            g_message,
            queue,
            g_perform_message_action
    );
//    printf("g_arg constructed\n");

    int error = 0;

    error = pthread_create(&(thread_ids[0]), NULL, &f, (void *) f_arg);
    if (error != 0) {
        printf("\nThread for function f() cannot be created: [%s]\n", strerror(error));
    }

    error = pthread_create(&(thread_ids[1]), NULL, &g, (void *) g_arg);
    if (error != 0) {
        printf("\nThread for function g() cannot be created: [%s]\n", strerror(error));
    }

    struct Result *f_res = NULL;
    struct Result *g_res = NULL;
    bool aborted = false;
    while (true) {
        sleep(2);
        if (f_arg->result_container->result != NULL) {
            printf("receive result from f(): %s\n", f_arg->result_container->result->value ? "true" : "false");
            f_res = f_arg->result_container->result;
        }
        if (g_arg->result_container->result != NULL) {
            printf("receive result from g(): %s\n", g_arg->result_container->result->value ? "true" : "false");
            g_res = g_arg->result_container->result;
        }
        if (f_res != NULL && g_res != NULL) {
            break;
        }
        if (f_res == NULL && g_res == NULL) {
            printf("both functions are not calculated yet, should we proceed? [y\\n]: ");
        } else if (f_res == NULL) {
            printf("f() not calculated yet, should we proceed? [y\\n]: ");
        } else {
            printf("g() not calculated yet, should we proceed? [y\\n]: ");
        }

        char answer = read_char();
        if (answer == 'n') {
            break;
        } else if (answer != 'y') {
            printf("illegal option %c aborting", answer);
            aborted = true;
        }
    }

    if (!aborted) {
        printf("result: ");
        if (f_res != NULL && g_res != NULL) {
            print_bool(f_res->value || g_res->value);
        } else if (f_res != NULL) {
//            printf("f result: %s\n", f_res->value ? "true" : "false");
            if (f_res->value) {
                print_bool(true);
            } else {
                printf("undefined");
            }
        } else if (g_res != NULL) {
            if (g_res->value) {
                print_bool(true);
            } else {
                printf("undefined");
            }
        } else {
            printf("undefined");
        }
    }

//    ThreadArg_delete(f_arg);
//    ThreadArg_delete(g_arg);
    return 0;
}

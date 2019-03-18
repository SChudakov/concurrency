//
// Created by Semen on 3/1/2019.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>


struct Result {
    bool value;
};

struct Result *Result_new(bool value) {
    struct Result *result = (struct Result *) malloc(sizeof(struct Result));
    result->value = value;
    return result;
}

struct ResultContainer {
    struct Result *result;
};

struct ResultContainer *ResultContainer_new(struct Result *result) {
    struct ResultContainer *container = (struct ResultContainer *) malloc(sizeof(struct ResultContainer));
    container->result = result;
    return container;
}

struct Message {

};

struct Message *Message_new() {
    struct Message *message = (struct Message *) malloc(sizeof(struct Message));
    return message;
}

struct MessageExchanger {
    struct Message *message;
    pthread_mutex_t put_lock;
    pthread_mutex_t get_lock;
};

struct MessageExchanger *MessageExchanger_new(struct Message *message) {
    struct MessageExchanger *container = (struct MessageExchanger *) malloc(sizeof(struct MessageExchanger));
    container->message = message;

    // init mutexes
    if (pthread_mutex_init(&(container->get_lock), NULL) != 0) {
        printf("init failed for get_mutex\n");
        return NULL;
    }
    // get operation should be locked hence there is no message in the exchanger
    pthread_mutex_lock(&container->get_lock);

    if (pthread_mutex_init(&(container->put_lock), NULL) != 0) {
        printf("init failed for put_mutex\n");
        return NULL;
    }
    return container;
}

struct Message *get_message(struct MessageExchanger *exchanger) {
    struct Message *result = NULL;
    // get operation is locked until the message is put with into
    // the exchanger with put_message() function. once the corresponding
    // mutex is initially locked, this operation can only succeed after
    // the put_message() function is called at least one time.
    if (pthread_mutex_lock(&(exchanger->get_lock)) != 0) {
        printf("failed to lock the get_lock\n");
    }

    result = exchanger->message;
    exchanger->message = NULL;
    // once the messaged is received the put operation should be enabled
    if (pthread_mutex_unlock(&(exchanger->put_lock)) != 0) {
        printf("failed to unlock the put_lock\n");
    }

    printf("get operation succeeds");
    return result;
}

void put_message(struct MessageExchanger *exchanger, struct Message *message) {
    // put operation is locked until the message is received my other thread via
    // get_message() function. once put_mutex is not initially locked this operation
    // succeed for the first caller.
    if (pthread_mutex_lock(&(exchanger->put_lock)) != 0) {
        printf("failed to lock the put_lock\n");
    }
    exchanger->message = message;
    // once a message is in the exchanger the get operation should be enabled
    if (pthread_mutex_unlock(&(exchanger->get_lock)) != 0) {
        printf("failed to unlock the get_lock\n");
    }
    printf("put operation succeeds\n");
}

struct ThreadArg {
    struct Result *result;
    struct ResultContainer *result_container;
    struct Message *message;
    struct MessageExchanger *message_container;
    bool perform_message_action;
};

struct ThreadArg *ThreadArg_new(struct Result *result, struct ResultContainer *result_container,
                                struct Message *message, struct MessageExchanger *message_container,
                                bool perform_message_action) {
    struct ThreadArg *arg = (struct ThreadArg *) malloc(sizeof(struct ThreadArg));
    arg->result = result;
    arg->result_container = result_container;
    arg->message = message;
    arg->message_container = message_container;
    arg->perform_message_action = perform_message_action;
    return arg;
}


void *f(void *arg) {
    printf("f() is started\n");
    struct ThreadArg *casted_arg = (struct ThreadArg *) arg;
    if (casted_arg->perform_message_action) {
        printf("f() tries to put message in the exchanger\n");
        put_message(casted_arg->message_container, casted_arg->message);
    }
    casted_arg->result_container->result = casted_arg->result;
    pthread_exit(0);
}

void *g(void *arg) {
    printf("g() is started\n");
    struct ThreadArg *casted_arg = (struct ThreadArg *) arg;
    if (casted_arg->perform_message_action) {
        printf("g() tries to get message from the exchanger\n");
        get_message(casted_arg->message_container);
    }
    casted_arg->result_container->result = casted_arg->result;
    pthread_exit(0);
}

void print_result(bool value) {
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


//int main() {
//    printf("main started\n");
//    pthread_t thread_ids[2];
//
//    struct Result *f_result = Result_new(false);
//    struct ResultContainer *f_result_container = ResultContainer_new(NULL);
//    struct Message *f_message = Message_new();
//    struct MessageExchanger *f_message_container = MessageExchanger_new(NULL);
//    bool f_perform_message_action = true;
//    struct ThreadArg *f_arg = ThreadArg_new(
//            f_result,
//            f_result_container,
//            f_message,
//            f_message_container,
//            f_perform_message_action
//    );
//    printf("f_arg constructed\n");
//
//    struct Result *g_result = Result_new(false);
//    struct ResultContainer *g_result_container = ResultContainer_new(NULL);
//    struct Message *g_message = Message_new();
//    struct MessageExchanger *g_message_container = MessageExchanger_new(NULL);
//    bool g_perform_message_action = true;
//    struct ThreadArg *g_arg = ThreadArg_new(
//            g_result,
//            g_result_container,
//            g_message,
//            g_message_container,
//            g_perform_message_action
//    );
//    printf("g_arg constructed\n");
//
//    int error = 0;
//
//    error = pthread_create(&(thread_ids[0]), NULL, &f, (void *) f_arg);
//    if (error != 0) {
//        printf("\nThread for function f() cannot be created: [%s]\n", strerror(error));
//    }
//
//    error = pthread_create(&(thread_ids[1]), NULL, &g, (void *) g_arg);
//    if (error != 0) {
//        printf("\nThread for function g() cannot be created: [%s]\n", strerror(error));
//    }
//
//    struct Result *f_res = NULL;
//    struct Result *g_res = NULL;
//    bool aborted = false;
//    while (true) {
//        sleep(10);
//        if (f_arg->result_container->result != NULL) {
//            printf("receive result from f()\n");
//            f_res = f_arg->result_container->result;
//        }
//        if (g_arg->result_container->result != NULL) {
//            printf("receive result from g()\n");
//            g_res = g_arg->result_container->result;
//        }
//        if (f_res != NULL && g_res != NULL) {
//            break;
//        }
//        if (f_res == NULL && g_res == NULL) {
//            printf("both functions are not calculated yet, should we proceed? [y\\n]: ");
//        } else if (f_res == NULL) {
//            printf("f() not calculated yet, should we proceed? [y\\n]: ");
//        } else {
//            printf("g() not calculated yet, should we proceed? [y\\n]: ");
//        }
//
//        char answer = read_char();
//        if (answer == 'n') {
//            break;
//        } else if (answer != 'y') {
//            printf("illegal option %c aborting", answer);
//            aborted = true;
//        }
//    }
//
//    if (!aborted) {
//        printf("result: ");
//        if (f_res != NULL && g_res != NULL) {
//            print_result(f_res->value || g_res->value);
//        } else if (f_res != NULL) {
//            if (f_res->value) {
//                print_result(true);
//            } else {
//                printf("undefined");
//            }
//        } else if (g_res != NULL) {
//            if (g_res->value) {
//                print_result(true);
//            } else {
//                printf("undefined");
//            }
//        } else {
//            printf("undefined");
//        }
//    }
//    return 0;
//}

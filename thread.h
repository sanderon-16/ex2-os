//
// Created by TLP-258 on 14/06/2024.
//

#ifndef DEMO_ITIMER_C_THREAD_H
#define DEMO_ITIMER_C_THREAD_H

#include "uthreads.h"

#define STACK_SIZE 4096

enum states {READY, RUNNING, SLEEP, BLOCKED, SNB};

struct Thread {
    states state;
    int id;
    int quantums_to_sleep;
    char *stack;
    int elapsed_quantums{};
    thread_entry_point entry_point;

    Thread(int i, thread_entry_point p_function);
    Thread(); // for the main thread

    ~Thread();
};


#endif //DEMO_ITIMER_C_THREAD_H
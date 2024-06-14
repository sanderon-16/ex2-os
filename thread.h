//
// Created by TLP-258 on 14/06/2024.
//

#ifndef DEMO_ITIMER_C_THREAD_H
#define DEMO_ITIMER_C_THREAD_H

#include "uthreads.h"

#define STACK_SIZE 4096


struct Thread {
    char state;
    int id;
    char stack[STACK_SIZE];
    int elapsed_quantums;
    thread_entry_point entry_point;
};


#endif //DEMO_ITIMER_C_THREAD_H

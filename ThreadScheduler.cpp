//
// Created by TLP-299 on 14/06/2024.
//

#include "ThreadScheduler.h"
#include <csignal>

#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr) {
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
                 "rol    $0x11,%0\n"
            : "=g" (ret)
            : "0" (addr));
    return ret;
}

void ThreadScheduler::setup_thread(int tid, char *stack, thread_entry_point entry_point) {
    // initializes env[tid] to use the right stack, and to run from the function 'entry_point', when we'll use
    // siglongjmp to jump into the thread.
    address_t sp = (address_t) stack + STACK_SIZE - sizeof(address_t);
    auto pc = (address_t) entry_point;
    int ret = sigsetjmp(env[tid], 1);
    if (ret == 0) {
        (env[tid]->__jmpbuf)[JB_SP] = translate_address(sp);
        (env[tid]->__jmpbuf)[JB_PC] = translate_address(pc);
        sigemptyset(&env[tid]->__saved_mask);
    }
}


ThreadScheduler::ThreadScheduler(int _quantum_usecs)
        : quantum_usecs(_quantum_usecs), elapsed_quantums(1), n_threads(1), RUNNING_id(0) {
    // initializing threads array
    threads_arr = new Thread *[MAX_THREAD_NUM];

    // setup main thread
    threads_arr[0] = new Thread();
    sigsetjmp(env[0], 1);

    // init the rest of the threads array as null ptrs
    for (int i = 1; i < MAX_THREAD_NUM; i++) {
        threads_arr[i] = nullptr;
    }
}

int ThreadScheduler::switch_threads(preempted_reason pr) {

    // advance sleeping thread one quantum
    sleeping_threads_handler();

    // add one quantum to total
    elapsed_quantums += 1;

    // add one quantum to the next thread
    threads_arr[queue_READY.front()->id]->elapsed_quantums += 1;

    // set the previous thread to its new state and saving its environment
    switch (pr) {
        case expired_quantum:
            threads_arr[RUNNING_id]->state = READY;
            queue_READY.push(threads_arr[RUNNING_id]);
            sigsetjmp(env[RUNNING_id], 1);

        case blocked_state:
            threads_arr[RUNNING_id]->state = BLOCKED;
            sigsetjmp(env[RUNNING_id], 1);

        case sleeping:
            threads_arr[RUNNING_id]->state = SLEEP;
            sigsetjmp(env[RUNNING_id], 1);

        case terminated:
            break;
    }

    // set up the environment of the previous thread
    setup_thread(RUNNING_id, threads_arr[RUNNING_id]->stack, threads_arr[RUNNING_id]->entry_point);

    // set the next thread to running state
    RUNNING_id = queue_READY.front()->id;
    queue_READY.pop();
    threads_arr[RUNNING_id]->state = RUNNING;

    // get the next thread going
    siglongjmp(env[RUNNING_id], 1);
}


int ThreadScheduler::get_thread_elapsed_quantums(int tid) {
    if (threads_arr[tid] != nullptr) {
        return threads_arr[tid]->elapsed_quantums;
    } else {
        return -1;
    }
}


int ThreadScheduler::get_elapsed_quantums() const {
    return elapsed_quantums;
}


int ThreadScheduler::get_RUNNING_id() const {
    return RUNNING_id;
}


int ThreadScheduler::spawn_thread(thread_entry_point entry_point) {
    // Checking if entry_point is null
    if (entry_point == nullptr) {
        return -1;
    }
    // Searching for an empty slot in threads_arr
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        if (threads_arr[i] == nullptr) {
            threads_arr[i] = new Thread(i, entry_point);
            setup_thread(i, threads_arr[i]->stack, entry_point);
            queue_READY.push(threads_arr[i]);
            n_threads++;
            return i;
        }
    }
    // TODO print an error message
    return -1;
}


int ThreadScheduler::sleep_handler(int num_quantums) {

    // saving the id of the previous thread
    int id_to_deal_with = get_RUNNING_id();

    // it's an error if the main thread calls this function
    if (id_to_deal_with == 0) {
        return -1;
    }

    // get the previous thread sleeping
    threads_arr[id_to_deal_with]->quantums_to_sleep = num_quantums;
    switch_threads(sleeping);
    return 0;
}


int ThreadScheduler::sleeping_threads_handler() {
    for (int i = 1; i < MAX_THREAD_NUM; i++) {
        if (threads_arr[i] == nullptr) {
            continue;
        }
        if (threads_arr[i]->quantums_to_sleep == 0) {
            if (threads_arr[i]->state == SLEEP) {
                threads_arr[i]->state = READY;
                queue_READY.push(threads_arr[i]);
            } else if (threads_arr[i]->state == SNB) {
                threads_arr[i]->state = BLOCKED;
            }
        } else if (threads_arr[i]->quantums_to_sleep > 0) {
            threads_arr[i]->quantums_to_sleep -= 1;
        }
    }
}


int ThreadScheduler::resume_thread(int tid) {
    if (threads_arr[tid] == nullptr) {
        return -1;
    }
    if (threads_arr[tid]->state == SNB) {
        threads_arr[tid]->state = SLEEP;
    } else if (threads_arr[tid]->state == BLOCKED) {
        threads_arr[tid]->state = READY;
        queue_READY.push(threads_arr[tid]);
    }
    return 0;
}

int ThreadScheduler::block_thread(int tid) {
    if (tid == 0) {
        return -1;
    }
    if (threads_arr[tid] == nullptr) {
        return -1;
    }
    if (threads_arr[tid]->state == SLEEP) {
        threads_arr[tid]->state = SNB;
    } else if (threads_arr[tid]->state == READY) {
        threads_arr[tid]->state = BLOCKED;

        std::queue<Thread *> temp;

        while (!queue_READY.empty()) {
            if (queue_READY.front()->id != tid)
                temp.push(queue_READY.front());
            queue_READY.pop();
        }

        while (!temp.empty()) {
            queue_READY.push(temp.front());
            temp.pop();
        }
    }
    if (tid == get_RUNNING_id()) {
        switch_threads(blocked_state);
    }
}


int ThreadScheduler::terminate_thread(int tid) {
    if (tid == 0) {
        // release all resources
        for (int i = 0; i < MAX_THREAD_NUM; i++) {
            if (threads_arr[i] != nullptr) {
                delete threads_arr[i];
            }
        }
        delete[] threads_arr;
        exit(0);
    }
    int quantums_elapsed = get_thread_elapsed_quantums(tid);
    if (quantums_elapsed == -1) {
        return -1;
    }
    delete threads_arr[tid];
    threads_arr[tid] = nullptr;
    n_threads--;

    // if the thread deleted itself, we need to switch threads
    if (tid == RUNNING_id) {
        switch_threads(terminated);
    }
    return quantums_elapsed;


}
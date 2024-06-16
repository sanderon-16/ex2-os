//
// Created by TLP-258 on 15/06/2024.
//

#include <memory>
#include "uthreads.h"
#include "thread_scheduler.h"

std::unique_ptr<ThreadScheduler> scheduler;

struct sigaction sa = {0};
struct itimerval timer;

void timer_handler(int sig) {
    scheduler->switch_threads(expired_quantum);
}

int uthread_init(int quantum_usecs) {
    if (quantum_usecs <= 0) {
        std::cerr << "thread library error: got non-positive number for quantum_usecs." << std::endl;
        return -1;
    }
    scheduler = std::unique_ptr<ThreadScheduler>(new ThreadScheduler());

    // installing the timer handler
    sa.sa_handler = &timer_handler;
    if (sigaction(SIGVTALRM, &sa, NULL) < 0) {
        std::cerr << "thread library error: setitimer install error." << std::endl;
        return -1;
    }

    // configuring the timer to expire after quantum_usecs mircoseconds
    timer.it_value.tv_sec = quantum_usecs / SECOND;
    timer.it_value.tv_usec = quantum_usecs % SECOND;

    timer.it_interval.tv_sec = quantum_usecs / SECOND;
    timer.it_interval.tv_usec = quantum_usecs % SECOND;

    // Start a virtual timer. It counts down whenever this process is executing.
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL)) {
        std::cerr << "system error: setitimer start error." << std::endl;
        exit(1);
    }
    return 0;
}


int uthread_spawn(thread_entry_point entry_point) {
    int ret_value = scheduler->spawn_thread(entry_point);
    if (ret_value == NULL_ENTRY_POINT_ERROR) {
        std::cerr << "thread library error: got a null pointer as thread entry point." << std::endl;
        return -1;
    } else if (ret_value == TOO_MANY_THREADS_ERROR) {
        std::cerr << "thread library error: too many threads were created." << std::endl;
        return -1;
    }
    return ret_value;
}


int uthread_terminate(int tid) {

    // delete all if the main thread is  deleted
    if (tid == 0) {
        //delete &scheduler; is called implicitly in cpp
        exit(0);
    }

    // restart timer if the quantum is forced to end
    if (scheduler->get_RUNNING_id() == tid) {
        if (setitimer(ITIMER_VIRTUAL, &timer, nullptr)) {
            std::cerr << "thread library error: setitimer start error." << std::endl;
            return -1;
        }
    }

    // terminating thread
    int ret_value = scheduler->terminate_thread(tid);
    if (ret_value == INVALID_ID_ERROR) {
        std::cerr << "thread library error: asked to terminate thread with invalid id (0 or illegal)." << std::endl;
        return -1;
    } else if (ret_value == NO_THREAD_WITH_THIS_ID) {
        std::cerr << "thread library error: asked to terminate a non-existing thread." << std::endl;
        return -1;
    }
    return ret_value;
}


int uthread_block(int tid) {

    // restart timer if the quantum is forced to end
    if (scheduler->get_RUNNING_id() == tid) {
        if (setitimer(ITIMER_VIRTUAL, &timer, nullptr)) {
            std::cerr << "thread library error: setitimer start error." << std::endl;
            return -1;
        }
    }

    // blocking thread
    int ret_value = scheduler->block_thread(tid);
    if (ret_value == INVALID_ID_ERROR) {
        std::cerr << "thread library error: asked to block thread with invalid id (0 or illegal)." << std::endl;
        return -1;
    } else if (ret_value == NO_THREAD_WITH_THIS_ID) {
        std::cerr << "thread library error: asked to block a non-existing thread." << std::endl;
        return -1;
    }
    return ret_value;
}


int uthread_resume(int tid) {
    if (scheduler->get_RUNNING_id() == tid) {
        return 0;
    }
    int ret_value = scheduler->resume_thread(tid);
    if (ret_value == INVALID_ID_ERROR) {
        std::cerr << "thread library error: asked to resume thread with invalid id." << std::endl;
        return -1;
    } else if (ret_value == NO_THREAD_WITH_THIS_ID) {
        std::cerr << "thread library error: asked to resume a non-existing thread." << std::endl;
        return -1;
    }
    return ret_value;
}


int uthread_sleep(int num_quantums) {

    // check input validity
    if (num_quantums < 1) {
        std::cerr << "thread library error: asked the thread to sleep non-positive number of quantums." << std::endl;
        return -1;
    }

    // reset the timer:
    if (setitimer(ITIMER_VIRTUAL, &timer, nullptr)) {
        std::cerr << "thread library error: setitimer start error." << std::endl;
        return -1;
    }

    // go to sleep
    int ret_value = scheduler->sleep_handler(num_quantums);
    if (ret_value == INVALID_ID_ERROR) {
        std::cerr << "thread library error: asked the main thread to sleep." << std::endl;
        return -1;
    }
    return ret_value;
}


int uthread_get_tid() {
    return scheduler->get_RUNNING_id();
}


int uthread_get_total_quantums() {
    return scheduler->get_elapsed_quantums();
}


int uthread_get_quantums(int tid) {
    int ret_value = scheduler->get_thread_elapsed_quantums(tid);
    if (ret_value == INVALID_ID_ERROR) {
        std::cerr << "thread library error: asked for the running quantums number of a "
                     "thread with invalid id." << std::endl;
        return -1;
    } else if (ret_value == NO_THREAD_WITH_THIS_ID) {
        std::cerr << "thread library error: asked for the running quantums number of a "
                     "non-existing thread." << std::endl;
        return -1;
    }
    return ret_value;
}
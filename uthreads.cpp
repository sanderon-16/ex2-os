//
// Created by TLP-258 on 15/06/2024.
//

#include <memory>
#include "uthreads.h"
#include "ThreadScheduler.h"

std::unique_ptr<ThreadScheduler> scheduler;

struct sigaction sa;
struct itimerval timer;

void timer_handler(int sig) {
    std::cout << "Quantum expired\n";
    scheduler->switch_threads(expired_quantum);
}

int uthread_init(int quantum_usecs) {
    if (quantum_usecs <= 0) {
        return -1;
    }
    scheduler = std::make_unique<ThreadScheduler>(quantum_usecs);

    // installing the timer handler
    sa.sa_handler = &timer_handler;
    if (sigaction(SIGVTALRM, &sa, nullptr) < 0) {
        std::cerr << "thread library error: setitimer install error.\n" << std::endl;
        return -1;
    }

    // configuring the timer to expire after quantum_usecs mircoseconds
    timer.it_value.tv_sec = quantum_usecs / SECOND;
    timer.it_value.tv_usec = quantum_usecs % SECOND;

    timer.it_interval.tv_sec = quantum_usecs / SECOND;
    timer.it_interval.tv_usec = quantum_usecs % SECOND;

    // Start a virtual timer. It counts down whenever this process is executing.
    if (setitimer(ITIMER_VIRTUAL, &timer, nullptr)) {
        std::cerr << "thread library error: setitimer start error.\n" << std::endl;
        return -1;
    }
    return 0;
}


int uthread_spawn(thread_entry_point entry_point) {
    return scheduler->spawn_thread(entry_point);
}


int uthread_terminate(int tid) {
    scheduler->terminate_thread(tid);
}


int uthread_block(int tid) {
    scheduler->block_thread(tid);
}


int uthread_resume(int tid) {
    return scheduler->resume_thread(tid);
}


int uthread_spawn(thread_entry_point entry_point) {
    return scheduler->spawn_thread(entry_point);
}

int uthread_terminate(int tid) {
    // reset the timer:

    return scheduler->terminate_thread(tid);
}


int uthread_sleep(int num_quantums) {

    // check input validity
    if (num_quantums < 1) {
        return -1;
    }

    // restart timer
    // TODO reset timer

    // go to sleep
    return scheduler->sleep_handler(num_quantums);
}


int uthread_get_tid() {
    return scheduler->get_RUNNING_id();
}


int uthread_get_total_quantums() {
    return scheduler->get_elapsed_quantums();
}


int uthread_get_quantums(int tid) {
    return scheduler->get_thread_elapsed_quantums(tid);
}
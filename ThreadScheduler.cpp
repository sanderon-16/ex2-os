//
// Created by TLP-299 on 14/06/2024.
//

#include "ThreadScheduler.h"

ThreadScheduler::ThreadScheduler(int _quantum_usecs)
        : quantum_usecs(_quantum_usecs), elapsed_quantums(0), n_threads(0), RUNNING_id(-1) {
    threads_arr = new Thread *[MAX_THREAD_NUM];
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        threads_arr[i] = nullptr;
    }
}


int ThreadScheduler::switch_threads() {
    stop_active_thread();
    sleeping_threads_handler();

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
            queue_READY.push(threads_arr[i]);
            n_threads++;
            return i;
        }
    }
    // TODO print an error message
    return -1;
}


int ThreadScheduler::sleep_handler(int num_quantums) const {
    if (get_RUNNING_id() == 0) {
        return -1;
    }
    threads_arr[get_RUNNING_id()]->quantums_to_sleep = num_quantums;
    threads_arr[get_RUNNING_id()]->state = 'B';
}

int ThreadScheduler::stop_active_thread() {
    // TODO fill function
}

int ThreadScheduler::sleeping_threads_handler() {
    for (int i = 1; i < MAX_THREAD_NUM; i++) {
        if (threads_arr[i] == nullptr) {
            continue;
        }
        if (threads_arr[i]->quantums_to_sleep <= 0) {
            continue;
        }
        threads_arr[i]->quantums_to_sleep -= 1;
        if (threads_arr[i]->quantums_to_sleep == 0) {
            if (threads_arr[i]->state == 'S'){
                threads_arr[i]->state = 'R';
                queue_READY.push(threads_arr[i]);
            } else {
                threads_arr[i]->state = 'B';
            }
        }
    }
}
//
// Created by TLP-299 on 14/06/2024.
//

#ifndef _THREADSCHEDULER_H_
#define _THREADSCHEDULER_H_

#include "uthreads.h"
#include "thread.h"
#include <queue>
#include <vector>
#include <csetjmp>
#include <csignal>
#include <sys/time.h>

#include <memory>
#include <iostream>
#define SECOND 1000000

typedef void (*thread_entry_point) (void);

class ThreadScheduler
{
  int quantum_usecs;
  int elapsed_quantums;

  // Thread managing:
  int n_threads;
  int RUNNING_id;
  sigjmp_buf env[MAX_THREAD_NUM];
  Thread **threads_arr;
  std::queue<Thread *> queue_READY;

public:
    /**
     * the constructor of the scheduler.
     * @param _quantum_usecs decides how much time is considered a quantum.
     */
    ThreadScheduler(int _quantum_usecs);

    /**
     * switches the currently running thread.
     */
    int switch_threads();

    /**
     * stops the currently active thread.
     */
    int stop_active_thread();

    /**
     * called during any active-thread switch, decreases the time left for any sleeping
     * thread to sleep, awakens those threads that are supposed to awake.
     */
    int sleeping_threads_handler();

    /**
     * the function creates another thread.
     * @param entry_point is a pointer to the code of the thread.
     * @return 0 for success, -1 otherwise.
     */
    int spawn_thread(thread_entry_point entry_point);

    /**
     * returns the amount of quantums the thread with id tid was running.
     * @param tid the id to look for.
     * @return 0 for success, -1 otherwise.
     */
    int get_thread_elapsed_quantums(int tid);

    /**
     * returns the total amount of quantum passed since the scheduler was initialized.
     * @return as explained.
     */
    int get_elapsed_quantums() const;

    /**
     * returns the id of the running thread.
     * @return as explained.
     */
    int get_RUNNING_id() const;

    /**
     * sets the running thread to sleeping mode.
     * @param num_quantums the amount of time the thread is asked to be sleeping.
     * @return 0 for success, -1 otherwise.
     */
    int sleep_handler(int num_quantums) const;
};

#endif //_THREADSCHEDULER_H_

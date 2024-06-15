//
// Created by TLP-258 on 15/06/2024.
//

#include <memory>
#include "uthreads.h"
#include "ThreadScheduler.h"

std::unique_ptr<ThreadScheduler> scheduler;

struct sigaction sa;
struct itimerval timer;

void timer_handler (int sig)
{
  std::cout << "Quantum expired\n";
  scheduler->switch_threads();
}


int uthread_init(int quantum_usecs) {
    if (quantum_usecs <= 0) {
        return -1;
    }
    scheduler = std::make_unique<ThreadScheduler>(quantum_usecs);
    return 0;
  scheduler = std::make_unique<ThreadScheduler> (quantum_usecs);

  // installing the timer handler
  sa.sa_handler = &timer_handler;
  if (sigaction (SIGVTALRM, &sa, nullptr) < 0)
    {
      //TODO print an error
    }

  // configuring the timer to expire after quantum_usecs mircoseconds
  timer.it_value.tv_sec = quantum_usecs / SECOND;
  timer.it_value.tv_usec = quantum_usecs % SECOND;

  timer.it_interval.tv_sec = quantum_usecs / SECOND;
  timer.it_interval.tv_usec = quantum_usecs % SECOND;

  // Start a virtual timer. It counts down whenever this process is executing.
  if (setitimer (ITIMER_VIRTUAL, &timer, nullptr))
    {
      // TODO print an error
    }
  return 0;
}


/**
 * @brief Returns the number of quantums the thread with ID tid was in RUNNING state.
 *
 * On the first time a thread runs, the function should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state when this function is called, include
 * also the current quantum). If no thread with ID tid exists it is considered an error.
 *
 * @return On success, return the number of quantums of the thread with ID tid. On failure, return -1.
*/
int uthread_get_quantums(int tid) {
    return scheduler->get_thread_elapsed_quantums(tid);
}


/**
 * @brief Returns the total number of quantums since the library was initialized, including the current quantum.
 *
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number should be increased by 1.
 *
 * @return The total number of quantums.
*/
int uthread_get_total_quantums() {
    return scheduler->get_elapsed_quantums();
}


/**
 * @brief Returns the thread ID of the calling thread.
 *
 * @return The ID of the calling thread.
*/
int uthread_get_tid() {
    return scheduler->get_RUNNING_id();
}


/**
 * @brief Blocks the RUNNING thread for num_quantums quantums.
 *
 * Immediately after the RUNNING thread transitions to the BLOCKED state a scheduling decision should be made.
 * After the sleeping time is over, the thread should go back to the end of the READY queue.
 * If the thread which was just RUNNING should also be added to the READY queue, or if multiple threads wake up
 * at the same time, the order in which they're added to the end of the READY queue doesn't matter.
 * The number of quantums refers to the number of times a new quantum starts, regardless of the reason. Specifically,
 * the quantum of the thread which has made the call to uthread_sleep isn’t counted.
 * It is considered an error if the main thread (tid == 0) calls this function.
 *
 * @return On success, return 0. On failure, return -1.
*/
int uthread_sleep(int num_quantums) {
    return scheduler->sleep_handler(num_quantums);
}


int uthread_spawn(thread_entry_point entry_point) {
    return scheduler->spawn_thread(entry_point);
}

int uthread_terminate (int tid)
{
  scheduler->terminate_thread (tid);
}


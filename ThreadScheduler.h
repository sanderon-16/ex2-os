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
  ThreadScheduler (int _quantum_usecs);

  void setup_thread (int tid, char *stack, thread_entry_point entry_point);

  /**
   * @brief Creates a new thread, whose entry point is the function entry_point with the signature
   * void entry_point(void).
   *
   * The thread is added to the end of the READY threads list.
   * The uthread_spawn function should fail if it would cause the number of concurrent threads to exceed the
   * limit (MAX_THREAD_NUM).
   * Each thread should be allocated with a stack of size STACK_SIZE bytes.
   * It is an error to call this function with a null entry_point.
   *
   * @return On success, return the ID of the created thread. On failure, return -1.
  */
  int spawn_thread (thread_entry_point entry_point);
};

#endif //_THREADSCHEDULER_H_

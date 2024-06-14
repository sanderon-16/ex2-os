//
// Created by TLP-299 on 14/06/2024.
//

#ifndef _THREADSCHEDULER_H_
#define _THREADSCHEDULER_H_

#include "uthreads.h"
#include "thread.h"
#include <queue>
#include <vector>


typedef void* Thread;


class ThreadScheduler{
  int quantum_usecs;
  int elapsed_quantums;
  int n_threads;
  int RUNNING_id;
  std::vector<Thread*> threads_arr;
  std::queue<Thread> queue_READY;

 public:
  ThreadScheduler(int _quantum_usecs);
};

#endif //_THREADSCHEDULER_H_

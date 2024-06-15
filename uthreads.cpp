//
// Created by TLP-258 on 15/06/2024.
//

#include <memory>
#include "uthreads.h"
#include "ThreadScheduler.h"

std::unique_ptr<ThreadScheduler> scheduler;

int uthread_init (int quantum_usecs)
{
  if (quantum_usecs <= 0)
    {
      return -1;
    }
  scheduler = std::make_unique<ThreadScheduler> (quantum_usecs);
  return 0;
}

int uthread_spawn (thread_entry_point entry_point)
{
  return scheduler->spawn_thread (entry_point);
}


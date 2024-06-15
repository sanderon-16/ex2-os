//
// Created by TLP-299 on 14/06/2024.
//

#include "ThreadScheduler.h"

ThreadScheduler::ThreadScheduler (int _quantum_usecs)
    : quantum_usecs (_quantum_usecs), elapsed_quantums (0), n_threads (0), RUNNING_id (-1)
{
  threads_arr = new Thread *[MAX_THREAD_NUM];
  for (int i = 0; i < MAX_THREAD_NUM; i++)
    {
      threads_arr[i] = nullptr;
    }
}

int ThreadScheduler::spawn_thread (thread_entry_point entry_point)
{
  // Checking if entry_point is null
  if (entry_point == nullptr)
    {
      return -1;
    }
  // Searching for an empty slot in threads_arr
  for (int i = 0; i < MAX_THREAD_NUM; i++)
    {
      if (threads_arr[i] == nullptr)
        {
          threads_arr[i] = new Thread (i, entry_point);
          queue_READY.push (threads_arr[i]);
          n_threads++;
          return i;
        }
    }
  // TODO print an error message
  return -1;
}
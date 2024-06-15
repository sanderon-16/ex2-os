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
address_t translate_address (address_t addr)
{
  address_t ret;
  asm volatile("xor    %%fs:0x30,%0\n"
               "rol    $0x11,%0\n"
      : "=g" (ret)
      : "0" (addr));
  return ret;
}

void
ThreadScheduler::setup_thread (int tid, char *stack, thread_entry_point entry_point)
{
  // initializes env[tid] to use the right stack, and to run from the function 'entry_point', when we'll use
  // siglongjmp to jump into the thread.
  address_t sp = (address_t) stack + STACK_SIZE - sizeof (address_t);
  auto pc = (address_t) entry_point;
  sigsetjmp(env[tid], 1);
  (env[tid]->__jmpbuf)[JB_SP] = translate_address (sp);
  (env[tid]->__jmpbuf)[JB_PC] = translate_address (pc);
  sigemptyset (&env[tid]->__saved_mask);
}

ThreadScheduler::ThreadScheduler (int _quantum_usecs)
    : quantum_usecs (_quantum_usecs), elapsed_quantums (0), n_threads (0), RUNNING_id (-1)
{
  // initializing threads array
  threads_arr = new Thread *[MAX_THREAD_NUM];
  for (int i = 0; i < MAX_THREAD_NUM; i++)
    {
      threads_arr[i] = nullptr;
    }
  //
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
          setup_thread (i, threads_arr[i]->stack, entry_point);
          queue_READY.push (threads_arr[i]);
          n_threads++;
          return i;
        }
    }
  // TODO print an error message
  return -1;
}


//
// Created by TLP-258 on 15/06/2024.
//
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

int uthread_init (int quantum_usecs)
{
  if (quantum_usecs <= 0)
    {
      return -1;
    }
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

int uthread_spawn (thread_entry_point entry_point)
{
  return scheduler->spawn_thread (entry_point);
}

int uthread_terminate (int tid)
{
  scheduler->terminate_thread (tid);
}


#include "thread.h"


Thread::Thread (int i, thread_entry_point p_function)
{
  id = i;
  entry_point = p_function;
  state = READY;
  elapsed_quantums = 0;
  quantums_to_sleep = 0;
  stack = new char[STACK_SIZE];
}

Thread::Thread()
{
  id = 0;
  entry_point = nullptr;
  state = RUNNING;
  elapsed_quantums = 1;
  quantums_to_sleep = 0;
  stack = nullptr;
}

Thread::~Thread ()
{
  delete[] stack;
}
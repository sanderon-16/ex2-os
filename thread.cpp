

#include "thread.h"
Thread::Thread (int i, thread_entry_point p_function)
{
  id = i;
  entry_point = p_function;
    state = 'R';
    elapsed_quantums = 0;
    stack = new char[STACK_SIZE];
}

Thread::~Thread()
{
    delete[] stack;
}
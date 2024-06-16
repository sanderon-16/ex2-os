guy.lidor, ron_sendrovich
Student guy lidor 213415771, Student ron sanderovich (215019803)
EX: 2

FILES:
uthreads.cpp -- the code that handles calls and requests to a ThreadScheduler class instance.
thread_scheduler.h -- the header file of the ThreadScheduler class with all the class methods.
thread_scheduler.cpp -- our implementation of the ThreadScheduler class. this is the main chunk of code.
thread.h -- the header file of the Thread struct.
thread.cpp -- implementation of the basic constructor and deconstructor of the Thread struct.

ANSWERS:

Q1:
(a). The function sigsetjmp recieves two arguments:
    env - the place where the calling environment is saved.
    savemask - if non-zero then env also saves the signal mask that the thread had.
    Essentially the function works like a bookmark, and siglongjump is like opening the bookmark.
    When siglongjmp is called with the same env that was saved earlier, the code will jump back to the line where
    sigsetjmp was called.
    the return value of sigsetjmp is zero when it is just called.
    the return value of sigsetjmp is non-zero when siglongjump is used to jump to it, and the returned value is
    defined by siglongjmp.
    The function siglongjmp recieves two arguments:
    env - some saved environment that was saved by sigsetjmp
    val - the return value of sigsetjmp when siglongjmp jumps to it.
(b). when setting the parameter savemask to a non-zero value, the env saves the signal mask that the current process
    holds. which means, if somewhere else the process decided to change its signal mask, and the process then calls
    siglongjmp with the previous env, the signal mask will change to its previous state.

Q2:
One use I can think of is a server that runs a real time online video game, that can host multiple players.
I propose that each player's input will be handled separate thread. Assuming a players input is not a difficult thing to process,
it won't need much system resources, and because the inputs are needed to be processed at real time, there will be
a lot of context switching between threads. Using kernel level threads in this example will give us a lot of overhead, because
the calculations are pretty lightweight, Opposed to user level threading, where there's less of a cost in system resources.


Q3:
Advantages:
Each process is isolated from one to another because it has a separate memory space. I can imagine that its more secure,
if there's a vulnerability in one tab that lets you access memory, it wont affect the other tabs.
Also, its more stable, because if one process crashes it wont crash the other processes.
Disadvantages:
Opening multiple tabs is resource heavy, because each tab will need its own allocated memory and cpu, in contrary to
a thread which shares its resources with the other threads.
Also, I assume that running multiple processes on an operating system has to use kernel level threads; The average
chrome user has a lot of idle tabs, and idle tabs probably don't need much computer resources,
so using kernel level threads will cause more overhead against something like user level threads.

Q4:
When i type "kill 3009" (which was the id I got) in the shell, the keyboard makes interruptions and the OS takes care of them
by putting them on the screen.
when pressing enter, the OS gets the interrupt from the keyboard and triggers the shell to send a system call for the OS to
send a signal to the process (the app).
the OS executes this system call.
the process gets the system call, cleans up and terminates.
then, the OS finishes the cleanup.

Q5:
Similarly to Real time, which a time unit (f.e a second) is proportional to the time that it takes the big clock hand
to move once, A virtual time unit is proportional to the time that it takes for the cpu to do one job with no interruptions.
You can use virtual time for CPU scheduling, limiting a process to run only for X cpu jobs.
You can use real time for simulations, computer clocks, and so on. knowing to exchange between virtual time and real time is very useful,
f.e it makes you able run simulations faster than real time, and enables you to make games run in real time (and not cpu time).
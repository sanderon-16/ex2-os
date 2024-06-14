//
// Created by TLP-299 on 14/06/2024.
//

#include "ThreadScheduler.h"

ThreadScheduler::ThreadScheduler (int _quantum_usecs)
    : quantum_usecs (_quantum_usecs), elapsed_quantums (0), n_threads (0), RUNNING_id (-1)
{}
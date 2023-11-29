#pragma once
#include <pthread.h> 

#define THREAD_NUMBER 1


class ThreadManager
{
    private:
        pthread_t threads[THREAD_NUMBER];

    public:
};

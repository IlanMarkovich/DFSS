#pragma once
#include <pthread.h> 

#define THREAD_NUMBER 100


class ThreadManager
{
    private:
        pthread_t threads[THREAD_NUMBER];
        
    public:
        pthread_t& getThread();
        void returnThread();
};

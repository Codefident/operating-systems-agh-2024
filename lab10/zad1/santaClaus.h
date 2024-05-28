// #pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define N_REINDEERS 9
#define N_CYCLES 4

// santa
pthread_t santa_thread;
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
void *santa_handler(void *arg);

// reindeers
int reindeers_counter = 0;
pthread_t reindeers_threads[N_REINDEERS];
pthread_mutex_t reindeers_counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeers_mutex[N_REINDEERS] =
    {PTHREAD_MUTEX_INITIALIZER,
     PTHREAD_MUTEX_INITIALIZER,
     PTHREAD_MUTEX_INITIALIZER,
     PTHREAD_MUTEX_INITIALIZER,
     PTHREAD_MUTEX_INITIALIZER,
     PTHREAD_MUTEX_INITIALIZER,
     PTHREAD_MUTEX_INITIALIZER,
     PTHREAD_MUTEX_INITIALIZER,
     PTHREAD_MUTEX_INITIALIZER};
void *reindeer_handler(void *arg);

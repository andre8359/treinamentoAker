#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "request_lib.h"
#define MSG_SIZE 128
#define NUM_THREADS 2

extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern pthread_t threads[NUM_THREADS];

void init_threads(struct manager_io **manager);
void join_threads();
void destroy_threads();
void create_threads(struct manager_io **manager);
void *thread_func(void *args);
#endif

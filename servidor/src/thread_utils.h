#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include  <fcntl.h>
#include <string.h>
#define NUM_THREADS  1

struct thread_args
{
  int  socket_id;
  int  quit;
  char *file_name;
  char *buffer;
  long offset;
  long size;
  };

extern pthread_mutex_t mutex;
extern pthread_cond_t cond;
extern pthread_t threads[NUM_THREADS];
extern pthread_attr_t attr;

void init_threads();
void join_threads();
void destroy_threads();

void init_thread_args(struct thread_args *args);
void create_threads();
void *thread_func(void *arguments);
#endif

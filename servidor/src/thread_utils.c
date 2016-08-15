#include "thread_utils.h"

pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_t threads[NUM_THREADS];
pthread_attr_t attr;

void init_threads()
{
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init (&cond, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}
void init_thread_args(struct thread_args *args)
{
  args->socket_id = 0;
  args->file_name = NULL;
  args->offset = 0;
  args->buffer = NULL;
  args->size = 0;
  args->quit = 1;
}
void join_threads()
{
  int i;
  for (i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i],NULL);
}
void destroy_threads()
{
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
  pthread_attr_destroy(&attr);
}
void create_threads(struct thread_args *args)
{
  int i = 0;
  for (i = 0; i < NUM_THREADS; i++)
    pthread_create(&threads[i], NULL, thread_func, (void *)args);
}
void *thread_func(void *arguments)
{
  struct thread_args *args = (struct thread_args *) arguments;
  while (args->quit)
  {
    char *msg = "shurembos\n"; 
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    /* FILE IO*/
    write(args->socket_id, msg, strlen(msg));
    /* FILE IO*/
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(NULL);
}

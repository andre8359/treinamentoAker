#include "thread_utils.h"

pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_t threads[NUM_THREADS] = {0};

void init_threads(struct manager_io **manager)
{
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init (&cond, NULL);
  create_threads(manager);
}
void join_threads()
{
  int i;
  for (i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i],NULL);
}
void destroy_threads(struct manager_io ** manager)
{
  pthread_mutex_lock(&mutex);
  (*manager)->quit = 0;
  pthread_mutex_unlock(&mutex);

  pthread_cond_broadcast(&cond);
  join_threads();
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
}
void create_threads(struct manager_io **manager)
{
  int i;
  for (i = 0; i < NUM_THREADS; i++)
    pthread_create(&threads[i], NULL, thread_func, (void *) manager);
}
void *thread_func(void *args)
{
  struct manager_io **manager = (struct manager_io **) args;
  struct manager_io *m = *manager;
  struct request_io *request = NULL;

  while (m->quit)
  {
    pthread_mutex_lock(&mutex);

    while(m->total_request == 0 && m->quit)
      pthread_cond_wait(&cond, &mutex);

      request = dequeue_request_io(manager);
      pthread_mutex_unlock(&mutex);
      if (request == NULL || request->fd == 0)
        continue;

      if (request->method == GET)
        request->size = pread(request->fd,
                              request->buffer,
                              request->size,
                              request->offset);
      else
        request->size = pwrite(request->fd,
                               request->buffer,
                               request->size,
                               request->offset);

      pthread_mutex_lock(&mutex);
      write(m->local_socket, &request, sizeof(struct request_io*));
      pthread_mutex_unlock(&mutex);
  }
  return 0;
}

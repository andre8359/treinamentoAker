#include "server_lib.h"
#include "request_lib.h"
#include "http_utils.h"
#include "socket_utils.h"
#include "thread_utils.h"


int quit = 1;
/*!
 * \brief Libera memoria para finalizar programa.
 */
void clean_up()
{
  quit = 0;
  fprintf(stderr,"SINAL : SIGINT recebido !\n");
  return;
}

int main(int argc,  char *argv[])
{
  int client_socket = 0;
  int i = 0;
  int max_socket = FD_SETSIZE;
  int min_socket = 0;
  int ret = 0;
  int fd[] = {0, 0};
  long speed_limit = 0;
  long port = 0;
  struct request_file *request = NULL;
  struct timeval time_waiting;
  int server_socket = 0;
  int local_socket = 0;
  struct request_file *head = NULL;
  struct manager_io *manager_thread = NULL, *manager_client = NULL;
  fd_set active_read_fd_set, active_write_fd_set, read_fd_set, write_fd_set;

  memset(&time_waiting, 0, sizeof(time_waiting));

  port = params_is_valid(argc, argv, &speed_limit);

  if (port < 0)
    goto on_error;

  if (speed_limit == 0)
    speed_limit = LONG_MAX;

  server_socket = make_connection(port);

  if (server_socket < 0)
    goto on_error;

  if (listen(server_socket, BACKLOG) < 0)
    goto on_error;

  if (make_local_socket(fd) < 0)
    goto on_error;

  local_socket = fd[0];

  manager_thread = (struct manager_io *) calloc(1, sizeof(struct manager_io));
  manager_client = (struct manager_io *) calloc(1, sizeof(struct manager_io));

  manager_thread->quit = 1;
  manager_thread->local_socket = fd[1];

  init_threads(&manager_thread);

  signal(SIGINT,clean_up);

  FD_ZERO (&active_read_fd_set);
  FD_ZERO (&active_write_fd_set);
  FD_ZERO (&read_fd_set);
  FD_ZERO (&write_fd_set);
  FD_SET (server_socket, &active_read_fd_set);
  FD_SET (local_socket, &active_read_fd_set);

  min_socket = min(server_socket, local_socket);
  max_socket = max(server_socket, local_socket) + 1;

  while (quit)
  {
    read_fd_set = active_read_fd_set;
    write_fd_set = active_write_fd_set;

    if (select(max_socket + 1, &read_fd_set, &write_fd_set,NULL,NULL) < 0)
    {
      fprintf(stderr,"Erro ao tentar selecionar sockets!\n");
      goto on_error;
    }

    for (i = min_socket; i <= max_socket; i++)
    {
      if (FD_ISSET (i, &read_fd_set))
      {
        if (i == server_socket)
        {
          client_socket = accept_new_connection(server_socket);

          if (client_socket < 0)
            goto on_error;

          min_socket = min(min_socket, client_socket);
          max_socket = max(max_socket, client_socket);
          FD_SET (client_socket, &active_read_fd_set);
          continue;
        }
        else if (i == local_socket)
        {
          ret = handle_thread_answer(local_socket, &head, &manager_client);

          if(ret > 0)
          {
            request = search_request_file(ret, &head);

            if (request->fd)
              close(request->fd);
            request->fd = 0;
            request->method = GET;
            request->transferred_size = 0;
            request->status = CREATED;
            rename_downloaded_file(request);

            set_std_response(request);
            FD_SET(ret, &active_write_fd_set);
            FD_CLR(ret, &active_read_fd_set);
          }
          else if (ret == ERROR)
          {
            request = search_request_file(ret, &head);
            if (request->fd)
              close(request->fd);
            request->method = GET;
            request->status = INTERNAL_ERROR;
            set_std_response(request);
            FD_SET(ret, &active_write_fd_set);
            FD_CLR(ret, &active_read_fd_set);
          }
          continue;
        }

        ret = receive_from_client(i, &head, &manager_thread, speed_limit);
        if (ret == READY_TO_SEND)
        {
          FD_SET(i, &active_write_fd_set);
          FD_CLR(i, &active_read_fd_set);
        }
      }
      else if (FD_ISSET(i, &write_fd_set))
      {
        send_to_client(i, speed_limit, &manager_client, &head);
        ret = request_read(i, &head, &manager_thread, speed_limit);

        if (ret == ENDED_DOWNLOAD)
        {
          rm_request_file(i, &head);
          FD_CLR(i, &active_write_fd_set);
        }
      }
    }
    gettimeofday(&time_waiting, NULL);
    if (head 
        && (check_speed_limit(head, speed_limit) == ERROR))
      usleep(1e6 - (time_waiting.tv_usec - head->last_pack.tv_usec));
  }

on_error:
  if (manager_thread)
  {
    manager_thread->quit = 0;
    pthread_cond_broadcast(&cond);
    destroy_threads();

    free_request_io_list(&manager_thread);

    if (manager_thread->local_socket > 0)
      close(manager_thread->local_socket);

    free(manager_thread);
  }

  if (manager_client)
  {
    if (manager_client->head)
      free_request_io_list(&manager_client);
    free(manager_client);
  }
  close_std_file_desc();

  free_request_file_list(&head);
  if (server_socket > 0)
    close(server_socket);

  if (local_socket > 0)
    close(local_socket);
  return 0;
}

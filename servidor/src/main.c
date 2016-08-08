#include "server_lib.h"
#include "request_lib.h"
#include "http_utils.h"
#include "socket_utils.h"

int server_socket = 0, end_prog = 1;
struct request_file *head = NULL;

/*!
 * \brief Libera memoria para finalizar programa.
 */
void clean_up()
{
  close_std_file_desc();
  free_request_list(&head);
  if (server_socket)
    close(server_socket);
  end_prog = 0;
}

int main(int argc,  char *argv[])
{
  long port = 0;
  int client_socket = 0, i = 0, max_socket = FD_SETSIZE, min_socket = 0;
  int ret = 0;
  long speed_limit = 0;
  int buf_size = 0;
  struct timeval time_out, time_waiting;
  memset(&time_waiting, 0, sizeof(time_waiting));
  time_out.tv_sec = 1;
  time_out.tv_usec = 0;
  fd_set active_read_fd_set, active_write_fd_set, read_fd_set, write_fd_set;
  port = params_is_valid(argc, argv, &speed_limit);

  if (port < 0)
    goto on_error;

  if (speed_limit == 0)
    speed_limit = LONG_MAX;
  buf_size = calc_buf_size(speed_limit);

  server_socket = make_connection(port);

  if (server_socket < 0)
    goto on_error;

  if (listen(server_socket, 1) < 0)
    goto on_error;

  signal(SIGINT,clean_up);

  FD_ZERO (&active_read_fd_set);
  FD_ZERO (&active_write_fd_set);
  FD_ZERO (&read_fd_set);
  FD_ZERO (&write_fd_set);
  FD_SET (server_socket, &active_read_fd_set);
  min_socket = server_socket;
  max_socket = server_socket+1;
  time_out.tv_sec = 1;
  time_out.tv_usec = 0;

  while (end_prog)
  {
    read_fd_set = active_read_fd_set;
    write_fd_set = active_write_fd_set;
    if (select(max_socket+1, &read_fd_set, &write_fd_set, NULL,&time_out) < 0)
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
          
          min_socket = min(min_socket,client_socket);
          min_socket = max(min_socket,0);
          max_socket = max(max_socket,client_socket);
          max_socket = min(max_socket,FD_SETSIZE);
          FD_SET (client_socket, &active_read_fd_set);
          continue;
        }
        ret = receive_request_from_client(i, &head, speed_limit);
        if (ret == READY_TO_SEND)
        {
          FD_SET(i, &active_write_fd_set);
          FD_CLR(i, &active_read_fd_set);
        }
        else 
        {
        }
      }
      else if (FD_ISSET(i, &write_fd_set))
        if (send_to_client(i, &head, speed_limit) == 0)
        {
          rm_request(i, &head);
          FD_CLR(i, &active_write_fd_set);
        }
    }
    gettimeofday(&time_waiting, NULL);
    if (head && (head->transf_last_sec + buf_size) > speed_limit)
      usleep(1e6 - (time_waiting.tv_usec - head->last_pack.tv_usec));
    time_out.tv_sec = 1;
    time_out.tv_usec = 0;
  }

on_error:
  return 0;
}

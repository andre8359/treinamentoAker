#include <stdio.h>
#include "server_lib.h"
#include "request_lib.h"
#include "http_utils.h"

int socket_id = 0;
struct request_file *head = NULL;

/*!
 * \brief Libera memoria para finalizar programa.
 */
void clean_up()
{
  close_std_file_desc();
  free_request_list(&head);
  if (socket_id)
    close(socket_id);
  exit(EXIT_FAILURE);
}
int main(int argc,  char *argv[])
{
  long port = 0;
  int new_socket_id = 0, i = 0, max_socket = FD_SETSIZE, min_socket = 0;
  fd_set active_read_fd_set, active_write_fd_set, read_fd_set, write_fd_set;
  port = params_is_valid(argc, argv);
  if (port <= 0)
    clean_up();
  socket_id = make_connection(port);
  if (socket_id < 0)
    goto on_error;
  if (listen(socket_id, 1) < 0)
    goto on_error;
  signal(SIGINT,clean_up); 
  FD_ZERO (&active_read_fd_set);
  FD_ZERO (&active_write_fd_set);
  FD_ZERO (&read_fd_set);
  FD_ZERO (&write_fd_set);
  FD_SET (socket_id, &active_read_fd_set);
  min_socket = socket_id;
  max_socket = socket_id;

  while (1)
  {
    read_fd_set = active_read_fd_set;
    write_fd_set = active_write_fd_set;
    if (select(FD_SETSIZE, &read_fd_set, &write_fd_set, NULL, NULL) < 0)
    {
      fprintf(stderr,"Erro ao tentar selecionar sockets!\n");
      exit(EXIT_FAILURE);
    }
    for (i = min_socket; i <= max_socket; i++)
    {
      if (FD_ISSET (i, &read_fd_set))
      {
        if (i == socket_id)
        {
          new_socket_id = accept_new_connection(socket_id);
          if (new_socket_id < 0)
            goto on_error;
          min_socket = min(min_socket,new_socket_id);
          min_socket = max(min_socket,0);
          max_socket = max(max_socket,new_socket_id);
          max_socket = min(max_socket,FD_SETSIZE);
          FD_SET (new_socket_id, &active_read_fd_set);
        }
        else
        {
           if (i == socket_id)
             break;
           else if (receive_request_from_client(i, &head) == 0)
           {
             FD_SET(i, &active_write_fd_set);
             FD_CLR(i, &active_read_fd_set);
           } 
        }
      }
      else if (FD_ISSET(i, &write_fd_set))
      {
        if (write_to_client(i, &head) == 0)
        {
          rm_request(i, &head);
          FD_CLR(i, &active_write_fd_set);
        }
      }
    }
  }  
  close_std_file_desc();
on_error:
  if (socket_id)
    close(socket_id);
  if (new_socket_id)
    close(new_socket_id);
  return 0;
}

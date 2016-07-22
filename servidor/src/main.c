#include <stdio.h>
#include "server_lib.h"
#include "request_lib.h"
#include "http_utils.h"
int main()
{
  const char port[] = "8081";
  char bufin[BUFSIZE], bufout[BUFSIZE];
  int socket_id = 0, new_socket_id = 0, i = 0;
  fd_set active_fd_set, read_fd_set, write_fd_set;
  socket_id = make_connection(port);
  struct request_file *head = NULL;
  if (socket_id < 0)
    goto on_error;
  if (listen(socket_id , 1) < 0)
    goto on_error;
  
  FD_ZERO (&active_fd_set);
  FD_ZERO (&write_fd_set);
  FD_SET (socket_id, &active_fd_set);

  while (1)
  {
    read_fd_set = active_fd_set;
    if (select(FD_SETSIZE, &read_fd_set, &write_fd_set, NULL, NULL) < 0)
    {
      fprintf(stderr,"Erro ao tentar selecionar sockets!\n");
      exit(EXIT_FAILURE);
    }
    for (i = 0; i < FD_SETSIZE; i++)
    {
      if (FD_ISSET (i, &read_fd_set))
      {
        if (i == socket_id)
        {
          new_socket_id = accept_new_connection(socket_id);
          FD_SET (new_socket_id, &active_fd_set);
        }
        else
        {
          char *request = receive_request_from_client(i);
          if (request == NULL)
            break;
          char *file_name =  get_resquest_info(request);
          close(i);
          FD_CLR(i, &read_fd_set);
          break;
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

#include <stdio.h>
#include "server_lib.h"

struct 
{
  FILE *fp;
  char *file_name;
  char *header;
  unsigned long header_size_sended;
  unsigned long file_size, sended_size;
  int socket_id, status_request;
  
} request_file;

int main()
{
  const char port[] = "8080";
  char bufin[BUFSIZE], bufout[BUFSIZE];
  struct sockaddr_in client_info;
  socklen_t client_len;
  int socket_id = 0, new_socket_id = 0, i = 0;
  fd_set active_fd_set, read_fd_set, write_fd_set;
  socket_id = make_connection(port);

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
          client_len = sizeof (client_info);
          new_socket_id = accept (socket_id,(struct sockaddr *) &client_info, \
            &client_len);
          if (new_socket_id < 0)
          {
            fprintf (stderr,"accept");
            exit (EXIT_FAILURE);
          }
          fprintf (stderr, "Server: connect from host %s, port %hd.\n",
            inet_ntoa (client_info.sin_addr), ntohs (client_info.sin_port));
          FD_SET (new_socket_id, &active_fd_set);
        }
        else
        {
          memset(bufin, 0, BUFSIZE);
          recv(i, bufin, BUFSIZE, 0);
          fprintf(stdout,"RECEBENDO --- %s\n",bufin);
          if (!strncmp(bufin,"quit",4))
          {
            close (i);
            FD_CLR (i, &read_fd_set);
          }
          else if(!strncmp(bufin, "recebedor",9 ))
          {
            FD_CLR (i,&read_fd_set);
            FD_SET (i, &write_fd_set);
          }   
          memset(bufout, 0, BUFSIZE);
          strncpy(bufout, bufin, strlen(bufin));     
        }
      }
      else if (FD_ISSET(i, &write_fd_set))
      {
        if (strlen(bufout) > 1)
        {
           fprintf(stdout,"ENVIANDO --- %s\n", bufout);
           send(i, bufout, strlen(bufout), 0);
           
           if(!strncmp(bufout, "quit",9 ))
           {
             close(i);
             FD_CLR (i,&active_fd_set);
           }
           memset(bufout, 0, BUFSIZE); 
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

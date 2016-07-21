#include <stdio.h>
#include "server_lib.h"
int main()
{
  const char port[] = "8081";
  char bufin[BUFSIZE];
  struct sockaddr_in client_info;
  socklen_t client_len;
  int socket_id = 0, new_socket_id = 0;
  socket_id = wait_connection(port); 
  if (socket_id < 0)
    goto on_error;
  memset(&client_info, 0, sizeof(client_info));
  client_len = sizeof(client_info);
  new_socket_id = accept(socket_id, (struct sockaddr *) &client_info, \
    &client_len);
  if(new_socket_id < 0)
    goto on_error;
  fprintf(stdout, "\n\nConectado!\n\n");
  while (1)
  {
    recv(new_socket_id, bufin, BUFSIZE, 0);
    fprintf(stdout,"%s\n", bufin);
    if (!strncmp(bufin,"quit",4))
      break;
  }
  close_std_file_desc();
on_error:
  if (socket_id)
    close(socket_id);
  if (new_socket_id)
    close(new_socket_id);
  return 0;
}

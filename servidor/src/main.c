#include "server_lib.h"
#include "request_lib.h"
#include "http_utils.h"
#include "socket_utils.h"
#include "thread_utils.h"


int quit = 1;
int reload_config = 0;
/*!
 * \brief Libera memoria para finalizar programa.
 */
void clean_up()
{
  quit = 0;
  fprintf(stderr,"SINAL : SIGINT recebido !\n");
  return;
}
void config_server()
{
  reload_config = 1;
  fprintf(stderr,"SINAL : SIGHUP recebido !\n");
  return;
}

int main(int argc,  char *argv[])
{
  char *new_root_dir = NULL;
  int client_socket = 0;
  int i = 0;
  int local_socket = 0;
  int max_socket = FD_SETSIZE;
  int min_socket = 0;
  int ret = 0;
  int server_socket = 0;
  long speed_limit = 0;
  long port = 0;
  long new_port = 0;
  long new_sp = 0;
  struct request_file *head = NULL;
  struct manager_io *manager_thread = NULL;
  struct manager_io *manager_client = NULL;
  struct timeval time_start;
  struct timeval time_last_transf;
  struct timeval time_out;
  struct sockaddr_un server_addr;
  fd_set active_read_fd_set;
  fd_set active_write_fd_set;
  fd_set read_fd_set;
  fd_set write_fd_set;

  memset(&time_start, 0, sizeof(time_start));
  memset(&time_last_transf, 0, sizeof(time_last_transf));

  port = params_is_valid(argc, argv, &speed_limit);

  if (port < 0)
    goto on_error;

  if (speed_limit == 0)
    speed_limit = LONG_MAX;

  server_socket = make_listening_socket(port);
  if (server_socket < 0)
    goto on_error;

  local_socket = make_local_socket(LOCAL_SOCKET_NAME,strlen(LOCAL_SOCKET_NAME));
  if (local_socket < 0)
    goto on_error;

  manager_thread = (struct manager_io *) calloc(1, sizeof(struct manager_io));
  manager_client = (struct manager_io *) calloc(1, sizeof(struct manager_io));

  manager_thread->quit = 1;
  manager_thread->local_socket = make_local_socket(CLIENT_LOCAL_SOCKET_NAME,
                                                  strlen(CLIENT_LOCAL_SOCKET_NAME));
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = PF_LOCAL;
  strncpy(server_addr.sun_path, LOCAL_SOCKET_NAME, strlen(LOCAL_SOCKET_NAME));

  if (connect(manager_thread->local_socket, (struct sockaddr *) &server_addr,
              sizeof(server_addr)) < 0)
    goto on_error;

  init_threads(&manager_thread);
  
  if (write_config_file(port,speed_limit))
    goto on_error;

  signal(SIGINT, clean_up);
  signal(SIGHUP, config_server);

  FD_ZERO (&active_read_fd_set);
  FD_ZERO (&active_write_fd_set);
  FD_ZERO (&read_fd_set);
  FD_ZERO (&write_fd_set);
  FD_SET (server_socket, &active_read_fd_set);
  FD_SET (local_socket, &active_read_fd_set);
  min_socket = min(server_socket, local_socket);
  max_socket = max(server_socket, local_socket) + 1;

  read_fd_set = active_read_fd_set;
  write_fd_set = active_write_fd_set;

  time_out.tv_sec = LONG_MAX;
  time_out.tv_usec = 0;

  while (quit)
  {
    if (reload_config)
    {
      new_root_dir = read_config_file(&new_port, &new_sp);
      fprintf(stderr, "%s - %ld - %ld \n", new_root_dir, new_port, new_sp); 
      if (check_config_params(new_root_dir, new_port, new_sp))
        goto on_config_error;

      change_root_directory(new_root_dir);
      port = new_port;
      if (new_sp == 0)
        speed_limit = LONG_MAX;
      else
        speed_limit = new_sp;
      
      if (server_socket)
        close(server_socket);
      server_socket = make_listening_socket(port);

      FD_SET (server_socket, &active_read_fd_set);
      min_socket = min(server_socket, min_socket);
      max_socket = max(server_socket, max_socket) + 1;
on_config_error:
      reload_config = 0;
      if (new_root_dir)
        free(new_root_dir);
    }

    ret = select(max_socket + 1, &read_fd_set, &write_fd_set,NULL, &time_out);
    if (ret < 0)
    {
      if (errno == EINTR) 
        continue; 

      fprintf(stderr,"Erro ao tentar selecionar sockets! %s\n",strerror(errno));
      goto on_error;
    }

    gettimeofday(&time_start, NULL);

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
            handle_end_upload(ret, &head);
            FD_SET(ret, &active_write_fd_set);
            FD_CLR(ret, &active_read_fd_set);
          }
          else if (ret == ERROR)
          {
            handle_server_error(i , &head);
            FD_SET(ret, &active_write_fd_set);
            FD_CLR(ret, &active_read_fd_set);
          }

          continue;
        }

        ret = receive_from_client(i, &head, &manager_thread, speed_limit);
        gettimeofday(&time_last_transf, NULL);
        if (ret == READY_TO_SEND)
        {
          FD_SET(i, &active_write_fd_set);
          FD_CLR(i, &active_read_fd_set);
        }
        else if(ret == ENDED_UPLOAD_UNCOMPLETED)
        {
          handle_uncompleted_transf(i, &head);
          FD_CLR(i, &active_read_fd_set);
        }
      }
      else if (FD_ISSET(i, &write_fd_set))
      {
        ret = request_read(i, &head, &manager_thread, speed_limit);
        ret = send_to_client(i, speed_limit, &manager_client, &head);

        gettimeofday(&time_last_transf, NULL);

        if (ret == ENDED_DOWNLOAD || ret == ENDED_DOWNLOAD_UNCOMPLETED)
        {
          rm_request_file(i, &head);
          FD_CLR(i, &active_write_fd_set);
        }
      }
    }

    ret = diff_time(&time_out, &time_start, &time_last_transf);

    if ((head) && (!ret) && check_speed_limit(head, speed_limit)
        && (time_out.tv_usec <  SECOND_TO_MICROSEC)
        && (time_last_transf.tv_sec !=0))
    {

      time_out.tv_usec = SECOND_TO_MICROSEC - time_out.tv_usec;
      FD_ZERO (&read_fd_set);
      FD_SET (server_socket, &read_fd_set);
      FD_ZERO(&write_fd_set);
    }
    else
    {
      time_out.tv_sec = LONG_MAX;
      time_out.tv_usec = 0;
      read_fd_set = active_read_fd_set;
      write_fd_set = active_write_fd_set;
    }

  }

on_error:
  if (manager_thread)
  {
    destroy_threads(&manager_thread);

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

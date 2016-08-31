/*!
 * \file   server_lib.c
 * \brief  Arquivo de implementação das funcoes para o recupe
 * \date 18/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */
#include "server_lib.h"

static int prepare_request_io(struct request_file *request,
                              struct request_io *request_thread,
                              int buf_size)
{
  const int file_name_size = strlen(request->request);
  char file_name[file_name_size];

  if (request->fd <= 0)
  {
    if (request->method == GET)
    {
      request->fd = open(request->file_name,O_RDONLY);
      if (request->fd <= 0)
        return ERROR;
      memset(request->buffer, 0, BUFSIZE);
    }
    else
    {
      snprintf(file_name, file_name_size, "%s~part", request->file_name);

      request->fd = open(file_name,O_WRONLY|O_CREAT, S_IRUSR | S_IWUSR);
      if (request->fd <= 0)
        return ERROR;
    }
  }
  request_thread->socket_id = request->socket_id;
  request_thread->fd = request->fd;
  request_thread->method = request->method;
  request_thread->buffer = request->buffer;
  request_thread->offset = request->transferred_size;
  request_thread->next = NULL;
  buf_size = (buf_size > (request->file_size - request->transferred_size)
              ? request->file_size - request->transferred_size : buf_size);

  request_thread->size = buf_size;
  request->sended_last_pack = 1;

  return SUCCESS;
}
static int split_request_from_data(struct request_file *request,
                                   struct manager_io **manager)
{
  char *begin_data = NULL;
  int buf_size = 0;
  struct request_io request_thread;

  begin_data = find_end_request(request->request);

  if (begin_data == NULL || *begin_data == '\0')
    return ERROR;

  buf_size =  request->transf_last_sec - (begin_data - request->request);

  memset(request->buffer, 0, BUFSIZE);
  memcpy(request->buffer,begin_data, buf_size);

  memset(&request_thread, 0, sizeof(struct request_io));

  prepare_request_io(request, &request_thread, buf_size);

  pthread_mutex_lock(&mutex);
  enqueue_request_io(manager, &request_thread);
  pthread_mutex_unlock(&mutex);

  pthread_cond_signal(&cond);

  return SUCCESS;
}
static int get_info_after_end_request(struct request_file *request,
                                      struct manager_io **manager)
{
  char *ch = find_end_request(request->request);
  if (ch != NULL)
  {
    check_request_info(request);

    if (request->status == OK)
    {
      if (request->method == GET)
        check_file_ready_to_send(request);
      else
        check_file_ready_to_receive(request);
    }

    if (request->method == GET || request->status  != OK)
      return READY_TO_SEND;

    split_request_from_data(request, manager);
    return READY_TO_RECEIVE;
  }

  return ERROR;
}
static int check_if_transf_end(struct request_file *request)
{
  if ((request->transferred_size >= request->file_size && request->file_size))
    return SUCCESS;
  return ERROR;
}

int check_speed_limit(struct request_file* request, long speed_limit)
{
  calc_if_sec_had_pass(&request);

  if (request->transf_last_sec + calc_buf_size(speed_limit) <= speed_limit)
    return SUCCESS;
  return ERROR;
}

static int rename_downloaded_file(struct request_file *request)
{
  char downloaded_file_name[PATH_MAX];

  if (request == NULL)
    return ERROR;

  sprintf(downloaded_file_name, "%s~part", request->file_name);
  return rename(downloaded_file_name, request->file_name);
}
static int delete_uncompleted_downloaded_file(struct request_file *request)
{

  char downloaded_file_name[PATH_MAX];

  if (request == NULL)
    return ERROR;

  sprintf(downloaded_file_name, "%s~part", request->file_name);
  return unlink(downloaded_file_name);
}
static struct request_file *set_method_equals_get(const int socket_id,
                                                  struct request_file **head)
{
  struct request_file *request = NULL;

  request = search_request_file(socket_id, head);
  if (request == NULL)
    return NULL;

  if (request->fd)
    close(request->fd);

  request->method = GET;
  return request;
}

static int check_if_file_is_forbidden(char *file_name)
{
  char root_directory[PATH_MAX];
  char abs_path[ PATH_MAX];

  getcwd(root_directory,PATH_MAX);
  realpath(file_name, abs_path);

  if (strstr(abs_path, root_directory) == NULL)
    return ERROR;
  return SUCCESS;
}
static int check_if_can_read_write_file(char *file_name, int flag)
{
  if (access(file_name, flag) < 0)
    return ERROR;
  return SUCCESS;
}

int check_if_is_directory(const char *path_file)
{
  struct stat path_stat;
  stat(path_file, &path_stat);
  if (S_ISDIR(path_stat.st_mode))
    return SUCCESS;
  return ERROR;
}
static int check_if_file_exists(char *file_name)
{
  if (access(file_name, F_OK) != -1 )
    return SUCCESS;
  return ERROR;
}
/*!
 * \brief Recebe a requisicao do cliente conectado.
 * \param[in] socket_id Descritor do socket da conexao.
 * \param[in] head Ponteiro para o primeiro item da lista de requisicoes.
 * \return Retorna 0 caso tenha recebido o fim da requisicao ou -1 caso nao.
 */
int receive_request_from_client(const int socket_id,
                                struct request_file **head,
                                struct manager_io **manager,
                                long speed_limit)
{
  int bufin_size = calc_buf_size(speed_limit);
  char bufin[bufin_size + 1];
  int nbytes = 0;
  int received_size = 0;
  struct request_file *request = NULL;

  request = search_request_file(socket_id, head);
  if (request == NULL)
    request = add_request_file(socket_id, head);

  memset(bufin, 0, bufin_size + 1);

  if (check_speed_limit(request,speed_limit))
    return ERROR;


  nbytes = recv(socket_id, bufin, bufin_size, 0);
  if (nbytes <= 0)
    return ENDED_DOWNLOAD_UNCOMPLETED;

  request->transf_last_sec += nbytes;

  received_size = (request->request != NULL) ? strlen(request->request) : 0;

  request->request = realloc(request->request, received_size + nbytes + 1);
  memcpy(request->request + received_size, bufin, nbytes);
  request->request[received_size + nbytes] = '\0';

  return get_info_after_end_request(request, manager);
}

int receive_from_client(const int socket_id,
                        struct request_file **head,
                        struct manager_io **manager,
                        long speed_limit)
{
  int buf_size = calc_buf_size(speed_limit);
  int nbytes = 0;
  struct request_file *request = NULL;
  struct request_io request_thread;
  request = search_request_file(socket_id, head);

  if (request == NULL || find_end_request(request->request) == NULL)
    return receive_request_from_client(socket_id, head, manager, speed_limit);

  if (check_if_transf_end(request) == SUCCESS)
    return ENDED_UPLOAD;

  if (check_speed_limit(request, speed_limit) || request->sended_last_pack)
    return ERROR;

  memset(request->buffer, 0, buf_size);

  buf_size = (buf_size > (request->file_size - request->transferred_size)) ?
    (request->file_size - request->transferred_size) : buf_size ;

  nbytes = recv(request->socket_id, request->buffer, buf_size, 0);
  if (nbytes <= 0)
    return ENDED_UPLOAD_UNCOMPLETED;

  prepare_request_io(request, &request_thread, nbytes);

  pthread_mutex_lock(&mutex);
  enqueue_request_io(manager, &request_thread);
  pthread_mutex_unlock(&mutex);

  pthread_cond_signal(&cond);

  return READY_TO_RECEIVE;
}

/*!
 * \brief Envia header pro cliente conectado.
 * \param[in] socket_id Descritor do socket da conexao.
 * \param[in] head Ponteiro para o primeiro item da lista de requisicoes.
 * \return Retorna 0 caso tenha recebido o fim da requisicao ou -1 caso nao.
 */
static int send_header_to_client(struct request_file **r, long speed_limit)
{
  const int bufin_size = calc_buf_size(speed_limit);
  int send_size = 0, nbytes = 0;
  struct request_file *request = *r;

  if (request->header == NULL)
    request->header = make_header(request);

  if ((unsigned)request->header_size_sended >= strlen(request->header))
    return SUCCESS;

  send_size = strlen(request->header) - request->header_size_sended;

  send_size = (send_size > bufin_size) ? bufin_size : send_size;

  if (check_speed_limit(request, speed_limit))
    return ERROR;

  nbytes = send(request->socket_id,
                request->header + request->header_size_sended,
                send_size,
                MSG_NOSIGNAL);

  if (nbytes > 0)
    request->header_size_sended += nbytes;

  return SUCCESS;
}
/*!
 * \brief Envia informacoes pro cliente conectado.
 * \param[in] socket_id Descritor do socket da conexao.
 * \param[in] head Ponteiro para o primeiro item da lista de requisicoes.
 * \return Retorna 0 caso tenha recebido o fim da requisicao ou -1 caso nao.
 */
int request_read(const int socket_id,
                 struct request_file **head,
                 struct manager_io **manager,
                 long speed_limit)
{
  int buf_size = calc_buf_size(speed_limit);
  struct request_file *request = NULL;
  struct request_io request_thread;

  request =  search_request_file(socket_id, head);

  if (request == NULL)
    return ERROR;

  if (check_if_transf_end(request) == SUCCESS)
    return ENDED_DOWNLOAD;

  if (send_header_to_client(&request, speed_limit)
      || request->sended_last_pack == 1)
    return ERROR;

  memset(&request_thread, 0, sizeof(struct request_io));

  prepare_request_io(request, &request_thread, buf_size);

  pthread_mutex_lock(&mutex);
  enqueue_request_io(manager, &request_thread);
  pthread_mutex_unlock(&mutex);

  pthread_cond_signal(&cond);

  return ERROR;
}
int handle_thread_answer(const int local_socket,
                         struct request_file **head,
                         struct manager_io **manager_client)
{
  struct request_io *request_thread = NULL;
  struct request_file *request = NULL;
  int nbytes = 0;
  int ret = 0;
  struct timeval tv;
  fd_set local_fd;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO(&local_fd);
  FD_SET(local_socket, &local_fd);

  while (1)
  {
    ret = select(local_socket + 1, &local_fd, NULL, NULL, &tv);
    if (ret < 0)
      return ERROR;

    if (ret == 0)
      break;

    memset(&request_thread, 0, sizeof(struct request_io *));
    nbytes = read (local_socket,
                   &request_thread,
                   sizeof(struct request_io *));

    if (nbytes < (signed)sizeof(struct request_io *))
    {
      fprintf(stderr, "ERRO : socket local!\n");
      return ERROR;
    }

    if (request_thread->method == GET)
      enqueue_request_io(manager_client, request_thread);
    else
    {
      request = search_request_file(request_thread->socket_id , head);
      if (request == NULL || request_thread->size < 0)
        return ERROR;
      request->transf_last_sec += request_thread->size;
      request->transferred_size += request_thread->size;
      request->sended_last_pack = 0;

      if (check_if_transf_end(request) == SUCCESS)
      {
        free_request_io(&request_thread);
        return request->socket_id;
      }
    }

    free_request_io(&request_thread);

    tv.tv_sec = 0;
    tv.tv_usec = 0;
  }
  return SUCCESS;
}

int send_to_client(const int socket_id,
                   const long speed_limit,
                   struct manager_io **manager_client,
                   struct request_file **head)
{
  int nbytes = 0;
  struct request_io *request_client = NULL;
  struct request_file *request = NULL;

  if ((*manager_client)->total_request <= 0)
    goto on_error;

  request_client = dequeue_request_io_with_socket_id(socket_id,
                                                     manager_client);
  if (request_client == NULL)
    goto on_error;

  request = search_request_file(socket_id, head);
  if (request == NULL)
    goto on_error;

  if (check_speed_limit(request, speed_limit) == ERROR)
  {
    enqueue_request_io(manager_client, request_client);
    goto on_error;
  }

  nbytes = send(request_client->socket_id,
                request_client->buffer,
                request_client->size,
                MSG_NOSIGNAL);

  if (nbytes <= 0 )
    return ENDED_DOWNLOAD_UNCOMPLETED;

  request->transf_last_sec += nbytes;
  request->transferred_size += nbytes;
  request->sended_last_pack = 0;

  if (check_if_transf_end(request) == SUCCESS)
  {
    free_request_io(&request_client);
    return ENDED_DOWNLOAD;
  }
on_error:
  if (request_client)
    free_request_io(&request_client);
  return ERROR;
}

int handle_uncompleted_transf(const int socket_id, struct request_file **head)
{
  struct request_file *request = NULL;

  request = search_request_file(socket_id, head);
  if (request == NULL)
    return ERROR;

  delete_uncompleted_downloaded_file(request);
  rm_request_file(socket_id, head);
  return SUCCESS;
}
int handle_end_upload(const int socket_id, struct request_file **head)
{
  struct request_file *request = NULL;

  request = set_method_equals_get(socket_id, head);
  if (request == NULL)
    return ERROR;

  request->transferred_size = 0;
  memset(request->buffer, 0, BUFSIZE);

  if (check_if_file_exists(request->file_name) == SUCCESS)
    request->status = OK;
  else
    request->status = CREATED;

  rename_downloaded_file(request);

  set_std_response(request);
  return SUCCESS;
}
int handle_server_error(const int socket_id, struct request_file **head)
{
  struct request_file *request = NULL;
  request = set_method_equals_get(socket_id, head);
  if (request == NULL)
    return ERROR;

  request->status = INTERNAL_ERROR;
  set_std_response(request);
  return SUCCESS;
}
/*!
 * \brief Change the current working directoy
 * \param[in] root_directory Diretorio que sera considera a raiz do servidor.
 * \return Retorna 0 caso tenha mudado com sucesso ou -1 caso nao.
 */
int change_root_directory(const char *root_directory)
{
  if (check_if_is_directory(root_directory))
    return ERROR;

  if ((chdir(root_directory)) < 0)
    return ERROR;

  if (create_default_response_files() < 0)
    return ERROR;

  return SUCCESS;
}
/*!
 * \brief Checa se o arquivo existe e pode ser enviado.
 * \param[in] request Estrutura com as informacoes do arquivo requisitado.
 * \return 0 para sucesso e <0 para falha.
 */
int check_file_ready_to_send(struct request_file * request)
{
  if (request->file_name == NULL)
    return ERROR;
  if (check_if_file_exists(request->file_name))
  {
    request->status = NOT_FOUND;
    goto on_error;
  }
  if (check_if_is_directory(request->file_name) == SUCCESS)
  {
    request->status = BAD_REQUEST;
    goto on_error;
  }
  if (check_if_file_is_forbidden(request->file_name))
  {
    request->status = FORBIDDEN;
    goto on_error;
  }
  if (check_if_can_read_write_file(request->file_name, R_OK))
  {
    request->status = UNAUTHORIZED;
    goto on_error;
  }
  return SUCCESS;
on_error:
  set_std_response(request);
  return ERROR;
}
/*!
 * \brief Checa pode ser recebido.
 * \param[in] request Estrutura com as informacoes do arquivo requisitado.
 * \return 0 para sucesso e <0 para falha.
 */
int check_file_ready_to_receive(struct request_file * request)
{
  char file_name_part[PATH_MAX];
  char *last_slash = NULL;
  char abs_path[PATH_MAX];

  if (request->file_name == NULL)
    return ERROR;

  if (check_if_file_is_forbidden(request->file_name))
  {
    request->status = FORBIDDEN;
    goto on_error;
  }

  realpath(request->file_name, abs_path);
  if (strstr(abs_path, request->file_name))
  {
    last_slash = strrchr(abs_path,'/');
    memset(last_slash, 0, strlen(last_slash));
  }

  if (check_if_file_exists(abs_path))
  {
    request->status = FORBIDDEN;
    goto on_error;
  }

  if (check_if_is_directory(request->file_name) == SUCCESS)
  {
    request->status = BAD_REQUEST;
    goto on_error;
  }

  if (check_if_can_read_write_file(abs_path, W_OK))
  {
    request->status = UNAUTHORIZED;
    goto on_error;
  }
  snprintf(file_name_part, strlen(request->file_name) + strlen("~part") + 1,
           "%s~part", request->file_name);

  if (check_if_file_exists(file_name_part) == SUCCESS)
  {
    request->status = CONFLICT;
    goto on_error;
  }

  return SUCCESS;
on_error:
  request->method = GET;
  set_std_response(request);
  return ERROR;
}

/*!
 * \brief Fecha os arquivos abertos pelo SO (STDIN, STDOUT, STDERR).
 */
void close_std_file_desc()
{
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}
/*!
 * \brief Cria processo pra rodar o Daemon.
 */
void open_background_process()
{
  pid_t pid;
  pid = fork();
  if (pid < 0)
    exit(EXIT_FAILURE);
  if (pid > 0)
    exit(EXIT_SUCCESS);
}
/*!
 * \brief Retorna o maior entre dois inteiros.
 * \param[in] a Valor para comparacao.
 * \param[in] b Valor para comparacao.
 * \return O maior valor entre os dois.
 */
int max(const int a , const int b)
{
  if (a > b)
    return (a > FD_SETSIZE ? FD_SETSIZE : a);
  return (b > FD_SETSIZE ? FD_SETSIZE : b);
}
/*!
 * \brief Retorna o menor entre dois inteiros.
 * \param[in] a Valor para comparacao.
 * \param[in] b Valor para comparacao.
 * \return O menor valor entre os dois.
 */
int min(const int a , const int b)
{
  if (a < b)
    return (a < 1 ? 1 : a);
  return (b < 1 ? 1 : b);

}
/*!
 * \brief Realiza a coleta dos parametros de entrada.
 * \param[in]  argc Numero de parametros passados na linha de comando.
 * \param[in]  argv Lista de parametros passados na linha de comando.
 * \param[out] port String com informacao sobre a porta da conexao.
 * \param[out] root_directory Diretorio que sera considerado raiz do servidor.
 * \return  0 em caso de parametros validos -1 caso parametros.
 */
static int get_param(int argc, char *argv[], char **port,char **root_directory,
                     char **speed_limit)
{
  int c;
  opterr = 0;
  if (argc < 5)
    goto on_error;
  while ((c = getopt(argc, argv, "p:d:l:")) != -1)
  {
    switch (c)
    {
    case 'p':
      *port = optarg;
      break;
    case 'd':
      *root_directory = optarg;
      break;
    case 'l':
      *speed_limit = optarg;
    case '?':
      if (optopt == 'p')
        goto on_error;
      else if (optopt == 'd')
        goto on_error;
      else if (optopt == 'l')
        goto on_error;
      else if (isprint(optopt))
      {
        printf("Opcao desconhecida '-%c'.", optopt);
        goto on_error;
      }
      break;
    default:
      goto on_error;
    }
  }
  return SUCCESS;
on_error:
  return ERROR;
}
/*!
 * \brief Realiza a validacao dos parametros de entrada.
 * \param[in]  argc Numero de parametros passados na linha de comando.
 * \param[in]  argv Lista de parametros passados na linha de comando.
 * \return  0 em caso de parametros validos -1 caso parametros.
 */
long params_is_valid(int argc , char *argv[], long *speed_limit)
{
  char *port = NULL;
  char *root_directory = NULL;
  char *end = NULL;
  char *sp_limit = NULL;
  long port_int = 0;
  const int base = 10;
  
  int ret = get_param(argc, argv, &port, &root_directory, &sp_limit);

  if ( ret < 0)
    goto on_error;

  if (port == NULL || root_directory == NULL)
    goto on_error;

  errno = 0;

  port_int = strtol(port, &end, base);
  if ((errno == ERANGE) || check_if_valid_port(port_int))
    goto on_error;
  
  if (sp_limit)
  {
    errno = 0;
    *speed_limit = strtol(sp_limit, &end, base);
    if ((errno == ERANGE) || (*speed_limit <= 0))
      goto on_error;
  }

  if (change_root_directory(root_directory) < 0)
  {
    fprintf(stderr, "\n\nNao foi possivel definir esse diretorio como raiz!\n");
    goto on_error;
  }

  return port_int;

on_error:
  fprintf(stderr, "%s%s%s%s%s",
          "Linha de comando incompleta :\n",
          "\t\t ./prog -p <PORTA> -d <DIRETORIO> -l <limite de velocidade>\n\n",
          "Lembre que caso o valor da porta seja menor que 1023 ",
          " o programa necessitara de permissoes de super usuario. Range de ",
          "portas 1-65535.\nO limite de velocidatde e dado em bytes/s\n");
  return ERROR;
}

int check_if_valid_port(int port)
{
  const int port_range_max = 65535;
  const int necessary_root_perms = 1024;

  if ((port <= 0) || (port > port_range_max))
    return ERROR;
  if ((port < necessary_root_perms) && (getuid() != 0))
    return ERROR;
  return SUCCESS;
}

/*!
 * \brief Calcula se passou um segundo desde a ultima requisicao.
 * [in] r Estrutura com informacoes da requisicao.
 * \return O caso tenha passado 1 ou mais segs -1 caso nao.
 */
void calc_if_sec_had_pass(struct request_file **r)
{
  struct request_file *request = *r;
  struct timeval now, result;

  if (request->last_pack.tv_sec == 0 && request->last_pack.tv_usec == 0)
    gettimeofday(&(request->last_pack), NULL);

  gettimeofday(&now, NULL);

  diff_time(&result, &(request->last_pack), &now);

  if (result.tv_usec > SECOND_TO_MICROSEC)
  {
    request->transf_last_sec = 0;
    gettimeofday(&(request->last_pack), NULL);
  }
}
/*!
 * \brief Calcula se passou um segundo desde a ultima requisicao.
 * [in] r Estrutura com informacoes da requisicao.
 * \return O caso tenha passado 1 ou mais segs -1 caso nao.
 */
int calc_buf_size(long speed_limit)
{
  if (speed_limit >= BUFSIZE)
    return BUFSIZE;
  else if (speed_limit != 0)
    return (int) speed_limit;
  else
    return ERROR;
}
int diff_time(struct timeval *result, struct timeval *x, struct timeval *y)
{
  result->tv_sec = 0;
  result->tv_usec = ((y->tv_sec * SECOND_TO_MICROSEC) + y->tv_usec)
                    - ((x->tv_sec * SECOND_TO_MICROSEC) + x->tv_usec);
  return result->tv_usec < 0;
}

int check_config_params(char *root_directory, long port, long speed_limit)
{
  if (check_if_is_directory(root_directory))
    return ERROR; 

  if (check_if_valid_port(port)) 
    return ERROR;

  if (speed_limit < 0)
    return ERROR;

  return SUCCESS;
}

char *read_config_file(long *port, long *speed_limit)
{
  char input[PATH_MAX];
  const int input_numbers = 3;
  FILE *fp;
  int ret = 0;
  pid_t pid;

  fp = fopen(CONFIG_FILE_PATH, "r");
  
  if (fp == NULL)
    return NULL;
  ret = fscanf(fp, "%d\n%s\n%ld\n%ld", &pid, input, port, speed_limit);
  if (ret < input_numbers)
    return NULL; 
  fclose(fp);
  
  return str_dup(input);
}
int write_config_file(long port, long speed_limit)
{
  FILE *fp;
  char root_dir[PATH_MAX];

  fp = fopen(CONFIG_FILE_PATH, "w");
  if (fp == NULL)
    return ERROR;

  if (getcwd(root_dir,PATH_MAX) == NULL)
    return ERROR;

  fprintf(fp,"%d\n%s\n%ld\n%ld\n",getpid(),root_dir, port, speed_limit);

  fclose(fp);
  return SUCCESS; 
}

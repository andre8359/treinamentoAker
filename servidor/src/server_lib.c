/*!
 * \file   server_lib.c
 * \brief  Arquivo de implementação das funcoes para o recupe
 * \date 18/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */
#include "server_lib.h"

static int write_file(struct request_file *request,
                      char *bufin,
                      int bufin_size)
{
  int nbytes = 0;
  const int file_name_size = strlen(request->request) + strlen("~part") + 1;
  char file_name[file_name_size];
  if (request->fd <= 0)
  {
    snprintf(file_name, file_name_size, "%s~part",request->file_name);
    request->fd = open(file_name,O_WRONLY|O_CREAT);
    if (request->fd <= 0)
      return ERROR;
  }

  nbytes = pwrite(request->fd, bufin, bufin_size, request->transferred_size);

  return nbytes;
}

static int read_file(struct request_file *request,
                      char *bufin,
                      int bufin_size)
{
  int nbytes = 0;

  if (request->fd <= 0)
  {
    request->fd = open(request->file_name,O_RDONLY, request->transferred_size);
    if (request->fd <= 0)
      return ERROR;
  }

  nbytes = pread(request->fd , bufin, bufin_size, request->transferred_size);

  return nbytes;
}
static int split_request_from_data(struct request_file *request)
{
  char *begin_data = NULL;
  int bufin_size = 0, nbytes = 0;

  begin_data = find_end_request(request->request);

  if (begin_data == NULL)
    return ERROR;

  bufin_size =  request->transf_last_sec - (begin_data - request->request);

  nbytes = write_file(request, begin_data, bufin_size);

  if (nbytes < 0)
    return ERROR;

  request->transferred_size += nbytes;
  return SUCCESS;

}
/*!
 * \brief Recebe a requisicao do cliente conectado.
 * \param[in] socket_id Descritor do socket da conexao.
 * \param[in] head Ponteiro para o primeiro item da lista de requisicoes.
 * \return Retorna 0 caso tenha recebido o fim da requisicao ou -1 caso nao.
 */
int receive_request_from_client(const int socket_id,
                                struct request_file **head,
                                long speed_limit)
{
  int bufin_size = calc_buf_size(speed_limit);
  char bufin[bufin_size + 1];
  int nbytes = 0, received_size = 0;
  struct request_file *request = NULL;

  request = search_request(socket_id, head);
  if (request == NULL)
    request = add_request(socket_id, head);

  if ((request->transferred_size >= request->file_size
      && request->file_size))
  {
    close(request->fd);
    request->fd = 0;
    return ENDED_UPLOAD;
  }

  if (find_end_request(request->request))
    return ERROR;

  memset(bufin, 0, bufin_size + 1);

  calc_if_sec_had_pass(&request);

  if (request->transf_last_sec < speed_limit)
  {
    nbytes = recv(socket_id, bufin, bufin_size, 0);
    request->transf_last_sec += nbytes;
  }
  else
    return ERROR;

  if (request->request != NULL)
    received_size = strlen(request->request);
  else
    received_size = 0;

  request->request = realloc(request->request, received_size + nbytes + 1);
  memcpy(request->request + received_size, bufin, nbytes);
  request->request[received_size + nbytes] = '\0';

  if (find_end_request(request->request) != NULL)
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

    split_request_from_data(request);
    if (request->transferred_size >= request->file_size)
    {
      close(request->fd);
      request->fd = 0;
      return ENDED_UPLOAD;
    }

    return READY_TO_RECEIVE;
  }

  return ERROR;
}

int receive_from_client(const int socket_id,
                        struct request_file **head,
                        long speed_limit)
{
  int bufin_size = calc_buf_size(speed_limit);
  char bufin[BUFSIZE];
  int nbytes = 0, ret = 0;
  struct request_file *request = NULL;

  request = search_request(socket_id, head);
  if (request == NULL)
  {
    ret = receive_request_from_client(socket_id, head, speed_limit);
    return ret;
  }

  if (request->transf_last_sec < speed_limit)
  {
    memset(bufin, 0, bufin_size);
    if (bufin_size > (request->file_size - request->transferred_size))
       bufin_size = (request->file_size - request->transferred_size);
    nbytes = recv(request->socket_id, bufin, bufin_size, 0);

    nbytes = write_file(request, bufin, nbytes);
    pthread_cond_signal(&cond);
    if (nbytes <= 0)
      return ENDED_UPLOAD;

    request->transferred_size += nbytes;

    if (request->transferred_size >= request->file_size)
      return ENDED_UPLOAD;

    return ERROR;
  }
  return SUCCESS;
}
int rename_downloaded_file(struct request_file *request)
{
  char downloaded_file_name[PATH_MAX];

  if (request == NULL)
    return ERROR;
  
  sprintf(downloaded_file_name, "%s~part", request->file_name);
  return rename(downloaded_file_name,request->file_name);
}
/*!
 * \brief Envia header pro cliente conectado.
 * \param[in] socket_id Descritor do socket da conexao.
 * \param[in] head Ponteiro para o primeiro item da lista de requisicoes.
 * \return Retorna 0 caso tenha recebido o fim da requisicao ou -1 caso nao.
 */
static int send_header_to_client( struct request_file **r,
                                  long speed_limit)
{
  const int bufin_size = calc_buf_size(speed_limit);
  int send_size = 0, nbytes = 0;
  struct request_file *request = *r;

  if (request->header == NULL)
    request->header = make_header( request->file_name,
                                   request->status,
                                   &(request->file_size));

  if ((unsigned)request->header_size_sended >= strlen(request->header))
    return SUCCESS;

  send_size = strlen(request->header) - request->header_size_sended;

  if (send_size > bufin_size)
    send_size =  bufin_size;

  if ((request->transf_last_sec + send_size) < speed_limit)
  {
    nbytes = send( request->socket_id,
                   request->header + request->header_size_sended,
                   send_size,
                   MSG_NOSIGNAL);

    if (nbytes <= 0)
      return SUCCESS;

    request->header_size_sended += nbytes;
    return ERROR;
  }

  return SUCCESS;
}
/*!
 * \brief Envia informacoes pro cliente conectado.
 * \param[in] socket_id Descritor do socket da conexao.
 * \param[in] head Ponteiro para o primeiro item da lista de requisicoes.
 * \return Retorna 0 caso tenha recebido o fim da requisicao ou -1 caso nao.
 */
int send_to_client( const int socket_id,
                    struct request_file **head,
                    long speed_limit)
{
  const int bufin_size = calc_buf_size(speed_limit);
  int nbytes = 0;
  struct request_file *request = NULL;
  char bufin[bufin_size];
  request =  search_request(socket_id, head);

  if (request == NULL)
    return ERROR;

  if (send_header_to_client(&request, speed_limit))
    return ERROR;

  if (request->transferred_size  >= request->file_size)
    return SUCCESS;

  memset(bufin, 0, bufin_size);

  calc_if_sec_had_pass(&request);

  if ((request->transf_last_sec + bufin_size) > (unsigned) speed_limit)
    return ERROR;

  nbytes = read_file(request, bufin, bufin_size);
  nbytes = send(socket_id, bufin, nbytes, MSG_NOSIGNAL);

  if (nbytes <= 0 )
    return SUCCESS;

  request->transferred_size += nbytes;
  request->transf_last_sec += nbytes;
  return ERROR;
}
/*!
 * \brief Change the current working directoy
 * \param[in] root_directory Diretorio que sera considera a raiz do servidor.
 * \return Retorna 0 caso tenha mudado com sucesso ou -1 caso nao.
 */
int change_root_directory(const char *root_directory)
{
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
  char root_directory[PATH_MAX];
  char abs_path[PATH_MAX];
  struct stat path_stat;

  if (request->file_name == NULL)
    return ERROR;

  if (access(request->file_name, F_OK) != -1 )
  {
    stat(request->file_name, &path_stat);
    if (S_ISDIR(path_stat.st_mode))
    {
      request->status = BAD_REQUEST;
      goto on_error;
    }
    getcwd(root_directory,PATH_MAX);
    realpath(request->file_name, abs_path);
    if (strstr(abs_path, root_directory) == NULL)
    {
      request->status = FORBIDDEN;
      goto on_error;
    }
    if (access(request->file_name, R_OK) < 0)
    {
      request->status = UNAUTHORIZED;
      goto on_error;
    }
  }
  else
  {
    request->status = NOT_FOUND;
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
  char root_directory[PATH_MAX];
  char abs_path[PATH_MAX];
  char file_name_part[PATH_MAX];
  char *last_slash = NULL; 
  struct stat path_stat;

  if (request->file_name == NULL)
    return ERROR;
  
  getcwd(root_directory,PATH_MAX);
  realpath(request->file_name, abs_path);

  if (strstr(abs_path, root_directory) == NULL)
  {
    request->status = FORBIDDEN;
    goto on_error;
  }
  last_slash = strrchr(abs_path,'/');
  memset(last_slash, 0, strlen(last_slash));
  if (access(abs_path, F_OK) != -1 )
  {
    stat(request->file_name, &path_stat);
    if (S_ISDIR(path_stat.st_mode))
    {
      request->status = BAD_REQUEST;
      goto on_error;
    }

    if (access(abs_path, W_OK) < 0)
    {
      request->status = UNAUTHORIZED;
      goto on_error;
    }
    snprintf(file_name_part, strlen(request->file_name) + strlen("~part") + 1,
            "%s~part", request->file_name);
    
    if (access(file_name_part, F_OK) == 0)
    {
      request->status = CONFLICT;
      goto on_error;
    }
  }
  else
  {
    request->status = FORBIDDEN;
    goto on_error;
  }

  return SUCCESS;
on_error:
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
    return (a < 0 ? 0 : a);
  return (b < 0 ? 0 : b);

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
  char *port = NULL, *root_directory = NULL, *end = NULL, *sp_limit = NULL;
  long port_int = 0;
  const int base = 10, port_range_max = 65535;
  int ret = get_param(argc, argv, &port, &root_directory, &sp_limit);

  if ( ret < 0)
    goto on_error;

  if (port == NULL || root_directory == NULL)
    goto on_error;

  errno = 0;

  port_int = strtol(port, &end, base);
  if ((errno == ERANGE && (port_int == LONG_MAX || port_int == LONG_MIN))
      || (port_int <= 0 || port_int > port_range_max))
    goto on_error;

  if (port_int < 1024 &&  strncmp(argv[0],"sudo", 4))
      goto on_error;

  if (sp_limit)
  {
    errno = 0;

    *speed_limit = strtol(sp_limit, &end, base);

    if ((errno == ERANGE && (*speed_limit == LONG_MAX 
                             || *speed_limit == LONG_MIN)) 
                             || (*speed_limit <= 0))
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
/*!
 * \brief Calcula se passou um segundo desde a ultima requisicao.
 * [in] r Estrutura com informacoes da requisicao.
 * \return O caso tenha passado 1 ou mais segs -1 caso nao.
 */
void calc_if_sec_had_pass(struct request_file **r)
{
  struct timeval now;
  double sec = 0;

  if ((*r)->last_pack.tv_sec == 0 && (*r)->last_pack.tv_usec == 0)
    gettimeofday(&((*r)->last_pack), NULL);

  gettimeofday(&now, NULL);
  sec = (now.tv_sec - (*r)->last_pack.tv_sec);

  if (sec >= 1)
  {
    (*r)->transf_last_sec = 0;
    gettimeofday(&((*r)->last_pack), NULL);
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
    return (long) BUFSIZE/speed_limit;
  else
    return ERROR;
}

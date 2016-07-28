/*!
 * \file   server_lib.c
 * \brief  Arquivo de implementação das funcoes para o recupe
 * \date 18/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */
#include "server_lib.h"
/*!
 * \brief Cria um socket.
 * \param[in]  p Estrutura que contem as informacoes necessarias para abertura 
 *  do socket.
 * \return Retona o descritor do socket em caso de sucesso ou ERROR em caso de 
 *  falha.
 */
int create_socket(const struct sockaddr_in *p)
{
  int socket_id = 0;
  if ((socket_id = socket(p->sin_family, SOCK_STREAM, 0)) == -1) 
    return ERROR;
  return socket_id;
}
/*!
 * \brief Seta as informacoes necessarias para criar um socket.
 */
void config_connection(const long port, struct sockaddr_in *serv_info)
{ 
  serv_info->sin_family = AF_INET;           
	serv_info->sin_addr.s_addr = INADDR_ANY; 	
	serv_info->sin_port = htons(port); 
}
/*!
 * \brief Cria socket e faz o bind com a porta especificada.
 * \param[in]  port  Inteiro que indica em qual porta a conexao acontecera. 
 * \return Retona o descritor do socket em caso de sucesso ou ERROR em caso de 
 *  falha.
 */
int make_connection(const long port)
{
  struct sockaddr_in serv_info;
  int socket_id = 0;
  socklen_t len = 0;
  memset(&serv_info, 0, sizeof(serv_info)); 
  config_connection(port, &serv_info);
  socket_id = create_socket(&serv_info);
  if (socket_id < 0)
    return ERROR;
  len = sizeof(serv_info);
  if (bind(socket_id, (struct sockaddr *) &serv_info, len) < 0)
    return ERROR;
  return socket_id; 
}
/*!
 * \brief Aceita novas conexoes.
 * \param[in]  socket_id Descritor do socket onde ocoreu o bind.
 * \return Retona o descritor do socket em caso de sucesso ou ERROR em caso de 
 *  falha.
 */
int accept_new_connection(const int socket_id)
{
  struct sockaddr_in client_info;
  socklen_t client_len;
  int new_socket_id;
  client_len = sizeof (client_info);                                    
  new_socket_id = accept (socket_id,(struct sockaddr *) &client_info, \
    &client_len);                                                       
  if (new_socket_id < 0)                                                
    return ERROR;                                                     
  fprintf (stderr, "Conexao aberta: conectado em host %s, porta %hd.\n",      
  inet_ntoa (client_info.sin_addr), ntohs (client_info.sin_port));
  return new_socket_id;
}
/*!
 * \brief Copia o vetor src no vertor dst
 * \param[out] dst Vetor modificado na copia.
 * \param[in] src Vetor que sera copiado.
 * \param[in] begin  Indica a posicao inicial do vertor dst.
 * \param[in] length Indica quantos elementos serao copiados
 */
void vector_cpy (char *dst, const char *src, const int begin, const int length)
{
  int i, j = begin;
  for (i = 0; i <= length; i++)
    dst[i+j] = src[i];
}
/*!
 * \brief Recebe a requisicao do cliente conectado.
 * \param[in] socket_id Descritor do socket da conexao.
 * \param[in] head Ponteiro para o primeiro item da lista de requisicoes.
 * \return Retorna 0 caso tenha recebido o fim da requisicao ou -1 caso nao.
 */
int receive_request_from_client(const int socket_id, 
  struct request_file **head)
{
  char *bufin = NULL;
  int nbytes = 0, received_size = 0; 
  struct request_file *request = NULL;
  
  request = search_request(socket_id, head);
  if (request == NULL)
    request = add_request(socket_id, head);
  bufin = (char *) malloc ((BUFSIZE + 1) * sizeof(char));
  if (bufin == NULL)
    goto on_error;   
  memset(bufin, 0, BUFSIZE + 1);
  if (!calc_if_seg_had_pass(request))
  {
    nbytes = recv(socket_id, bufin, BUFSIZE, 0);
    request->last_pack = clock();
  } 
  else 
    goto on_error;
  if (request->request != NULL)
      received_size = strlen(request->request);
  else
      received_size = 0;
  request->request = realloc(request->request, received_size + nbytes + 1);
  vector_cpy(request->request, bufin, received_size, nbytes - 1);
  request->request[received_size + nbytes ] = '\0';
  if (!find_end_request(request->request))
  {
    request->file_name = get_resquest_info(request);
    if (request->file_name == NULL || check_file_ready_to_send(request) < 0)
      set_std_response(request);
    free(bufin);
    return SUCCESS;
  }
  
on_error:
  free(bufin);
  return ERROR;
} 
/*!
 * \brief Envia informacoes pro cliente conectado.
 * \param[in] socket_id Descritor do socket da conexao.
 * \param[in] head Ponteiro para o primeiro item da lista de requisicoes.
 * \return Retorna 0 caso tenha recebido o fim da requisicao ou -1 caso nao.
 */
int write_to_client (const int socket_id, struct request_file **head)
{
  int nbytes = 0;
  struct request_file *request = NULL;
  char *bufin = NULL;
  request =  search_request(socket_id, head);

  if (request == NULL)
    return ERROR;

  if (request->header == NULL)
    request->header = make_header(request->file_name,request->status_request, 
      &(request->file_size));
  if (request->header_size_sended < strlen(request->header))
  {
    if (!calc_if_seg_had_pass(request))
    {
      int send_size = strlen(request->header) - request->header_size_sended;
      if (send_size > BUFSIZE)
        send_size = BUFSIZE;
      nbytes = send(socket_id, request->header + request->header_size_sended, 
        send_size, MSG_NOSIGNAL);  
        request->last_pack = clock();
      if (nbytes <= 0 )
        return SUCCESS;
      request->header_size_sended += nbytes;
      return ERROR;
    }
    else 
      return ERROR;
  }
  if (request->sended_size < request->file_size)
  {
    if (!calc_if_seg_had_pass(request))
    {
      bufin = (char *) malloc ((BUFSIZE + 1) * sizeof(char));
      if (bufin == NULL)
        goto on_error;   
      memset(bufin, 0, BUFSIZE);
      if(request->fp == NULL)
      {
        request->fp = fopen(request->file_name,"r");
        if (request->fp == NULL)
          goto on_error;
      }
            nbytes = fread(bufin, 1, BUFSIZE,request->fp);
      nbytes = send(socket_id, bufin, nbytes, MSG_NOSIGNAL);
      request->last_pack = clock();
      if (bufin != NULL)
        free(bufin);
      if (nbytes <= 0 ) 
        return SUCCESS;
      request->sended_size += nbytes;
      goto on_error;
    }
    else
      return ERROR;
  }
  else
    return SUCCESS;
on_error:
  if (bufin != NULL)
    free(bufin);
  return ERROR;
} 
/*!
 * \brief Change the current working directory
 * \param[in] root_diretory Diretorio que sera considera a raiz do servidor.
 * \return Retorna 0 caso tenha mudado com sucesso ou -1 caso nao.
*/
int change_root_directory(const char *root_directory)
{
  if ((chdir(root_directory)) < 0) 
  {
    fprintf(stderr,"Nao foi possivel mudar diretorio root!");
    return ERROR;
  }
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
  
  if (access(request->file_name, F_OK) != -1) 
  {
    char root_diretory[PATH_MAX];
    getcwd(root_diretory,PATH_MAX);
    char abs_path[PATH_MAX];
    realpath(request->file_name, abs_path);
    if (strstr(abs_path, root_diretory) == NULL)
    {
      request->status_request = FORBIDDEN;
      return ERROR;
    }
    if (access(request->file_name, R_OK) < 0)
    { 
      request->status_request = UNAUTHORIZED;
      return ERROR;
    }
  }
  else
  {
    request->status_request = NOT_FOUND;
    return ERROR;
  }
  return SUCCESS; 
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
    return a;
  else 
    return b;
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
    return a;
  else 
    return b;
}
/*!
 * \brief Realiza a coleta dos parametros de entrada.
 * \param[in]  argc Numero de parametros passados na linha de comando.
 * \param[in]  argv Lista de parametros passados na linha de comando.
 * \param[out] port String com informacao sobre a porta da conexao.
 * \param[out] root_directory Diretorio que sera considerado a raiz do servidor.
 * \return  0 em caso de parametros validos -1 caso parametros.
 */
static int get_param(int argc, char *argv[], char **port, char **root_directory)
{
  int c;
  opterr = 0;
  if (argc < 5)
    goto on_error;
  while ((c = getopt(argc, argv, "p:d:")) != -1)
  {
    switch (c)
    {
    case 'p':
      *port = optarg;
      break;
    case 'd':
      *root_directory = optarg;
      break;
    case '?':
      if (optopt == 'p')
        goto on_error;
      else if (optopt == 'd')
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
   return 0;
on_error:
  return ERROR;
}
/*!
 * \brief Realiza a validacao dos parametros de entrada.
 * \param[in]  argc Numero de parametros passados na linha de comando.
 * \param[in]  argv Lista de parametros passados na linha de comando. 
 * \return  0 em caso de parametros validos -1 caso parametros.
 */
long params_is_valid(int argc , char *argv[])
{
  char *port, *root_diretory, *end_port;
  long port_int = 0;
  const int base = 10, port_range_max = 65535;
  int ret = get_param(argc, argv, &port, &root_diretory);
  if ( ret < 0)
    goto on_error;
  if (port == NULL || root_diretory == NULL)
    goto on_error;

  errno = 0;
  port_int = strtol(port, &end_port, base);
  if ((errno == ERANGE && (port_int == LONG_MAX || port_int == LONG_MIN))
     || (port_int <= 0 || port_int > port_range_max))
    goto on_error; 
  if (change_root_directory(root_diretory) < 0)
  {
    fprintf(stderr, "Nao foi possivel definir esse diretorio como raiz!\n");
    return ERROR;
  }
  return port_int;
on_error:
  fprintf(stderr, "%s%s%s%s%s",
    "Linha de comando incompleta :\n",
    "\t\t ./prog -p <PORTA> -d <DIRETORIO RAIZ>\n\n", 
    "Lembre que caso o valor da porta seja menor que 1023 ",
    " o programa necessitara de permissoes de super usuario.",
    " Range de portas 1-65535.\n");
  return ERROR;
}
/*!
 * \brief Calcula se passou um segundo desde a ultima requisicao.
 * \param[in] r Estrutura com informacoes da requisicao.
 * \return O caso tenha passado 1 ou mais segs -1 caso nao.
 */
int calc_if_seg_had_pass(struct request_file *r)
{
  if (r->last_pack == 0)
    r->last_pack = clock();
  clock_t time_pass  = clock() - r->last_pack;
  if ( (((double)time_pass)/CLOCKS_PER_SEC) > 1)
    return SUCCESS;
  else
    return ERROR;
}

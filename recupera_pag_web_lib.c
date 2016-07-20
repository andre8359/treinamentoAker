/*!
 * \file   recupera_pag_web_lib.c
 * \brief  arquivo de implementação das funcoes para o  recuperador de 
           paginas WEB
 * \date 11/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */
#include "recupera_pag_web_lib.h"
/*!
 * \brief Realiza a validacao dos parametros de entrada
 * \param[in]  url  URL do arquivo para download.
 * \param[in]  file_name   Nome do arquivo de saida. 
 * \param[in]  overwrite_flag Flag que indica a possibilidade de sobrescrita.
 * \return  0 em caso de parametros validos <0 caso parametros 
 */
int params_is_valid(const char *url, const char *file_name, \
  const int overwrite_flag)
{
  if (strncmp(url,"http://",7))
    return ERROR_PARAM_BAD_FORMULATED_URL;
  if (strlen(file_name) <= 3 && (!strncmp(file_name, ".", 3)\
    || !strncmp(file_name, "..", 3)))
    return ERROR_PARAM_BAD_FORMULATED_FILENAME;
  if (access(file_name, F_OK) != -1)
  {
    if (overwrite_flag == 0)
      return ERROR_PARAM_BAD_FOUMULATED_FILE_EXIST;
    if (access(file_name, W_OK) == -1)
      return ERROR_PARAM_BAD_FOUMULATED_FILE_ACCESS_DENIED;
  }
  return 0;
}
/*!
 * \brief Mostra a mensagem de erro adequada na tela.
 * \param[in]  error  numero associado ao tipo de erro.
 */
void show_error_message(const int error)
{
  const char *error_message [] = 
  { "\nLinha de comando incompleta: ./prog -u <URI>  -f <arquivo_de_saida> \
 [OPTIONAL]-o\n\n",
    "\nErro! URL mal formulada.\n\n", 
    "\nErro!Nome do arquivo de saida mal formulado!\n\n",
    "\nO arquivo ja existe, caso deseje sobrescrever adicione a '-o' ao fim da\
 linha de comando. \n\n",
    "\nO arquivo informado existe e possui restricoes para escrita. \n\n",
    "\nErro!Servidor desconhecido!\n\n",
    "\nErro!Servidor movido permanentemente!\n\n",
    "\nErro!Servidor movido temporariamente!\n",
    "\nErro [BAD REQUEST]! Parametros mal formulados.\n\n",                                              
    "\nErro!Conexao nao autorizada.\n\n",
    "\nErro!Conexao proibida.\n\n",
    "\nErro!Servido nao encontrado.\n\n",
    "\nErro!!\n\n",
  };
  if (error >= 0) 
    printf("%s", error_message[error]);
  else
    printf("%s",error_message[12]);
}
/*!
 * \brief Cria um socket.
 * \param[in]  p Estrutura que contem as informacoes necessarias para abertura 
 *  do socket.
 * \return Retona o descritor do socket em caso de sucesso ou ERROR em caso de 
 *  falha.
 */
int create_socket(const struct addrinfo *p)
{
  int socket_id = 0;
  if ((socket_id = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) 
    {
      return ERROR;
    }
  return socket_id;
}
/*!
 * \brief Seta as informacoes necessarias para criar um socket.
 * \param[out]  hints Estruta que recebe as informacoes.
 */
void config_connection(struct addrinfo *hints)
{
  memset(hints, 0, sizeof(*hints)); 
  hints->ai_family = AF_UNSPEC;
  hints->ai_socktype = SOCK_STREAM;
}
/*!
 * \brief Monta a string para requisicao do arquivo.
 * \param[in] url  URL do arquivo para ownload.
 * \return Ponteiro para string de requisicao em caso de sucesso 
 *  ou NULL caso fracasso.
 */
char *get_request(const char *url)
{
  char *path_file = NULL, *ch = NULL, *request = NULL;
  const int http_length = 7;
  int length_std_request = sizeof("GET HTTP/1.0\r\n\r\n");
  int path_length = 0; 
  ch = strchr(url + http_length, '/');
 
  if (ch == NULL || strlen(ch) == 1)
  { 
    path_length = 2;
  }
  else 
  {
    ch++;
    path_length = strlen(ch) + 1;
  }
  path_file = (char *) malloc ((path_length + 1) * sizeof(char));
  if (ch)
    snprintf(path_file, path_length, "%s", ch);
  else
    snprintf(path_file, path_length, "%s", ".");  
  request = (char *) malloc((length_std_request + path_length \
    + 1 ) * sizeof(char));
  snprintf(request, length_std_request + path_length + 1, "%s%s%s", "GET ",\
      path_file, " HTTP/1.0\r\n\r\n");
  free(path_file);
  return request; 
}
/*!
 * \brief Seta as estruturas necessarias para conexao via socket.
 * \param[in] url URL do servidor.
 * \param[in] hints Estrutura com as informacoes sobre a conexao.
 * \param[out] serv_info Estrutura com as informacoes do servidor.
 * \return 0 em caso de sucesso ou < 0 em caso de falha. 
 */
int get_serv_connect_info(const char *url, const struct addrinfo *hints, \
  struct addrinfo **serv_info)
{
  int ret = 0;
  if ((ret = getaddrinfo(url, "http", hints, serv_info)) != 0) 
  {
    if(ret == EAI_NONAME)
      return ERROR_NAME_SERVICE_NOT_KNOW;
    else 
      return ERROR_PARAM_BAD_FORMULATED_URL;
  }
  return 0; 
}
/*!
 * \brief Pega informacoes do header de resposta do HTTP.
 * \param[in] header String contendo o header.
 * \param[out] request_status Representa o status da conexao.
 * \param[out] file_length Tamanho do arquivo de downlaod.
 */
void get_header_info(const char *header, int *request_status, \
  unsigned long *file_length)
{
  char *ch = NULL;
  ch = strstr(header, "HTTP/1.");
  if (ch != NULL)
  {
    sscanf(ch,"HTTP/1.%*[0-9] %d %*[^|]", request_status);
  }
  ch = strstr(header,"Content-Length:");
  if (ch != NULL)
  {
    sscanf(ch, "Content-Length: %ld %*[^|]", file_length);
  } 
}   
/*!
 * \brief Copia o vetor src no vertor dst
 * \param[out] dst Vetor modificado na copia.
 * \param[in] src Vetor que sera copiado.
 * \param[in] begin  Indica a posicao inicial do vertor dst.
 * \param[in] length Indica quantos elementos serao copiados
 */
void vector_cpy (char *dst, char *src, int begin, int length)
{
  int i, j = begin;
  for (i = 0; i <= length; i++)
    dst[i+j] = src[i];
}
/*!
 * \brief Verifica o status da requisicao feita pelo GET.
 * \param[in] request_status Representa do status da conexao.
 * \return 0 em caso de requisicao bem sucedida ou ERROR em caso de falha. 
 */
int request_is_valid(const int  request_status)
{ 
  switch(request_status) 
  {
  case SERVER_CONNECTED:
    return 0;
  case SERVER_NOT_FOUND:
    show_error_message(ERROR_SERVER_NOT_FOUND);
    return ERROR;
  case SERVICE_UNAVAILABLE:
    show_error_message(ERROR_SERVICE_UNAVAILABLE);
    return ERROR;
  default:
    show_error_message(ERROR);
    return ERROR;
  }
}
/*!
 * \brief Checa se o final do header ja foi recebido.
 * \param[in] header String contendo header.
 * \return Ponteiro com conteudo do arquivo de saida ou NULL. 
 */
char *find_end_header(const char* header)
{
  char *ch = NULL;  
  if ((ch = strstr(header, "\r\n\r\n")) != NULL)
  {
    ch += 4;
    return ch;
  }
  else if ((ch = strstr(header,"\n\n")) != NULL)
  {
    ch += 2;
    return ch;
  }
  else if ((ch = strstr(header,"\n\r\n\r")) != NULL)
  {
    ch += 4;
    return ch;
  }
  else
    return NULL;
}
/*!
 * \brief Recebe as informacoes do cabecalho HTTP.
 * \param[in] scoket_id Descritor do socket que estabelece conexao.
 * \param[out] file_length Tamanho do arquivo de saida.
 * \param[out] body_part Possivel parte do arquivo de saida que foi
 *  recebida com o cabecalho.
 * \return Ponteiro com conteudo do arquivo de saida ou NULL. 
 */
char* receive_header(const int socket_id, unsigned long *file_length, \
  int *body_part_length)
{
  char bufin[BUFSIZE], *header, *header_realloc, *ch, *body_part = NULL;
  int ret, header_length = 0, request_status = 0;
  header = (char *) malloc ((BUFSIZE + 1) * sizeof(char));
  memset(header, 0, BUFSIZE + 1); 
  memset(bufin, 0, BUFSIZE);
  while (((ret = recv(socket_id, bufin, BUFSIZE,0)) > 0)\
    && (header_length < MAX_HEADER_LENGTH))  
  {
    vector_cpy(header, bufin, header_length, ret - 1);
    header_length += ret;
    ch = find_end_header(header);
    if ( ch != NULL)
    {
      get_header_info(header, &request_status, file_length);
      break;
    }
    else
      header_realloc = (char *) realloc(header, header_length + BUFSIZE + 1);
    if (header_realloc == NULL)
      goto on_error_receive_header;
    else
      header = header_realloc;
  }
  if (request_is_valid(request_status) || (header_length >= MAX_HEADER_LENGTH))
    goto on_error_receive_header; 
  if (strlen(ch) > 0)
  {
    *body_part_length = header_length - (ch - header); 
    body_part = (char *) malloc((*body_part_length + 1) * sizeof(char));  
    vector_cpy(body_part, ch, 0, *body_part_length - 1);
  }
on_error_receive_header:
  if (header != NULL)
    free(header);
  return body_part;
}
/*!
 * \brief Recebe e escreve o arquivo de saida.
 * \param[in] scoket_id Descritor do socket que estabelece conexao.
 * \param[in] file_name Nome do arquivo de saida.
 * \return 0 em caso de sucesso ou < 0 em caso de falha. 
 */
int write_file(const int socket_id, const char *file_name)
{
  int  ret = 1, body_part_length = 0;
  char bufin[BUFSIZE], *body_part = NULL;
  FILE *fout;
  unsigned long file_length = 0, downloaded_length = 0;
  body_part = receive_header(socket_id, &file_length, &body_part_length);
  if (body_part == NULL)
    return ERROR;
  fout = fopen(file_name, "wb");
  if (fout == NULL)
  {
    printf("Nao foi possivel abrir arquivo de saida!");
    return ERROR;
  }
    if (body_part_length > 0)
    fwrite(body_part, 1, body_part_length, fout);
  free(body_part);
  memset(bufin, 0, BUFSIZE);
  while (((ret = recv(socket_id, bufin, BUFSIZE,0)) > 0) 
    && (downloaded_length < file_length)) 
    fwrite(bufin,1,ret,fout);
  if (ret < 0)
    printf("Erro durante o download!");
  close(socket_id);
  fclose(fout);
  return ret;
}
/*!
 * \brief Abre socket e conecta ao servidor da URL indicada.
 * \param[in]  url   URL do servidor.
 * \return  Retorna o descritor do socket abertor ou ERROR. 
 */
int server_connect(const char *url)
{
  int ret = 0, socket_id;
  struct addrinfo hints, *serv_info = NULL, *p = NULL;
  memset(&hints, 0, sizeof(struct addrinfo));  
  config_connection(&hints); 
  ret = get_serv_connect_info (url, &hints, &serv_info);
  if (ret != 0)
  {
    show_error_message(ret);
    socket_id = ERROR;
    goto on_error_server_connecet;
  }
  for (p = serv_info; p != NULL; p = p->ai_next) 
  {
    if ((socket_id = create_socket(p)) < 0)
      continue;
    if (connect(socket_id, p->ai_addr, p->ai_addrlen) == -1) 
    {
        close(socket_id);
        continue;
    }
   break; 
  }
  if (p == NULL)
  {
    printf("Nao foi possivel conectar ao servidor.\n");
    show_error_message(ERROR_PARAM_BAD_FORMULATED_URL); 
  }
on_error_server_connecet:
  if (serv_info != NULL)
    freeaddrinfo(serv_info);
  return socket_id;
}
/*!
 * \brief Abre um socket, conecta-se e realizar o download
 *  do arquivo especificado pelo args_list.          
 * \param[in]  args_list   strings que determinam o endereco da
 *  pagina e do arquivo. 
 * \return  0 em caso de sucesso ou o <0 em fracasso. 
 */
int download_file(const char *url, const char *file_name)
{
  int socket_id, ret = 0;
  const int http_length = strlen("http://");
  char *request = NULL, *url_serv = NULL;
  url_serv = (char *) malloc((strlen(url) + 1) * sizeof(char));
  memset(url_serv, 0, (strlen(url) + 1));
  char *ch = strstr(url + http_length,"/");
  if (ch == NULL)
    strncpy(url_serv, url + http_length, strlen(url + http_length) + 1);
  else
    strncpy(url_serv, url + http_length, strlen(url+http_length) - strlen(ch));
  socket_id = server_connect (url_serv);
  if (socket_id < 0)
    goto on_error_downdload_file;  
  
  request = get_request(url);
  send(socket_id, request, strlen(request),0); 
  write_file(socket_id,file_name);
on_error_downdload_file:
  if (url_serv != NULL)
    free(url_serv);
  if (request != NULL)
    free(request);
  return ret;
}

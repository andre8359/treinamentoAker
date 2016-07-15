/*!
 * \file   recupera_pag_web_lib.c
 * \brief  arquivo de implementação das funcoes para o  recuperador de 
           paginas WEB
 * \datae 11/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */
#include "recupera_pag_web_lib.h"
/*!
 * \brief Realiza a validacao dos parametros de entrada
 * \param[in]  n_args      numero de argumentos passados
 * \param[in]  args_list   conteudo dos argumentos(strings) 
 * \return  0 em caso de parametros validos <0 caso parametros 
 */
int params_is_valid(char *url, char *file_name, int overwrite_flag)
{
  if (url == NULL || file_name == NULL)
    return ERROR_INCOMP_COMMAND_LINE;
  if (strlen(url) < 8)
    return ERROR_PARAM_BAD_FORMULATED_URL;
  if (strncmp(url,"http://",7))
    return ERROR_PARAM_BAD_FORMULATED_URL;
  if (strrchr(url + 8, '/') == NULL)
    return ERROR_PARAM_BAD_FORMULATED_URL;
  if (strlen(file_name) <= 3 && (!strncmp(file_name, ".", 3)\
    || !strncmp(file_name, "..", 3)))
    return ERROR_PARAM_BAD_FORMULATED_FILENAME;
  if (access(file_name, F_OK) != -1)
  {
    if (overwrite_flag == 0)
      return ERROR_PARAM_BAD_FOUMUALTED_FILE_EXIST;
    if (access(file_name, W_OK) == -1)
      return ERROR_PARAM_BAD_FOUMUALTED_FILE_ACCESS_DENIED;
  }
  return 0;
}
/*!
 * \brief Mostra a mensagem de erro adequada na tela.
 * \param[in]  error  numero associado ao tipo de erro.
 */
void show_error_message(int error)
{
  switch (error)
  {
  case ERROR_INCOMP_COMMAND_LINE:
    printf("Linha de comando incompleta: ./prog -u <URI> -f \
 <arquivo_de_saida> [OPTIONAL]-o\n");
    break;
  case ERROR_PARAM_BAD_FORMULATED_URL:
    printf("\nErro! URL mal formulada.\n");
    break;
  case ERROR_PARAM_BAD_FORMULATED_FILENAME:
    printf("\nErro!Nome do arquivo de saida mal formulado!\n");
    break; 
  case ERROR_PARAM_BAD_FOUMUALTED_FILE_EXIST:
    printf("\nO arquivo ja existe, caso deseje sobrescrever%s",\
      " adicione a '-o' ao fim da linha de comando. \n");
    break;
  case ERROR_PARAM_BAD_FOUMUALTED_FILE_ACCESS_DENIED:
    printf("\nO arquivo informado existe e possui restricoes %s",\
      "para escrita. \n");
    break;
  case ERROR_NAME_SERVICE_NOT_KNOW:
    printf("\nErro!Servidor desconhecido!\n");
    break;
  case ERROR_SERVER_MOVE_PERM:
    printf("\nErro!Servidor movido permanentemente!\n");
    break;
  case ERROR_SERVER_MOVE_TEMP:
    printf("\nErro!Servidor movido temporariamente!\n");
    break;
  case ERROR_SERVER_BAD_REQUEST:
    printf("\nErro [BAD REQUEST]!Parametros mal formulados.\n");
    break;
  case ERRROR_SERVER_UNAUTHORIZED:
    printf("\nErro!Conexao nao autorizada.\n");
    break;
  case ERROR_SERVER_FORBIDDEN:
    printf("\nErro!Conexao proibida.\n");
    break;
  case ERROR_SERVER_NOT_FOUND:
    printf("\nErro!Servido nao encontrado.\n"); 
    break; 
  default:
     printf("Erro!! -- %d\n", error);
   }
}
/*!
 * \brief Cria um socket.
 * \param[in]  p Estrutura que contem as informacoes necessarias
 *  para abertura do socket.
 * \return Retona o descritor do socket em caso de sucesso ou 
 *  ERROR em caso de falha.
 */
int create_socket(struct addrinfo *p)
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
 * \param[in] url  URL do arquivo para download.
 * \return Ponteiro para string de requisicao em caso de sucesso 
 *  ou NULL caso fracasso.
 */
char *get_request(char *url)
{
  char *path_file, *request = NULL;
  int length_std_request = sizeof("GET HTTP/1.0\r\n\r\n");
  path_file = url + 7;
  path_file = strchr(path_file,'/');   
  path_file++;  
  request = (char *) malloc((length_std_request + strlen(path_file)\
    + 1 ) * sizeof(char));
  request[0] = '\0';
  strncpy(request,"GET ", strlen("GET ") + 1); 
  strncat(request, path_file, strlen(path_file) + 1);
  strncat(request," HTTP/1.0\r\n\r\n", strlen(" HTTP/1.0\r\n\r\n") + 1);
  return request; 
}
/*!
 * \brief Seta as estruturas necessarias para conexao via socket.
 * \param[in] url URL do servidor.
 * \param[in] hints Estrutura com as informacoes sobre a conexao.
 * \param[out] serv_info Estrutura com as informacoes do servidor.
 * \return 0 em caso de sucesso ou < 0 em caso de falha. 
 */
int get_serv_connect_info(char *url, struct addrinfo *hints, \
  struct addrinfo **serv_info)
{
  int ret = 0;
  if ((ret = getaddrinfo(url, "http", \
    hints, serv_info)) != 0) 
  {
    if(ret == EAI_NONAME)
      return ERROR_NAME_SERVICE_NOT_KNOW;
    else 
      return ERROR_PARAM_BAD_FORMULATED_URL;
  }
  return 0; 
}
/*!
 * \brief Recebe e escreve o arquivo de saida.
 * \param[in] scoket_id Descritor do socket que estabelece conexao.
 * \param[in] file_name Nome do arquivo de saida.
 * \param[in] file_param Parametro de abertura do arquivo de saida.
 * \return 0 em caso de sucesso ou < 0 em caso de falha. 
 */
void get_header_info(char *header, int *request_status, \
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
 * \brief Recebe e escreve o arquivo de saida.
 * \param[in] scoket_id Descritor do socket que estabelece conexao.
 * \param[in] file_name Nome do arquivo de saida.
 * \param[in] file_param Parametro de abertura do arquivo de saida.
 * \return 0 em caso de sucesso ou < 0 em caso de falha. 
 */
int write_file(int socket_id, char *file_name)
{
  int  ret = 1;
  char bufin[BUFSIZE], *header, *header_realloc, *ch;
  FILE *fout;
  unsigned long file_length = 999999, downloaded_length = 0;
  int end_header = 0, header_length = 0, request_status = 0;
  fout = fopen(file_name, "w");
  if (fout == NULL)
  {
    printf("Nao foi possivel abrir arquivo de saida!");
    return ERROR;
  }
  header = (char *) malloc ((BUFSIZE + 1) * sizeof(char));
  header[0] = '\0'; 
  memset(bufin, 0, BUFSIZE);
  while (((ret = recv(socket_id, bufin, BUFSIZE,0)) > 0) \
    && (downloaded_length < file_length)) 
  {
    if(!end_header)
    {
      strncat(header, bufin, ret);
      //fwrite(bufin, 1,ret, fout);
      header_length += ret;
      if ((ch = strstr(header, "\r\n\r\n")) != NULL)
      {
        ch += 4;
        end_header = 1;
        fwrite(ch, 1, strlen(ch), fout);
        downloaded_length = strlen(ch);
        get_header_info(header, &request_status, &file_length);
      }
      else if ((ch = strstr(header,"\n\n")) != NULL)
      {
        ch += 2;
        end_header = 1;
        fwrite(ch, 1, strlen(ch), fout);
        downloaded_length = strlen(ch); 
      }
      else if ((ch = strstr(header,"\n\r\n\r")) != NULL)
      {
        end_header = 1;
        fwrite(ch, 1, strlen(ch), fout);
        downloaded_length = strlen(ch);
      }
      else
        header_realloc = (char *) realloc(header, header_length \
          + BUFSIZE + 1);
      if (header_realloc == NULL)
      {
        free(header);
        return ERROR;
      }
      else if(!end_header)
        header = header_realloc;
      memset(bufin, 0, BUFSIZE);  
    }
    else
    {
      fwrite(bufin, 1, ret, fout);
      memset(bufin, 0, BUFSIZE);
      downloaded_length += ret;
    }   
  }
  if (ret < 0)
  {
    printf("Erro durante o download!");
    fclose(fout);
    return ERROR;
  }
  free(header);
  close(socket_id);
  fclose(fout);
  return 0;
}
/*!
 * \brief Abre um socket, conecta-se e realizar o download
 *  do arquivo especificado pelo args_list.          
 * \param[in]  args_list   strings que determinam o endereco da
 *  pagina e do arquivo. 
 * \return  0 em caso de sucesso ou o <0 em fracasso. 
 */

int download_file(char *url, char *file_name)
{
  int socket_id, ret;
  struct addrinfo hints, *serv_info, *p;
  char *request = NULL, *url_serv = NULL;  
  config_connection(&hints); 
  url_serv = (char *) malloc((strlen(url) + 1) * sizeof(char));
  url_serv[0] = '\0';
  strncpy(url_serv, url + 7, strlen(url + 1));
  url_serv[(unsigned) (strchr(url_serv,'/') - url_serv )] = '\0'; 
  ret = get_serv_connect_info (url_serv, &hints, &serv_info);
  if (ret < 0)
  {
    free(url_serv);
    return ret;
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
    printf("Nao foi possivel conectar.\n"); 
    free(url_serv);
    freeaddrinfo(serv_info);
    return ERROR;  
  }
  request = get_request(url);
  send(socket_id, request, strlen(request),0); 
  write_file(socket_id,file_name);
  free(url_serv);
  freeaddrinfo(serv_info);
  free(request);
  return ret;
}

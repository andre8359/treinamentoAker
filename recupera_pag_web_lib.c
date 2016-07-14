/*!
 * \file   recupera_pag_web_lib.c
 * \brief  arquivo de implementação das funcoes para o  recuperador de 
           paginas WEB
 * \datae 11/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */
#include "recupera_pag_web_lib.h"
#define ERROR_INCOMP_COMMAND_LINE -1 
#define ERROR_PARAM_BAD_FORMULATED_URL -2
#define ERROR_PARAM_BAD_FORMULATED_FLAG -3
#define ERROR_PARAM_BAD_FORMULATED_FILENAME -4
#define ERROR_PARAM_BAD_FOUMUALTED_FILE_EXIST -5
#define ERROR_PARAM_BAD_FOUMUALTED_FILE_ACCESS_DENIED -6
#define ERROR_NAME_SERVICE_NOT_KNOW -7
#define ERROR -10
#define ERROR_SERVER_MOVE_PERM 301
#define ERROR_SERVER_MOVE_TEMP 302
#define ERROR_SERVER_BAD_REQUEST 400
#define ERRROR_SERVER_UNAUTHORIZED 401
#define ERROR_SERVER_FORBIDDEN 403
#define ERROR_SERVER_NOT_FOUND 404
#define BUFSIZE 1
/*!
 * \brief Realiza a validacao dos parametros de entrada
 * \param[in]  n_args      numero de argumentos passados
 * \param[in]  args_list   conteudo dos argumentos(strings) 
 * \return  0 em caso de parametros validos <0 caso parametros 
 */
int params_is_valid(int n_args, char **args_list)
{
  char *str = NULL, ch = '/';
    
  if (n_args < 3 || n_args > 4)
    return ERROR_INCOMP_COMMAND_LINE;
  if (strlen(args_list[1]) < 5)
    return ERROR_PARAM_BAD_FORMULATED_URL;
  if ((str = strrchr(args_list[1]+7,ch)) == '\0')
    return ERROR_PARAM_BAD_FORMULATED_URL; 
  if ((unsigned)(str - args_list[1]) == (strlen(args_list[1]) - 1))
    return ERROR_PARAM_BAD_FORMULATED_URL;
  if (strncmp(args_list[1],"http://",7))
    return ERROR_PARAM_BAD_FORMULATED_URL;
  if (strlen(args_list[2]) <= 3 && (!strncmp(args_list[2],".",3)\
    || !strncmp(args_list[2],"..",3)))
    return ERROR_PARAM_BAD_FORMULATED_FILENAME;
  if (access(args_list[2], F_OK) != -1)
  {
    if (n_args == 4)
    {
      if (strncmp(args_list[3],"-o",3))
        return ERROR_PARAM_BAD_FOUMUALTED_FILE_EXIST;
    }
    else 
      return ERROR_PARAM_BAD_FOUMUALTED_FILE_EXIST;
    if (access(args_list[2], W_OK) == -1)
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
    printf("Linha de comando incompleta: ./prog <URI> <arquivo_de_saida>\n");
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
    printf("\nErro!Parametros mal formulados.\n");
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
      return ERROR;
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
int write_file(int socket_id, char *file_name)
{
  int  ret = 1;
  char bufin[BUFSIZE];
  FILE* fout;
  fout = fopen(file_name, "w");
  if (fout == NULL)
  {
    printf("Nao foi possivel abrir arquivo de saida!");
    return ERROR;
  }
  memset(bufin, 0, BUFSIZE);
  while ((ret = recv(socket_id, bufin, BUFSIZE,0)) > 0) 
  {
    fwrite(bufin,1,ret,fout );
    memset(bufin, 0, BUFSIZE);
  }
  if (ret < 0)
  {
    printf("Erro durante o download!");
    fclose(fout);
    return ERROR;
  }
  
  close(socket_id);
  fclose(fout);
  return 0;
}
/*!
 * \brief Retira o cabecalho do HTTP do arquivo de saida.
 * \param[in]  file_name Nome do arquivo de saida.
 * \return  0 em caso de parametros validos <0 caso parametros. 
 */
int get_header(char *file_name)
{
  char ch, end_header[] ={'a','a','a','a'}, begin_body = 0;
  int i = 0, status_request = 0, version = 0;
  FILE *fout = fopen(file_name,"r");
  FILE *ftemp = fopen(".temp_file","w");
  if (fout == NULL)
    return ERROR;
  if (ftemp == NULL)
    return ERROR; 
  fscanf(fout,"HTTP/1.%d %d ",&version, &status_request); 
  if (status_request != 200)
  {
    fclose(fout);
    fclose(ftemp);
    remove(file_name);
    remove(".temp_file");

    return status_request;
  }
  while (!feof(fout) && !ferror(fout))
  {
    if (begin_body)
    {
      if (fread(&ch,1,1,fout) != 0)
        fwrite(&ch,1,1,ftemp);
    }
    else
    {
      fread(&ch,1,1,fout);
      end_header[i] = ch;
      if (!strncmp(end_header,"\r\n\r\n", 4))
        begin_body = 1;
      if (i == 3)
      {
        end_header[0] = end_header[1];
        end_header[1] = end_header[2];
        end_header[2] = end_header[3];
      }
      else 
        i++;
    }    
  }
  fclose(ftemp);
  fclose(fout);
  remove(file_name);
  rename(".temp_file",file_name);
  return 0;
}
/*!
 * \brief Abre um socket, conecta-se e realizar o download
 *  do arquivo especificado pelo args_list.          
 * \param[in]  args_list   strings que determinam o endereco da
 *  pagina e do arquivo. 
 * \return  0 em caso de sucesso ou o <0 em fracasso. 
 */

int download_file(char **args_list)
{
  int socket_id, ret;
  struct addrinfo hints, *serv_info, *p;
  char *request = NULL, *url_serv = NULL;  
  config_connection(&hints); 
  url_serv = (char *) malloc((strlen(args_list[1]) + 1) * sizeof(char));
  url_serv[0] = '\0';
  strncpy(url_serv,args_list[1] + 7, strlen(args_list[1] + 1));
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
  request = get_request(args_list[1]);
  send(socket_id, request, strlen(request),0); 
  write_file(socket_id,args_list[2]);
  ret = get_header(args_list[2]);  
  free(url_serv);
  freeaddrinfo(serv_info);
  free(request);
  return ret;
}

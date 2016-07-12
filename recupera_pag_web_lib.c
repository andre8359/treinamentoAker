/*!
 * \file   recupera_pag_web.c
 * \brief  arquivo de implementação das funcoes para o  recuperador de 
           paginas WEB
 * \datae 11/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <limits.h>
#include "recupera_pag_web_lib.h"

#define ERROR_INCOMP_COMMAND_LINE -1 
#define ERROR_PARAM_BAD_FORMULATED_URL -2
#define ERROR_PARAM_BAD_FORMULATED_FLAG -3
#define ERROR_PARAM_BAD_FORMULATED_FILENAME -4
#define ERROR_PARAM_BAD_FOUMUALTED_FILE_EXIST -5
#define ERROR_NAME_SERVICE_NOT_KNOW -6
#define ERROR -10
#define BUFSIZE 256

/*!
 * \brief Realiza a validacao dos parametros de entrada
 * \param[in]  n_args      numero de argumentos passados
 * \param[in]  args_list   conteudo dos argumentos(strings) 
 * \return  0 em caso de parametros validos <0 caso parametros 
 */
int params_is_valid(int n_args, char **args_list)
{
  char *str = NULL, ch = '/';
  int ret;
    
  if (n_args < 3 || n_args > 4)
    return ERROR_INCOMP_COMMAND_LINE;
  if (strlen(args_list[1]) < 5)
    return ERROR_PARAM_BAD_FORMULATED_URL;
  if ((str = strrchr(args_list[1],ch)) == '\0')
    return ERROR_PARAM_BAD_FORMULATED_URL; 
  if ((str - args_list[1]) == strlen(args_list[1]) - 1)
    return ERROR_PARAM_BAD_FORMULATED_URL;
  if (strncmp(args_list[1],"http://",7))
    return ERROR_PARAM_BAD_FORMULATED_URL;
  if(strlen(args_list[2]) <= 3 && (!strncmp(args_list[2],".",3)\
    || !strncmp(args_list[2],"..",3)))
    return ERROR_PARAM_BAD_FORMULATED_FILENAME;
  if(access(args_list[2], F_OK) != -1)
  {
    if(n_args == 4)
      if(strncmp(args_list[3],"-o",3))
        return ERROR_PARAM_BAD_FOUMUALTED_FILE_EXIST;
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
    printf("Erro! URL mal formulada.\n");
    break;
  case ERROR_PARAM_BAD_FORMULATED_FILENAME:
    printf("Erro!Nome do arquivo de saida mal formulado!\n");
    break; 
  case ERROR_PARAM_BAD_FOUMUALTED_FILE_EXIST:
    printf("Erro! O arquivo ja existe, caso deseje sobrescrever \
      adicione a '-o' ao fim da linha de comando. \n");
    break;
  default:
     printf("Erro!!");
   }
}
/*!
 * \brief Mostra a mensagem de erro adequada na tela.
 * \param[in]  error  numero associado ao tipo de erro.
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
 * \brief Mostra a mensagem de erro adequada na tela.
 * \param[in]  error      numero associado ao tipo de erro.
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
 * \param[out] request String utilizada para requisicao do arquivo.
 * \param[out] root_directory URL raiz da pagina.
 */
void get_request(char *url, char *request, char *root_directory)
{
  char *temp; 
}
/*!
 * \brief Recebe e escreve o arquivo de saida.
 * \param[in] scoket_id Descritor do socket que estabelece conexao.
 * \param[in] file_name Nome do arquivo de saida.
 * \param[in] file_param Parametro de abertura do arquivo de saida.
 * \return 0 em caso de sucesso ou < 0 em caso de falha. 
 */
int write_file(int socket_id, char *file_name, char *file_param)
{
  int begin_file = 0, ret = 1;
  char bufin[BUFSIZE], *file_content;
  FILE* fout;
  
  fout = fopen(file_name, file_param);
  
  if (fout == NULL)
  {
    printf("Nao foi possivel abrir arquivo de saida!");
    return ERROR;
  }
  memset(bufin,0,BUFSIZE);
  char request[] = "GET cc32fw64.patch HTTP/1.0\r\n\r\n";
  send(socket_id, &request,strlen(request),0);

  while ((ret = recv(socket_id, &bufin, BUFSIZE,0)) > 0) 
  {    
    if (!begin_file)
    {
      file_content = strstr(bufin, "\r\n\r\n");
      if (file_content != NULL)
      {
        begin_file = 1;
        file_content += 4;
      }
    }
    else
      file_content = bufin;
    if (begin_file) 
      fprintf(fout,"%s",file_content);
   else
     printf("%s",bufin);
   memset(bufin, 0, BUFSIZE);
  }
  if (ret < 0)
    printf("Erro durante o download!");
  close(socket_id);
  fclose(fout);  
  return 0;
}
/*!
 * \brief Abre um socket, conecta-se e tenta realizar o download
 *  do arquivo especificado pelo args_list.          
 * \param[in]  args_list   strings que determinam o endereco da
 *  pagina e do arquivo. 
 * \return  0 em caso de parametros validos <0 caso parametros 
 */
int download_file(char **args_list, int num_args)
{
  int socket_id, ret;
  struct addrinfo hints, *servinfo, *p;
  char file_info[3];  
  config_connection(&hints); 
  if ((ret = getaddrinfo(args_list[1], "http", \
    &hints, &servinfo)) != 0) 
  {
    printf("%s - %d\n", gai_strerror(ret), ret);
    return ERROR;
  }   
  for (p = servinfo; p != NULL; p = p->ai_next) 
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
    return ERROR;
  }
  write_file(socket_id,args_list[2],"w");
  return 0;
}

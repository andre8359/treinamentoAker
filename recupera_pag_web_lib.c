/*i
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
#include "recupera_pag_web_lib.h"

#define ERROR_INCOMP_COMMAND_LINE -1 
#define ERROR_PARAM_BAD_FORMULATED -2
#define ERROR -10
#define BUFSIZE 200

/*!
 * \brief Realiza a validacao dos parametros de entrada
 * \param[in]  n_args      numero de argumentos passados
 * \param[in]  args_list   conteudo dos argumentos(strings) 
 * \return  0 em caso de parametros validos <0 caso parametros 
 */
int params_is_valid(int n_args, char **args_list)
{
  if (n_args < 3)
    return ERROR_INCOMP_COMMAND_LINE;
  
  if (strlen(args_list[1]) < 5)
    return ERROR_PARAM_BAD_FORMULATED;
  return 0;
}

/*!
 * \brief Mostra a mensagem de erro adequada na tela.
 * \param[in]  error      numero associado ao tipo de erro.
 */
void show_error_message(int error)
{
  switch (error)
   {
   case ERROR_INCOMP_COMMAND_LINE:
     printf("Linha de comando incompleta: ./prog <URI> <arquivo_de_saida>\n");
     break;
   case ERROR_PARAM_BAD_FORMULATED:
     printf("Erro! Parametros mal formulados.\n");
     break;
   default:
     printf("Erro!!");
   }
}

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

void config_connection(struct addrinfo *hints)
{
  memset(hints, 0, sizeof(*hints)); 
  hints->ai_family = AF_UNSPEC;
  hints->ai_socktype = SOCK_STREAM;
}

int write_file(int socket_id, char **args_list, char *flag)
{
  int begin_file = 0, ret = 1;
  char bufin[BUFSIZE], *file_content;
  FILE* fout;
  
  fout = fopen(args_list[2], "w");
  
  if (fout == NULL)
  {
    printf("Nao foi possivel abrir arquivo de saida!");
    return ERROR;
  }
  memset(bufin,0,BUFSIZE);
  char request[] = "GET Arquivos/cc32fw64.patch HTTP/1.0\r\n\r\n";
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
 *        do arquivo especificado pelo args_list.          
 * \param[in]  args_list   strings que determinam o endereco da
 *                         pagina e do arquivo. 
 * \return  0 em caso de parametros validos <0 caso parametros 
 */
int download_file(char **args_list)
{
  int socket_id, ret;
  struct addrinfo hints, *servinfo, *p;
    
  config_connection(&hints);
    
  if ((ret = getaddrinfo(args_list[1], "http", \
    &hints, &servinfo)) != 0) 
  {
    printf("%s\n", gai_strerror(ret) );
    return ERROR;
  }
   
  for (p = servinfo; p != NULL; p = p->ai_next) 
  {
    if ((socket_id = create_socket(p)) < 0)
    {
      continue;
    }
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
  write_file(socket_id,args_list,"w");
  return 0;
}

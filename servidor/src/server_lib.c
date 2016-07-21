/*!
 * \file   server_lib.c
 * \brief  Arquivo de implementação das funcoes para o recupe
 * \date 11/07/2016
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
 * \return Descritor do socket ou -1.
 */
void config_connection(const char *port, struct sockaddr_in *serv_info)
{ 
  serv_info->sin_family = AF_INET;           
	serv_info->sin_addr.s_addr = INADDR_ANY; 	
	serv_info->sin_port = htons(atoi(port)); 
}
/*!
 * \brief Aguarda e aceita conexao.
 * \param[in]  p Estrutura que contem as informacoes necessarias para abertura 
 *  do socket.
 * \return Retona o descritor do socket em caso de sucesso ou ERROR em caso de 
 *  falha.
 */
int make_connection(const char *port)
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
 * \brief Fecha os arquivos abertos pelo padrão (STDIN, STDOUT, STDERR).
 */
void write_to_client (struct request *r)
{
  char bufin[BUFSIZE];
  int nbytes = 0;
  if (r->header_size_sended < strlen(r->header))
  {
    nbytes = send(r->socket_id, r->header + r->header_size_sended, \
      strlen(r->header) - r->header_size_sended, 0);
    
  }
  r->fp = fopen(r->file_name,"r");
  if (r->fp == NULL)
  {
    fprintf(stderr,"Nao foi possivel abrir arquivo");
    return;
  }
  fseek(r->fp, r->sended_size, SEEK_SET);
  nbytes = fread(bufin,1,BUFSIZE,r->fp);
  nbytes = send(r->socket_id, bufin, nbytes, 0);
  r->sended_size += nbytes;
}  
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

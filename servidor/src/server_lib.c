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
  {                                                                     
    fprintf (stderr,"accept");                                          
    exit (EXIT_FAILURE);                                                
  }                                                                     
  fprintf (stderr, "Servidor: conectado em host %s, porta %hd.\n",      
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
void vector_cpy (char *dst, char *src, int begin, int length)
{
  int i, j = begin;
  for (i = 0; i <= length; i++)
    dst[i+j] = src[i];
}
/*!
 * \brief Recebe a requisicao do cliente conectado.
 * \param[in] socket_id Descritor do socket da conexao.
 * \return Ponteiro para a string da requisicao recebida.
 */
char* receive_request_from_client(const int socket_id)
{
  char bufin[BUFSIZE];
  int nbytes = 0, received_size = 0;
  char *request = NULL, *request_realloc = NULL;
  request = (char *) malloc ((BUFSIZE + 1) * sizeof(char));
  memset(request, 0, BUFSIZE + 1); 
  memset(bufin, 0, BUFSIZE);
  while(((nbytes = recv(socket_id, bufin, BUFSIZE, 0)) > 0) \
    && received_size < MAX_HEADER_SIZE)
  {
    vector_cpy(request, bufin, received_size, nbytes - 1);
    received_size += nbytes;
    if (!find_end_request(request))
      break;
    else
      request_realloc = (char *) realloc(request, received_size + BUFSIZE + 1);
    if (request_realloc == NULL)
      goto on_error_receive_request;
    else
      request = request_realloc; 
  }
  if (received_size >= MAX_HEADER_SIZE)
    goto on_error_receive_request;
  return request;
on_error_receive_request:
  free(request);
  return NULL;
} 
/*!
 * \brief Envia informacoes pro cliente conectado.
 * \param[in] r Estrutura que representa a requisicao.
 */
void write_to_client (struct request_file *r)
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
/*!
 * \brief Checa se o arquivo existe e pode ser enviado.
 * \param[in] file_name Nome do arquivo.
 * \return 0 para sucesso e <0 para falha.
 */
int check_file_ready_to_send(char *file_name)
{
  char *root_diretory;   
  if (getwd(root_diretory));
  if (access(file_name,F_OK) != -1);
}
/*!
 * \brief Fecha os arquivos abertos pelo padrão (STDIN, STDOUT, STDERR).
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

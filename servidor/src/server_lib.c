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

int receive_request_from_client(const int socket_id,\
  struct request_file **head)
{
  char bufin[BUFSIZE + 1];
  int nbytes = 0, received_size = 0;
  
  struct request_file *request = NULL;
  request = search_request(socket_id, head);
 
  if (request == NULL)
    request = add_request(socket_id, head);
  nbytes = recv(socket_id, bufin, BUFSIZE, 0);
  
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
    return 0;
  }
  else
    return -1;   
} 
/*!
 * \brief Envia informacoes pro cliente conectado.
 * \param[in] r Estrutura que representa a requisicao.
 */
int write_to_client (const int socket_id, struct request_file **head)
{
  int nbytes = 0;
  struct request_file *request = NULL;
  request =  search_request(socket_id, head);
  if (request->header == NULL)
    request->header = make_header(request->file_name,request->status_request, 
      &(request->file_size));
  if (request->header_size_sended < strlen(request->header))
  {
    int send_size = strlen(request->header) - request->header_size_sended;
    if (send_size > BUFSIZE)
      send_size = BUFSIZE;
    
    nbytes = send(socket_id, request->header + request->header_size_sended, 
      send_size,0);  
    request->header_size_sended += nbytes;
    
    return -1;
  }
  if (request->sended_size < request->file_size)
  {
    char bufin[BUFSIZE];
    request->fp = fopen(request->file_name,"r");
    if (request->fp == NULL)
      return -1;
    fseek(request->fp, request->sended_size, SEEK_SET);
    memset(bufin, 0, BUFSIZE);
    nbytes = fread(bufin, 1, BUFSIZE,request->fp);
    send(socket_id, bufin, nbytes, 0);
    request->sended_size += nbytes;
    fclose(request->fp);
    return -1;
  }
  else
    return 0;
} 
/*!
 * \brief Change the current working directory 
 */
int change_root_directory(const char *root_diretory)
{
  if ((chdir(root_diretory)) < 0) 
  {
    fprintf(stderr,"Nao foi possivel mudar diretorio root!");
    return -1;
  }
  if (create_default_response_files() < 0)
    return -1;
  return 0;
}
/*!
 * \brief Checa se o arquivo existe e pode ser enviado.
 * \param[in] file_name Nome do arquivo.
 * \return 0 para sucesso e <0 para falha.
 */
int check_file_ready_to_send(struct request_file * request)
{
  if (request->file_name == NULL)
    return -1;
  
  if (access(request->file_name, F_OK) != -1) 
  {
    char root_diretory[PATH_MAX];
    getcwd(root_diretory,PATH_MAX);
    char abs_path[PATH_MAX];
    realpath(request->file_name, abs_path);
    if (strstr(abs_path, root_diretory) == NULL)
    {
      request->status_request = FORBIDDEN;
      return -1;
    }
    if (access(request->file_name, R_OK) < 0)
    { 
      request->status_request = UNAUTHORIZED;
      return -1;
    }
  }
  else
  {
    request->status_request = NOT_FOUND;
    return -1;
  }
  return 0; 
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

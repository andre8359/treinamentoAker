
#include "socket_utils.h"
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
 * \param[in]  port  Inteiro que indica em qual porta a conexao
 * acontecera.
 * \return Retona o descritor do socket em caso de sucesso ou ERROR em
 * caso de
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

  if (bind(socket_id, (struct sockaddr *)
           &serv_info, len) < 0)
    return ERROR;

  return socket_id;
}
/*!
 * \brief Aceita novas conexoes.
 * \param[in]  socket_id Descritor do socket onde ocoreu o bind.
 * \return Retona o descritor do socket em caso de sucesso ou
 * ERROR em caso de
 *  falha.
 */
int accept_new_connection(const int socket_id)
{
  struct sockaddr_in client_info;
  socklen_t client_len;
  int new_socket_id;

  client_len = sizeof (client_info);
  new_socket_id = accept (socket_id,
                          (struct sockaddr
                           *)&client_info,
                          &client_len);
  if (new_socket_id < 0)
    return ERROR;

  fprintf (stderr, "Conexao aberta: conectado em host %s, fd  %d.\n",
           inet_ntoa(client_info.sin_addr),
           socket_id);

  return new_socket_id;
}

int make_local_socket (int *fd)
{
  int ret = 0;

  ret = socketpair(AF_UNIX, SOCK_DGRAM, 0, fd);
  return ret;
}

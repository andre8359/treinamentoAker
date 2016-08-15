
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

  fprintf (stderr, "Conexao aberta: conectado em host %s, porta %hd.\n",
           inet_ntoa(client_info.sin_addr),
           ntohs(client_info.sin_port));

  return new_socket_id;
}

int make_named_socket (const char *file_name)
{
  struct sockaddr_un name;
  int socket_id = 0;
  size_t size = 0;

  unlink (file_name);

  socket_id = socket (PF_LOCAL, SOCK_DGRAM, 0);

  if (socket_id < 0)
    return ERROR;

  name.sun_family = AF_LOCAL;
  strncpy (name.sun_path, file_name, sizeof(name.sun_path));
  name.sun_path[sizeof (name.sun_path) - 1] = '\0';

  size = (offsetof (struct sockaddr_un, sun_path) + strlen(name.sun_path));

  if (bind(socket_id, (struct sockaddr *) &name, size) < 0)
    return ERROR;
  return socket_id;
}

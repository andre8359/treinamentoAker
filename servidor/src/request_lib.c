/*!
 * \file   request_lib.c
 * \brief  Arquivo de implementação das funcoes para o manipulacao da lista de
 * requisicoes.
 * \date 18/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */
#include "request_lib.h"
/*!
 * \brief Realiza a adicao de uma requisicao na lista.
 * \param[in]  socket_id  Descritor do socket da conexao que originou a
 *  requisicao.
 * \param[in]  head  Ponteiro para o primeiro elemento da lista.
 * \return  NULL em caso de erro ou um ponteiro para nova requisicao em caso de
 *  sucesso.
 */
struct request_file *add_request_file(const int socket_id,
                                      struct request_file **head)
{
  struct request_file *aux;

  if (*head == NULL)
  {
    *head = (struct request_file *) calloc (1, sizeof(struct request_file));
    (*head)->socket_id = socket_id;

    return *head;
  }

  aux = *head;

  while (aux->next)
    aux = aux->next;

  struct request_file *new_request =
    (struct request_file *) calloc(1, sizeof(struct request_file));
  new_request->socket_id = socket_id;

  new_request->prev = aux;
  aux->next = new_request;

  return new_request;
}
/*!
 * \brief Realiza a liberacao da memoria alocada para um elemento da lista.
 * \param[in]  r Ponteiro para o elemento que sera desalocado.
 * \return  NULL em caso de sucesso ou um ponteiro para nova requisicao em caso
 *  de falha.
 */
static void free_request_file(struct request_file **r)
{
  free((*r)->file_name);
  free((*r)->request);
  free((*r)->header);

  if ((*r)->fd)
    close((*r)->fd);
  (*r)->fd = 0;
  if ((*r)->socket_id)
    close((*r)->socket_id);

  free((*r));
  (*r) = NULL;
}
/*!
 * \brief Realiza a remocao de uma requisicao na lista.
 * \param[in]  socket_id  Descritor do socket da conexao que originou a
 *  requisicao.
 * \param[in]  head  Ponteiro para o primeiro elemento da lista.
 * \return NULL em caso de erro ou um ponteiro para deletada requisicao em caso
 *  de sucesso.
 */
int rm_request_file(const int socket_id, struct request_file **head)
{
  struct request_file *deleted_request = NULL;
  struct request_file *swap_request_prev = NULL;
  struct request_file *swap_request_next = NULL;

  if (*head == NULL)
    return ERROR;

  deleted_request = search_request_file(socket_id, head);

  if (deleted_request == NULL)
    return ERROR;

  if (deleted_request->prev == NULL)
  {
    if (deleted_request->next == NULL)
    {
      free_request_file(head);
      return SUCCESS;
    }

    deleted_request = (*head)->next;
    deleted_request->prev = NULL;
    free_request_file(head);
    *head = deleted_request;

    return SUCCESS;
  }

  swap_request_prev = deleted_request->prev;
  swap_request_next = deleted_request->next;
  swap_request_prev->next = swap_request_next;

  if (swap_request_next != NULL)
    swap_request_next->prev = swap_request_prev;

  free_request_file(&deleted_request);

  return SUCCESS;
}
/*!
 * \brief Realiza busca por uma requisicao baseado no descritor que originou-a.
 * \param[in]  socket_id  Descritor do socket da conexao que originou a
 *  requisicao.
 * \param[in]  head  Ponteiro para o primeiro elemento da lista.
 * \return  NULL em caso de erro ou um ponteiro para requisicao em caso de
 *  sucesso.
 */
struct request_file *search_request_file(const int socket_id,
                                         struct request_file **head)
{
  struct request_file *aux;
  if (*head == NULL)
    return NULL;

  aux = *head;
  while ((aux) && (aux->socket_id != socket_id ))
    aux = aux->next;

  if (aux == NULL)
    return NULL;

  return aux;
}
/*!
 * \brief Realiza a liberacao de uma lista de requisicoes.
 * \param[in]  head  Ponteiro para o primeiro elemento da lista.
 */
void free_request_file_list(struct request_file **head)
{
  struct request_file *i, *j = NULL;
  i = *head;
  while (i)
  {
    j = i->next;
    free_request_file(&i);
    i = j;
  }
}
/*!
 * \brief Realiza a print dos descritores de uma lista de requisicoes.
 * \param[in]  head  Ponteiro para o primeiro elemento da lista.
 */
void print_request_file_list(struct request_file **head)
{
  struct request_file *aux;

  if (*head == NULL)
    return;

  aux =  *head;

  while (aux)
  {
    fprintf(stdout, "%d \n", aux->socket_id);
    aux = aux->next;
  }
}

struct request_io *enqueue_request_io(struct manager_io **manager,
                                      struct request_io *request)
{
  struct manager_io *m = *manager;
  struct request_io *aux = NULL;
  struct request_io *new_request = NULL;

  if (m->head == NULL)
  {
    m->head = (struct request_io *) calloc (1, sizeof(struct request_io));
    memcpy(m->head, request, sizeof(struct request_io));

    m->head->next = NULL;
    m->total_request++;
    return m->head;
  }

  aux = m->head;

  while (aux->next)
    aux = aux->next;

  new_request = (struct request_io *) calloc(1, sizeof(struct request_io));
  memcpy(new_request, request, sizeof(struct request_io));

  new_request->next = NULL;
  aux->next = new_request;
  m->total_request++;
  return new_request;
}

struct request_io *dequeue_request_io(struct manager_io **manager)
{
  struct manager_io *m = *manager;
  struct request_io *aux = NULL;

  if (m->head == NULL)
    return NULL;

  aux = m->head;
  m->head = m->head->next;
  m->total_request--;

  return aux;
}

struct request_io *dequeue_request_io_with_socket_id(const int socket_id,
                                                     struct manager_io **manager)
{
  struct manager_io *m = *manager;
  struct request_io *aux = NULL;
  struct request_io *aux_prev = NULL;

  if (m->head == NULL)
    return NULL;

  aux = m->head;
  while (aux != NULL && (aux->socket_id != socket_id))
  {
    aux_prev = aux;
    aux = aux->next;
  }

  if(aux)
  {
    m->total_request--;
    if (aux_prev == NULL)
      m->head = m->head->next;
    else
      aux_prev->next = aux->next;
  }

  return aux;
}
void free_request_io(struct request_io **request)
{
  struct request_io *r = *request;
  free(r);
  r = NULL;
}

void free_request_io_list(struct manager_io **manager)
{
  struct manager_io *m = *manager;
  struct request_io *i, *j = NULL;

  i = m->head;
  while (i)
  {
    j = i->next;
    free_request_io(&i);
    i = j;
  }
  m->total_request = 0;
}

void rm_request_io(const int socket_id, struct manager_io **manager)
{
  struct manager_io *m = *manager;
  struct request_io *node = NULL, *prev_node = NULL;

  node = m->head;
  if (node == NULL)
    return;

  while (node)
  {
    if ((prev_node == NULL) && (node->socket_id == socket_id))
    {
      m->head = node->next;
      free_request_io(&node);
      m->total_request--;
      node = m->head;
    }
    else if (node->socket_id == socket_id)
    {
      prev_node->next = node->next;
      free_request_io(&node);
      m->total_request--;
      node = prev_node->next;
    }
    else
    {
      prev_node = node;
      node = node->next;
    }
  }
  return;
}

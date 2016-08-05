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
struct request_file* add_request(const int socket_id,
                                 struct request_file **head)
{
  struct request_file *temp;
  if (*head == NULL)
  {
    *head = (struct request_file *) malloc (sizeof(struct request_file));
    memset(*head, 0, sizeof(**head));
    (*head)->socket_id = socket_id;
    return *head;
  }
  temp = *head;
  while (temp->next)
    temp = temp->next;
  struct request_file *new_request =
    (struct request_file *) malloc(sizeof(struct request_file));
  memset(new_request, 0, sizeof(*new_request));
  new_request->socket_id = socket_id;
  new_request->prev = temp;
  temp->next = new_request;

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

  if ((*r)->fp != NULL)
    fclose((*r)->fp);

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
int rm_request(const int socket_id, struct request_file **head)
{
  struct request_file *deleted_request = NULL;
  struct request_file *swap_request_prev = NULL;
  struct request_file *swap_request_next = NULL;

  if (*head == NULL)
    return ERROR;

  deleted_request = search_request(socket_id, head);

  if (deleted_request == NULL)
    return ERROR;

  if (deleted_request->prev == NULL)
  {
    if (deleted_request->next == NULL)
      return SUCCESS;

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
struct request_file* search_request(const int socket_id,
                                    struct request_file **head)
{
  struct request_file *temp;
  if (*head == NULL)
    return NULL;

  temp = *head;
  while ((temp) && (temp->socket_id != socket_id ))
    temp = temp->next;

 if (temp == NULL)
    return NULL;

  return temp;
}
/*!
 * \brief Realiza a liberacao de uma lista de requisicoes.
 * \param[in]  head  Ponteiro para o primeiro elemento da lista.
 */
void free_request_list(struct request_file **head)
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
void print_request_list(struct request_file **head)
{
  struct request_file *temp;

  if (*head == NULL)
    return;

  temp =  *head;

  while (temp)
  {
    fprintf(stdout, "%d \n", temp->socket_id);
    temp = temp->next;
  }
}

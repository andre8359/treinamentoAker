#include "request_lib.h"
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
  else
  {
    temp = *head;
    while(temp->next)
      temp = temp->next;
    struct request_file *new_request = (struct request_file *) malloc \
      (sizeof(struct request_file));
    memset(new_request, 0, sizeof(*new_request));
    new_request->socket_id = socket_id;
    new_request->prev = temp;
    temp->next = new_request;
    return new_request; 
  }  
}
static struct request_file* free_request_file(struct request_file **r)
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
  return (*r);
}
struct request_file* rm_request(const int socket_id, struct request_file **head)
{
  if (*head == NULL)
    return NULL;
  else
  {
    struct request_file *deleted_request = search_request(socket_id, head);
    if (deleted_request == NULL)
      return NULL;
    
    if (deleted_request->prev == NULL)
    {
      if (deleted_request->next == NULL)
        return free_request_file(head);
      else
      {
        deleted_request = (*head)->next;
        deleted_request->prev = NULL;
        free_request_file(head);
        *head = deleted_request;
        return deleted_request;
      }
    }
    else 
    {
      struct request_file *swap_request_prev = deleted_request->prev;
      struct request_file *swap_request_next = deleted_request->next;
      swap_request_prev->next = swap_request_next;
      if (swap_request_next != NULL)
        swap_request_next->prev = swap_request_prev;
     
      return free_request_file(&deleted_request);
    }
  }
}
struct request_file* search_request(const int socket_id,
  struct request_file **head)
{
  struct request_file *temp;
  if (*head == NULL)
    return NULL;  
  else
  {
    temp = *head;
    while ((temp) && (temp->socket_id != socket_id ))
      temp = temp->next;
    if (temp == NULL)
      return NULL;
    else
      return temp; 
  }
}
void free_request_list(struct request_file **head)
{
  struct request_file *i, *j = NULL;
  i = *head;
  while(i)
  {
    j = i->next;
    free_request_file(&i);
    i = j;
  }
}
void print_request_list(struct request_file **head)
{
  if (*head == NULL)
    return;
  struct request_file *temp;
  temp =  *head;
  while (temp)
  {
    fprintf(stdout, "%d \n", temp->socket_id);
    temp = temp->next; 
  }
}

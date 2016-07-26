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
      {
        free((*head)->file_name);
        free((*head)->request);
        free((*head)->header);
        free(*head);
        *head = NULL;
        return NULL;
      }
      deleted_request = (*head)->next;
      deleted_request->prev = NULL;
      free(*head);
      *head = NULL;
      
      return deleted_request;
    }
    else 
    {
      struct request_file *swap_request = deleted_request->prev;
      swap_request->next = deleted_request-> next;
    }
    if (deleted_request->next != NULL)
    {
      struct request_file *swap_request = deleted_request->next;
      swap_request->prev = deleted_request->prev;   
    } 
    free(deleted_request);
    return NULL;
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
    free(i->file_name);
    free(i->request);
    free(i->header);
    free(i);
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

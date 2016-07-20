#include <stdio.h>
#include "server_lib.h"
int main()
{
  open_background_process();
  close_std_file_desc();
  sleep(5);
  printf("Hello!");
  return 0;
}

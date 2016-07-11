/*i
 * \file   recupera_pag_web.c
 * \brief  arquivo de implementação das funcoes para o  recuperador de 
           paginas WEB
 * \datae 11/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "recupera_pag_web_lib.h"

#define ERROR_INCOMP_COMMAND_LINE -1 
#define ERROR_PARAM_BAD_FORMULATED -2

/*!
 * \brief Realiza a validacao dos parametros de entrada
 * \param[in]  n_args      numero de argumentos passados
 * \param[in]  args_list   conteudo dos argumentos(strings) 
 * \return  0 em caso de parametros validos <0 caso parametros 
 */
int params_is_valid(int n_args, char **args_list)
{
  if (n_args < 3)
    return ERROR_INCOMP_COMMAND_LINE;
  
  if (strlen(args_list[1]) < 5)
    return ERROR_PARAM_BAD_FORMULATED;
    
  if (strncmp("http", args_list[1], 4))
    return ERROR_PARAM_BAD_FORMULATED;
     
  return 0;
}
void show_error_message(int error)
{
  switch (error)
   {
   case ERROR_INCOMP_COMMAND_LINE:
     printf("Linha de comando incompleta: ./prog <URI> <arquivo_de_saida>\n");
     break;
   case ERROR_PARAM_BAD_FORMULATED:
     printf("Erro! Parametros mal formulados.\n");
     break;
   default:
     printf("Erro!!");
   }

}

/*i
 * \file   recupera_pag_web.c
 * \brief  arquivo de implementação das funcoes para o  recuperador de 
           paginas WEB
 * \data 11/07/2016
 * \author Andre Dantas <andre.dantas@aker.com.br>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "recupera_pag_web_lib.h"
/*!
 * Essa funcao realiza a validacao dos parametros de entrada
 * \param  n_args      numero de argumentos passados
 * \param  args_list   conteudo dos argumentos(strings) 
 * \return  0 em caso de parametros validos -1 caso parametros invalidos
 */
int params_is_valid(int n_args, char *args_list)
{
  if (n_args < 3){
    return -1;
  

  printf("teste");
  return 0;
}

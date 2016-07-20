#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_SIZE 100

typedef struct {
	int sk;
}argumentos;


void* adiciona_data(void);
void *receber(void *args);
void *enviar(void *args);

int main(int argc,char * argv[]) {
	
	struct  sockaddr_in ladoServ;       	                    
	int sk;  
	pthread_t thread_0,thread_1;
	argumentos args;
 
  	if(argc<3)  {
    	   printf("Para estabelecer comunicacao: %s <ip_do_servidor> <porta_do_servidor>\n", argv[0]);
    	   exit(1);  
	}

	memset((char *)&ladoServ,0,sizeof(ladoServ)); 
	
	ladoServ.sin_family      = AF_INET; /* config. socket p. TCP/IP*/
	ladoServ.sin_addr.s_addr = inet_addr(argv[1]); // ip do servidor
	ladoServ.sin_port        = htons(atoi(argv[2]));// porta da conexão


	sk = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sk < 0) {
		fprintf(stderr, "Criacao do socket falhou!\n");
		exit(1); 
	}


	/* Conecta socket ao servidor definido */
	if (connect(sk, (struct sockaddr *)&ladoServ, sizeof(ladoServ)) < 0) {
		fprintf(stderr,"Tentativa de conexao falhou!\n");
		exit(1);
	}
	fprintf(stdout, "Servidor conectado.\n");
	args.sk=sk;

	//pthread_create(&thread_0,NULL,(void*)&receber,(void*)&args);
	pthread_create(&thread_1,NULL,(void*)&enviar,(void*)&args);	
	//pthread_join(thread_0,NULL);			
	pthread_join(thread_1,NULL);
	printf("------- encerrando conexao com o servidor -----\n");

	return (0);

} /* fim do programa */

void *enviar(void *args){
	char msg[MAX_SIZE];
	argumentos* dados = (argumentos*) args;
	pthread_t thread_id;
	while(1){
		
		memset((char *)&msg,0,sizeof(msg));

		fgets(msg,MAX_SIZE,stdin);

		
		send(dados->sk, &msg,strlen(msg),0);
		if(strncmp(msg,"quit.",5)==0)	break;
		else{
			pthread_create(&thread_id,NULL,(void*)&adiciona_data,NULL);
			pthread_join(thread_id,NULL);
			printf("Cliente -- ");
			printf("%s\n",msg);
		}	
}	 
	fprintf(stdout,"Encerrando conexao!!!\n\n");
	close (dados->sk);
	exit(0);
}
void *receber(void *args){
	char bufin[MAX_SIZE];
	argumentos* dados = (argumentos*) args;
	pthread_t thread_id;
	while (1) {
			memset((char *)&bufin,0,sizeof(bufin));

			recv(dados->sk, &bufin, sizeof(bufin),0);
      printf("%s\n", bufin);

			if (strncmp(bufin, "quit.",5)==0)	break;
			else{
				pthread_create(&thread_id,NULL,(void*)&adiciona_data,NULL);
				pthread_join(thread_id,NULL);
				fprintf(stdout, "Cliente --- %s\n", bufin);	
			}
	} 
	fprintf(stdout,"Encerrando conexao!!!\n\n");
	close (dados->sk);
}
void* adiciona_data(void)  {
	struct tm *local;
	time_t t;
	t= time(NULL);
	local=localtime(&t);
 
	printf("%d/%d/%d %dh%dm ",local->tm_mday,local->tm_mon+1,local->tm_year+1900,local->tm_hour,local->tm_min);
} 


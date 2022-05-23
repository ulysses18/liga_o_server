/*
   Exemplo de servidor multi-thread - Servidor
   Laboratório de Sistemas Operacionais
   Compilar com: gcc -Wall servidor_concorrente.c -o srv -pthread
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <pthread.h>
#include <netdb.h>
#include <time.h>

#define MAXBUFF		1024
#define MAXLENGTH    2
#define SRV_PORT	   1443
#define CTRL_BACKLOG	5
#define FIM		      "fim da transmissão"
#define TRUE		   1
#define FALSE	   	0

void * NewSocketThread(void *);
int server(int);

int main(void) {
   int    socketfd, serv_len;
   struct sockaddr_un serv_addr;
   char buff[MAXBUFF] = {};
   int socketfd2 = 0;
   FILE *arq;
   // Cria um socket TCP (SOCK_STREAM) para comunicação na Internet (AF_INET)
   if ((socketfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        printf("\nservidor: erro na criação do socket!\n");
        exit(0);
   }
   

   bzero((char*) &serv_addr, sizeof(serv_addr));

   // Preenche a estrutura do servidor e faz o bind local
   serv_addr.sun_family		= AF_UNIX;
   strcpy(serv_addr.sun_path, "./abc");
   unlink(serv_addr.sun_path);

   serv_len = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
   if (bind(socketfd, (struct sockaddr *) &serv_addr, serv_len) < 0) {
      perror("Erro:");
      printf("\nservidor: erro no bind local!\n");
      close(socketfd);
      exit(0);
   }
   if( listen(socketfd, 5) != 0 )
	{
		   printf("Error on listen call \n");
	}
   socketfd2 = accept(socketfd, (struct sockaddr*)&serv_addr, (socklen_t *)&serv_len);

   recv(socketfd2, buff, MAXBUFF, 0);
   printf(">Recebi %s\n", buff);
   arq = fopen(buff, "r");
   char c[MAXLENGTH] = {""};

	do
	{
		c[0] = fgetc(arq);
		if(c[0] != EOF){
			c[1] = 0;
		}else{
			c[1] = 1;
		}
		send(socketfd2, c, MAXLENGTH, 0);
	}while (c[0] != EOF);

   // server(socketfd2);

   close(socketfd);
   return(0);
}

/*--------------------------------------------------------------------
 Função server: - Conexões para vários clientes - servidor multithread
 /////////////
---------------------------------------------------------------------*/
int server(int socketfd) {
   int *newsocketfd, cli_len;
   struct sockaddr_in cli_addr;
   pthread_t *sockthread;

   // Prepara para receber conexões dos clientes
   listen(socketfd, CTRL_BACKLOG);

   // Estrutura de repetição para continuar recebendo conexões de clientes
   for ( ; ; ) {
       // Aloca espaço na memoria para o novo descritor - newsocketfd
       newsocketfd = (int *)malloc(sizeof(newsocketfd));

       // Aguarda a conexão de algum cliente e quando o cliente conecta, o valor de accept
       // eh recebido como conteúdo do endereço newsocketfd (que esta alocado na memoria)
       cli_len = sizeof(newsocketfd);
       if ((*newsocketfd = accept(socketfd, (struct sockaddr *)&cli_addr, (socklen_t *)&cli_len)) < 0) {
           printf("\nFunção server: erro no accept!\n");
       	   close(socketfd); // Fecha o socket do servidor
           exit(0);
       }

       // Aloca espaço na memoria para uma nova thread - sockthread
       sockthread = (pthread_t *)malloc(sizeof(pthread_t));

       // Cria e chama a nova thread(NewSocketThread) para tratar da comunicação do novo
       // cliente, passando como argumento o descritor(newsocketfd) do novo cliente
       pthread_create(sockthread, NULL, NewSocketThread, newsocketfd);
   }
   return(0);
} // Fim função server

/*--------------------------------------------------------------------------------
 Função NewSocketThread: 
 - Recebe o descritor do novo cliente.
 - Le a mensagem enviada pelo cliente e escreve a mesma mensagem
  para este cliente. Assim, cada thread trata da comunicação                        
  de leitura e escrita para cada cliente, individualmente                        
  (1 thread para 1 cliente).                        
- Enquanto o cliente estiver conectado, a thread correspondente                        
  ao descritor deste cliente estará efetuando a comunicação.----------------------------------------------------------------------------------*/
void * NewSocketThread(void *fdnewsock) {
   char buff[MAXBUFF] = {};
   int *newsockfd = (int *)fdnewsock;	// Cast do tipo void para tipo int *
   int n;

   // Estrutura de repetição para continuar efetuando leitura e escrita na comunicação
   for ( ; ; ) {
      // O servidor le mensagem do cliente "dono" deste descritor
      n = read(*newsockfd, buff, MAXBUFF);
      n = strlen(buff);

      // O servidor escreve mensagem no cliente "dono" deste descritor
      write(*newsockfd, buff, n);
      strncpy(buff, "", strlen(buff));
   }
   close(*newsockfd);
   return(NULL);
}

/* server.c */
/******************************************************************
 *         -------- S E R V I C O - T C P -----------             *
 *  Programa demonstrativo da utilizacao de sockets no dominio    *
 *  Internet (AF_INET), utilizando protocolo de transporte TCP.   *
 *   . Deve ser executado antes do cliente e sua execucao         *
 *     apresentarah a porta alocada dinamicamente a ser           *
 *     utilizada pelo cliente.                                    *
 *                                                                *
 *  Linha de comando: ./server                                    *
 *  Compilar com: clang -Wall server.c -o server                  *
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define		MAXBUFF		1024
#define		CTRL_BACKLOG	5
#define		FIM		"fim da transmissao"

int server(int);

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr;
    int socketfd, serv_len;

    // Cria o socket de comunicacao TCP
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0) {
	printf("\nservidor: erro na criacao do socket\n");
	exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    // Preenche a estrutura do servidor e faz o bind local
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port        = htons(0);

    serv_len = sizeof(serv_addr);
    if (bind(socketfd, (struct sockaddr *) &serv_addr, serv_len) < 0) {
	printf("\nservidor: erro no bind local\n");
	close(socketfd);
	exit(0);
    }

    // Apresenta a porta de servico alocada dinamicamente pelo SO
    if (getsockname(socketfd, (struct sockaddr *)&serv_addr, &serv_len) < 0) {
	printf("\nservidor: erro na chamada da funcao getsockname\n");
	close(socketfd);
	exit(0);
    }

    printf("\nPorta alocada para o servico: %d\n", ntohs(serv_addr.sin_port));
    // Chama a funcao server para esperar uma requisicao de servico

    server(socketfd);

    puts("\n");
    // Fecha o socket apos uso
    close(socketfd);
    return(0);
}

/*--------------------------------------------------------------------
  Funcao server: - Recebe o nome do arquivo enviado pelo cliente
  /////////////  - Abre o arquivo no disco e envia seu conteudo, ou
                 - Envia uma mensagem de erro se o arquivo nao existe
---------------------------------------------------------------------*/
int server(int socketfd) {
   struct sockaddr_in cli_addr;
   char buff[MAXBUFF] = {};
   int newsocketfd, n, fd, cli_len;

   // Prepara para receber conexoes dos clientes
   listen(socketfd, CTRL_BACKLOG);

   cli_len = sizeof(cli_addr);

   // Aguarda a conexao de algum cliente
   if ((newsocketfd = accept(socketfd, (struct sockaddr *) &cli_addr, &cli_len)) < 0) {
	printf("\nFuncao server: erro no accept\n");
	close(socketfd);
	exit(0);
   }

   // Le o nome do arquivo enviado pelo cliente atraves do socket
   if ((n = read(newsocketfd, buff, MAXBUFF)) < 0) {
	printf("\nFuncao server: erro no recebimento do nome do arquivo\n");
	close(socketfd);
	close(newsocketfd);
	exit(0);
   }
   buff[n] = '\0';
   printf("\n\tNome do arquivo: %s\t(%d bytes)", buff, n);

   // Abre o arquivo para leitura e envia seu conteudo pelo socket
   if ((fd = open(buff, 0)) < 0) {
	// Se ocorreu erro, envia uma mensagem de erro ao cliente
	sprintf(buff, "\nServidor nao consegue abrir arquivo solicitado\n");
	n = strlen(buff);
	if (write(newsocketfd, buff, n) != n) {
		printf("\nFuncao server: erro no envio da mensagem de erro\n");
		close(socketfd);
		close(newsocketfd);
		exit(0);
	}
   } else {	// Se o arquivo existe, le o conteudo e envia para o cliente
	while ((n = read(fd, buff, MAXBUFF)) > 0) {
		if (write(newsocketfd, buff, n) != n) {
		    printf("\nFuncao server: erro no envio dos dados do arq. pelo socket\n");
		    close(socketfd);
		    close(newsocketfd);
		    exit(0);
		}
	}
	if (n < 0) {
	    printf("\nFuncao server: erro na leitura do arquivo\n");
	    close(socketfd);
	    close(newsocketfd);
	    exit(0);
	}
   }		// Fim do else

   // sleep(1);

   // Envia aviso de fim de transmissao
   n = sizeof(FIM);
   if (write(newsocketfd, FIM, n) != n) {
	printf("\nFuncao server: erro no envio do fim de transmissao pelo socket\n");
	close(socketfd);
	close(newsocketfd);
	exit(0);
   }
   return(0);
}		// Fim funcao server


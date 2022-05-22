/* clientcp.c */
/******************************************************************
 *         -------- C L I E N T E - T C P -----------             *
 *  Programa demonstrativo da utilizacao de sockets no dominio    *
 *  Internet (AF_INET), utilizando protocolo de transporte TCP    *
 *                                                                *
 *  Linha de comando: clientcp ipaddress portnumber filename      *
 *  onde                                                          *
 *      clientcp:   nome do programa cliente                      *
 *      ipaddress:  endereco IP do host onde esta o servico       *
 *      portnumber: nro. da porta de servico no host remoto       *
 *      filename:   nome do arquivo no sistema remoto             *
 *                                                                *
 * Compilar com: clang -Wall clientcp.c -o clientcp               *
 ******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netdb.h>
#include <fcntl.h>

#define		MAXBUFF	1024
#define		FNAME	256
#define		FIM	"fim da transmissao"

void client(int, char [FNAME]);

int main(int argc, char *argv[]) {
    struct sockaddr_un serv_addr;	// Estrutura de endereco do servidor
    char   filename[FNAME] = {"./texto.txt"};
    int    socketfd;
    int serv_len;
    // Verifica se todos os parametros foram informados
    //if (argc != 4) {
	//    printf("\nUsage: clientcp ipaddress portnumber filename\n\n");
	//    exit(0);
    //}

    // Guarda, em filename, o nome do arquivo informado na linha de comando
    //strcpy(filename, argv[3]);

    bzero((char *) &serv_addr, sizeof(serv_addr));

    // Preenche a estrutura "serv_addr" com o endereco do servidor
    
    if ((socketfd = socket(AF_UNIX, SOCK_STREAM, 0) ) < 0) {
	    printf("\ncliente: erro na criacao do socket!\n");
	    exit(0);
    }
    serv_addr.sun_family      = AF_UNIX;
    strcpy(serv_addr.sun_path, "./abc");

    serv_len = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
    connect(socketfd, (struct sockaddr*)&serv_addr, serv_len);
    // Chama a funcao client para enviar o pedido e receber a resposta
    client(socketfd, filename);

    puts("\n");
    // Fecha o socket apos uso
    close (socketfd);
    return(0);
}	// Fim main()

/*------------------------------------------------------------
  Funcao client: - Envia o nome do arquivo ao servidor
  /////////////  - Recebe os dados vindos do servidor e os
                   apresenta no terminal de texto
-------------------------------------------------------------*/
void client(int socketfd, char filename[FNAME]) {
    char buff[MAXBUFF] = {};
    int  n;
    char c[2] = {""};
    // Envia o nome do arquivo ao servidor
    n = strlen(filename);
    printf("Enviando > %s", filename);
    printf("Enviando > %d", n);
    if (write(socketfd, filename, n) != n) {
        perror("falhou");
        //printf("\nFuncao client: erro no envio do nome do arquivo ao servidor\n");
        close(socketfd);
        exit(0);
    }
    // read(socketfd, c, 2);
	// printf("\nteste: %c", c[0]);

    do{
		recv(socketfd, c, 2, 0);
		
		if(c[1] != 1){
			printf("%c", c[0]);
		}
	}while (c[1] != 1);
	printf("\n");

    // Le os dados vindos do servidor e os escreve para a saida padrao
        /*
    do {
        read(socketfd, buff, MAXBUFF);
        printf("%s", buff);
	if ((n = read(socketfd, buff, MAXBUFF)) < 0) {
		printf("\nFuncao client: erro no recebimento do conteudo do arquivo\n");
		close(socketfd);
		exit(0);
	}
	if (strcmp(buff, FIM) != 0)		// Se nao eh fim de transmissao
		if (write(1, buff, n) != n) {	// Escreve os dados para a saida padrao (fd 1 = stdout)
		    printf("\nFuncao client: erro na escrita para o video\n");
		    close(socketfd);
		    exit(0);
		}
    } while (buff != EOF);
        */
    return;
}	// Fim da funcao client


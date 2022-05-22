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
 * Compilar com: gcc -Wall clientcp.c -o clientcp               *
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
#define		MAXLENGTH 2
#define		FNAME	256
#define		FIM	"fim da transmissão"

void client(int, char [FNAME]);

int main(int argc, char *argv[]) {
    struct sockaddr_un serv_addr;
    char   filename[FNAME] = {"./texto.txt"};
    int    socketfd;
    int serv_len;

    bzero((char *) &serv_addr, sizeof(serv_addr));

    if ((socketfd = socket(AF_UNIX, SOCK_STREAM, 0) ) < 0) {
	    printf("\ncliente: erro na criação do socket!\n");
	    exit(0);
    }
    serv_addr.sun_family      = AF_UNIX;
    strcpy(serv_addr.sun_path, "./abc");

    serv_len = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
    connect(socketfd, (struct sockaddr*)&serv_addr, serv_len);
    client(socketfd, filename);

    puts("\n");
    close (socketfd);
    return(0);
}

void client(int socketfd, char filename[FNAME]) {
    char c[MAXLENGTH] = {""};
    int  n;

    n = strlen(filename);
    if (write(socketfd, filename, n) != n) {
        printf("\nFunção client: erro no envio do nome do arquivo ao servidor\n");
        close(socketfd);
        exit(0);
    }
	while (c[1] != 1){
        recv(socketfd, c, MAXLENGTH, 0);
		if(c[1] != 1){
            printf("%c",c[0]);
		}
    }
	puts("\n");

    close(socketfd);
    return;
}


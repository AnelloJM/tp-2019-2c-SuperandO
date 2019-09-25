#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
/**

cliente para pruebas
**/
/*
int main(int argc, char *argv[]){

	if(argc > 2){
		char *ip;
		int fd,numbytes,puerto;
		char buf[100];
		puerto = atoi(argv[2]);
		ip=argv[1];

		struct hostent *he;
		struct sockaddr_in server;

		if((he = gethostbyname(ip))==NULL){
			printf("[-] Error gethostbyname() \n");
			exit(-1);
		}

		if((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1){
			printf("[-] Error socket()");
			exit(-1);
		}

		server.sin_family = AF_INET;
		server.sin_port = htons(puerto);
		server.sin_addr = *((struct in_addr *)he -> h_addr);

		bzero(&(server.sin_zero),8);

		if(connect(fd, (struct sockaddr *)&server,sizeof(struct sockaddr))==-1){
			printf("[-] Erro connect() \n");
			exit(-1);
		}

		if((numbytes=recv(fd,buf,100,0)) == -1){
			printf("[-] Error en recv() \n");
			exit(-1);

		}
		buf[numbytes]='\0';

		printf("Mensaje del servidor %s\n",buf);
		close(fd);


	}else{
		printf("[-] No se ingreso el ip y el puerto por parametros \n");
	}


	return 0;
}
*/

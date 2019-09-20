#include "conexion_test.h"

/*
conexion de prueba porque no me deja importar libreria comun
*/

int iniciar_servidor(char* ip, int puerto){


		int fd,fd2,longitud_cliente;

		struct sockaddr_in server;
		struct sockaddr_in client;

		server.sin_family = AF_INET;
		server.sin_port = htons(puerto);
		server.sin_addr.s_addr = INADDR_ANY;
		bzero(&(server.sin_zero),8);

		if((fd=socket(AF_INET,SOCK_STREAM,0) )<0)
    {
			perror("[-] Error de apertura de socket.");
			exit(-1);
		}

		if(bind(fd,(struct sockaddr*)&server,sizeof(struct sockaddr)) == 1)
    {
			printf("[-] Error en bind() \n");
			exit(-1);
		}

    printf("Servidor esperando en el puerto : %d\n",puerto );

		if(listen(fd,5) == -1)
    {
			printf("[-] Error en listen() \n");
			exit(-1);
		}

		while(1)
    {
			longitud_cliente = sizeof(struct sockaddr_in);
			if((fd2 = accept(fd,(struct sockaddr *)&client,&longitud_cliente)) == -1)
      {
				printf("[-] Error en accept() \n");
				exit(-1);
		  }

		    send(fd2,"MUSE > FUSE , lo dice la biblia. \n",50,0);
		    close(fd2);
		}


    close(fd);

    return 0;

}

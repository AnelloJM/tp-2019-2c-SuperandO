#include "conexion_muse.h"


uint32_t iniciar_servidor(uint32_t puerto)  //estaria bueno que el logger no se maneje aca
{

  serv.sin_family = AF_INET;
  serv.sin_port = htons(puerto); //Define the port at which the server will listen for connections.
  serv.sin_addr.s_addr = INADDR_ANY;
  fd = socket(AF_INET, SOCK_STREAM, 0); //This will create a new socket and also return the identifier of the socket into fd.
  // To handle errors, you can add an if condition that checks whether fd is greater than 0. If it isn't, prompt an error


  bind(fd, (struct sockaddr *)&serv, sizeof(serv)); //assigns the address specified by serv to the socket
  listen(fd,5); //Listen for client connections. Maximum 5 connections will be permitted.
  //Now we start handling the connections.
  printf("Servidor levantado en el puerto %d\n",puerto);
  conn = accept(fd, (struct sockaddr *)NULL, NULL);
  int i=0;
  while(i < 4 ) {
    recibir_paquete(conn);


/*    uint32_t next_buff = recibir_primer_mensaje(conn);
    recibir_mensaje(conn,next_buff);
  /*    int pid;
      if((pid = fork()) == 0) {
            while (recv(conn, message, 100, 0)>0) {
              printf("Message Received: %s\n", message);
                  //An extra breaking condition can be added here (to terminate the child process)

              }
            exit(0);
      }
*/
    i = i +1;
    }
    return 0;
}

uint32_t conectarse_a_servidor(char *ip,uint32_t puerto)
{
  fd = socket(AF_INET, SOCK_STREAM, 0);
  serv.sin_family = AF_INET;
  serv.sin_port = htons(puerto);

  inet_pton(AF_INET, ip, &serv.sin_addr); //This binds the client to localhost
  if(connect(fd, (struct sockaddr *)&serv, sizeof(serv))==-1) //This connects the client to the server.
  {
    printf("Error al conectarse\n");
    exit(0);
  }
  return fd;
}

void recibir_paquete(uint32_t destinatario)
{

  uint32_t codigo_op;
  recv(destinatario,&codigo_op,4,0);

  switch (codigo_op)
  {
    case 0:
      printf("Se recibio un muse_alloc\n" );
      recibir_muse_alloc(destinatario);
      break;
    case 1:
      printf("Se recibio un muse_free\n" );
      break;
    case 2:
      printf("Se recibio un muse_get\n" );
      break;
    case 3:
      printf("Se recibio un muse_copy\n" );
      break;
    case 4:
      printf("Se recibio un muse_map\n" );
      break;
    case 5:
      printf("Se recibio un muse_sync\n" );
      break;
    case 6:
      printf("Se recibio un muse_unmap\n" );
      break;
    case 7:
      printf("Se recibio un muse_close\n" );
      break;

    default:
      printf("Codigo de operacion invalido\n");
  }

}

uint32_t recibir_muse_alloc(uint32_t destinatario)
{
  uint32_t tam;
  recv(destinatario,&tam,4,0);
  printf("El tamanio pedido es de %d\n",tam );
  return 0;
}


uint32_t enviar_muse_alloc(uint32_t destino,Paquete_muse_alloc *paquete)
{

  void *buffer = malloc(8);
  memcpy(buffer,&(paquete->op),4);
  memcpy(buffer+4,&(paquete->size_alloc),4);
  send(destino,buffer,8,0);
  printf("Operacion muse alloc %d enviada\n",paquete->size_alloc );
}

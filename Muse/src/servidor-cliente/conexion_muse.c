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


void enviar_paquete(uint32_t destino,uint32_t codigo,uint32_t tamanio_leer,void * data)
{
  void * buffer = malloc(20); //enviamos un buffer de 20 bytes siempre
  //rellenamos con "\0" el buffer ?
  Paquete *paquete_armado;
  paquete_armado->op = codigo;
  paquete_armado->size_next = tamanio_leer;
  paquete_armado->valor = data;
  memcpy(buffer,&(paquete_armado->op),4);
  memcpy(buffer+4,&(paquete_armado->size_next),4);
  memcpy(buffer+8,&(paquete_armado->valor),strlen(data));
  send(destino,buffer,8,0);
  free(buffer);
}

void enviar_paquete_v2(uint32_t destino,Paquete *paquete)
{
  void * buffer=malloc(8+paquete->size_next);
  memcpy(buffer,&(paquete->size_next),4);
  memcpy(buffer+4,&(paquete->op),4);
  memcpy(buffer+8,&(paquete->valor),paquete->size_next);
  printf("op: %d\n",paquete->op);
  printf("valor: %d\n",paquete->valor);
  send(destino,buffer,8+(paquete->size_next),0);
  printf("Flag FInal\n" );
}
void recibir_paquete(uint32_t destinatario)
{
  size_t largo_mensaje;
  uint32_t codigo_op;
  recv(destinatario,largo_mensaje,4,0);
  recv(destinatario,codigo_op,4,0);
  void *buffer = malloc(largo_mensaje);
  recv(destinatario,buffer,largo_mensaje,0);


  Paquete *paquete =malloc(sizeof(Paquete));
  paquete->size_next = largo_mensaje;
  printf("asdasd\n" );
  paquete->op = codigo_op;

  printf("El codigo es : %d\n",paquete->op );
  printf("fasd\n" );
  memcpy(&(paquete->valor),buffer,largo_mensaje);
  printf("Azulasdasdasdasda\n" );
  //printf("El mensaje es: %p\n",paquete->valor );


  //desserealizar(buffer,4+largo_mensaje);

}

void desserealizar(char *paquete_recibido,size_t largo_paquete)
{
  Paquete *paquete =malloc(sizeof(Paquete));
  memcpy(&(paquete->op),paquete_recibido,4);
  memcpy(&(paquete->valor),paquete_recibido+4,largo_paquete-8);
  printf("El codigo es : %d\n",paquete->op );
  printf("El mensaje es: %p\n",paquete->valor );

}



void recibir_mensaje(uint32_t destinatario,uint32_t tamanio_recibir)
{

  void *buffer = malloc(4+tamanio_recibir);

  recv(destinatario,buffer,4+tamanio_recibir,0);
  estructura_recibir *paquete = malloc(sizeof(estructura_recibir));

  memcpy(&(paquete->operacion),buffer,4);

  memcpy(&(paquete->valor),buffer+4,tamanio_recibir);
  //tratar_operacion(struct estructura_recibir)
  free(buffer);
}

uint32_t recibir_primer_mensaje(uint32_t destinatario)
{
  void *buffer = malloc(4);
  recv(destinatario,buffer,4,0);

  Next_size *next_size = malloc(sizeof(Next_size));

  memcpy(&(next_size->size),buffer,4);
  recibir_mensaje(destinatario,next_size->size);
  printf("tamanio del proximo buffer : %d\n",next_size->size );

  free(buffer);
  return next_size->size;
}

void enviar_primer_mensaje(uint32_t destino,uint32_t tamanio_enviar)
{
  Next_size *tam = malloc(sizeof(Next_size));
  tam->size = tamanio_enviar;
  void *buff = malloc(4);
  memcpy(buff,&(tam->size),4);
  send(destino,buff,4,0);
  free(buff);
}

void enviar_mensaje(uint32_t destino,uint32_t codigo_op,char parametros)
{
  size_t largo_de_parametros = sizeof(parametros);
  void *buffer = malloc(4+largo_de_parametros);
  memcpy(buffer,codigo_op,4);
  //memcpy(buffer+4,largo_de_parametros,4);
  memcpy(buffer+4,parametros,largo_de_parametros);
  send(destino,buffer,4+largo_de_parametros,0);
  printf("operacion enviada!\n");
  free(buffer);
}

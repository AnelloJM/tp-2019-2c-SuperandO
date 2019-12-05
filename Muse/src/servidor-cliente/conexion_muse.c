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

  while(1)
  {
    conn = accept(fd, (struct sockaddr *)NULL, NULL);
    recibir_paquete(conn);
    //enviar_respuesta(fd,1);
    close(conn);
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

  uint32_t codigo_op,respuesta;
  recv(destinatario,&codigo_op,4,0);

  switch (codigo_op)
  {

    case 0:
      printf("Se recibio un muse_alloc\n" );
      Paquete_respuesta *paquete_malloc = malloc(sizeof(Paquete_respuesta));
      respuesta = recibir_muse_alloc(destinatario);
      paquete_malloc->resp = respuesta;
      responder_proceso(destinatario,paquete_malloc);
      free(paquete_malloc);
      break;
    case 1:
      printf("Se recibio un muse_free\n" );
      recibir_muse_free(destinatario);
      break;
    case 2:
      printf("Se recibio un muse_get\n" );
      Paquete_respuesta *paquete_get = malloc(sizeof(Paquete_respuesta));
      respuesta = recibir_muse_get(destinatario);
      paquete_get->resp = respuesta;
      responder_proceso(destinatario,paquete_get);
      free(paquete_get);
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


void enviar_muse_alloc(uint32_t destino,Paquete_muse_alloc *paquete)
{

  void *buffer = malloc(8);
  memcpy(buffer,&(paquete->op),4);
  memcpy(buffer+4,&(paquete->size_alloc),4);
  send(destino,buffer,8,0);
  printf("Operacion muse_alloc(%d) enviada\n",paquete->size_alloc );
  free(buffer);
}



uint32_t recibir_muse_free(uint32_t destinatario)
{
  uint32_t dst;
  recv(destinatario,&dst,4,0);
  printf("La direccion a liberear es %d\n",dst );
  return 0;
}
void enviar_muse_free(uint32_t destino,Paquete_muse_free *paquete)
{
  void *buffer = malloc(8);
  memcpy(buffer,&(paquete->op),4);
  memcpy(buffer+4,&(paquete->direccion),4);
  send(destino,buffer,8,0);
  printf("Operacion muse_free(%d) enviada\n",paquete->direccion );

  free(buffer);
}


uint32_t recibir_muse_get(uint32_t destinatario)
{
  uint32_t valor = 1111;
  uint32_t read_pos,read_size;
  recv(destinatario,&read_pos,4,0);
  recv(destinatario,&read_size,4,0);
  printf("Se pidio obtener de la posicion %d los %d siguientes bits\n",read_pos,read_size );

  return valor;
}


void enviar_muse_get(uint32_t destino,Paquete_muse_get *paquete)
{
  void *buffer = malloc(12);
  memcpy(buffer,&(paquete->op),4);
  memcpy(buffer+4,&(paquete->p_muse_read),4);
  memcpy(buffer+8,&(paquete->read_size),4);
  send(destino,buffer,12,0);
  printf("Operacion muse_get(%d,%d) enviada\n",paquete->p_muse_read,paquete->read_size );
  free(buffer);

}



void responder_proceso(uint32_t destinatario,Paquete_respuesta *paquete)
{
  void *buffer = malloc(4);
  memcpy(buffer,&(paquete->resp),4);
  send(destinatario,buffer,4,0);
  printf("respuesta enviada!\n" );
  free(buffer);
}

uint32_t esperar_respuesta_uint(uint32_t destino)
{
  uint32_t respuesta;
  recv(destino,&respuesta,4,0);
  printf("Respuesta de muse: %d\n",respuesta );
  return respuesta;
}







/*




uint32_t recibir_muse_cpy(uint32_t destinatario);
uint32_t enviar_muse_cpy(uint32_t destino,Paquete_muse_cpy *paquete);

uint32_t recibir_muse_close(uint32_t destinatario);
uint32_t enviar_muse_close(uint32_t destino,Paquete_muse_close *paquete);
*/

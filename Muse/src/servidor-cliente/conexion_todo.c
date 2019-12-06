#include "conexion_muse.h"


uint32_t iniciar_servidor(uint32_t puerto)  //estaria bueno que el logger no se maneje aca
{

  serv.sin_family = AF_INET;
  serv.sin_port = htons(puerto);
  serv.sin_addr.s_addr = INADDR_ANY;
  fd = socket(AF_INET, SOCK_STREAM, 0);

  bind(fd, (struct sockaddr *)&serv, sizeof(serv));
  listen(fd,MAXCONN);
  printf("Servidor levantado en el puerto %d\n",puerto);

  while(1)
  {
    conn = accept(fd, (struct sockaddr *)NULL, NULL);
    recibir_paquete(conn);
    close(conn);
  }


  return 0;
}

uint32_t conectarse_a_servidor(char *ip,uint32_t puerto)
{
  fd = socket(AF_INET, SOCK_STREAM, 0);
  serv.sin_family = AF_INET;
  serv.sin_port = htons(puerto);

  inet_pton(AF_INET, ip, &serv.sin_addr);
  if(connect(fd, (struct sockaddr *)&serv, sizeof(serv))==-1)
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
      Paquete_respuesta_general *paquete_malloc = malloc(sizeof(Paquete_respuesta_general));
      paquete_malloc = recibir_muse_alloc(destinatario);
      enviar_respuesta_general(destinatario,paquete_malloc);
      free(paquete_malloc);
      break;

    case 1:
      printf("Se recibio un muse_free\n" );
      recibir_muse_free(destinatario);
      break;

    case 2:
      printf("Se recibio un muse_get\n" );
      Paquete_respuesta_general *paquete_get = malloc(sizeof(Paquete_respuesta_general));
      paquete_get = recibir_muse_get(destinatario);
      enviar_respuesta_general(destinatario,paquete_get);
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

Paquete_respuesta_general * recibir_muse_alloc(uint32_t destinatario)
{
  Paquete_respuesta_general *paquete = malloc(sizeof(Paquete_respuesta_general));
  uint32_t tam;
  recv(destinatario,&tam,4,0);

  uint32_t resp_muse;
  resp_muse = tratar_muse_alloc(tam);
  paquete->size_resp = 4;
  paquete->respuesta = resp_muse;
  printf("El tamanio pedido es de %d\n",paquete->respuesta );


  return paquete;
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


Paquete_respuesta_general * recibir_muse_get(uint32_t destinatario)
{
  Paquete_respuesta_general * paquete =malloc(sizeof(Paquete_respuesta_general));

  uint32_t read_pos,read_size;
  recv(destinatario,&read_pos,4,0);
  recv(destinatario,&read_size,4,0);
  printf("Se pidio obtener de la posicion %d los %d siguientes bits\n",read_pos,read_size );


  char * mensaje = malloc(strlen("elmateesamargo"));
  memcpy(mensaje,"elmateesamargo",strlen("elmateesamargo")); //aca copiamos la direcicon que nos pide

  paquete->size_resp = strlen(mensaje);
  memcpy(&(paquete->respuesta),mensaje,strlen(mensaje));
  free(mensaje);
  return paquete;
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


void esperar_respuesta_muse_get(uint32_t destino,Paquete_muse_get *paquete)
{
  void *buffer = malloc(paquete->read_size);
  recv(destino,buffer,paquete->read_size,0);

  printf("Respuesta de muse_get recibida\n");
  free(buffer);
}

void enviar_respuesta_general(uint32_t destino,Paquete_respuesta_general *paquete)
{
  void *buffer = malloc(4+paquete->size_resp);
  memcpy(buffer,&(paquete->size_resp),4);
  memcpy(buffer+4,&(paquete->respuesta),paquete->size_resp);
  send(destino,buffer,4+(paquete->size_resp),0);
  printf("Respuesta enviada!\n" );
  free(buffer);
}

Paquete_respuesta_general * recibir_respuesta_general(uint32_t destinatario)
{
  uint32_t next_buff;
  Paquete_respuesta_general * paquete = malloc(sizeof(Paquete_respuesta_general));
  recv(destinatario,&(next_buff),4,0);
  paquete->size_resp = next_buff;
  void * buffer = malloc(next_buff);
  recv(destinatario,buffer,next_buff,0);
  memcpy(&(paquete->respuesta),buffer,next_buff);

  printf("Respuesta recibida!\n");
  free(buffer);

  return paquete;
}





/*




uint32_t recibir_muse_cpy(uint32_t destinatario);
uint32_t enviar_muse_cpy(uint32_t destino,Paquete_muse_cpy *paquete);

uint32_t recibir_muse_close(uint32_t destinatario);
uint32_t enviar_muse_close(uint32_t destino,Paquete_muse_close *paquete);
*/

#include "lib_conexion.h"

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

void enviar_muse_alloc(uint32_t destino,Paquete_muse_alloc *paquete)
{

  void *buffer = malloc(8);
  memcpy(buffer,&(paquete->op),4);
  memcpy(buffer+4,&(paquete->size_alloc),4);
  send(destino,buffer,8,0);
  printf("Operacion muse_alloc(%d) enviada\n",paquete->size_alloc );
  free(buffer);
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

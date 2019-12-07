#include "lib_conexion.h"


uint32_t iniciar_servidor(uint32_t puerto)  //estaria bueno que el logger no se maneje aca
{

  serv.sin_family = AF_INET;
  serv.sin_port = htons(puerto);
  serv.sin_addr.s_addr = INADDR_ANY;
  fd = socket(AF_INET, SOCK_STREAM, 0);

  bind(fd, (struct sockaddr *)&serv, sizeof(serv));
  listen(fd,MAXCONN);
  printf("Servidor levantado en el puerto %d\n",puerto);

  while(1) //mandarlo a thread
  {
    conn = accept(fd, (struct sockaddr *)NULL, NULL); //mandarlo a thread
    recibir_paquete(conn);
    close(conn);
  }


  return 0;
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
  resp_muse = tratar_muse_alloc(tam,1); // el segundo parametro es el id del proceso
  paquete->size_resp = 4;
  paquete->respuesta = resp_muse;
  printf("El tamanio pedido es de %d\n",paquete->respuesta );


  return paquete;
}

uint32_t recibir_muse_free(uint32_t destinatario)
{
  uint32_t dst;
  recv(destinatario,&dst,4,0);
  printf("La direccion a liberear es %d\n",dst );
  return 0;
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

void enviar_respuesta_general(uint32_t destino,Paquete_respuesta_general *paquete)
{
  void *buffer = malloc(4+paquete->size_resp);
  memcpy(buffer,&(paquete->size_resp),4);
  memcpy(buffer+4,&(paquete->respuesta),paquete->size_resp);
  send(destino,buffer,4+(paquete->size_resp),0);
  printf("Respuesta enviada!\n" );
  free(buffer);
}


/*




uint32_t recibir_muse_cpy(uint32_t destinatario);
uint32_t enviar_muse_cpy(uint32_t destino,Paquete_muse_cpy *paquete);

uint32_t recibir_muse_close(uint32_t destinatario);
uint32_t enviar_muse_close(uint32_t destino,Paquete_muse_close *paquete);
*/

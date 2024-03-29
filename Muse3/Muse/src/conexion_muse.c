///*
// * conexion_muse.c
// *
// *  Created on: Dec 12, 2019
// *      Author: cruz636
// */
//
//
//
#include "conexion_muse.h"
uint32_t iniciar_servidor(char* puerto,t_log* logger)
{
	log_info(logger,"Dentro de iniciar servidor");
	uint32_t socket_servidor;
  char * ip= "127.0.0.1";

  struct addrinfo hints, *servinfo, *p;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;


  getaddrinfo(ip, puerto, &hints, &servinfo);

  for (p=servinfo; p != NULL; p = p->ai_next)
  {
      if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
          continue;
      if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
          close(socket_servidor);
          continue;
      }
      break;
  }

  listen(socket_servidor, MAXCONN);

  freeaddrinfo(servinfo);

  return socket_servidor;
}


uint32_t esperar_cliente_con_accept(uint32_t socket_servidor, t_log* logger)
{
	struct sockaddr_in dir_cliente;
	int tam_direccion = sizeof(struct sockaddr_in);
	log_info(logger, "esperando proceso cliente");

	uint32_t socket_cliente = accept(socket_servidor, &dir_cliente, &tam_direccion);

	log_info(logger, "Se conecto un proceso!");
	return socket_cliente;
}


void recibir_paquete(uint32_t destinatario)
{

  uint32_t codigo_op,respuesta,n_proceso;
  //recibimos el id del proceso
  recv(destinatario,&n_proceso,4,0);
  //recibimos el codigo de operacion
  recv(destinatario,&codigo_op,4,0);

  switch (codigo_op)
  {

    case 0:
      printf("Se recibio un muse_alloc\n" );
      Paquete_respuesta_general *paquete_malloc = malloc(sizeof(Paquete_respuesta_general));
      paquete_malloc = recibir_muse_alloc(destinatario,n_proceso);
      enviar_respuesta_general(destinatario,paquete_malloc);
      free(paquete_malloc);
      break;

    case 1:
      printf("Se recibio un muse_free\n" );
      recibir_muse_free(destinatario,n_proceso);
      break;

    case 2:
      printf("Se recibio un muse_get\n" );
      Paquete_respuesta_general *paquete_get = malloc(sizeof(Paquete_respuesta_general));
      paquete_get = recibir_muse_get(destinatario, n_proceso);
      enviar_respuesta_general(destinatario,paquete_get);
      free(paquete_get);
      break;

    case 3:
      printf("Se recibio un muse_cpy\n" );
      Paquete_respuesta_general *paquete_cpy = malloc(sizeof(Paquete_respuesta_general));
      paquete_cpy = recibir_muse_cpy(destinatario,n_proceso);
      printf("Flgaaa\n");
      enviar_respuesta_general(destinatario,paquete_cpy);
      free(paquete_cpy);
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





Paquete_respuesta_general * recibir_muse_cpy(uint32_t destinatario,uint32_t id_proceso)
{
	Paquete_respuesta_general *paquete_dos = malloc(sizeof(Paquete_respuesta_general));
	uint32_t posicion_a_copiar;
	int tam;
	//recibe tamanio a copiar
	recv(destinatario,&tam,4,0);
	void *buffer = malloc(tam);
	//recibe posicion en la cual copiar
	recv(destinatario,&posicion_a_copiar,4,0);
	//recibe datos a copiar
	recv(destinatario,&buffer,tam,0);
	uint32_t resp_muse;
	tratar_muse_cpy(tam,posicion_a_copiar,buffer,id_proceso);
	paquete_dos->size_resp = 4;
	paquete_dos->respuesta = 0;
	printf("flg\n" );
	/*memcpy(&(paquete->size_resp),4,4);
	printf("Flagg\n" );
	paquete->respuesta = resp_muse;
*/
	printf("Se pidio copiar %d bytes de la posicion %d\n",tam,posicion_a_copiar);
	return paquete_dos;
}


Paquete_respuesta_general * recibir_muse_alloc(uint32_t destinatario,uint32_t id_proceso)
{
  Paquete_respuesta_general *paquete = malloc(sizeof(Paquete_respuesta_general));
  uint32_t tam;
  recv(destinatario,&tam,4,0);

  uint32_t resp_muse;
  resp_muse = tratar_muse_alloc(tam,id_proceso); // el segundo parametro es el id del proceso
  paquete->size_resp = 4;
  paquete->respuesta = resp_muse;
  printf("El tamanio pedido es de %d\n",paquete->respuesta );


  return paquete;
}

Paquete_respuesta_general * recibir_muse_free(uint32_t destinatario,uint32_t id_proceso)
{
  Paquete_respuesta_general *paquete = malloc(sizeof(Paquete_respuesta_general));
  uint32_t dst;
  recv(destinatario,&dst,4,0);
  tratar_muse_free(id_proceso,dst);
  printf("La direccion a liberear es %d\n",dst );


  paquete->size_resp = 1;
  paquete->respuesta = 0;

  return paquete;

}

Paquete_respuesta_general * recibir_muse_get(uint32_t destinatario,uint32_t n_proceso)
{
  Paquete_respuesta_general * paquete =malloc(sizeof(Paquete_respuesta_general));

  uint32_t read_pos,read_size;
  recv(destinatario,&read_pos,4,0);
  recv(destinatario,&read_size,4,0);
  printf("Se pidio obtener de la posicion %d los %d siguientes bits\n",read_pos,read_size );
  //tratar_muse_get(id_proceso,...);

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

//
///*
//
//
//
//
//uint32_t recibir_muse_cpy(uint32_t destinatario);
//uint32_t enviar_muse_cpy(uint32_t destino,Paquete_muse_cpy *paquete);
//
//uint32_t recibir_muse_close(uint32_t destinatario);
//uint32_t enviar_muse_close(uint32_t destino,Paquete_muse_close *paquete);
//*/
//
//

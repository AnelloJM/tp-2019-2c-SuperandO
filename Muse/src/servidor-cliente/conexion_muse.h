#ifndef CONEXION_H_
#define CONEXION_H_
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <string.h>

/*
Funciones para conectar MUSE con los procesos que usan
la libreria libmuse.h
*/

//Estructuras


typedef struct
{
  uint32_t op;
  uint32_t size_alloc;

}Paquete_muse_alloc;

typedef struct
{
  uint32_t op;
  
}Paquete_muse_free;


typedef struct
{
  uint32_t op;
}Paquete_muse_get;


typedef struct
{
  uint32_t op;
}Paquete_muse_copy;


typedef struct
{
  uint32_t op;
}Paquete_muse_map;


typedef struct
{
  uint32_t op;
}Paquete_muse_sync;


typedef struct
{
  uint32_t op;
}Paquete_muse_unmap;


typedef struct
{
  uint32_t op;
}Paquete_muse_close;


//Variables globales

struct sockaddr_in serv; //This is our main socket variable.
uint32_t fd; //This is the socket file descriptor that will be used to identify the socket
uint32_t conn; //This is the connection file descriptor that will be used to distinguish client connections.
char message[100] = ""; //This array will store the messages that are sent by the server


//Funciones


uint32_t conectarse_a_servidor(char *ip,uint32_t puerto);
uint32_t iniciar_servidor(uint32_t puerto);
uint32_t esperar_cliente(uint32_t cliente);
void recibir_paquete(uint32_t destinatario);

uint32_t recibir_muse_alloc(uint32_t destinatario);
uint32_t enviar_muse_alloc(uint32_t destino,Paquete_muse_alloc *paquete);

uint32_t enviar_muse_free(uint32_t destino,Paquete_muse_free *paquete);

#endif

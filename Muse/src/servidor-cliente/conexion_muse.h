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
  uint32_t codigo_op;
  uint32_t tamanio_buffer;
}estructura_codigo;

typedef struct
{
  uint32_t operacion;
  void *valor;
}estructura_recibir;

typedef struct
{
  uint32_t size;
}Next_size;

typedef struct
{
  uint32_t op;
  uint32_t size_alloc;

}Paquete_muse_alloc;



//Variables globales

struct sockaddr_in serv; //This is our main socket variable.
uint32_t fd; //This is the socket file descriptor that will be used to identify the socket
uint32_t conn; //This is the connection file descriptor that will be used to distinguish client connections.
char message[100] = ""; //This array will store the messages that are sent by the server


//Funciones


uint32_t conectarse_a_servidor(char *ip,uint32_t puerto);
uint32_t iniciar_servidor(uint32_t puerto);
uint32_t esperar_cliente(uint32_t cliente);

void enviar_mensaje(uint32_t destino,uint32_t codigo_op,char parametros);
void enviar_primer_mensaje(uint32_t destino,uint32_t tamanio_enviar);
void enviar_paquete(uint32_t destino,uint32_t codigo,uint32_t amanio_leer,void * data);
uint32_t recibir_primer_mensaje(uint32_t destinatario);
void recibir_mensaje(uint32_t destinatario,uint32_t tamanio_recibir);

//void enviar_paquete_v2(uint32_t destino,Paquete *paquete);
void recibir_paquete(uint32_t destinatario);
void desserealizar(char *paquete_recibido,size_t largo_paquete);

//Paquete *crear_paquete(uint32_t operacion,void * data);
uint32_t recibir_muse_alloc(uint32_t destinatario);
uint32_t enviar_muse_alloc(uint32_t destino,Paquete_muse_alloc *paquete);


#endif

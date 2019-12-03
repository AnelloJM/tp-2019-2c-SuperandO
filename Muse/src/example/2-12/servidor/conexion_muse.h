#ifndef CONEXION_H_
#define CONEXION_H_
#include <string.h>
#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

/*
Funciones para conectar MUSE con los procesos que usan
la libreria libmuse.h
*/

//Variables globales

struct sockaddr_in serv; //This is our main socket variable.
uint32_t fd; //This is the socket file descriptor that will be used to identify the socket
uint32_t conn; //This is the connection file descriptor that will be used to distinguish client connections.
char message[100]=""; //This array will store the messages that are sent by the server
//char message_resp[100]="";

//Funciones

void iniciar_servidor(uint32_t puerto);
uint32_t esperar_cliente();
void enviar_mensaje(uint32_t destino,char mensaje);
void recibir_mensaje(uint32_t destinatario);
void atender_cliente(uint32_t pid,char* respuesta);




#endif

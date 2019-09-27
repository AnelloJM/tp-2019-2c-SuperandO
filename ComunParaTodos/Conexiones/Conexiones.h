#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <readline/readline.h>
#include <unistd.h>
#include <commons/collections/list.h>
#include <string.h>

#ifndef CONECCIONES_H_
#define CONECCIONES_H_

int conectarse_a_un_servidor(char *ip, char* puerto,t_log* log);//Retorna la conexion del socket cliente
int iniciar_servidor(char* ip, char* puerto,t_log* logger);//Retorna la conexion del socket servidor
int esperar_cliente_con_accept(int socket_servidor, t_log* logger);//El servidor espera para aceptar un cliente
int enviar_mensaje(int socket_servidor,t_log* logger);
#endif /* CONECCIONES_H_ */

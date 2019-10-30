/*
 ============================================================================
 Name        : Sac-Server.c
 Author      : Superand0
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <Conexiones/Conexiones.h>
#include <commons/log.h>
#include <Serializacion-FileSystem/Serializacion-FileSystem.h>

t_log *logger;

int main(void) {

	logger = log_create("Sac-Server.log", "Sac-Server", 1, LOG_LEVEL_INFO);
	log_info(logger, "Se ha creado un nuevo logger\n");
	//puts("Superand0 Sac-Server Iniciando..."); //prints Superand0 Sac-Server Iniciando...
	int conexion, cliente;
	Paquete *paquete = malloc(sizeof(Paquete));
	paquete = NULL;
	conexion = iniciar_servidor("127.0.0.1", "8457", logger);
	cliente = esperar_cliente_con_accept(conexion, logger);
	RecibirPaqueteServidorFuse(cliente, paquete);
	Paquete buff;
	//int err=recv(cliente, &buff,sizeof(Paquete),MSG_WAITALL);
/*	if (err = -1){
		log_error(logger, "ERROR GATO\n");
	}
*/	f_getattr *info = buff.mensaje;
	puts(&(info->path));
	log_info(logger, "recibi: %s", &(info->path));
	//RecibirPaqueteCliente(cliente, paquete);
	//EnviarHandshake(cliente, 2);
	free(paquete);
	return EXIT_SUCCESS;
}

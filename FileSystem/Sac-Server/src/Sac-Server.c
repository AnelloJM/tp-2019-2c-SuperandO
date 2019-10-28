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
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <Conexiones/Conexiones.h>
#include <Serializacion/serializacion.h>
#include <commons/log.h>
#include <Comun-FileSystem/Serializacion-FileSystem/Serializacion-FileSystem.h>

t_log *logger;

int main(void) {

	logger = log_create("Sac-Server.log", "Sac-Server", 1, LOG_LEVEL_INFO);
	log_info(logger, "Se ha creado un nuevo logger\n");
	//puts("Superand0 Sac-Server Iniciando..."); //prints Superand0 Sac-Server Iniciando...
	int conexion, cliente;
	Paquete *paquete;
	conexion = iniciar_servidor("127.0.0.1", "8080", logger);
	cliente = esperar_cliente_con_accept(conexion, logger);
	RecibirPaqueteServidor(cliente, 3, paquete);
	//RecibirPaqueteCliente(cliente, paquete);
	//EnviarHandshake(cliente, 2);
	return EXIT_SUCCESS;
}

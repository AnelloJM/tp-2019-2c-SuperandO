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
#include <commons/collections/list.h>
#include <Serializacion-FileSystem/Serializacion-FileSystem.h>
#include <pthread.h>

t_log *logger;
int conexion;

void FuseGetattr(){}

void* funcionMagica(int cliente){
	while(1){
		HeaderFuse headerRecibido;
		headerRecibido = Fuse_RecieveHeader(cliente);
		log_info(logger, "Codigo de operacion: %i", headerRecibido.operaciones);
		log_info(logger, "Tamanio: %i", headerRecibido.tamanioMensaje);
		switch(headerRecibido.operaciones){
			case f_GETATTR:;
				//desempaquetar pack y hacer el codigo
				char *path_getattr= Fuse_ReceiveAndUnpack_Path(cliente, headerRecibido.tamanioMensaje);
				log_info(logger,"tamanio del path que recive: %i \0", strlen(path_getattr)+1);
				log_info(logger, path_getattr);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi GETATTR"), f_HANDSHAKE);
				break;
			case f_READDIR: ;
				//desempaquetar pack y hacer el codigo
				break;
			case f_READ: ;
				//desempaquetar pack y hacer el codigo
				break;
			case f_OPEN: ;
				//desempaquetar pack y hacer el codigo
				break;
			case f_HANDSHAKE: ;
				//desempaquetar pack y hacer el codigo
				char *path_handshake = Fuse_ReceiveAndUnpack_Path(cliente, headerRecibido.tamanioMensaje);
				log_info(logger,"tamanio del path que recive: %i \0", strlen(path_handshake)+1);
				log_info(logger, path_handshake);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi GETATTR"), f_HANDSHAKE);
				break;
			default:
				log_error(logger, "No es un codigo conocido: %i", headerRecibido.operaciones);
				break;
		}
	}
}

int main(void) {

	logger = log_create("Sac-Server.log", "Sac-Server", 1, LOG_LEVEL_INFO);
	log_info(logger, "Se ha creado un nuevo logger\n");
	int cliente;
	conexion = iniciar_servidor("127.0.0.1", "8999", logger);



	t_list* hilosClientes = list_create();

	while(1){
		cliente = esperar_cliente_con_accept(conexion, logger);

		pthread_t* cody = malloc(sizeof(pthread_t));
		list_add(hilosClientes,cody);
		if(pthread_create(cody,NULL,(void*)funcionMagica,cliente) == 0){
			pthread_detach(cody);
			log_info(logger,"Se creo el hilo sin problema, cliente: %i", cliente);
		}else{
			log_error(logger,"No se pudo crear el hilo, cliente: %i", cliente);
		}
	};
	return EXIT_SUCCESS;
}

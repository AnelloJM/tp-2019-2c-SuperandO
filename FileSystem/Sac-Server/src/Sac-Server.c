/*
 ============================================================================
 Name        : Sac-Server.c
 Author      : Superand0
 Version     : 0.1
 Copyright   : Superand0 2C 2019
 Description : Proceso que se encarga de recibir peticiones de los Sac-Cli's
 	 	 	   Manejara un File System a traves de un bitmap en la computadora
 	 	 	   en la que se corra
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
		uint32_t tam = headerRecibido.tamanioMensaje;
		switch(headerRecibido.operaciones){
			case f_GETATTR:;
				char *pathGetAttr= Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathGetAttr)+1);
				log_error(logger, pathGetAttr);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi GETATTR"), f_HANDSHAKE);
				free(pathGetAttr);

				//Esto es para probar si funciona Fuse_PackAndSend_IntResponse
				//Fuse_PackAndSend_IntResponse(cliente, 1, f_RESPONSE);

				break;

			case f_READDIR: ;
				char *pathReadDir = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathReadDir)+1);
				log_error(logger, pathReadDir);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi READDIR"), f_HANDSHAKE);
				free(pathReadDir);
				break;

			case f_READ: ;
				char *pathRead = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRead)+1);
				log_error(logger, pathRead);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi READ"), f_HANDSHAKE);
				free(pathRead);
				break;

			case f_OPEN: ;
				char *pathOpen = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathOpen)+1);
				log_error(logger, pathOpen);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi OPEN"), f_HANDSHAKE);
				free(pathOpen);
				break;

			case f_RELEASE: ;
				char *pathRelease = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRelease)+1);
				log_error(logger, pathRelease);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi RELEASE"), f_HANDSHAKE);
				free(pathRelease);
				break;

			case f_WRITE: ;
				char *pathWrite = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathWrite)+1);
				log_error(logger, pathWrite);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi WRITE"), f_HANDSHAKE);
				free(pathWrite);
				break;

			case f_MKNOD: ;
				char *pathMKNod = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathMKNod)+1);
				log_error(logger, pathMKNod);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi MKNOD"), f_HANDSHAKE);
				free(pathMKNod);
				break;

			case f_UNLINK: ;
				char *pathUnlink = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathUnlink)+1);
				log_error(logger, pathUnlink);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi UNLINK"), f_HANDSHAKE);
				free(pathUnlink);
				break;

			case f_MKDIR: ;
				char *pathMKDir = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathMKDir)+1);
				log_error(logger, pathMKDir);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi MKDIR"), f_HANDSHAKE);
				free(pathMKDir);
				break;

			case f_RMDIR: ;
				char *pathRMDir = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRMDir)+1);
				log_error(logger, pathRMDir);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi RMDIR"), f_HANDSHAKE);
				free(pathRMDir);
				break;

			case f_CHMOD: ;
				char *pathCHMod = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathCHMod)+1);
				log_error(logger, pathCHMod);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi CHMOD"), f_HANDSHAKE);
				free(pathCHMod);
				break;

			case f_UTIME: ;
				char *pathUtime = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathUtime)+1);
				log_error(logger, pathUtime);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi UTIME"), f_HANDSHAKE);
				free(pathUtime);
				break;

			case f_RENAME: ;
				char *pathRename = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRename)+1);
				log_error(logger, pathRename);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi RENAME"), f_HANDSHAKE);
				free(pathRename);
				break;

			case f_TRUNCATE: ;
				char *pathTruncate = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathTruncate)+1);
				log_error(logger, pathTruncate);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi TRUNCATE"), f_HANDSHAKE);
				free(pathTruncate);
				break;

			case f_SETXATTR: ;
				char *pathSetXAttr = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathSetXAttr)+1);
				log_error(logger, pathSetXAttr);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi SETXATTR"), f_HANDSHAKE);
				free(pathSetXAttr);
				break;

			case f_HANDSHAKE: ;
				//desempaquetar pack y hacer el codigo
				char *pathHandshake = Fuse_ReceiveAndUnpack_Path(cliente, headerRecibido.tamanioMensaje);
				log_info(logger,"tamanio del path que recive: %i \0", strlen(pathHandshake)+1);
				log_info(logger, pathHandshake);
				Fuse_PackAndSend_Path(cliente, strdup("Hola, recibi HANDSHAKE"), f_HANDSHAKE);
				free(pathHandshake);
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
	conexion = iniciar_servidor("127.0.0.1", "8799", logger);



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

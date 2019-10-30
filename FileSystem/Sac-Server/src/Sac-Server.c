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
#include <pthread.h>


t_log *logger;
/*int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}
void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}
t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
	return NULL;
}
*/

void* funcionMagica(int cliente){
	Paquete *paquete = malloc(sizeof(Paquete));
	paquete = NULL;
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
}

int main(void) {

	logger = log_create("Sac-Server.log", "Sac-Server", 1, LOG_LEVEL_INFO);
	log_info(logger, "Se ha creado un nuevo logger\n");
	int conexion, cliente;

	conexion = iniciar_servidor("127.0.0.1", "8223", logger);
	int cantidad,err;
	pthread_t cody[cantidad];
	cantidad=0;
	do{
		cliente = esperar_cliente_con_accept(conexion, logger);
		err=pthread_create(cody[cantidad],NULL,funcionMagica,cliente);
		if(err){
			log_error(logger,strerror(err));
		}
		pthread_detach(cody[cantidad]);
		cantidad+=1;
	}while(cantidad<10);
	return EXIT_SUCCESS;
}

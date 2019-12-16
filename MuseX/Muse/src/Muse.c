/*
 ============================================================================
 Name        : Muse.c
 Author      : Superand0
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "Muse.h"

void iniciar_logger(){
	logger = log_create("Muse.log", "Muse", 1, LOG_LEVEL_INFO);
	log_info(logger, "::::::Se ha creado un nuevo logger::::::");
}

void *funcionMagica(int cliente){
	while(1){
		HeaderMuse headerRecibido;
		headerRecibido = Muse_RecieveHeader(cliente);
		log_info(logger, "Codigo de operacion: %i", headerRecibido.operaciones);
		log_info(logger, "Tamanio: %i", headerRecibido.tamanioMensaje);
		if(headerRecibido.operaciones == -1){
			log_error(logger, "Se desconecto el cliente\n");
			break;
		}
		uint32_t tam = headerRecibido.tamanioMensaje;
		switch(headerRecibido.operaciones){
			case m_INIT:;
				log_error(logger,"Me llego un: %i",m_INIT);
				break;
			case m_CLOSE:;
				log_info(logger,"Me llego un: %i",m_CLOSE);
				char* resivido_close = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"resivi: %s",resivido_close);
				free(resivido_close);
				break;
			case m_ALLOC:;
				log_info(logger,"Me llego un: %i",m_ALLOC);
				char* resivido_alloc = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"resivi: %s",resivido_alloc);
				free(resivido_alloc);
				break;
			case m_FREE:;
				log_info(logger,"Me llego un: %i",m_FREE);
				char* resivido_free = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"resivi: %s",resivido_free);
				free(resivido_free);
				break;
			case m_GET:;
				log_info(logger,"Me llego un: %i",m_GET);
				char* resivido_get = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"resivi: %s",resivido_get);
				free(resivido_get);
				break;
			case m_CPY:;
				log_info(logger,"Me llego un: %i",m_CPY);
				char* resivido_cpy = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"resivi: %s",resivido_cpy);
				free(resivido_cpy);
				break;
			case m_MAP:;
				log_info(logger,"Me llego un: %i",m_MAP);
				char* resivido_map = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"resivi: %s",resivido_map);
				free(resivido_map);
				break;
			case m_SYNC:;
				log_info(logger,"Me llego un: %i",m_SYNC);
				char* resivido_sync = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"resivi: %s",resivido_sync);
				free(resivido_sync);
				break;
			case m_UNMAP:;
				log_info(logger,"Me llego un: %i",m_UNMAP);
				char* resivido_unmap = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"resivi: %s",resivido_unmap);
				free(resivido_unmap);
				break;
			case m_HANDSHAKE:;
				log_info(logger,"Me llego un: %i",m_HANDSHAKE);
				char* resivido_hand = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"resivi: %s",resivido_hand);
				free(resivido_hand);
				break;
			default:
				log_error(logger, "No es un codigo conocido: %i", headerRecibido.operaciones);
				break;
		}
	}
}

int main(void) {
	iniciar_logger();
	int conexion, cliente;
	conexion = iniciar_servidor("127.0.0.1","6969",logger);

	while(1){
		cliente = esperar_cliente_con_accept(conexion, logger);

		pthread_t* proceso = malloc(sizeof(pthread_t));
		if(pthread_create(proceso,NULL,(void*)funcionMagica,(void*)cliente) == 0){
			pthread_detach(*proceso);
			log_info(logger,"Se creo el hilo sin problema, cliente: %i", cliente);
		}else{
			log_error(logger,"No se pudo crear el hilo, cliente: %i", cliente);
		}
	}
	return EXIT_SUCCESS;
}

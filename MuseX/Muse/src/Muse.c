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

void iniciar_config(){
	config = config_create("Muse.config");
	log_info(logger, "::::::Se ha levantado el \"Muse.config\"::::::");

	puerto = strdup(config_get_string_value(config,"LISTEN_PORT"));
	log_info(logger, ":::Puerto: %s:::",puerto);

	memory_size = config_get_int_value(config,"MEMORY_SIZE");
	log_info(logger, ":::memory_size: %i:::",memory_size);

	page_size =	config_get_int_value(config,"PAGE_SIZE");
	log_info(logger, ":::page_size: %i:::",page_size);

	swap_size = config_get_int_value(config,"SWAP_SIZE");
	log_info(logger, ":::swap_size: %i:::",swap_size);
}


char* obtener_IP(int sock_cliente){
	struct sockaddr_in sock_addr;
	uint32_t longitud = sizeof(sock_addr);
	getpeername(sock_cliente, (struct sockaddr*)&sock_addr, &longitud);
	char* ip = inet_ntoa(sock_addr.sin_addr);
	log_info(logger, ":::Ip del proceso: %s", ip);
	return ip;
}

void proceso_nuevo(char* id_del_proceso){
	log_info(logger,"::::ID a cargar: %s::::", id_del_proceso);
	Proceso* nuevo_proceso = malloc(sizeof(Proceso));
	int tam = strlen(id_del_proceso)+1;
	nuevo_proceso->Id_del_proceso = malloc(tam);
	memcpy(nuevo_proceso->Id_del_proceso,id_del_proceso,tam);
	log_info(logger,"::::ID cargado: %s::::", nuevo_proceso->Id_del_proceso);
	nuevo_proceso->segmentos_del_proceso = list_create();
	nuevo_proceso->tabla_de_segmentos = list_create();
	nuevo_proceso->tabla_de_pagina_por_segmento = list_create();
	list_add(procesos_conectados,nuevo_proceso);
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
				log_info(logger,"Me llego un: %i",m_INIT);
				char* pid = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"::::ID local recivido: %s", pid);
				char* ip_del_proceso = obtener_IP(cliente);
				char* id_del_proceso = string_duplicate(ip_del_proceso);
				string_append_with_format(&id_del_proceso,"-%s",pid);
				log_info(logger,"::::ID global del proceso: %s", id_del_proceso);
				proceso_nuevo(id_del_proceso);
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

void iniciar_bitmap_de_frames(){
	int cantidad_de_frames = memory_size/page_size;
	log_info(logger, ":::Cantidad de Frames: %i:::", cantidad_de_frames);
	char *bitarray = malloc(cantidad_de_frames);
	bitmap_para_frames = bitarray_create_with_mode(bitarray, cantidad_de_frames, MSB_FIRST);
	log_info(logger, ":::Bitmap de Frames Creado:::");
}

void iniciar_bitmap_de_swap(){
	char *bitarray = malloc(swap_size);
	bitmap_para_swap = bitarray_create_with_mode(bitarray, swap_size, MSB_FIRST);
	log_info(logger, ":::Bitmap de Swap Creado:::");
}

void iniciar_muse(){
	iniciar_logger();
	iniciar_config();
	procesos_conectados = list_create();
	log_info(logger, ":::Se creo la lista de procesos:::");
	iniciar_bitmap_de_frames();
	iniciar_bitmap_de_swap();
}


int main(void) {
	iniciar_muse();
	int conexion, cliente;
	conexion = iniciar_servidor("127.0.0.1",puerto,logger);

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

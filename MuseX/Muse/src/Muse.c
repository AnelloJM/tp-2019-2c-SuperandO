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


char* obtener_Id_real(int sock_cliente, char* id){
	struct sockaddr_in sock_addr;
	uint32_t longitud = sizeof(sock_addr);
	getpeername(sock_cliente, (struct sockaddr*)&sock_addr, &longitud);
	char* ip = inet_ntoa(sock_addr.sin_addr);
	log_info(logger, ":::Ip del proceso: %s", ip);
	char* id_del_proceso_alloc = string_duplicate(ip);
	string_append_with_format(&id_del_proceso_alloc,"-%s",id);
	return id_del_proceso_alloc;
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



Proceso* buscar_proceso(char* id){
	int es_este(Proceso *proceso) {
		return string_equals_ignore_case(proceso->Id_del_proceso, id);
	}
	return list_find(procesos_conectados,(void*)es_este);
}

int buscar_espacio_en_bitmap(t_bitarray* bitmap){
	int total = bitarray_get_max_bit(bitmap);
	int indice = 0;
	while(total){
		if(!bitarray_test_bit(bitmap,indice)){
			return indice;
		}
		total=total-1;
		indice=indice+1;
	}
	log_error(logger, "No hay bits dentro del bitmap libres");
	return -1;
}

t_list* buscar_segmentos_de_heap(t_list* segmentos){
	bool es_heap(Segmento* segmento) {
		return segmento->tipo_de_segmento == HEAP;
	}
	t_list* filtered = list_filter(segmentos, (void*) es_heap);
	return filtered;
}
t_list* buscar_segmentos_de_map(t_list* segmentos){
	bool es_map(Segmento* segmento) {
		return segmento->tipo_de_segmento == 1;
	}
	t_list* filtered = list_filter(segmentos, (void*) es_map);
	return filtered;
}

//Segmento *buscar_segmento_para(segmentos_de_heap, tamanio){
//	int es_este_segmento(Segmento *s) {
//		return s->numero_de_segmento;
//	}
//	return list_find(segmentos_de_heap,(void*)es_este_segmento);
//}

t_list *buscar_en_tabla_de_segmentos(t_list* tabla_de_segmentos, uint32_t numero_de_segmento){
	bool es_de_mi_segmento(Elemento_de_mi_tabla_de_segmentos* elemento_de_mi_tabla) {
		return elemento_de_mi_tabla->numero_de_segmento == numero_de_segmento;
	}
	t_list* filtered = list_find(tabla_de_segmentos, (void*) es_de_mi_segmento);
	return filtered;
}

void ordenar_por_numero_de_paginas(t_list * elementos_de_tabla){
	bool paginas_ascendentes(Elemento_de_mi_tabla_de_pagina_por_segmento *con_pagina_mayor, Elemento_de_mi_tabla_de_pagina_por_segmento *con_pagina_menor) {
	    return con_pagina_mayor->numero_de_pagina < con_pagina_mayor->numero_de_pagina;
	}
    list_sort(elementos_de_tabla, (void*) paginas_ascendentes);
}

t_list* tabla_de_dicho_segmento(t_list* tabla_de_paginas, uint32_t numero){
	int es_esta_pagina(Elemento_de_mi_tabla_de_pagina_por_segmento *pagina) {
		return pagina->numero_de_pagina == numero;
	}
	t_list* filtered = list_find(tabla_de_paginas, (void*) es_esta_pagina);
	return filtered;
}

bool es_extensible_a_comparacion_del_otro(Elemento_de_mi_tabla_de_segmentos *a_extender,Elemento_de_mi_tabla_de_segmentos *el_siguiente, uint32_t tam){
	uint32_t limite_a_extender = a_extender->base_logica + a_extender->tamanio_del_segmento;
	uint32_t cantidad_a_extender = (tam/page_size)+sizeof(Heap_de_metadata_de_segmento);//No tiene sentido que pueda agregar una pagina pero no 2 y necesite 3. Por defecto siempre el segmento tiene un
	return(limite_a_extender + cantidad_a_extender > el_siguiente->base_logica);//para ver si no caigo uh ocupo espacio del segmento siguiente queriendo crecer
}

Elemento_de_mi_tabla_de_pagina_por_segmento * crear_pagina(uint32_t num){
	Elemento_de_mi_tabla_de_pagina_por_segmento *nuevaPagina;
	nuevaPagina->modificado = false;
	nuevaPagina->numero_de_frame = asignar_frame();
	nuevaPagina->numero_de_pagina = num + 1;
	nuevaPagina->prencia = true;
	return nuevaPagina;
}

uint32_t ver_si_entra_y_poner(Elemento_de_mi_tabla_de_pagina_por_segmento *unaPagina, uint32_t tamanio, bool es_extensible){
	if(unaPagina->prencia){
		int frame = unaPagina->numero_de_frame;
		void* seg = UPCM + page_size*frame;
		Heap_de_metadata_de_segmento * metadata;
		int magic = 0;
		while(magic<page_size){
			memcpy(metadata,seg,sizeof(Heap_de_metadata_de_segmento));
			if(metadata->esta_libre && metadata->tamanio < tamanio + sizeof(Heap_de_metadata_de_segmento)){
				if(magic + tamanio + sizeof(Heap_de_metadata_de_segmento)<=page_size){
					Heap_de_metadata_de_segmento * metadata_nueva;
					metadata_nueva->esta_libre = true;
					metadata_nueva->tamanio = metadata->tamanio - tamanio - sizeof(Heap_de_metadata_de_segmento);

					metadata->esta_libre = false;
					metadata->tamanio = tamanio;

					memcpy(seg + sizeof(Heap_de_metadata_de_segmento) + tamanio,metadata_nueva,sizeof(Heap_de_metadata_de_segmento));
					return tamanio;
				}else{
					if(es_extensible){
						int resta = page_size - magic;
						Heap_de_metadata_de_segmento * metadata_nueva;
						metadata_nueva->esta_libre = true;
						metadata_nueva->tamanio = metadata->tamanio - tamanio - sizeof(Heap_de_metadata_de_segmento);

						metadata->esta_libre = false;
						metadata->tamanio = tamanio;

						memcpy(seg + sizeof(Heap_de_metadata_de_segmento) + tamanio,metadata_nueva,resta);
						Elemento_de_mi_tabla_de_pagina_por_segmento *nuevaPagina = crear_pagina(unaPagina->numero_de_pagina);
						//buscar_nueva_pagina para el segmento y asignarle (tamanio-resta)
						return tamanio;
					}
				}
			}
			if(magic + sizeof(Heap_de_metadata_de_segmento) + metadata->tamanio <= page_size){
				seg = seg + sizeof(Heap_de_metadata_de_segmento) + metadata->tamanio;
				magic = sizeof(Heap_de_metadata_de_segmento) + metadata->tamanio;
			}else{
				break;
			}
		}
	}
}

uint32_t hacer_alloc(char* id,uint32_t tamanio){
	Proceso *unproceso = buscar_proceso(id);
	t_list* segmentos = unproceso->segmentos_del_proceso;
	if(list_is_empty(segmentos)){
		//dar segmento
	}
	for(int i = 0; i < list_size(segmentos); i= i+1){
		Segmento* unSegmento = list_get(segmentos,i);
		if(unSegmento->tipo_de_segmento == HEAP){
			Elemento_de_mi_tabla_de_segmentos *tabla_que_son_de_un_segmento = buscar_en_tabla_de_segmentos(segmentos,unSegmento->numero_de_segmento);
			bool es_extensible = true;

			//bucle?
			if(i==list_size(segmentos)-1){
				Segmento* otroSegmento = list_get(segmentos,i+1);
				Elemento_de_mi_tabla_de_segmentos *tabla_que_son_de_otro_segmento = buscar_en_tabla_de_segmentos(segmentos,otroSegmento->numero_de_segmento);
				es_extensible = es_extensible_a_comparacion_del_otro(tabla_que_son_de_un_segmento,tabla_que_son_de_otro_segmento,tamanio);
			}
			uint32_t retorno = 0;
			t_list *tabla_de_paginas_para_ese_segmento = tabla_de_dicho_segmento(unproceso->tabla_de_pagina_por_segmento,tabla_que_son_de_un_segmento->puntero_a_tabla_de_paginas_del_segmento);
			ordenar_por_numero_de_paginas(tabla_de_paginas_para_ese_segmento);
			for(int j = 0; j < list_size(tabla_de_paginas_para_ese_segmento); j = j+1){
//				uint32_t retorno = ver_si_tengo_espacio_y_reservar_en_pagina(tabla_de_paginas_para_ese_segmento,tamanio);
				Elemento_de_mi_tabla_de_pagina_por_segmento* unaPagina = list_get(tabla_de_paginas_para_ese_segmento,j);
				retorno = ver_si_entra_y_poner(unaPagina,tamanio,es_extensible);

			}
		}
	}
	//Si no entra en ninguno es que justo todos los previos de heap no se extienden tanto y el ultimo es map, asi que hay que crear un nuevo heap :)
	return -ENOMEM;


	sem_wait(&mutex_bitmap_frame);
	uint32_t frame_libre = buscar_espacio_en_bitmap(bitmap_para_frames);
	if(frame_libre == -1)
	{
		//swap
	}
	bitarray_set_bit(bitmap_para_frames,frame_libre);
	sem_post(&mutex_bitmap_frame);

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
				char* pid_init = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"::::ID local recivido: %s", pid_init);
				char* ip_del_proceso = obtener_Id_real(cliente, pid_init);
				log_info(logger,"::::ID global del proceso: %s", ip_del_proceso);
				proceso_nuevo(ip_del_proceso);
				break;
			case m_CLOSE:;
				log_info(logger,"Me llego un: %i",m_CLOSE);
				char* resivido_close = Muse_ReceiveAndUnpack(cliente,tam);
				log_info(logger,"resivi: %s",resivido_close);
				free(resivido_close);
				break;
			case m_ALLOC:;
				log_info(logger,"Me llego un: %i",m_ALLOC);
				char* pid_alloc = Muse_ReceiveAndUnpack(cliente,tam);
				char* id_del_proceso_alloc = obtener_Id_real(cliente, pid_alloc);
				log_info(logger,"::::ID global del proceso Alloc: %s", id_del_proceso_alloc);
				uint32_t tamanio_a_allocar;
				memcpy(&tamanio_a_allocar, Muse_ReceiveAndUnpack(cliente,sizeof(uint32_t)),sizeof(uint32_t));
				log_info(logger,"resivi tamanio: %i",tamanio_a_allocar);
				hacer_alloc(id_del_proceso_alloc,tamanio_a_allocar);
				free(pid_alloc);
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
	UPCM = malloc(memory_size);
	procesos_conectados = list_create();
	tabla_de_swap = list_create();
	log_info(logger, ":::Se creo la lista de procesos:::");
	iniciar_bitmap_de_frames();
	iniciar_bitmap_de_swap();
	sem_init(&mutex_bitmap_frame,0,1);
	sem_init(&mutex_bitmap_swap,0,1);
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

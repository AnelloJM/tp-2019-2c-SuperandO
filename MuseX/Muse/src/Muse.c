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
	nuevo_proceso->ultimo_id_de_tabla_de_pagina = 0;
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
	log_info(logger, "Buscando en bitmap");

	int total = bitarray_get_max_bit(bitmap);
	int indice = 0;
	while(total){
		if(!bitarray_test_bit(bitmap,indice)){
			log_info(logger, "saliendo en bitmap, indice: %i", indice);
			return indice;
		}
		total=total-1;
		indice=indice+1;
	}
	log_error(logger, "No hay bits dentro del bitmap libres");
	return -1;
}

int buscar_espacio_en_bitmap_frame(){
	log_info(logger, "Buscando en bitmap");

	int total = bitarray_get_max_bit(bitmap_para_frames);
	int indice = 0;
	while(total){
		if(bitarray_test_bit(bitmap_para_frames,indice) == 0){
			log_info(logger, "saliendo en bitmap, indice: %i", indice);
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

Elemento_de_mi_tabla_de_segmentos *buscar_en_tabla_de_segmentos_por_numeroSegmento(t_list* tabla_de_segmentos, uint32_t numero_de_segmento){
	bool es_de_mi_segmento(Elemento_de_mi_tabla_de_segmentos* elemento_de_mi_tabla) {
		return elemento_de_mi_tabla->numero_de_segmento == numero_de_segmento;
	}
	return list_find(tabla_de_segmentos, (void*) es_de_mi_segmento);
}


Elemento_de_mi_tabla_de_segmentos* buscar_en_tabla_de_segmento(t_list* aux, uint32_t id_de_tabla){
	int es_este(Elemento_de_mi_tabla_de_segmentos* elem) {
		return elem->puntero_a_tabla_de_paginas_del_segmento == id_de_tabla;
	}
	return list_find(aux,(void*)es_este);
}

void ordenar_por_numero_de_paginas(t_list * elementos_de_tabla){
	bool paginas_ascendentes(Elemento_de_mi_tabla_de_pagina_por_segmento *con_pagina_mayor, Elemento_de_mi_tabla_de_pagina_por_segmento *con_pagina_menor) {
	    return con_pagina_mayor->numero_de_pagina < con_pagina_mayor->numero_de_pagina;
	}
    list_sort(elementos_de_tabla, (void*) paginas_ascendentes);
}

t_list* tabla_de_dicho_segmento(t_list* tabla_de_paginas, uint32_t numero){
	int es_esta_pagina(Elemento_de_mi_tabla_de_pagina_por_segmento *pagina) {
		return pagina->id_de_tabla == numero;
	}
	t_list* filtered = list_filter(tabla_de_paginas, (void*) es_esta_pagina);
	return filtered;
}

bool es_extensible_a_comparacion_del_otro(Elemento_de_mi_tabla_de_segmentos *a_extender,Elemento_de_mi_tabla_de_segmentos *el_siguiente, uint32_t tam){
	uint32_t limite_a_extender = a_extender->base_logica + a_extender->tamanio_del_segmento;
	uint32_t cantidad_a_extender = (tam/page_size)+sizeof(Heap_de_metadata_de_segmento);//No tiene sentido que pueda agregar una pagina pero no 2 y necesite 3. Por defecto siempre el segmento tiene un
	return(limite_a_extender + cantidad_a_extender > el_siguiente->base_logica);//para ver si no caigo uh ocupo espacio del segmento siguiente queriendo crecer
}

Elemento_de_mi_tabla_global_de_frames_por_pagina *buscar_pagina_en_frame(uint32_t numero_de_frame){
	bool es_de_mi_segmento(Elemento_de_mi_tabla_global_de_frames_por_pagina* elemento_de_frame) {
		return elemento_de_frame->numero_de_frame == numero_de_frame;
	}
	Elemento_de_mi_tabla_global_de_frames_por_pagina* encontrado = list_find(tabla_de_frames_por_pagina, (void*) es_de_mi_segmento);
	return encontrado;
}


Elemento_de_mi_tabla_de_pagina_por_segmento* buscar_pagina_en_por_segmento(t_list* tabla_de_paginas,uint32_t numero_de_pagina){
	int encontrado(Elemento_de_mi_tabla_de_pagina_por_segmento* elem) {
			return elem->numero_de_pagina == numero_de_pagina;
		}
		return list_find(tabla_de_paginas,(void*)encontrado);
}


/*
 * Empezando desde la posición actual del puntero, recorrer la lista de marcos.
 * 			Durante el recorrido, dejar el bit de uso (U) intacto.
 * 					El primer marco que se encuentre con U = 0 y M = 0 se elige para el reemplazo.
 *
 * Si el paso 1 falla, recorrer nuevamente, buscando un marco con U = 0 y M = 1. El primer marco que cumpla la condición es seleccionado para el reemplazo.
 * 		 Durante este recorrido, cambiar el bit de uso a 0 de todos los marcos que no se elijan.
 * Si el paso 2 falla, volver al paso 1.
 */
uint32_t clock_modificado(){
	int tam = list_size(tabla_de_frames_por_pagina);
	while(1){
		for(int i = 0; i < tam; i = i+1){
			Elemento_de_mi_tabla_global_de_frames_por_pagina* elemento_global = list_get(tabla_de_frames_por_pagina,puntero_de_clock);
			Proceso* proceso= buscar_proceso(elemento_global->Id_del_proceso);
			t_list* tabla_de_paginas = proceso->tabla_de_pagina_por_segmento;
			Elemento_de_mi_tabla_de_pagina_por_segmento* elemton_de_tabla_de_pag_por_seg = buscar_pagina_en_por_segmento(tabla_de_paginas,elemento_global->numero_de_pagina);
			if(elemton_de_tabla_de_pag_por_seg->uso == 0 && elemton_de_tabla_de_pag_por_seg->modificado == 0){
				puntero_de_clock = (puntero_de_clock + 1) %  tam;
				return elemton_de_tabla_de_pag_por_seg->numero_de_frame;
			}
		}
		for(int i = 0; i < tam; i = i+1){
			Elemento_de_mi_tabla_global_de_frames_por_pagina* elemento_global = list_get(tabla_de_frames_por_pagina,puntero_de_clock);
			Proceso* proceso= buscar_proceso(elemento_global->Id_del_proceso);
			t_list* tabla_de_paginas = proceso->tabla_de_pagina_por_segmento;
			Elemento_de_mi_tabla_de_pagina_por_segmento* elemton_de_tabla_de_pag_por_seg = buscar_pagina_en_por_segmento(tabla_de_paginas,elemento_global->numero_de_pagina);
			if(elemton_de_tabla_de_pag_por_seg->uso == 0 && elemton_de_tabla_de_pag_por_seg->modificado == 1){
				puntero_de_clock = (puntero_de_clock + 1) %  tam;
				return elemton_de_tabla_de_pag_por_seg->numero_de_frame;
			}else{
				puntero_de_clock = (puntero_de_clock + 1) %  tam;
				elemton_de_tabla_de_pag_por_seg->uso = 1;
			}
		}
	}
}

Elemento_de_mi_tabla_de_pagina_por_segmento* buscar_pagina_por_numero(t_list* tabla_de_paginas,uint32_t numero_de_pag){
	bool es_de_mi_segmento(Elemento_de_mi_tabla_de_pagina_por_segmento* elemento_de_mi_tabla) {
			return elemento_de_mi_tabla->numero_de_pagina == numero_de_pag;
		}
		return list_find(tabla_de_paginas, (void*) es_de_mi_segmento);
}

void jubilar_pagina(Elemento_de_mi_tabla_global_de_frames_por_pagina *pagina_a_dar_de_baja){
	Proceso * proceso = buscar_proceso(pagina_a_dar_de_baja->Id_del_proceso);
	t_list* tabla_de_paginas = proceso->tabla_de_pagina_por_segmento;
	Elemento_de_mi_tabla_de_pagina_por_segmento* pagina = buscar_pagina_por_numero(tabla_de_paginas,pagina_a_dar_de_baja->numero_de_pagina);
	pagina->prencia=false;
	pagina->uso=false;
	pagina->numero_de_frame = pagina_a_dar_de_baja->numero_de_frame;
}

uint32_t bajar_para_swap(uint32_t index_swap){
	uint32_t frame_a_swapear = clock_modificado();

	Elemento_de_mi_tabla_global_de_frames_por_pagina* pagina_a_bajar = buscar_pagina_en_frame(frame_a_swapear);
	uint32_t frame_liberado = pagina_a_bajar->numero_de_frame;
	bitarray_clean_bit(bitmap_para_frames,frame_liberado);
	pagina_a_bajar->numero_de_frame = index_swap;

	jubilar_pagina(pagina_a_bajar);

	Elemento_de_tabla_de_swap* elem_swap;
	elem_swap->Id_del_proceso = pagina_a_bajar->Id_del_proceso;
	elem_swap->numero_de_pagina = pagina_a_bajar->numero_de_pagina;
	elem_swap->posicion_en_swap = index_swap;
	list_add_in_index(tabla_de_swap,index_swap,elem_swap);
	memcpy(archivo_de_swap + (index_swap*page_size), UPCM + frame_liberado*page_size, page_size);

	return frame_liberado;
}

uint32_t asignar_frame_nuevo(Elemento_de_mi_tabla_de_pagina_por_segmento* pag){
	sem_wait(&mutex_bitmap_frame);
	uint32_t frame = 0;// buscar_espacio_en_bitmap_frame();
	if(frame == -1){
		uint32_t swap = buscar_espacio_en_bitmap(bitmap_para_swap);
		if(swap == -1){
			sem_post(&mutex_bitmap_frame);
			return -ENOENT;
		}
		frame = bajar_para_swap(swap);
	}
	pag->numero_de_frame=frame;
	pag->prencia=true;
	Heap_de_metadata_de_segmento *nuevo_heap = malloc(sizeof(Heap_de_metadata_de_segmento));
	nuevo_heap->esta_libre = true;
	nuevo_heap->tamanio =page_size;
	void* frame_en_upcm = UPCM + page_size * frame;
	memcpy(frame_en_upcm, nuevo_heap,sizeof(Heap_de_metadata_de_segmento));
	sem_post(&mutex_bitmap_frame);
	return frame;
}

Elemento_de_mi_tabla_de_pagina_por_segmento * crear_pagina(uint32_t num, uint32_t id_tabla){
	Elemento_de_mi_tabla_de_pagina_por_segmento *nuevaPagina = malloc(sizeof(Elemento_de_mi_tabla_de_pagina_por_segmento));
	nuevaPagina->id_de_tabla = id_tabla;
	nuevaPagina->modificado = false;
	nuevaPagina->numero_de_pagina = num;
	nuevaPagina->prencia = true;
	asignar_frame_nuevo(nuevaPagina);
	return nuevaPagina;
}

Elemento_de_tabla_de_swap* buscar_en_swap(uint32_t numero_de_pagina,char* id){
	int es_este_elemento_del_swap(Elemento_de_tabla_de_swap* swap) {
			return strcmp(swap->Id_del_proceso, id) && swap->numero_de_pagina == numero_de_pagina;
		}
	return list_find(tabla_de_swap,(void*)es_este_elemento_del_swap);
}

bool compardor(Elemento_de_tabla_de_swap* un_elemento,Elemento_de_tabla_de_swap* otro_elemento){
	return strcmp(un_elemento->Id_del_proceso, otro_elemento->Id_del_proceso) && (un_elemento->numero_de_pagina == otro_elemento->numero_de_pagina);
}


uint32_t encontrar_numero_de_segmento(Elemento_de_mi_tabla_de_pagina_por_segmento* unaPagina, char* id){
	Proceso* proceso = buscar_proceso(id);
	t_list* aux = proceso->tabla_de_segmentos;
	Elemento_de_mi_tabla_de_segmentos* elem = buscar_en_tabla_de_segmento(aux, unaPagina->id_de_tabla);
	uint32_t numero_de_segmento = elem->numero_de_segmento;
	return numero_de_segmento;
}

void revivir_pagina(Elemento_de_mi_tabla_de_pagina_por_segmento* unaPagina, char* id){
	uint32_t numero_de_pagina = unaPagina->numero_de_pagina;
	Elemento_de_tabla_de_swap* elemento_en_swap = buscar_en_swap(numero_de_pagina,id);
	int index_a_remover = list_get_index(tabla_de_swap,elemento_en_swap,(void*)compardor);
	list_remove(tabla_de_swap,index_a_remover);
	bitarray_clean_bit(bitmap_para_swap,index_a_remover);

	sem_wait(&mutex_bitmap_frame);
	uint32_t frame = buscar_espacio_en_bitmap(bitmap_para_frames);
	if(frame == -1){
		frame = clock_modificado();
		void* frame_en_upcm = UPCM + page_size * frame;
		for(int i = 0; i < page_size; i = i + 1){
			memcpy(frame_en_upcm+i, "\0",1);
		}
		Elemento_de_mi_tabla_global_de_frames_por_pagina* elemento_global;
		elemento_global->Id_del_proceso = unaPagina->id_de_tabla;
		elemento_global->numero_de_frame = unaPagina->numero_de_frame;
		elemento_global->numero_de_pagina = unaPagina->numero_de_pagina;
		uint32_t numero_de_segmento = encontrar_numero_de_segmento(unaPagina,id);
		elemento_global->numero_de_segmento = numero_de_segmento;

		list_add(tabla_de_frames_por_pagina, elemento_global);
		unaPagina->numero_de_frame = frame;
	}
	sem_post(&mutex_bitmap_frame);

}

uint32_t me_entro_en_mi_segmento(t_list *tabla_de_paginas_para_ese_segmento, bool es_extensible, uint32_t tamanio, char*id){
	for(int j = 0; j < list_size(tabla_de_paginas_para_ese_segmento); j = j+1){
		log_info(logger, "\n En el for \n");
		Elemento_de_mi_tabla_de_pagina_por_segmento* unaPagina = list_get(tabla_de_paginas_para_ese_segmento,j);
		bool es_extensible_la_pagina = es_extensible && j == list_size(tabla_de_paginas_para_ese_segmento)-1;
		if(!unaPagina->prencia){
			log_error(logger, "\n No esta \n");
			revivir_pagina(unaPagina,id);
		}
			int frame = unaPagina->numero_de_frame;
			void* seg = UPCM + page_size*frame;
			Heap_de_metadata_de_segmento * metadata = malloc(sizeof(Heap_de_metadata_de_segmento));
			int magic = 0;
			while(magic<page_size){
				log_info(logger, "\n En while \n");
				memcpy(metadata, seg + magic, sizeof(Heap_de_metadata_de_segmento));
				log_info(logger, "\n tamanio meta: %i \n", metadata->tamanio);

				log_error(logger, "sizeof(Heap_de_metadata_de_segmento): %i", sizeof(Heap_de_metadata_de_segmento));

				log_info(logger, "\n tamanio a escribir: %i \n", tamanio + sizeof(Heap_de_metadata_de_segmento));
				log_info(logger, "\n libre meta: %i\n",metadata->esta_libre);
				if(metadata->esta_libre && (metadata->tamanio > tamanio + sizeof(Heap_de_metadata_de_segmento))){
					log_info(logger, "\n En if \n");
					if(magic + tamanio + sizeof(Heap_de_metadata_de_segmento) <= page_size){
						log_error(logger, ":::::::::::::::::ACA  4:::::::::::::::::::");
						Heap_de_metadata_de_segmento * metadata_nueva = malloc(sizeof(Heap_de_metadata_de_segmento));
						metadata_nueva->esta_libre = true;
						uint32_t tamAnterior = metadata->tamanio;
						metadata_nueva->tamanio = metadata->tamanio - tamanio - sizeof(Heap_de_metadata_de_segmento);

						metadata->esta_libre = false;
						metadata->tamanio = tamanio;
						memcpy(seg + sizeof(Heap_de_metadata_de_segmento) + tamanio,metadata_nueva,sizeof(Heap_de_metadata_de_segmento));

						return magic + sizeof(Heap_de_metadata_de_segmento) + tamAnterior + sizeof(Heap_de_metadata_de_segmento);
					}else{
						if(es_extensible_la_pagina){
							int resta = page_size - magic;
							Heap_de_metadata_de_segmento * metadata_nueva;
							metadata_nueva->esta_libre = true;
							metadata_nueva->tamanio = metadata->tamanio - tamanio - sizeof(Heap_de_metadata_de_segmento);

							metadata->esta_libre = false;
							metadata->tamanio = tamanio;

							memcpy(seg + sizeof(Heap_de_metadata_de_segmento) + tamanio,metadata_nueva,resta);
							Elemento_de_mi_tabla_de_pagina_por_segmento *nuevaPagina = crear_pagina(unaPagina->numero_de_pagina +1 , unaPagina->id_de_tabla);
							seg = UPCM + page_size*nuevaPagina->numero_de_frame;
							int faltante = sizeof(Heap_de_metadata_de_segmento) + resta;
							memcpy(seg + sizeof(Heap_de_metadata_de_segmento) + tamanio + resta,metadata_nueva, faltante);
							list_add(tabla_de_paginas_para_ese_segmento,nuevaPagina);
							return faltante+page_size;
						}
					}
				}
				if(magic + sizeof(Heap_de_metadata_de_segmento) + metadata->tamanio <= page_size){
					seg = seg + sizeof(Heap_de_metadata_de_segmento) + metadata->tamanio;
					magic = sizeof(Heap_de_metadata_de_segmento) + metadata->tamanio;
				}else{
					//pasa a la otra pagina
					break;
				}
			}
	}
	return -1;
}

Segmento* nuevo_segmento_heap(int numero_de_segmento){
	Segmento* unSegmento = malloc(sizeof(Segmento));
	unSegmento->numero_de_segmento = numero_de_segmento;
	unSegmento->tipo_de_segmento = HEAP;
	return unSegmento;
}

Elemento_de_mi_tabla_de_segmentos* nuevo_tabla_de_elemento(uint32_t num_de_seg, uint32_t base, uint32_t id_de_tabla){
	Elemento_de_mi_tabla_de_segmentos* elem_de_tabla = malloc(sizeof(Elemento_de_mi_tabla_de_segmentos));
	elem_de_tabla->base_logica = base;
	elem_de_tabla->numero_de_segmento = num_de_seg;
	elem_de_tabla->tamanio_del_segmento = 0;
	elem_de_tabla->puntero_a_tabla_de_paginas_del_segmento = id_de_tabla;
	return elem_de_tabla;
}

uint32_t hacer_alloc(char* id,uint32_t tamanio){
	Proceso *unproceso = buscar_proceso(id);
	t_list* segmentos = unproceso->segmentos_del_proceso;

	if(list_is_empty(segmentos)){
		Segmento* segmento_nuevo= nuevo_segmento_heap(0);
		Elemento_de_mi_tabla_de_segmentos* new_tabla_de_segmento = nuevo_tabla_de_elemento(segmento_nuevo->numero_de_segmento, 0, 0);
		Elemento_de_mi_tabla_de_pagina_por_segmento* new_tabla_de_pagina_por_segmento = crear_pagina(0,new_tabla_de_segmento->puntero_a_tabla_de_paginas_del_segmento);

		log_info(logger, "::::   Frame: %i   ::::",new_tabla_de_pagina_por_segmento->numero_de_frame);

		void *seg = UPCM + (page_size * new_tabla_de_pagina_por_segmento->numero_de_frame);

		Heap_de_metadata_de_segmento * metadata_nueva = malloc(sizeof(Heap_de_metadata_de_segmento));
		metadata_nueva->esta_libre = true;
		metadata_nueva->tamanio = page_size - sizeof(Heap_de_metadata_de_segmento);

		memcpy(seg, metadata_nueva, sizeof(Heap_de_metadata_de_segmento));

		new_tabla_de_segmento->tamanio_del_segmento = new_tabla_de_segmento->tamanio_del_segmento + new_tabla_de_segmento->tamanio_del_segmento + sizeof(Heap_de_metadata_de_segmento);

		list_add(unproceso->segmentos_del_proceso,segmento_nuevo);
		list_add(unproceso->tabla_de_segmentos,new_tabla_de_segmento);
		list_add(unproceso->tabla_de_pagina_por_segmento,new_tabla_de_pagina_por_segmento);

		Elemento_de_mi_tabla_de_segmentos *tabla_que_son_de_un_segmento = buscar_en_tabla_de_segmentos_por_numeroSegmento(unproceso->tabla_de_segmentos,segmento_nuevo->numero_de_segmento);
		bool es_extensible = true;
		uint32_t retorno = 0;
		t_list *tabla_de_paginas_para_ese_segmento = tabla_de_dicho_segmento(unproceso->tabla_de_pagina_por_segmento,tabla_que_son_de_un_segmento->puntero_a_tabla_de_paginas_del_segmento);
		ordenar_por_numero_de_paginas(tabla_de_paginas_para_ese_segmento);
		retorno = me_entro_en_mi_segmento(tabla_de_paginas_para_ese_segmento,es_extensible, tamanio, id);
		if(retorno != -1){
			log_info(logger, "Alloc a mandar: %i", retorno);
			return retorno;
		}
		log_error(logger, "Sin memo");
		return -ENOMEM;
	}

	for(int i = 0; i < list_size(segmentos); i= i+1){
		log_error(logger, ":::::::::::::::::ACA:::::::::::::::::::");
		Segmento* unSegmento = list_get(segmentos,i);

		if(unSegmento->tipo_de_segmento == HEAP){

			log_error(logger, ":::::::::::::::::ACA   2:::::::::::::::::::");
			Elemento_de_mi_tabla_de_segmentos *tabla_que_son_de_un_segmento = buscar_en_tabla_de_segmentos_por_numeroSegmento(unproceso->tabla_de_segmentos,unSegmento->numero_de_segmento);
			bool es_extensible = true;
			if(i!=list_size(segmentos)-1){
				log_error(logger, ":::::::::::::::::ACA   3:::::::::::::::::::");
				Segmento* otroSegmento = list_get(segmentos,i+1);
				Elemento_de_mi_tabla_de_segmentos *tabla_que_son_de_otro_segmento = buscar_en_tabla_de_segmentos_por_numeroSegmento(unproceso->tabla_de_segmentos,otroSegmento->numero_de_segmento);
				es_extensible = es_extensible_a_comparacion_del_otro(tabla_que_son_de_un_segmento,tabla_que_son_de_otro_segmento,tamanio);
			}
			uint32_t retorno = 0;
			t_list *tabla_de_paginas_para_ese_segmento = tabla_de_dicho_segmento(unproceso->tabla_de_pagina_por_segmento,tabla_que_son_de_un_segmento->puntero_a_tabla_de_paginas_del_segmento);
			ordenar_por_numero_de_paginas(tabla_de_paginas_para_ese_segmento);
			retorno = me_entro_en_mi_segmento(tabla_de_paginas_para_ese_segmento,es_extensible, tamanio, id);
			if(retorno != -1){
				return retorno;
			}
		}
	}

	Segmento* ultimo_segmento = list_get(segmentos,list_size(segmentos)-1);
	Elemento_de_mi_tabla_de_segmentos *tabla_que_son_del_ultimo_segmento = buscar_en_tabla_de_segmentos_por_numeroSegmento(unproceso->tabla_de_segmentos,ultimo_segmento->numero_de_segmento);

	Segmento* segmento_nuevo = nuevo_segmento_heap(list_size(segmentos));

	uint32_t base_nueva = tabla_que_son_del_ultimo_segmento->base_logica + tabla_que_son_del_ultimo_segmento->tamanio_del_segmento;
	uint32_t id_de_tabla = unproceso->ultimo_id_de_tabla_de_pagina + 1;
	unproceso->ultimo_id_de_tabla_de_pagina = unproceso->ultimo_id_de_tabla_de_pagina + 1;

	Elemento_de_mi_tabla_de_segmentos* new_tabla_de_segmento = nuevo_tabla_de_elemento(segmento_nuevo->numero_de_segmento, base_nueva, id_de_tabla);
	t_list *paginas_del_ultimo_segmento = tabla_de_dicho_segmento(unproceso->tabla_de_pagina_por_segmento,new_tabla_de_segmento->puntero_a_tabla_de_paginas_del_segmento);
	ordenar_por_numero_de_paginas(paginas_del_ultimo_segmento);
	Elemento_de_mi_tabla_de_pagina_por_segmento* ultima_pagina = list_get(paginas_del_ultimo_segmento,list_size(segmentos)-1);

	Elemento_de_mi_tabla_de_pagina_por_segmento* new_tabla_de_pagina_por_segmento = crear_pagina(ultima_pagina->numero_de_pagina + 1,new_tabla_de_segmento->puntero_a_tabla_de_paginas_del_segmento);

	void *seg = UPCM + page_size * new_tabla_de_pagina_por_segmento->numero_de_frame;

	Heap_de_metadata_de_segmento * metadata_nueva;
	metadata_nueva->esta_libre = true;
	metadata_nueva->tamanio = page_size - sizeof(Heap_de_metadata_de_segmento);

	memcpy(seg, metadata_nueva, sizeof(Heap_de_metadata_de_segmento));
	new_tabla_de_segmento->tamanio_del_segmento = new_tabla_de_segmento->tamanio_del_segmento + new_tabla_de_segmento->tamanio_del_segmento + sizeof(Heap_de_metadata_de_segmento);

	list_add(unproceso->segmentos_del_proceso,segmento_nuevo);
	list_add(unproceso->tabla_de_segmentos,new_tabla_de_segmento);
	list_add(unproceso->tabla_de_pagina_por_segmento,new_tabla_de_pagina_por_segmento);

	uint32_t retorno = me_entro_en_mi_segmento(paginas_del_ultimo_segmento,true, tamanio, id); //nunca tendria que fallar por que estoy recien reservando toda la pagina ypuede crecer, pero tal vez me quede sin espacio

	if(retorno != -1){
		return retorno;
	}

	return -ENOMEM;

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
				uint32_t posicion_de_alloc = hacer_alloc(id_del_proceso_alloc,tamanio_a_allocar);

				Muse_PackAndSend_Uint32_Response(cliente,posicion_de_alloc);

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
	char data[cantidad_de_frames];
	bitmap_para_frames = bitarray_create_with_mode(data, cantidad_de_frames, MSB_FIRST);
	log_info(logger, ":::Bitmap de Frames Creado:::");
}

void iniciar_bitmap_de_swap(){
	char *bitarray = malloc(swap_size);
	bitmap_para_swap = bitarray_create_with_mode(bitarray, swap_size, MSB_FIRST);
	log_info(logger, ":::Bitmap de Swap Creado:::");
}

void iniciar_archivo_swap(){
	int archi = open("archivo_de_swaping", O_CREAT, S_IRUSR | S_IWUSR);
	archivo_de_swap = mmap(NULL, swap_size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FILE, archi, 0);
}

void iniciar_muse(){
	iniciar_logger();
	iniciar_config();
	UPCM = malloc(memory_size);
	procesos_conectados = list_create();
	tabla_de_swap = list_create();
	tabla_de_frames_por_pagina = list_create();
	log_info(logger, ":::Se creo la lista de procesos:::");
	iniciar_bitmap_de_frames();
	iniciar_bitmap_de_swap();
	sem_init(&mutex_bitmap_frame,0,1);
	puntero_de_clock = 0;
	iniciar_archivo_swap();
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

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

#include "Sac-Server.h"
#include <Serializacion-FileSystem/Serializacion-FileSystem.h>
#include <Manejos-Comunes/Manejos-Comunes.h>

t_log *logger;
int conexion;

Bloque *inicio_de_disco;
uint32_t bloques_del_bitmap;
uint32_t tamanio_disco;
uint64_t cantidad_de_bloques_de_datos;
t_bitarray *tBitarray;
Header *header;
Bitmap bitmap;
Tabla_de_nodos *tabla_de_nodos;
Bloque *bloques_de_datos;


uint32_t Hacer_Getattr(char *path){
	uint32_t getattr = exite_path_retornando_nodo(path);
	if(getattr == -1)
		return 0;
	if(tabla_de_nodos->nodos[getattr].estado == 1){
		return 1;
	}else{
		return 2;
	}
}

char *Hacer_ReadDir(char *path){
	uint32_t nodo = 0;
	if( !(strcmp(path,"/") == 0) )
		nodo = exite_path_retornando_nodo(path);
	char *hijos;
	if(nodo == -1){
		hijos = malloc(strlen("No existe el path")+1);
		memcpy(hijos,"No existe el path",strlen("No existe el path")+1);
		return hijos;
	}
	if(tabla_de_nodos->nodos[nodo].estado != 2 && (nodo != 0)){
		hijos = malloc(strlen("No es un directorio")+1);
		memcpy(hijos,"No es un directorio",strlen("No es un directorio")+1);
		return hijos;
	}
	t_list *nodos_de_hijos=hijos_de_nodo(nodo);
	char* nombre_nodo;
	uint32_t nodo_del_hijo = 0;
	int desplazamiento = 0;

	int tamanio = 0;
	for(int j = 0; j < list_size(nodos_de_hijos); j=j+1){
		nodo_del_hijo = list_get(nodos_de_hijos,j);
		tamanio = tamanio + strlen(tabla_de_nodos->nodos[nodo_del_hijo].nombre_del_archivo) + 1;
	}

	nodo_del_hijo = 0;
	hijos = malloc(tamanio+1);
	for(int i = 0; i < list_size(nodos_de_hijos); i = i+1){
		nodo_del_hijo = list_get(nodos_de_hijos,i);
		nombre_nodo = malloc(strlen(tabla_de_nodos->nodos[nodo_del_hijo].nombre_del_archivo)+1);
		nombre_nodo = tabla_de_nodos->nodos[nodo_del_hijo].nombre_del_archivo;
		memcpy(hijos + desplazamiento, nombre_nodo, strlen(nombre_nodo));
		desplazamiento = desplazamiento + strlen(nombre_nodo);
		memcpy(hijos + desplazamiento, "/",strlen("/"));
		desplazamiento = desplazamiento + strlen("/");
	}
	memcpy(hijos+desplazamiento, "\0", 1);
	list_destroy(nodos_de_hijos);
	return hijos;
}

uint32_t Hacer_Open(char *path){ return 0; }

char *Hacer_Read(char *path, size_t size, off_t offset){
	uint32_t nodo = exite_path_retornando_nodo(path);
	if(nodo == -1)
		return "-1";

	//DONDE EMPEZAR

	/*
	 * Es entera porque me da la cantidad de
	 * bloques de datos desde donde comienzo
	*/
	uint32_t cantida_de_bloque_desde_donde_comienzo = offset / sizeof(Bloque);

	/*
	 * Es resto porque me da la cantidad que
	 * debo moverme desde el comienzo del bloque
	*/
	uint32_t desplazamiento = offset % sizeof(Bloque);

	/*
	 * por que cada poscion del array
	 * hace referencia a 1024 bloques
	*/
	uint32_t poscion_en_array = cantida_de_bloque_desde_donde_comienzo / 1024;

	/*
	 * falta para llegar dentro
	 * del los punteros
	*/
	uint32_t puntero_indirecto = cantida_de_bloque_desde_donde_comienzo % 1024;
	uint32_t numero_de_bloque_de_puntero = tabla_de_nodos->nodos[nodo].array_de_punteros[poscion_en_array];
	Bloque_de_puntero *punteros_indirectos = inicio_de_disco + numero_de_bloque_de_puntero;
	ptrGBloque numero_de_bloque_a_leer = punteros_indirectos->bloques_de_datos[puntero_indirecto];

	//CUANTO LEER

	uint32_t lo_que_me_queda_despues_del_bloque = sizeof(Bloque) - desplazamiento;
	uint32_t faltante = size;

	char *buffer = malloc(size);
//	buffer = "";
	Bloque *bloque_a_leer = inicio_de_disco + numero_de_bloque_a_leer;

	if(faltante < lo_que_me_queda_despues_del_bloque){
//		for(int i = 0; i < faltante; i = i + 1){
			memcpy(buffer, &(bloque_a_leer->bytes[desplazamiento]), faltante);
//		}
		return buffer;
	}
	memcpy(buffer, &(bloque_a_leer->bytes[desplazamiento]), lo_que_me_queda_despues_del_bloque);
	faltante = faltante - lo_que_me_queda_despues_del_bloque;
	offset = offset + lo_que_me_queda_despues_del_bloque;

	/*
	 * Los bloques que me faltan leer,
	 * entera para que si en menos de uno de cero
	*/
	uint32_t cantidad_bloques_a_leer_que_me_faltan = faltante / sizeof(Bloque);

	/*
	 * Desplazamiento dentro del ultimo bloque que me faltan leer,
	 * resto para que me de el
	*/
	uint32_t cantidad_dentro_que_falta_leer = faltante % sizeof(Bloque);

	uint32_t lo_que_me_queda_despues_de_los_punteros = 1024 - puntero_indirecto;

	if(lo_que_me_queda_despues_de_los_punteros < cantidad_bloques_a_leer_que_me_faltan){
		for(int i = 0; i < lo_que_me_queda_despues_de_los_punteros;i = i+1){
			numero_de_bloque_a_leer = punteros_indirectos->bloques_de_datos[puntero_indirecto + i];
			bloque_a_leer = inicio_de_disco + numero_de_bloque_a_leer;
			memcpy(buffer, bloque_a_leer ,sizeof(Bloque));
			faltante = faltante - sizeof(Bloque);
			offset = offset + sizeof(Bloque);
		}
	}else{
		for(int i = 0; i < cantidad_bloques_a_leer_que_me_faltan-1;i = i+1){
			numero_de_bloque_a_leer = punteros_indirectos->bloques_de_datos[puntero_indirecto + i];
			bloque_a_leer = inicio_de_disco + numero_de_bloque_a_leer;
			memcpy(buffer, bloque_a_leer ,sizeof(Bloque));
			faltante = faltante - sizeof(Bloque);
			offset = offset + sizeof(Bloque);
		}
		numero_de_bloque_a_leer = punteros_indirectos->bloques_de_datos[puntero_indirecto + 1];
		bloque_a_leer = inicio_de_disco + numero_de_bloque_a_leer;
		memcpy(buffer, &(bloque_a_leer->bytes[0]),cantidad_dentro_que_falta_leer);
		return buffer;
	}

	char *recursivo = Hacer_Read(path, faltante, offset);
	memcpy(buffer, recursivo,faltante);
	free(recursivo);

	return buffer;

}

uint32_t Hacer_Release(char *path){ return 0; }

uint32_t Hacer_Write(char *path, char *buffer){ return 0; } 

uint32_t Hacer_MKNod(char *path){
	if( exite_path_retornando_nodo(path) != -1){
		return -EEXIST;
	}

	char **path_separado = string_split(path,"/");
	uint32_t posicion_final = damePosicionFinalDoblePuntero(path_separado);

	uint32_t nodo = 0;

	if(posicion_final != 0){
		int total=0;

		for(int i = 0; i <posicion_final; i = i+1){
			total = total + string_length(path_separado[i]) + 1;
		}
		char *padre = string_substring(path, 0, total);
		nodo = exite_path_retornando_nodo(padre);
		if(nodo == -1){
			log_error(logger, "No pude hacer mkdir");
			log_error(logger, "El padre fue: %s", padre);
			return -EFAULT;
		}
	}

	crear_archivo_en_padre(nodo,path_separado[posicion_final]);
	liberarDoblePuntero(path_separado);
	return 0;
}

uint32_t Hacer_Unlink(char *path){
	ptrGBloque nodo = exite_path_retornando_nodo(path);
	if(nodo == -1)
	{
		return -ENOENT;
	}
	if(tabla_de_nodos->nodos[nodo].estado != 1){
		return -EISDIR;
	}
	limpiar_nodo(nodo);
	return 0;
}

uint32_t Hacer_MKDir(char *path){
	if( exite_path_retornando_nodo(path) != -1){
		return -EEXIST;
	}

	char **path_separado = string_split(path,"/");
	uint32_t posicion_final = damePosicionFinalDoblePuntero(path_separado);
	//consegir nodo padre

	uint32_t nodo = 0;

	if(posicion_final != 0){
		int total=0;

		for(int i = 0; i <posicion_final; i = i+1){
			total = total + string_length(path_separado[i]) + 1;
		}
		char *padre = string_substring(path, 0, total);
		nodo = exite_path_retornando_nodo(padre);
		if(nodo == -1){
			log_error(logger, "No pude hacer mkdir");
			log_error(logger, "El padre fue: %s", padre);
			return -ENOENT;
		}
	}

	crear_directorio_en_padre(nodo,path_separado[posicion_final]);
	liberarDoblePuntero(path_separado);
	return 0;
}

void limpiar_nodo(uint32_t nodo){
	uint32_t bloques_de_metadata = 1 + bloques_del_bitmap + 1024;
	uint32_t movimiento_en_bloques_de_datos = 0;
	ptrGBloque bloque;
	Bloque_de_puntero *punteros;
	Bloque *aux;

	for(int i = 0; i < 1000; i = i+1){
		if(tabla_de_nodos->nodos[nodo].array_de_punteros[i] != 0){
			punteros = inicio_de_disco + tabla_de_nodos->nodos[nodo].array_de_punteros[i];
			for(int p = 0; p<1024; p = p+1){
				if(punteros->bloques_de_datos[p] != 0){
					bloque = punteros->bloques_de_datos[p];
					bitarray_clean_bit(tBitarray,bloque);
					movimiento_en_bloques_de_datos = bloque - bloques_de_metadata;
					aux = (bloques_de_datos + movimiento_en_bloques_de_datos);
					for(int j = 0; j<4096; j = j+1){
						aux->bytes[j] = '\0';
					}
				}
			}
			bloque = tabla_de_nodos->nodos[nodo].array_de_punteros[i];
			bitarray_clean_bit(tBitarray,bloque);
			for(int j = 0; j<4096; j = j+1){
				(inicio_de_disco + bloque)->bytes[j] = '\0';
			}
		}
	}
	tabla_de_nodos->nodos[nodo].estado = 0;
	strncpy(tabla_de_nodos->nodos[nodo].nombre_del_archivo, "\0", 70);
	tabla_de_nodos->nodos[nodo].modificado=0;
	tabla_de_nodos->nodos[nodo].creacion=0;
	tabla_de_nodos->nodos[nodo].padre=0;
	tabla_de_nodos->nodos[nodo].tamanio_del_archivo=0;

}

uint32_t Hacer_RMDir(char *path){
	ptrGBloque nodo = exite_path_retornando_nodo(path);
	if(nodo == -1)
	{
		return -ENOENT;
	}
	if(tabla_de_nodos->nodos[nodo].estado != 2){
		return -ENOTDIR;
	}

	t_list* hijos = hijos_de_nodo(nodo);

	if(list_is_empty(hijos)){
		limpiar_nodo(nodo);
		list_destroy(hijos);
		return 0;
	}
	list_destroy(hijos);
	return -ENOTEMPTY;
}

uint32_t Hacer_Rename(char *path, char *buffer){
	ptrGBloque nodo = exite_path_retornando_nodo(path);
	if(nodo == -1)
		return -ENOENT;
	char **buffer_separado = string_split(buffer,"/");
	uint32_t posicion_final = damePosicionFinalDoblePuntero(buffer_separado);
	strncpy(tabla_de_nodos->nodos[nodo].nombre_del_archivo, "\0", 70);
	strncpy(tabla_de_nodos->nodos[nodo].nombre_del_archivo, buffer_separado[posicion_final], 70);
	tabla_de_nodos->nodos[nodo].modificado=timestamp();
	return 0;
}

uint32_t Hacer_Truncate(char *path, uint32_t nuevo_tamanio) {
	uint32_t nodo = exite_path_retornando_nodo(path);
	if(nodo == -1)
		return -ENOENT;
	if(tabla_de_nodos->nodos[nodo].estado !=1)
		return -EISDIR;
	tabla_de_nodos->nodos[nodo].tamanio_del_archivo = nuevo_tamanio;
	tabla_de_nodos->nodos[nodo].modificado=timestamp();
	// LIBERAR/OCUPAR LOS ESPACIOS CORRESPONDIENTES EN TABLA DE NODOS
	return 0;

}

void* funcionMagica(int cliente){
	while(1){
		HeaderFuse headerRecibido;
		headerRecibido = Fuse_RecieveHeader(cliente);
		log_info(logger, "Codigo de operacion: %i", headerRecibido.operaciones);
		log_info(logger, "Tamanio: %i", headerRecibido.tamanioMensaje);
		if(headerRecibido.operaciones == -1){
			log_error(logger, "Se desconecto el cliente\n");
			break;
		}
		uint32_t tam = headerRecibido.tamanioMensaje;
		switch(headerRecibido.operaciones){
			case f_GETATTR:;
				char *pathGetAttr= Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathGetAttr)+1);
				log_error(logger, pathGetAttr);
				uint32_t getattr = Hacer_Getattr(pathGetAttr);
				log_info(logger, "LE VOY A MANDAR: %i", getattr);
				Fuse_PackAndSend_Uint32_Response(cliente, getattr);
				free(pathGetAttr);
				break;

			case f_READDIR: ;
				char *pathReadDir = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathReadDir)+1);
				log_error(logger, pathReadDir);
				char *hijos_separados_con_barra = Hacer_ReadDir(pathReadDir);
				log_info(logger, "------ACA PERRO: %s", hijos_separados_con_barra);
				Fuse_PackAndSend(cliente, hijos_separados_con_barra, strlen(hijos_separados_con_barra)+1, f_RESPONSE);
				free(pathReadDir);
				free(hijos_separados_con_barra);
				break;

			case f_READ: ;
				char *pathRead = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRead)+1);
				log_error(logger, pathRead);
				char *respuestaRead = Hacer_Read(pathRead, 10, 0);
				log_error(logger, "lo que habia adentro es: %s", respuestaRead);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi READ"), strlen("Hola, recibi READ")+1, f_RESPONSE);
				free(pathRead);
				free(respuestaRead);
				break;

			case f_OPEN: ;
				char *pathOpen = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathOpen)+1);
				log_error(logger, pathOpen);
				//Hacer_Open(pathOpen);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi OPEN"), strlen("Hola, recibi OPEN")+1, f_RESPONSE);
				free(pathOpen);
				break;

			case f_RELEASE: ;
				char *pathRelease = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRelease)+1);
				log_error(logger, pathRelease);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi RELEASE"), strlen("Hola, recibi RELEASE")+1, f_RESPONSE);
				free(pathRelease);
				break;

			case f_WRITE: ;
				char *pathWrite = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathWrite)+1);
				log_error(logger, pathWrite);
				//Hacer_Write(pathWrite, escritura);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi WRITE"), strlen("Hola, recibi WRITE")+1, f_RESPONSE);
				free(pathWrite);
				break;

			case f_MKNOD: ;
				char *pathMKNod = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathMKNod)+1);
				log_error(logger, pathMKNod);
				uint32_t respuestaMKNOD = Hacer_MKNod(pathMKNod);
				log_info(logger, "LE VOY A MANDAR %i", respuestaMKNOD);
				Fuse_PackAndSend_Uint32_Response(cliente, respuestaMKNOD);
//				Fuse_PackAndSend(cliente, respuestaMKNOD, sizeof(respuestaMKNOD), f_RESPONSE);
				free(pathMKNod);
				break;

			case f_UNLINK: ;
				char *pathUnlink = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathUnlink)+1);
				log_error(logger, pathUnlink);
				uint32_t responseUnlink = Hacer_Unlink(pathUnlink);
				log_info(logger, "LE VOY A MANDAR: %i", responseUnlink);
				Fuse_PackAndSend_Uint32_Response(cliente, responseUnlink);
				free(pathUnlink);
				break;

			case f_MKDIR: ;
				char *pathMKDir = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathMKDir)+1);
				log_error(logger, pathMKDir);
				uint32_t responseMKDIR = Hacer_MKDir(pathMKDir);
				log_info(logger, "LE VOY A MANDAR: %i", responseMKDIR);
				Fuse_PackAndSend_Uint32_Response(cliente, responseMKDIR);
//				if(Hacer_MKDir(pathMKDir))
//					Fuse_PackAndSend(cliente, "Pude", strlen("pude")+1, f_RESPONSE);
//				else
//					Fuse_PackAndSend(cliente, "No pude", strlen("No pude")+1, f_RESPONSE);
				free(pathMKDir);
				break;

			case f_RMDIR: ;
				char *pathRMDir = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRMDir)+1);
				log_error(logger, pathRMDir);
				uint32_t responseRMDir = Hacer_RMDir(pathRMDir);
				log_info(logger, "LE VOY A MANDAR: %i", responseRMDir);
				Fuse_PackAndSend_Uint32_Response(cliente, responseRMDir);
				free(pathRMDir);
				break;

			case f_RENAME: ;
				void *packRename = Fuse_ReceiveAndUnpack(cliente, tam);
				char *pathRename = Fuse_Unpack_Path(packRename);
				char *nuevo_nombre = Fuse_Unpack_Rename_Nombre(packRename);
				free(packRename);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRename)+1);
				log_error(logger, pathRename);
				uint32_t responseRename = Hacer_Rename(pathRename, nuevo_nombre);
				log_info(logger, "LE VOY A MANDAR: %i", responseRename);
				Fuse_PackAndSend_Uint32_Response(cliente, responseRename);
				free(pathRename);
				free(nuevo_nombre);
				break;

			case f_TRUNCATE: ;
				void *packTruncate = Fuse_ReceiveAndUnpack(cliente,tam);
				char *pathTruncate = Fuse_Unpack_Path(packTruncate);
				uint32_t nuevo_tamanio; Fuse_Unpack_Truncate_offset(packTruncate);
				free(packTruncate);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathTruncate)+1);
				log_error(logger, pathTruncate);
				uint32_t resultadoDeTruncar = Hacer_Truncate(pathTruncate, nuevo_tamanio);
				log_info(logger, "LE VOY A MANDAR: %i", resultadoDeTruncar);
				Fuse_PackAndSend_Uint32_Response(cliente, resultadoDeTruncar);
				free(pathTruncate);
				break;

			case f_HANDSHAKE: ;
				//desempaquetar pack y hacer el codigo
				char *pathHandshake = Fuse_ReceiveAndUnpack(cliente, headerRecibido.tamanioMensaje);
				log_info(logger,"tamanio del path que recive: %i \0", strlen(pathHandshake)+1);
				log_info(logger, pathHandshake);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi HANDSHAKE"), strlen("Hola, recibi HANDSHAKE")+1, f_RESPONSE);
				free(pathHandshake);
				break;

			default:
				log_error(logger, "No es un codigo conocido: %i", headerRecibido.operaciones);
				break;
		}
	}
}

uint64_t timestamp(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	long long result = (((long long )tv.tv_sec) * 1000 + ((long) tv.tv_usec) / 1000); //opero con milisegundos
	uint64_t timefinal = result;
	return timefinal;
}

void iniciar_header(){
	header = inicio_de_disco;
	header->identificador[0] = 'S';
	header->identificador[1] = 'A';
	header->identificador[2] = 'C';
	header->version = 1;
	header->inicio_bitmap = 1;
	header->tamanio_bitmap = bloques_del_bitmap;
}

void cargar_bitmap(int cantidad){
	int cargado = 0;
	while(cargado < cantidad){
		bitarray_set_bit(tBitarray, cargado);
		if(!bitarray_test_bit(tBitarray, cargado)){
			log_error(logger, "cargado: %i", cargado);
		}
		cargado = cargado + 1;
	}
	log_info(logger, "cargado: %i", cargado);
}

int buscar_espacio_en_bitmap(){
	int total = bitarray_get_max_bit(tBitarray);
	int indice = 0;
	while(total){
		if(!bitarray_test_bit(tBitarray,indice)){
			return indice;
		}
		total=total-1;
		indice=indice+1;
	}
	log_error(logger, "No hay bits dentro del bitmap libres");
	return -1;
}

void ocupar_bloque_en_bitmap(int indice){
	bitarray_set_bit(tBitarray,indice);
}

void liberar_bloque_en_bitmap(int indice){
	bitarray_clean_bit(tBitarray,indice);
}

void iniciar_tabla_de_nodos(){

	tabla_de_nodos = inicio_de_disco + 1 + bloques_del_bitmap;
	for(int i = 0; i <= 1024; i = i+1){
		tabla_de_nodos->nodos[i].estado = 0;
		strncpy(tabla_de_nodos->nodos[i].nombre_del_archivo, "\0", 70);
		for(int j = 0; j <= 1000; j = j+1){
			tabla_de_nodos->nodos[i].array_de_punteros[j] = 0;
		}
	}
}

void limbiar_bloques_de_datos(){
	bloques_de_datos = inicio_de_disco + 1 + bloques_del_bitmap + 1024;

	cantidad_de_bloques_de_datos =  ceil((float)tamanio_disco/sizeof(Bloque) - 1 - bloques_del_bitmap - 1024);
	log_info(logger, "cantidad_de_bloques_de_datos: %llu",cantidad_de_bloques_de_datos);

	Bloque * aux;
	for(int i=0; i < cantidad_de_bloques_de_datos; i = i+1){
		aux = bloques_de_datos + i;
		for(int j = 0; j<4096; j = j+1){
			aux->bytes[j] = '\0';
		}
	}

}

void iniciar_Sac_Server(){
	bloques_del_bitmap = ceil(((float)tamanio_disco/sizeof(Bloque)/8)/sizeof(Bloque));
	log_info(logger, "bloques_del_bitmap: %i",bloques_del_bitmap);

	iniciar_header();
	log_info(logger, "Listo header");

	int bits = ceil((float)tamanio_disco/sizeof(Bloque));
	log_info(logger, "Bits: %i", bits);

	tBitarray = bitarray_create_with_mode(inicio_de_disco + 1, ceil((float)bits/8), MSB_FIRST);

	log_info(logger, "tamanio cargado del bitmap: %i", bitarray_get_max_bit(tBitarray));

	cargar_bitmap(1 + bloques_del_bitmap + 1024);

	iniciar_tabla_de_nodos();

	log_info(logger, "Limpiando bloque de datos");
	limbiar_bloques_de_datos();
	log_info(logger, "Bloque de datos limpio");
}


uint32_t tamanio_archivo(char *archivo){
	FILE* file = fopen(archivo,"r");
	fseek(file, 0L, SEEK_END);
	uint32_t tamanio = ftell(file);
	fclose(file);
	return tamanio;
}

int tamanio_archivo_en_bloques(uint32_t tamanio){
	if(tamanio%sizeof(Bloque)>0){
		return tamanio/sizeof(Bloque);
	}return (tamanio/sizeof(Bloque))+1;
}

int hallar_nodo_libre(){
	for( int i= 1; i < 1024; i = i+1){
		if(tabla_de_nodos->nodos[i].estado == 0)
			return i;
	}
	log_error(logger, "No existe un nodo libre");
}


void crear_directorio_en_padre(uint32_t numero_de_nodo_padre, char *nombre_de_archivo){
	int numero_de_nodo = hallar_nodo_libre();
	tabla_de_nodos->nodos[numero_de_nodo].estado = 2;
	strncpy(tabla_de_nodos->nodos[numero_de_nodo].nombre_del_archivo, nombre_de_archivo, 70);
	tabla_de_nodos->nodos[numero_de_nodo].nombre_del_archivo[71] = '\0';
	tabla_de_nodos->nodos[numero_de_nodo].creacion=timestamp();
	tabla_de_nodos->nodos[numero_de_nodo].modificado=timestamp();
	tabla_de_nodos->nodos[numero_de_nodo].tamanio_del_archivo = sizeof(Bloque);
	tabla_de_nodos->nodos[numero_de_nodo].padre=numero_de_nodo_padre;
	uint32_t numero_de_bloque = buscar_espacio_en_bitmap();
	bitarray_set_bit(tBitarray, numero_de_bloque);
	tabla_de_nodos->nodos[numero_de_nodo].array_de_punteros[0] = numero_de_bloque;
	Bloque_de_puntero *punteros = inicio_de_disco + tabla_de_nodos->nodos[numero_de_nodo].array_de_punteros[0];
	numero_de_bloque = buscar_espacio_en_bitmap();
	bitarray_set_bit(tBitarray, numero_de_bloque);
	punteros->bloques_de_datos[0] = numero_de_bloque;
	log_info(logger,"creo directorio en bloque: %i", tabla_de_nodos->nodos[numero_de_nodo].array_de_punteros[0]);
}

void crear_archivo_en_padre(uint32_t numero_de_nodo_padre, char *nombre_de_archivo){
	int numero_de_nodo = hallar_nodo_libre();
	tabla_de_nodos->nodos[numero_de_nodo].estado = 1;
	strncpy(tabla_de_nodos->nodos[numero_de_nodo].nombre_del_archivo, nombre_de_archivo, 70);
	tabla_de_nodos->nodos[numero_de_nodo].nombre_del_archivo[71] = '\0';
	tabla_de_nodos->nodos[numero_de_nodo].creacion=timestamp();
	tabla_de_nodos->nodos[numero_de_nodo].modificado=timestamp();
	tabla_de_nodos->nodos[numero_de_nodo].tamanio_del_archivo = 0;
	tabla_de_nodos->nodos[numero_de_nodo].padre=numero_de_nodo_padre;
	uint32_t numero_de_bloque = buscar_espacio_en_bitmap();
	log_info(logger,"cargo en bitmap: %i", numero_de_bloque);
	bitarray_set_bit(tBitarray, numero_de_bloque);
	tabla_de_nodos->nodos[numero_de_nodo].array_de_punteros[0] = numero_de_bloque;
	Bloque_de_puntero *punteros = inicio_de_disco + tabla_de_nodos->nodos[numero_de_nodo].array_de_punteros[0];
	numero_de_bloque = buscar_espacio_en_bitmap();
	bitarray_set_bit(tBitarray, numero_de_bloque);
	punteros->bloques_de_datos[0] = numero_de_bloque;
	log_info(logger,"creo archivo en bloque: %i", tabla_de_nodos->nodos[numero_de_nodo].array_de_punteros[0]);
}
//Para tener en cuenta cuando hagamos el crear archivo
//	int bloques_que_ocupa_archivo = tamanio_archivo_en_bloques(tabla_de_nodos.nodos[numero_de_nodo].tamanio_del_archivo);
//	for(int i = 0; i <= bloques_que_ocupa_archivo; i = i+1){

char *obtener_nombre_nodo(uint32_t numero_de_nodo){
	char *nombre_retornado;
	nombre_retornado = malloc(strlen(tabla_de_nodos->nodos[numero_de_nodo].nombre_del_archivo)+1);
	strcpy(nombre_retornado, tabla_de_nodos->nodos[numero_de_nodo].nombre_del_archivo);
	return nombre_retornado;
}



bool existe_nodo_con_nombre(char* nombre) {
	for(int i=0; i<1024; i=i+1) {
		if(string_equals_ignore_case(nombre, obtener_nombre_nodo(i))
				&& tabla_de_nodos->nodos[i].estado != 0)
			return true;
	}
	return false;
}


uint32_t hallar_nodo_con_nombre_y_padre(char* nombre, uint32_t padre){
	for(int i=0; i < 1024; i = i+1){
		if(string_equals_ignore_case(tabla_de_nodos->nodos[i].nombre_del_archivo,nombre) &&
				tabla_de_nodos->nodos[i].padre == padre){
			log_info(logger, "nodo: %i", i);
			return i;
		}
	}
	return -1;
}

uint32_t exite_path_retornando_nodo(char* path){
	if(strcmp(path,"/") == 0)
	{
		return 0;
	}
	char **path_separado = string_split(path,"/");
	uint32_t posicion_final = damePosicionFinalDoblePuntero(path_separado);
	if(!existe_nodo_con_nombre(path_separado[posicion_final])){
		log_error(logger, "No existe el archivo");
		return -1;
	}
	uint32_t nodo_actual = 0;
	for(int inicio = 0; inicio <= posicion_final; inicio = inicio +1){
		nodo_actual = hallar_nodo_con_nombre_y_padre(path_separado[inicio],nodo_actual);

		if(nodo_actual == -1){
			log_error(logger, "No existe el path");
			return nodo_actual;
		}
	}

	return nodo_actual;
}

t_list *hijos_de_nodo(uint32_t nodo_padre){
	t_list *hijos = list_create();
	for(int i = 0; i < 1024; i = i+1){
		if(tabla_de_nodos->nodos[i].padre == nodo_padre && tabla_de_nodos->nodos[i].estado != 0){
			list_add(hijos, i);
		}
	}
	return hijos;
}

t_list *hallar_hijos_de_path(char* path){
	uint32_t nodo_por_el_que_consulto = exite_path_retornando_nodo(path);
	t_list *hijos = list_create();
	if(nodo_por_el_que_consulto == -1){
		list_add(hijos,nodo_por_el_que_consulto);
		return hijos;
	}
	hijos = hijos_de_nodo(nodo_por_el_que_consulto);
	return hijos;
}

void mostrar_hijos_de(char* path){
	t_list *hijos_home = list_create();
	hijos_home = hallar_hijos_de_path(path);
	for(int i = 0; i<list_size(hijos_home); i= i+1){
		log_info(logger, "hijo: %s", obtener_nombre_nodo(list_get(hijos_home,i)));
	}
	list_destroy(hijos_home);
}

int main(int argc, char *argv[]) {
	//Log:
	logger = log_create("Sac-Server.log", "Sac-Server", 1, LOG_LEVEL_INFO);
	log_info(logger, "Se ha creado un nuevo logger\n");

	t_config *archivo_de_configuracion = config_create("../../Sac.config");
	char *puerto = "6969";//config_get_string_value(archivo_de_configuracion, "LISTEN_PORT ");

	log_info(logger, "p: %s",puerto);
	//Archivos:
	char *archivo = argv[1];
	log_info(logger, "Archivo: %s", archivo);
	tamanio_disco = tamanio_archivo(archivo);
	log_info(logger, "Tamanio disco: %i", tamanio_disco);
	//tamanio_disco

	int disco = open(archivo, O_RDWR, 0);
	inicio_de_disco = mmap(NULL, tamanio_disco, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FILE, disco, 0);

	iniciar_Sac_Server();
	log_info(logger, "bloques_del_bitmap: %i", bloques_del_bitmap);

	log_info(logger, "sizeof(Tabla_de_nodos): %i", sizeof(Tabla_de_nodos));

	crear_archivo_en_padre(0,"archivo");
	uint32_t numero_bloque_de_punteros = tabla_de_nodos->nodos[1].array_de_punteros[0];
	Bloque_de_puntero *algo = inicio_de_disco + numero_bloque_de_punteros;
	uint32_t numero_bloque = algo->bloques_de_datos[0];
	Bloque *dato = inicio_de_disco+numero_bloque;
	dato->bytes[0] = 'p';
	dato->bytes[1] = 'u';
	dato->bytes[2] = 'e';
	dato->bytes[3] = 'r';
	dato->bytes[4] = 't';
	dato->bytes[5] = 'a';
	dato->bytes[6] = '\0';

	tabla_de_nodos->nodos[1].tamanio_del_archivo = 6*sizeof(char);

	char *respuestaRead = Hacer_Read("/archivo", 6, 0);
	log_error(logger, "lo que habia adentro es: %s", respuestaRead);
	respuestaRead = Hacer_Read("/archivo", 3, 3);
	log_error(logger, "lo que habia adentro desde 3: %s", respuestaRead);

	int cliente;
	conexion = iniciar_servidor("127.0.0.1", puerto, logger);

	while(1){
		cliente = esperar_cliente_con_accept(conexion, logger);

		pthread_t* cody = malloc(sizeof(pthread_t));
		if(pthread_create(cody,NULL,(void*)funcionMagica,(void*)cliente) == 0){
			pthread_detach(*cody);
			log_info(logger,"Se creo el hilo sin problema, cliente: %i", cliente);
		}else{
			log_error(logger,"No se pudo crear el hilo, cliente: %i", cliente);
		}
	};
	return EXIT_SUCCESS;
}

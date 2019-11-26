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

int contador;

uint32_t Hacer_Getattr(char *path){ return 0; }

uint32_t Hacer_ReadDir(char *path){ return 0; }

uint32_t Hacer_Open(char *path){ return 0; }

uint32_t Hacer_Read(char *path){ return 0; }

uint32_t Hacer_Release(char *path){ return 0; }

uint32_t Hacer_Write(char *path, char *buffer){ return 0; } 

uint32_t Hacer_MKNod(char *path){
	uint32_t numero_de_nodo = buscar_nodo_libre();
	return 0;
}

uint32_t Hacer_Unlink(char *path){ return 0; }

uint32_t Hacer_MKDir(char *path){
	char **StringSeparado = string_split(path, "/");
	uint32_t posicionFinal = damePosicionFinalDoblePuntero(StringSeparado);
	crear_directorio_en_nodo(contador, StringSeparado[posicionFinal]);
	contador = contador+1;
	liberarDoblePuntero(StringSeparado);
	return 1;
}

uint32_t Hacer_RMDir(char *path){ return 0; }

uint32_t Hacer_Rename(char *path, char *buffer){ return 0; }

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
				//Hacer_Getattr(pathGetAttr);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi GETATTR"), strlen("Hola, recibi GETATTR")+1, f_RESPONSE);
				free(pathGetAttr);
				break;

			case f_READDIR: ;
				char *pathReadDir = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathReadDir)+1);
				log_error(logger, pathReadDir);
				//Hacer_ReadDir(pathReadDir);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi READDIR"), strlen("Hola, recibi READDIR")+1, f_RESPONSE);
				free(pathReadDir);
				break;

			case f_READ: ;
				char *pathRead = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRead)+1);
				log_error(logger, pathRead);
				//Hacer_Read(pathRead);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi READ"), strlen("Hola, recibi READ")+1, f_RESPONSE);
				free(pathRead);
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
				//Hacer_MKNod(pathMKNod);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi MKNOD"), strlen("Hola, recibi MKNOD")+1, f_RESPONSE);
				free(pathMKNod);
				break;

			case f_UNLINK: ;
				char *pathUnlink = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathUnlink)+1);
				log_error(logger, pathUnlink);
				//Hacer_Unlink(pathUnlink);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi UNLINK"), strlen("Hola, recibi UNLINK")+1, f_RESPONSE);
				free(pathUnlink);
				break;

			case f_MKDIR: ;
				char *pathMKDir = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathMKDir)+1);
				log_error(logger, pathMKDir);
				if(Hacer_MKDir(pathMKDir))
					Fuse_PackAndSend(cliente, "Pude", strlen("pude")+1, f_RESPONSE);
				else
					Fuse_PackAndSend(cliente, "No pude", strlen("No pude")+1, f_RESPONSE);
				free(pathMKDir);
				break;

			case f_RMDIR: ;
				char *pathRMDir = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRMDir)+1);
				log_error(logger, pathRMDir);
				//Hacer_RMDir(pathRMDir);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi RMDIR"), strlen("Hola, recibi RMDIR")+1, f_RESPONSE);
				free(pathRMDir);
				break;

			case f_RENAME: ;
				char *pathRename = Fuse_ReceiveAndUnpack(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRename)+1);
				log_error(logger, pathRename);
				//Hacer_Rename(pathRename);	
				Fuse_PackAndSend(cliente, strdup("Hola, recibi RENAME"), strlen("Hola, recibi RENAME")+1, f_RESPONSE);
				free(pathRename);
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
	header->version = 3;
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

void iniciar_Sac_Server(){
	bloques_del_bitmap = ceil(((float)tamanio_disco/sizeof(Bloque)/8)/sizeof(Bloque));
	log_info(logger, "bloques_del_bitmap: %i",bloques_del_bitmap);

	cantidad_de_bloques_de_datos =  tamanio_disco/sizeof(Bloque) - 1 - bloques_del_bitmap - 1024;
	log_info(logger, "cantidad_de_bloques_de_datos: %llu",cantidad_de_bloques_de_datos);

	iniciar_header();
	log_info(logger, "Listo header");

	int bits = ceil((float)tamanio_disco/sizeof(Bloque));
	log_info(logger, "Bits: %i", bits);

	tBitarray = bitarray_create_with_mode(inicio_de_disco + 1, ceil((float)bits/8), MSB_FIRST);

	log_info(logger, "tamanio cargado del bitmap: %i", bitarray_get_max_bit(tBitarray));

	cargar_bitmap(1 + bloques_del_bitmap + 1024);

	iniciar_tabla_de_nodos();
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

void crear_directorio_en_nodo(int numero_de_nodo, char *nombre_de_archivo){
	tabla_de_nodos->nodos[numero_de_nodo].estado = 2;
	strncpy(tabla_de_nodos->nodos[numero_de_nodo].nombre_del_archivo, nombre_de_archivo, 70);
	tabla_de_nodos->nodos[numero_de_nodo].nombre_del_archivo[71] = '\0';
	tabla_de_nodos->nodos[numero_de_nodo].creacion=timestamp();
	tabla_de_nodos->nodos[numero_de_nodo].modificado=timestamp();
	tabla_de_nodos->nodos[numero_de_nodo].tamanio_del_archivo = sizeof(Bloque);
	tabla_de_nodos->nodos[numero_de_nodo].padre=0;
	log_info(logger, "bitmap: %i", buscar_espacio_en_bitmap());
	uint32_t numero_de_bloque = buscar_espacio_en_bitmap();
	log_info(logger, "bloque numero: %i", numero_de_bloque);
	bitarray_set_bit(tBitarray, numero_de_bloque);
	tabla_de_nodos->nodos[numero_de_nodo].array_de_punteros[0] = numero_de_bloque;
	log_info(logger,"cosas: %i",tabla_de_nodos->nodos[numero_de_nodo].array_de_punteros[0]);
}

//Para tener en cuenta cuando hagamos el crear archivo
//	int bloques_que_ocupa_archivo = tamanio_archivo_en_bloques(tabla_de_nodos.nodos[numero_de_nodo].tamanio_del_archivo);
//	for(int i = 0; i <= bloques_que_ocupa_archivo; i = i+1){

char* obtener_nombre_nodo(uint32_t numero_de_nodo){
	char *nombre_retornado;
	nombre_retornado = malloc(strlen(tabla_de_nodos->nodos[numero_de_nodo].nombre_del_archivo));
	strcpy(nombre_retornado, tabla_de_nodos->nodos[numero_de_nodo].nombre_del_archivo);
	return nombre_retornado;
}

char* obtener_nombre_padre_nodo(uint32_t numero_de_nodo){
	return obtener_nombre_nodo(tabla_de_nodos->nodos[numero_de_nodo].padre);
}

int buscar_nodo_libre(){
	for(int i = 0; i<=1024; i = i+1){
		if(!tabla_de_nodos->nodos[i].estado){
			return i;
		}
	}
	log_error(logger, "No hay nodos libres");
	return -1;
}

char** hallar_padres(char* nombre_buscado) {
	char* nombre_de_archivo;
	char* padre_actual;
	char** padres = malloc(70); //no estoy muy seguro si esto es necesario pero sino me tira un millon de warnings
	uint32_t contadorPadre = 0;
	for(int i=0; i<1024; i=i+1){
		nombre_de_archivo = obtener_nombre_nodo(i);
		if(strcmp(nombre_buscado,nombre_de_archivo)){
			padre_actual = obtener_nombre_padre_nodo(i);
			padres[contadorPadre] = &padre_actual;
			contadorPadre = contadorPadre+1;
		}
		free(nombre_de_archivo);
	}
	padres[contadorPadre] = NULL;
	return padres;
}

bool existe_path(char* path) {
	return true;
}

int main(int argc, char *argv[]) {
	contador = 2;
	//Log:
	logger = log_create("Sac-Server.log", "Sac-Server", 1, LOG_LEVEL_INFO);
	log_info(logger, "Se ha creado un nuevo logger\n");
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

	crear_directorio_en_nodo(0,strdup("carpetita"));
	crear_directorio_en_nodo(1, strdup("carpetota perro"));
	crear_directorio_en_nodo(2,strdup("carpetita explosiva"));

	int cliente;
	conexion = iniciar_servidor("127.0.0.1", "8081", logger);

	while(1){
		cliente = esperar_cliente_con_accept(conexion, logger);

		pthread_t* cody = malloc(sizeof(pthread_t));
		if(pthread_create(cody,NULL,(void*)funcionMagica,cliente) == 0){
			pthread_detach(cody);
			log_info(logger,"Se creo el hilo sin problema, cliente: %i", cliente);
		}else{
			log_error(logger,"No se pudo crear el hilo, cliente: %i", cliente);
		}
	};
	return EXIT_SUCCESS;
}

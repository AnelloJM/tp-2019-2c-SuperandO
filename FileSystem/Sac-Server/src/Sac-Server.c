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

t_log *logger;
int conexion;
uint32_t bloques_del_bitmap;
uint64_t tamanio_disco; //Como no se de que tamanio va a ser decido sobre exagerar
uint64_t cantidad_de_bloques_de_datos;
t_bitarray *tBitarray;
Header header;
Bitmap bitmap;
Tabla_de_nodos tabla_de_nodos;

uint32_t Hacer_Getattr(char *path){ return 0; }

uint32_t Hacer_ReadDir(char *path){ return 0; }

uint32_t Hacer_Open(char *path){ return 0; }

uint32_t Hacer_Read(char *path){ return 0; }

uint32_t Hacer_Release(char *path){ return 0; }

uint32_t Hacer_Write(char *path, char *buffer){ return 0; } 

uint32_t Hacer_MKNod(char *path){ return 0; }

uint32_t Hacer_Unlink(char *path){ return 0; }

uint32_t Hacer_MKDir(char *path, char *buffer){ return 0; }

uint32_t Hacer_RMDir(char *path){ return 0; }

uint32_t Hacer_CHMod(char *path){ return 0; }

uint32_t Hacer_Utime(char *path){ return 0; }

uint32_t Hacer_Rename(char *path, char *buffer){ return 0; }

uint32_t Hacer_Truncate(char *path){ return 0; }

uint32_t Hacer_SetXAttr(char *path){ return 0; }

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
				char *pathGetAttr= Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathGetAttr)+1);
				log_error(logger, pathGetAttr);
				//Hacer_Getattr(pathGetAttr);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi GETATTR"), strlen("Hola, recibi GETATTR")+1, f_RESPONSE);
				free(pathGetAttr);
				break;

			case f_READDIR: ;
				char *pathReadDir = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathReadDir)+1);
				log_error(logger, pathReadDir);
				//Hacer_ReadDir(pathReadDir);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi READDIR"), strlen("Hola, recibi READDIR")+1, f_RESPONSE);
				free(pathReadDir);
				break;

			case f_READ: ;
				char *pathRead = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRead)+1);
				log_error(logger, pathRead);
				//Hacer_Read(pathRead);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi READ"), strlen("Hola, recibi READ")+1, f_RESPONSE);
				free(pathRead);
				break;

			case f_OPEN: ;
				char *pathOpen = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathOpen)+1);
				log_error(logger, pathOpen);
				//Hacer_Open(pathOpen);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi OPEN"), strlen("Hola, recibi OPEN")+1, f_RESPONSE);
				free(pathOpen);
				break;

			case f_RELEASE: ;
				char *pathRelease = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRelease)+1);
				log_error(logger, pathRelease);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi RELEASE"), strlen("Hola, recibi RELEASE")+1, f_RESPONSE);
				free(pathRelease);
				break;

			case f_WRITE: ;
				char *pathWrite = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathWrite)+1);
				log_error(logger, pathWrite);
				//Hacer_Write(pathWrite, escritura);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi WRITE"), strlen("Hola, recibi WRITE")+1, f_RESPONSE);
				free(pathWrite);
				break;

			case f_MKNOD: ;
				char *pathMKNod = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathMKNod)+1);
				log_error(logger, pathMKNod);
				//Hacer_MKNod(pathMKNod);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi MKNOD"), strlen("Hola, recibi MKNOD")+1, f_RESPONSE);
				free(pathMKNod);
				break;

			case f_UNLINK: ;
				char *pathUnlink = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathUnlink)+1);
				log_error(logger, pathUnlink);
				//Hacer_Unlink(pathUnlink);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi UNLINK"), strlen("Hola, recibi UNLINK")+1, f_RESPONSE);
				free(pathUnlink);
				break;

			case f_MKDIR: ;
				char *pathMKDir = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathMKDir)+1);
				log_error(logger, pathMKDir);
				//Hacer_MKDir(pathMKDir, nombreDirectorio);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi MKDIR"), strlen("Hola, recibi MKDIR")+1, f_RESPONSE);
				free(pathMKDir);
				break;

			case f_RMDIR: ;
				char *pathRMDir = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRMDir)+1);
				log_error(logger, pathRMDir);
				//Hacer_RMDir(pathRMDir);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi RMDIR"), strlen("Hola, recibi RMDIR")+1, f_RESPONSE);
				free(pathRMDir);
				break;

			case f_CHMOD: ;
				char *pathCHMod = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathCHMod)+1);
				log_error(logger, pathCHMod);
				//Hacer_CHMod(pathCHMod);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi CHMOD"), strlen("Hola, recibi CHMOD")+1, f_RESPONSE);
				free(pathCHMod);
				break;

			case f_UTIME: ;
				char *pathUtime = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathUtime)+1);
				log_error(logger, pathUtime);
				//Hacer_Utime(pathUtime);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi UTIME"), strlen("Hola, recibi UTIME")+1, f_RESPONSE);
				free(pathUtime);
				break;

			case f_RENAME: ;
				char *pathRename = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRename)+1);
				log_error(logger, pathRename);
				//Hacer_Rename(pathRename);	
				Fuse_PackAndSend(cliente, strdup("Hola, recibi RENAME"), strlen("Hola, recibi RENAME")+1, f_RESPONSE);
				free(pathRename);
				break;

			case f_TRUNCATE: ;
				char *pathTruncate = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathTruncate)+1);
				log_error(logger, pathTruncate);
				//Hacer_Truncate(pathTruncate);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi TRUNCATE"), strlen("Hola, recibi TRUNCATE")+1, f_RESPONSE);
				free(pathTruncate);
				break;

			case f_SETXATTR: ;
				char *pathSetXAttr = Fuse_ReceiveAndUnpack_Path(cliente, tam);
				log_error(logger,"tamanio del path que recive: %i \0", strlen(pathSetXAttr)+1);
				log_error(logger, pathSetXAttr);
				//Hacer_SetXAttr(pathSetXAttr);
				Fuse_PackAndSend(cliente, strdup("Hola, recibi SETXATTR"), strlen("Hola, recibi SETXATTR")+1, f_RESPONSE);
				free(pathSetXAttr);
				break;

			case f_HANDSHAKE: ;
				//desempaquetar pack y hacer el codigo
				char *pathHandshake = Fuse_ReceiveAndUnpack_Path(cliente, headerRecibido.tamanioMensaje);
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
	bloques_del_bitmap =(tamanio_disco/sizeof(Bloque)/8)/sizeof(Bloque);
	log_info(logger, "bloques_del_bitmap: %i",bloques_del_bitmap);
	cantidad_de_bloques_de_datos =  tamanio_disco/sizeof(Bloque) - 1 - bloques_del_bitmap - 1024;
	log_info(logger, "cantidad_de_bloques_de_datos: %llu",cantidad_de_bloques_de_datos);

	header.identificador[0] = 'S';
	header.identificador[1] = 'A';
	header.identificador[2] = 'C';

	header.version = 3;
	header.inicio_bitmap = 1;
	header.tamanio_bitmap = bloques_del_bitmap;
}

void cargar_bitmap(int cantidad){
	int cargado = 0;
	while(cargado < cantidad){
		bitarray_set_bit(tBitarray, cargado);
		if(!bitarray_test_bit(tBitarray, cargado)){
			log_error(logger, "cargado: %i", cargado);
		}
		cargado = cargado+1;
	}
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
	return -1;
}

void ocupar_bloque_en_bitmap(int indice){
	bitarray_set_bit(tBitarray,indice);
}

void liberar_bloque_en_bitmap(int indice){
	bitarray_clean_bit(tBitarray,indice);
}

void iniciar_tabla_de_nodos(){
	for(int i = 0; i <= 1024; i = i+1){
		tabla_de_nodos.nodos[i].estado = 0;
	}
}

void iniciar_Sac_Server(){
	iniciar_header();
	log_info(logger, "Listo header");
	int bits = tamanio_disco/sizeof(Bloque);
	log_info(logger, "Bits: %i", bits);
	bitmap.bitArray = malloc(bits);
	tBitarray = bitarray_create_with_mode(bitmap.bitArray, (bits/8), MSB_FIRST);
	log_info(logger, "tamanio: %i", bitarray_get_max_bit(tBitarray));
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

int main(int argc, char *argv[]) {

	logger = log_create("Sac-Server.log", "Sac-Server", 1, LOG_LEVEL_INFO);
	log_info(logger, "Se ha creado un nuevo logger\n");

	tamanio_disco = 13421772800;

	log_info(logger, "Tamanio disco: %llu", tamanio_disco);

	iniciar_Sac_Server();
	log_info(logger, "bloques_del_bitmap: %i", bloques_del_bitmap);
	/*
	for(int cargado = 20; cargado <= 30; cargado++){
		bitarray_clean_bit(tBitarray, cargado);
	}*/
/*
	char *archivo = malloc(32);
	memcpy(archivo,"/home/utnso/workspace/disco.bin",32);
*/
	char *archivo = argv[1];
	log_info(logger, "Archivo: %s", archivo);
	uint32_t tamanio_disco_a_levantar = tamanio_archivo(archivo);
	log_info(logger, "Tamanio: %i", tamanio_disco_a_levantar);
	int disco = open(archivo, O_RDWR, 0);
	Bloque *inicio_de_disco = mmap(NULL, tamanio_disco_a_levantar, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FILE, disco, 0);

	memcpy(inicio_de_disco, &header,sizeof(Bloque));
	memcpy(inicio_de_disco+ 1, (void *)tBitarray->bitarray, tBitarray->size);
	log_info(logger, "sizeof(Tabla_de_nodos): %i", sizeof(Tabla_de_nodos));
	memcpy(inicio_de_disco + 1 + bloques_del_bitmap, &tabla_de_nodos, sizeof(Tabla_de_nodos));

	int cliente;
	conexion = iniciar_servidor("127.0.0.1", "6060", logger);

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

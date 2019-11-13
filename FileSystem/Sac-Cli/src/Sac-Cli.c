/*
 ============================================================================
 Name        : Sac-Cli.c
 Author      : Superand0
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <Conexiones/Conexiones.h>
#include <commons/log.h>
#include <Serializacion-FileSystem/Serializacion-FileSystem.h>

/* Este es el contenido por defecto que va a contener
 * el unico archivo que se encuentre presente en el FS.
 * Si se modifica la cadena se podra ver reflejado cuando
 * se lea el contenido del archivo
 */
#define DEFAULT_FILE_CONTENT "Contenido del archivito\n"

/*
 * Este es el nombre del archivo que se va a encontrar dentro de nuestro FS
 */
#define DEFAULT_FILE_NAME "ElArchivito"

/*
 * Este es el path de nuestro, relativo al punto de montaje, archivo dentro del FS
 */
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME


t_log *logger;
int conexion;


static int fusesito_getattr(const char *path, struct stat *stbuf) {
	log_info(logger, "Se llamo a fusesito_getattr\n");
	log_info(logger,path);
	int res = 0;
	/*log_info(logger,"%i \0", sizeof(HeaderFuse));
	log_info(logger,"%i \0", sizeof(*path));
	log_info(logger,"%i \0", sizeof(*stbuf));
	log_info(logger,"%i \0", (2*(sizeof(uint32_t))));
	char vector[sizeof(*path)];
	for(int i = 0; i<sizeof(*path); i++){
		vector[i]=*(path+i);
		}
	for(int i = 0; i<sizeof(*path); i++){
		log_info(logger,"%c",vector[i]);
	}*/
//	int tamPack = sizeof(HeaderFuse) + strlen(path) + strlen(stbuf) + (2*(sizeof(uint32_t)));
//	//log_info(logger,"%i \0", tamPack);
//	PaqueteFuse *pack = malloc(sizeof(PaqueteFuse));
//	FuseEmpaquetarPackGetAttr(path,stbuf,pack);
//	log_error(logger, "Codigo de operacion: %i", pack->headerFuse.operaciones);
//	log_error(logger, "Tamanio: %i", pack->headerFuse.tamanioMensaje);
//	char* pruebita = malloc(tamPack);
//	memcpy(pruebita,pack->mensaje,tamPack);
//	log_error(logger, "el mensaje pesa: %i", (strlen(pruebita)));
//	log_error(logger, "el path pesa: %i", (strlen(path)));
//	log_error(logger, "el stbuf pesa: %i", (strlen(stbuf)));
//
	if(Fuse_PackAndSend_Path(conexion, path, f_GETATTR)){
		log_info(logger, "se pudo enviar pack");
	}
	else{
		log_error(logger, "no se pudo enviar pack");
	}
//	sleep(60);

	//Continuo con lo que deberia hacer para que no cuelge, esto es solo para testear

		memset(stbuf, 0, sizeof(struct stat));

		//Si path es igual a "/" nos estan pidiendo los atributos del punto de montaje

		if (strcmp(path, "/") == 0) {
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		} else if (strcmp(path, DEFAULT_FILE_PATH) == 0) {
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_size = strlen(DEFAULT_FILE_CONTENT);
		} else {
			res = -ENOENT;
		}
		//free(pack);
		return res;
}


static int fusesito_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	log_info(logger, "Se llamo a fusesito_readdir\n");
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, DEFAULT_FILE_NAME , NULL, 0);
	filler(buf, "LaCosa" , NULL, 0);
	return 0;
}

static int fusesito_open(const char *path, struct fuse_file_info *fi) {
	log_info(logger, "Se llamo a fusesito_open\n");
	if (strcmp(path, DEFAULT_FILE_PATH) != 0)
			return -ENOENT;

		if ((fi->flags & 3) != O_RDONLY)
			return -EACCES;

		return 0;
}


static int fusesito_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	log_info(logger, "Se llamo a fusesito_read\n");
	size_t len;
	(void) fi;
	if (strcmp(path, DEFAULT_FILE_PATH) != 0)
		return -ENOENT;

	len = strlen(DEFAULT_FILE_CONTENT);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, DEFAULT_FILE_CONTENT + offset, size);
	} else
		size = 0;

	return size;
}



static struct fuse_operations fusesito_oper = {
		.getattr = fusesito_getattr,
		.readdir = fusesito_readdir,
		.open = fusesito_open,
		.read = fusesito_read,
};


int main(int argc, char *argv[]) {

	logger = log_create("Sac-Cli.log", "Sac-Cli", 1, LOG_LEVEL_INFO);
	log_info(logger, "Se ha iniciado una nueva instancia del logger\n");
	conexion = conectarse_a_un_servidor("127.0.0.1" , "8089", logger);

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	// Esta es la funcion principal de FUSE, es la que se encarga
	// de realizar el montaje, comuniscarse con el kernel, delegar todo
	// en varios threads
	return fuse_main(args.argc, args.argv, &fusesito_oper, NULL);

	/* LO ANTERIOR
	puts("Superand0 Sac-Cli Iniciando..."); prints Supernd0 Sac-Cli Iniciando...
	t_log *logger = log_create("Sac-Cliente.log", "Sac-Cliente", 1, LOG_LEVEL_INFO);
	int conexion, server;
	Paquete *paquete;
	conexion = conectarse_a_un_servidor("127.0.0.1" , "8080", logger);
	EnviarHandshake(conexion, 3);
	return EXIT_SUCCESS;
	*/
}

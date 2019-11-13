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
#include <semaphore.h>

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
sem_t mutex_buffer;

static int fusesito_getattr(const char *path, struct stat *stbuf) {
	log_info(logger, "Se llamo a fusesito_getattr\n");
	log_info(logger,path);
	int res = 0;
	if(Fuse_PackAndSend_Path(conexion, path, f_GETATTR)){
		log_info(logger, "se pudo enviar pack");
	}
	else{
		log_error(logger, "no se pudo enviar pack");
	}
	HeaderFuse headerRecibido;
	sem_wait(&mutex_buffer);
	headerRecibido = Fuse_RecieveHeader(conexion);
	log_error(logger, "Codigo de operacion: %i", headerRecibido.operaciones);
	log_error(logger, "Tamanio: %i", headerRecibido.tamanioMensaje);
	uint32_t tam = headerRecibido.tamanioMensaje;
	char *pathRecibido= Fuse_ReceiveAndUnpack_Path(conexion, tam);
	sem_post(&mutex_buffer);
	log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRecibido)+1);
	log_error(logger, pathRecibido);


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
	log_info(logger,path);
	if(Fuse_PackAndSend_Path(conexion, path, f_READDIR)){
		log_info(logger, "se pudo enviar pack");
	}
	else{
		log_error(logger, "no se pudo enviar pack");
	}
	HeaderFuse headerRecibido;
	sem_wait(&mutex_buffer);
	headerRecibido = Fuse_RecieveHeader(conexion);
	log_error(logger, "Codigo de operacion: %i", headerRecibido.operaciones);
	log_error(logger, "Tamanio: %i", headerRecibido.tamanioMensaje);
	uint32_t tam = headerRecibido.tamanioMensaje;
	char *pathRecibido= Fuse_ReceiveAndUnpack_Path(conexion, tam);
	sem_post(&mutex_buffer);
	log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRecibido)+1);
	log_error(logger, pathRecibido);

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, DEFAULT_FILE_NAME , NULL, 0);
	filler(buf, "LaCosa" , NULL, 0);
	return 0;
}

static int fusesito_open(const char *path, struct fuse_file_info *fi) {
	log_info(logger, "Se llamo a fusesito_open\n");

	log_info(logger,path);
	if(Fuse_PackAndSend_Path(conexion, path, f_OPEN)){
		log_info(logger, "se pudo enviar pack");
	}
	else{
		log_error(logger, "no se pudo enviar pack");
	}
	HeaderFuse headerRecibido;
	sem_wait(&mutex_buffer);
	headerRecibido = Fuse_RecieveHeader(conexion);
	log_error(logger, "Codigo de operacion: %i", headerRecibido.operaciones);
	log_error(logger, "Tamanio: %i", headerRecibido.tamanioMensaje);
	uint32_t tam = headerRecibido.tamanioMensaje;
	char *pathRecibido= Fuse_ReceiveAndUnpack_Path(conexion, tam);
	sem_post(&mutex_buffer);
	log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRecibido)+1);
	log_error(logger, pathRecibido);

	if (strcmp(path, DEFAULT_FILE_PATH) != 0)
			return -ENOENT;

		if ((fi->flags & 3) != O_RDONLY)
			return -EACCES;

		return 0;
}


static int fusesito_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	log_info(logger, "Se llamo a fusesito_read\n");

	log_info(logger,path);
	if(Fuse_PackAndSend_Path(conexion, path, f_READ)){
		log_info(logger, "se pudo enviar pack");
	}
	else{
		log_error(logger, "no se pudo enviar pack");
	}

	HeaderFuse headerRecibido;
	sem_wait(&mutex_buffer);
	headerRecibido = Fuse_RecieveHeader(conexion);
	log_error(logger, "Codigo de operacion: %i", headerRecibido.operaciones);
	log_error(logger, "Tamanio: %i", headerRecibido.tamanioMensaje);
	uint32_t tam = headerRecibido.tamanioMensaje;
	char *pathRecibido= Fuse_ReceiveAndUnpack_Path(conexion, tam);
	sem_post(&mutex_buffer);
	log_error(logger,"tamanio del path que recive: %i \0", strlen(pathRecibido)+1);
	log_error(logger, pathRecibido);

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

static int fusesito_release(const char * creo_que_es_un_path, struct fuse_file_info *no_se_que_es){
	return 0;
}

static int fusesito_write(const char * creo_que_es_un_path, struct fuse_file_info *no_se_que_es){
	return 0;
}
static int fusesito_mknod(const char * creo_que_es_un_path, struct fuse_file_info *no_se_que_es){
	return 0;
}
static int fusesito_unlink(const char * creo_que_es_un_path, struct fuse_file_info *no_se_que_es){
	return 0;
}
static int fusesito_mkdir(const char * creo_que_es_un_path, struct fuse_file_info *no_se_que_es){
	return 0;
}
static int fusesito_rmdir(const char * creo_que_es_un_path, struct fuse_file_info *no_se_que_es){
	return 0;
}
static int fusesito_utime(const char * creo_que_es_un_path, struct fuse_file_info *no_se_que_es){
	return 0;
}
static int fusesito_chmod(const char * creo_que_es_un_path, struct fuse_file_info *no_se_que_es){
	return 0;
}
static int fusesito_rename(const char * creo_que_es_un_path, struct fuse_file_info *no_se_que_es){
	return 0;
}
static int fusesito_truncate(const char * creo_que_es_un_path, struct fuse_file_info *no_se_que_es){
	return 0;
}
static int fusesito_setxattr(const char * creo_que_es_un_path, struct fuse_file_info *no_se_que_es){
	return 0;
}


static struct fuse_operations fusesito_oper = {
		.getattr = fusesito_getattr,
		.readdir = fusesito_readdir,
		.open = fusesito_open,
		.read = fusesito_read,
		.release = fusesito_release,
		.write = fusesito_write,
		.mknod = fusesito_mknod,
		.unlink = fusesito_unlink,
		.mkdir = fusesito_mkdir,
		.rmdir = fusesito_rmdir,
		.chmod = fusesito_chmod,
		.utime = fusesito_utime,
		.rename = fusesito_rename,
		.truncate = fusesito_truncate,
		.setxattr = fusesito_setxattr,
};


int main(int argc, char *argv[]) {

	logger = log_create("Sac-Cli.log", "Sac-Cli", 1, LOG_LEVEL_INFO);
	log_info(logger, "Se ha iniciado una nueva instancia del logger\n");
	conexion = conectarse_a_un_servidor("127.0.0.1" , "8999", logger);
	sem_init(&mutex_buffer,0,1);

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	// Esta es la funcion principal de FUSE, es la que se encarga
	// de realizar el montaje, comuniscarse con el kernel, delegar todo
	// en varios threads
	return fuse_main(args.argc, args.argv, &fusesito_oper, NULL);

}

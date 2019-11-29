/*
 ==================================================================================
 Name        : Sac-Cli.c
 Author      : Superand0
 Version     : 0.1
 Copyright   : Superand0 2C 2019
 Description : Proceso que se encarga de "catchear" las funciones del File System
 	 	 	   y se las envia a Sac-Server, espera una respuesta y en base a esta
 	 	 	   retorna al SO
 ==================================================================================
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
#include <commons/config.h>
#include <Serializacion-FileSystem/Serializacion-FileSystem.h>
#include <Manejos-Comunes/Manejos-Comunes.h>
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

void* enviarMiPathYRecibirResponse(t_log *logger, const char *path, int conexion, f_operacion operacion) {
	log_info(logger,path);
	if(Fuse_PackAndSend(conexion, path, (strlen(path)+1) , operacion)){
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
	void *pathRecibido= Fuse_ReceiveAndUnpack(conexion, tam);
	sem_post(&mutex_buffer);
	log_error(logger,"tamanio del path que recibe: %i \0", strlen(pathRecibido)+1);
	log_error(logger, pathRecibido);
	return pathRecibido;
}

static int fusesito_getattr(const char *path, struct stat *stbuf) {
	log_info(logger, "Se llamo a fusesito_getattr\n");
	int res = 0;
	char *response = enviarMiPathYRecibirResponse(logger, path, conexion, f_GETATTR);
	if(string_starts_with(response,"0"))
		res = -ENOENT;
	if(string_starts_with(response,"1")){
			stbuf->st_mode = S_IFREG | 0777;
	}
	if(string_starts_with(response,"2")){
			stbuf->st_mode = S_IFDIR | 0777;
	}
	stbuf->st_nlink = 1;
	free(response);
	return res;
/*
	//Continuo con lo que deberia hacer para que no cuelge, esto es solo para testear

		memset(stbuf, 0, sizeof(struct stat));

		//Si path es igual a "/" nos estan pidiendo los atributos del punto de montaje

		if (strcmp(path, "/") == 0) {
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 1;
		} else if (strcmp(path, DEFAULT_FILE_PATH) == 0) {
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_size = strlen(DEFAULT_FILE_CONTENT);
		} else {
			res = -ENOENT;
		}
		static bool test = false;
		if(test && strcmp(path, "/hola") == 0){
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 1;
		res = 0;
		}
		if(strcmp(path, "/hola"))
			test = true;

		return res;
		*/
}


static int fusesito_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	log_info(logger, "Se llamo a fusesito_readdir\n");
	
	char *response = enviarMiPathYRecibirResponse(logger, path, conexion, f_READDIR);
	free(response);

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, DEFAULT_FILE_NAME , NULL, 0);
	filler(buf, "LaCosa" , NULL, 0);
	return 0;
}

static int fusesito_open(const char *path, struct fuse_file_info *fi) {
	log_info(logger, "Se llamo a fusesito_open\n");

	char *response = enviarMiPathYRecibirResponse(logger, path, conexion, f_OPEN);
	free(response);
//
//	if (strcmp(path, DEFAULT_FILE_PATH) != 0)
//			return -ENOENT;
//
//		if ((fi->flags & 3) != O_RDONLY)
//			return -EACCES;

		return 0;
}


static int fusesito_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	log_info(logger, "Se llamo a fusesito_read\n");

	char *response = enviarMiPathYRecibirResponse(logger, path, conexion, f_READ);
	free(response);

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

static int fusesito_release(const char *path, struct fuse_file_info *fi){
	log_info(logger, "Se llamo a fusesito_release\n");
	char *response = enviarMiPathYRecibirResponse(logger, path, conexion, f_RELEASE);
	free(response);
	return 0;
}
static int fusesito_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	log_info(logger, "Se llamo a fusesito_write\n");
	char *response = enviarMiPathYRecibirResponse(logger, path, conexion, f_WRITE);
	free(response);
	return 0;
}
static int fusesito_mknod(const char *path, mode_t mode, dev_t dev){
	log_info(logger, "Se llamo a fusesito_mknod\n");
	char *response = enviarMiPathYRecibirResponse(logger, path, conexion, f_MKNOD);
	free(response);
	return 0;
}
static int fusesito_unlink(const char *path){
	log_info(logger, "Se llamo a fusesito_unlink\n");
	char *response = enviarMiPathYRecibirResponse(logger, path, conexion, f_UNLINK);
	free(response);
	return 0;
}
static int fusesito_mkdir(const char *path, mode_t mode){
	log_info(logger, "Se llamo a fusesito_mkdir\n");
	char *response = enviarMiPathYRecibirResponse(logger, path, conexion, f_MKDIR);
	bool result = string_starts_with(response,"Pude");
	free(response);
	if(result)
	{
		log_info(logger, "Se pudo hacer el mkdir");
		return 0;
	}
	log_info(logger, "No se pudo hacer el mkdir");
	return EPERM;
}
static int fusesito_rmdir(const char *path){
	log_info(logger, "Se llamo a fusesito_rmdir\n");
	char *response = enviarMiPathYRecibirResponse(logger, path, conexion, f_RMDIR);
	free(response);
	return 0;
}
static int fusesito_rename(const char *path, const char *buf){
	log_info(logger, "Se llamo a fusesito_rename\n");
	/*char *response = enviarMiPathYRecibirResponse(logger, path, conexion, f_RENAME);
	free(response);*/
	if(!Fuse_PackAndSend_Rename(conexion, path, buf))
		return -ENOENT;
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
		.rename = fusesito_rename,
};


int main(int argc, char *argv[]) {

	logger = log_create("Sac-Cli.log", "Sac-Cli", 1, LOG_LEVEL_INFO);
	log_info(logger, "Se ha iniciado una nueva instancia del logger\n");

	t_config *archivo_de_configuracion = config_create("../../Sac.config");
	char *puerto = config_get_string_value(archivo_de_configuracion, "LISTEN_PORT ");
	log_info(logger, puerto);

	conexion = conectarse_a_un_servidor("127.0.0.1" , puerto, logger);
	sem_init(&mutex_buffer,0,1);

	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	/* Esta es la funcion principal de FUSE, es la que se encarga
	 de realizar el montaje, comuniscarse con el kernel, delegar todo
	 en varios threads */
	return fuse_main(args.argc, args.argv, &fusesito_oper, NULL);

}

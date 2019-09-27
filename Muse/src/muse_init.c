#include "muse_init.h"
/**
     * Inicializa la biblioteca de MUSE.
     * @param id El Process (o Thread) ID para identificar el caller en MUSE.
     * @param ip IP en dot-notation de MUSE.
     * @param puerto Puerto de conexión a MUSE.
     * @return Si pasa un error, retorna -1. Si se inicializó correctamente, retorna 0.
     * @see Para obtener el id de un proceso pueden usar getpid() de la lib POSIX (unistd.h)
     * @note Debido a la naturaleza centralizada de MUSE, esta función deberá definir
     *  el ID del proceso/hilo según "IP-ID".
     */
int main()
{
  crearLogger();
  leerArchivoDeConfiguracion();

  log_info(logger,"\n [+]La configuracion es la siguiente \n");
  log_info(logger,"ID ->  %d",id );
  log_info(logger,"IP ->  %s",ip );
  log_info(logger,"Puerto ->  %s",puerto );
  log_info(logger,"memory size ->  %d",memory_size);
  log_info(logger,"page size ->  %d",page_size);
  log_info(logger,"swap size ->  %d",swap_size);
  log_info(logger,"--------------\n");

  printf("\n\n::::::::INICIAMOS EL SERVIDOR::::::::\n");

  socketMuse = iniciar_servidor(ip,puerto,logger);
  socket_cliente = esperar_cliente_con_accept(socketMuse,logger);
  enviar_mensaje(socket_cliente,logger);

  return 0;
}

void crearLogger()
{
	char* logPath = /*"MUSE.log";*/"/home/utnso/workspace/tp-2019-2c-SuperandO/Muse/src/MUSE.log";
	char* nombreArch = "MUSE";
	bool consolaActiva = true;
	logger = log_create(logPath, nombreArch, consolaActiva, LOG_LEVEL_INFO);
	log_info(logger, "El logger se creo con exito");
//	free(logPath);
}

void leerArchivoDeConfiguracion()
{
	char* configPath = /*"MUSE.cfg";*/ "/home/utnso/workspace/tp-2019-2c-SuperandO/Muse/src/MUSE.cfg";
	archivoConfig = config_create(configPath);
	if (archivoConfig == NULL){
		perror("ERROR: Archivo de configuracion no encontrado");
	}
	setearValores(archivoConfig);
	log_info(logger,"La configuracion fue cargada exitosamente");
	config_destroy(archivoConfig);
}

void setearValores(t_config* archivoConfig)
{
	ip = strdup(config_get_string_value(archivoConfig,"IP"));
	id = config_get_int_value(archivoConfig,"ID");
	puerto = strdup(config_get_string_value(archivoConfig,"PUERTO"));
	memory_size = config_get_int_value(archivoConfig,"MEMORY_SIZE");
	page_size =	config_get_int_value(archivoConfig,"PAGE_SIZE");
	swap_size = config_get_int_value(archivoConfig,"SWAP_SIZE");
}



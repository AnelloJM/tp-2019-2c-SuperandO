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
int main(){
  crearLogger();
  leerArchivoDeConfiguracion();

  log_info(logger,"\n [+]La configuracion es la siguiente \n");
  log_info(logger,"ID ->  %d\n",id );
  log_info(logger,"IP ->  %s\n",ip );
  log_info(logger,"Puerto ->  %d\n",puerto );
  log_info(logger,"--------------\n");

  printf("\n\n::::::::INICIAMOS EL SERVIDOR::::::::\n");

  socketMuse= iniciar_servidor(ip,puerto,logger);

  return 0;
}

void leerArchivoDeConfiguracion(){
	char* configPath = strdup("/home/utnso/workspace/tp-2019-2c-SuperandO/Muse/src/MUSE.cfg");
	archivoConfig = config_create(configPath);
	if (archivoConfig == NULL){
		perror("ERROR: Archivo de configuracion no encontrado");
		//loggearlo
	}
	setearValores(archivoConfig);
	config_destroy(archivoConfig);
}

void setearValores(t_config* archivoConfig){
	id = config_get_int_value(archivoConfig,"ID");
	ip = config_get_string_value(archivoConfig,"IP");
	puerto = config_get_int_value(archivoConfig,"PUERTO");

}

void crearLogger(){
	char* logPath = strdup("/home/utnso/workspace/tp-2019-2c-SuperandO/Muse/src/MUSE.log");
	char* nombreArch = "MUSE";
	bool consolaActiva = true;
	logger = log_create(logPath, nombreArch, consolaActiva, LOG_LEVEL_INFO);
	free(logPath);
}



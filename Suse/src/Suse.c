/*
 ============================================================================
 Name        : Suse.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Suse.h"

int main()
{

	crearLogger();
	leerArchivoDeConfiguracion();

	log_info(logger,"\n [+]La configuracion es la siguiente \n");
	log_info(logger,"LISTEN_PORT ->  %s",listen_port );
	log_info(logger,"Metrics_timer ->  %d",metrics_timer );
	log_info(logger,"--------------\n");

	printf("\n\n::::::::INICIAMOS EL SERVIDOR SUSE::::::::\n");

	socket_Suse = iniciar_servidor("127.0.0.1",listen_port,logger);
	socket_cliente = esperar_cliente_con_accept(socket_Suse,logger);
	enviar_mensaje(socket_cliente,logger);
	return 0;
}

void crearLogger()
{
	char* logPath = "SUSE.log"; //"/home/utnso/workspace/tp-2019-2c-SuperandO/Suse/src/SUSE.log";
	char* nombreArch = "SUSE";
	bool consolaActiva = true;
	logger = log_create(logPath, nombreArch, consolaActiva, LOG_LEVEL_INFO);
	log_info(logger, "El logger se creo con exito");
//	free(logPath);
}


void leerArchivoDeConfiguracion()
{
	char* configPath = "SUSE.cfg"; // "/home/utnso/workspace/tp-2019-2c-SuperandO/Suse/src/SUSE.cfg";
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
	listen_port = strdup(config_get_string_value(archivoConfig,"LISTEN_PORT"));
	metrics_timer = config_get_int_value(archivoConfig,"METRICS_TIMER");
	gMultiprogramacion = cofig_get_int_value(archivoConfig,"MAX_MULTIPROG");
}


void suse_init(){}

//Crear nuevo hilo -> pasar funcion por parametro que sera el main del hilo -> el hilo finaliza cuando termina funcion.
//Cambiar pthread por hilolay
void suse_create(void(*fmain)(Paquete)){

	pthread_t hilo;
	char * arg;
	int estado;

	estado = pthread_create(&hilo, NULL, fmain,(void*) arg);
	pthread_join(hilo,NULL);

	puts("El hilo devolvió: %d", estado);
}

//Obtiene el proximo hilo a ejecutar
//Parametro: Cola de procesos READY en SUSE
void suse_schedule_next(){}

void suse_wait(int sem){
	sem--;
}

void suse_signal(int sem){
	sem++;
}

//hace lo mismo que pthread_join. TIene como parametro un hilo (y un PID?)
void suse_join(){}



//Funcion que crea las colas ready segun el grado de multiprogramacion


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

void *funcionMagica(int cliente){

}

int main(void) {
	iniciar_logger();
	int conexion, cliente;
	conexion = iniciar_servidor("127.0.0.1","6969",logger);

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


}

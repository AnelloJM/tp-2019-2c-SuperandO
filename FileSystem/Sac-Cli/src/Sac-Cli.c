/*
 ============================================================================
 Name        : Sac-Cli.c
 Author      : Superand0
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <Conexiones/Conexiones.h>
#include <Serializacion/serializacion.h>
#include <commons/log.h>



int main(void) {
	//puts("Superand0 Sac-Cli Iniciando..."); /* prints Supernd0 Sac-Cli Iniciando... */
	t_log *logger = log_create("Sac-Cliente.log", "Sac-Cliente", 1, LOG_LEVEL_INFO);
	int conexion, server;
	Paquete *paquete;
	conexion = conectarse_a_un_servidor("127.0.0.1" , "8080", logger);
	EnviarHandshake(conexion, 3);
	return EXIT_SUCCESS;

}

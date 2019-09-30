#ifndef SUSE_H
#define SUSE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "../../ComunParaTodos/Conexiones/Conexiones.h" //esta libreria se define asi porque está en nuestro workspace
#include "../../ComunParaTodos/Conexiones/Conexiones.c" // esta la sigo poniendo por mi editor, ignorenla
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/queue.h>
//#include "../../ComunParaTodos/Serealizacion/serealizacion.h"

t_log* logger;
t_config* archivoConfig;

/*VARIABLES*/

int socket_Suse;
int socket_cliente;

//Colas
t_queue * new;
t_queue * blocked;
t_queue * exit;

//cómo implemento los estados

char* listen_port;
int metrics_timer;
//Sems_ids[array]
//sem_init[array]
//sem_max[array]
//alpha_sjf[numerico]



/* FUNCIONES */


void crearLogger();
void leerArchivoDeConfiguracion();
void setearValores();


#endif

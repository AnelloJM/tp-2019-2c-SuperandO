#ifndef SUSE_H
#define SUSE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "../../ComunParaTodos/Conexiones/Conexiones.h" //esta libreria se define asi porque está en nuestro workspace
//#include "../../ComunParaTodos/Conexiones/Conexiones.c" // esta la sigo poniendo por mi editor, ignorenla
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <hilolay/hilolay.h>
#include "../../ComunParaTodos/Serializacion/serializacion.h"
#define PATH_ARCH_CONFIG "/home/utnso/git/tp-2019-2c-SuperandO/Suse/src/SUSE.cfg"


t_log* logger;
t_config* archivoConfig;

/*VARIABLES*/

int socket_Suse;
int socket_cliente;

//Colas comunes para todos los programas
t_queue * cola_new;
t_queue * cola_blocked;
t_queue * cola_exit;

//cómo implemento los estados

//Arch de Config
char* listen_port;
int metrics_timer;
int max_multiprog;
//Sems_ids[array]
//sem_init[array]
//sem_max[array]
//alpha_sjf[numerico]

//PAQUETE//
Paquete* pack;

/* FUNCIONES */


void crearLogger();
void leerArchivoDeConfiguracion();
void setearValores();


#endif

#ifndef SUSE_H
#define SUSE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include "../../ComunParaTodos/Conexiones/Conexiones.h"
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <hilolay/hilolay.h>
#include "../../ComunParaTodos/Serializacion/serializacion.h"


t_log* logger;
t_config* archivoConfig;

/*VARIABLES*/

int socket_Suse;
int socket_cliente;

//Colas comunes para todos los programas
t_list * cola_new;
t_list * cola_blocked;
t_list * cola_exit;

//Creo que es mejor que usemos listas en vez de colas porque son mucho mas versatiles y tienen muchas mas funciones en las commons que nos pueden servir

//cómo implemento los estados

//Arch de Config
char* listen_port;
int metrics_timer;
int max_multiprog;
void* sems_ids;
void* sem_init;
void* sem_max;
float alpha_sjf;

//PAQUETE//
Paquete* pack;

/* FUNCIONES */


void crearLogger();
void leerArchivoDeConfiguracion();
void setearValores();
float calcularEstimacion();


#endif

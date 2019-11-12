#ifndef SUSE_H
#define SUSE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <readline/readline.h>
#include "../../ComunParaTodos/Conexiones/Conexiones.h"
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
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
t_list * semaforos;

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

//PAQUETES//
typedef struct {
	hilolay_t *thread;
	hilolay_attr_t *attr;
	void *arg;
	int pid;
	int tid;
	int estimacionAnterior;
	float rafagasEjecutadas;
	float rafagasEstimadas;
}hilo_t;

typedef struct {
	char* semID;
	int semInit;
	int semActual;
	int semMax;
	t_list* procesos;
}semaforo_t;

/* FUNCIONES */
void crearLogger();
void leerArchivoDeConfiguracion();
void setearValores();
void suse_init();
void cargarSemaforos();
void suse_create(hilo_t* hilo);
void* suse_schedule_next();
hilo_t calcularEstimacion();
bool comparador(hilo_t* unHilo, hilo_t* otroHilo);
bool comparadorDeRafagas();
int list_get_index(t_list* self, void* elemento, bool(*comparador (void*, void*)));
void suse_wait(semaforo_t* sem);
bool comparadorDeSemaforos(semaforo_t unSem, semaforo_t otroSem);
void suse_signal(semaforo_t* sem);
void suse_join();
void suse_close();

#endif

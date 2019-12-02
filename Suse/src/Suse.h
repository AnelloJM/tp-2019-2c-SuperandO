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
#include <commons/collections/queue.h>
#include <pthread.h>

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
int colasLibres;
t_queue * colanew;
//Arch de Config
char* listen_port;
int metrics_timer;
int max_multiprog;
void* sems_ids;
void* sem_init;
void* sem_max;
float alpha_sjf;
int tidMAX;

//PAQUETES//
typedef struct {
	hilolay_t *thread;
	hilolay_attr_t *attr;
	void *arg;
	char * pid;
	char * tid;
	int estimacionAnterior;
	float rafagasEjecutadas;
	float rafagasEstimadas;
	int tiempo_espera;
	int tiempo_bloqueado;
	char * razon_bloqueado;
	int tiempo_ejecutando;
	bool finalizado;
} hilo_t; //TCB

typedef struct {
	//int pid; este ya esta en la tcb t_hilo;
	char * pid;
	t_list * cola_ready;
	t_list * cola_exec;
} programa_t;

typedef struct {
	char* semID;
	int semInit;
	int semActual;
	int semMax;
	t_list* hilosEnEspera;
} semaforo_t;

/* FUNCIONES */
void crearLogger();
void leerArchivoDeConfiguracion();
void setearValores();
void cargarSemaforos();
//void suse_create(int programa, int variable);
void * suse_schedule_next(int socket_cliente);
int dispatcher(hilo_t* hilo);
hilo_t calcularEstimacion();
bool comparador(hilo_t* unHilo, hilo_t* otroHilo);
bool comparadorDeRafagas();
int list_get_index(t_list* self, void* elemento,
		bool (*comparador(void*, void*)));
int buscadorSemaforo(semaforo_t* semaforo);
//int suse_wait(semaforo_t* semaforo, char*tid);
bool comparadorDeSemaforos(semaforo_t unSem, semaforo_t otroSem);
//int suse_signal(semaforo_t* semaforo, char*tid);
//void suse_join();
void * suse_close(int socket_cliente, t_list * tid);

int sumar2(int);

#endif

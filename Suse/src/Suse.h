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
#include "../../ComunParaTodos/Serializacion/serializacion.h"
#include <commons/collections/queue.h>
#include <pthread.h>
#include "../../ComunParaTodos/Lista/lista.h"

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
	char * pid;
	t_list * cola_ready;
	t_list * cola_exec;
} programa_t;

t_list * lista_programas;

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
void * suse_create(int pid_prog);
void * suse_schedule_next(int pid_prog);
bool comparadorPrograma(char* unPid, programa_t* unPrograma);
hilo_t calcularEstimacion(hilo_t unHilo);
bool comparador(hilo_t* unHilo, hilo_t* otroHilo);
bool comparadorDeRafagas();
int buscadorSemaforo(semaforo_t* semaforo);
//void * suse_wait(int socket_cliente, char * semaforo);
bool comparadorDeSemaforos(semaforo_t unSem, semaforo_t otroSem);
//void * suse_signal(int socket_cliente, char * semaforo);
//void * suse_join(int socket_cliente, char * tid);
void * suse_close(int socket_cliente, char * tid);
void * planificador_NEW_READY();

int sumar2(int);

#endif

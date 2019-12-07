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
#include <time.h>
#include <semaphore.h>

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
char* server_ip;
char* listen_port;
int metrics_timer;
int max_multiprog;
void* sems_ids;
void* sem_init_s;
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
	char * razon_bloqueado;
	bool finalizado;
	//Tiempo entre la creacion y la toma de la metrica
	int tiempoEjecucionInicial;
	int tiempoEjecucion;
	//Tiempo que pasa en el estado READY
	int tiempoEsperaInicial; /*CUANDO LO MANDE A READY TENGO QUE TOMAR EL TIEMPO DE ESE INSTANTE, NO OLVIDARSE (CUANDO PASA DE NEW A READY)*/
	int tiempoEsperaFinal;
	int tiempo_espera;
	//Tiempo que pasa en el estado EXEC
	int tiempoUsoCPUInicial;
	int tiempoUsoCPUFinal;
	int tiempoUsoCPU;
	//Tiempo ejecucion del hilo dividido el total de tiempo de ejecucion de todos los hilos del proceso
	float porcentajeTiempoEjecucion;
} hilo_t; //TCB

typedef struct {
	char * pid;
	t_list * hilos;
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
void * tomarMetricasAutomaticas();
void calcularTiempoEjecucion(hilo_t* hilo);
void tomarMetricas();
void * suse_create(int pid_prog);
void * suse_schedule_next(int pid_prog);
bool comparadorPrograma(char* unPid, programa_t* unPrograma);
hilo_t calcularEstimacion(hilo_t unHilo);
bool comparadorDeHilos(hilo_t* unHilo, hilo_t* otroHilo);
bool comparadorDeRafagas();
int buscadorSemaforo(char* semaforo);
void * suse_wait(int pid_prog, char * semaforo);
bool comparadorDeSemaforos(char* unSem, semaforo_t otroSem);
void * suse_signal(int socket_cliente, char * semaforo);
bool buscadorDeHilos(char* tid, hilo_t* hilo);
void * suse_join(int pid_prog, char * tid);
void * suse_close(int pid_prog, char * tid);
void * planificador_NEW_READY();
bool comparadorMismoPrograma(hilo_t * hilo1, char * pid_programa);
int recibir_paquete_deserializar(int socket_cliente, Paquete* pack);

//int sumar2(int);

#endif
